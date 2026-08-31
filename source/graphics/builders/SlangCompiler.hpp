#pragma once

#include <slang.h>
#include <slang-com-ptr.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <ExecutablePath.hpp>

namespace Eve::Graphics
{

    struct ShaderBytecode
    {
        std::vector<uint32_t> vertex;
        std::vector<uint32_t> fragment;
        std::vector<uint32_t> compute;
    };

    class SlangCompiler
    {
        public:

            inline static void Initialize(std::vector<std::string>& searchPaths)
            {
                if (SLANG_FAILED(slang::createGlobalSession(globalSession.writeRef())))
                {
                    throw std::runtime_error("Errore GlobalSession");
                }

                slang::TargetDesc targetDesc = {};
                targetDesc.format = SLANG_SPIRV;
                targetDesc.profile = globalSession->findProfile("spirv_1_5");

                fs::path executablePath = GetExecutableDirectory();

                std::vector<std::string> fullPaths;
                fullPaths.reserve(searchPaths.size());

                for (const auto& path : searchPaths)
                {
                    fs::path fullPath = executablePath / path;
                    fullPaths.push_back(fullPath.string());
                }

                std::vector<const char*> searchPathPtrs;
                searchPathPtrs.reserve(fullPaths.size());

                for (const auto& path : fullPaths)
                {
                    searchPathPtrs.push_back(path.c_str());
                }
                slang::SessionDesc sessionDesc = {};
                sessionDesc.targets = &targetDesc;
                sessionDesc.targetCount = 1;
                sessionDesc.searchPaths = searchPathPtrs.data();
                sessionDesc.searchPathCount = static_cast<uint32_t>(searchPathPtrs.size());
                sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_ROW_MAJOR; 

                if (SLANG_FAILED(globalSession->createSession(sessionDesc, session.writeRef()))) 
                {
                    throw std::runtime_error("Error Session");
                }
            }

            inline static ShaderBytecode CompileVertFrag(const char* shaderModule)
            {
                Slang::ComPtr<slang::IBlob> diagnostics;
                slang::IModule* module = session->loadModule(shaderModule, diagnostics.writeRef());
                CheckSlangDiagnostics(diagnostics.get(), "Module Load");

                if (!module) {
                    throw std::runtime_error("Unable to get a valid Slang module");
                }

                ShaderBytecode result;
                slang::ProgramLayout* programLayout = nullptr;

                result.vertex = CompileEntryPoint(session, module, "vertex", &programLayout);
                result.fragment = CompileEntryPoint(session, module, "fragment");

                if (programLayout) 
                {
                    SearchProperties(programLayout);
                }

                return result;
            }

            inline static ShaderBytecode CompileCompute(const char* shaderModule)
            {
                Slang::ComPtr<slang::IBlob> diagnostics;
                slang::IModule* module = session->loadModule(shaderModule, diagnostics.writeRef());
                CheckSlangDiagnostics(diagnostics.get(), "Compute Module Load");

                if (!module) {
                    throw std::runtime_error("Unable to get a valid Slang module");
                }

                ShaderBytecode result;
                slang::ProgramLayout* programLayout = nullptr;
                
                result.compute = CompileEntryPoint(session, module, "compute", &programLayout);

                if (programLayout) 
                {
                    SearchProperties(programLayout); 
                }

                return result;
            }

        private:

            inline static void SearchProperties(slang::ProgramLayout* programLayout)
            {
                slang::TypeReflection* propertiesType = programLayout->findTypeByName("Properties");

                if(propertiesType == nullptr) { return; }

                slang::TypeReflection::Kind kind = propertiesType->getKind();

                if(kind != slang::TypeReflection::Kind::Struct) { return; }

                std::cout << "Struct Materiale Trovata" << std::endl;

                slang::TypeLayoutReflection* propertiesStruct = programLayout->getTypeLayout(propertiesType);

                uint32_t fieldCount = propertiesStruct->getFieldCount();

                for(uint32_t i = 0; i < fieldCount; i++)
                {
                    slang::VariableLayoutReflection* fieldLayout = propertiesStruct->getFieldByIndex(i);
                    slang::VariableReflection* field = fieldLayout->getVariable();

                    size_t offset = fieldLayout->getOffset(slang::ParameterCategory::Uniform);


                    std::cout << field->getName() << std::endl;
                }
            }

            inline static std::vector<uint32_t> CompileEntryPoint(slang::ISession* session, slang::IModule* module, 
                const char* entryPointName, slang::ProgramLayout** outLayout = nullptr) 
            {
                Slang::ComPtr<slang::IEntryPoint> entryPoint;
                module->findEntryPointByName(entryPointName, entryPoint.writeRef());
                if (!entryPoint) {
                    throw std::runtime_error(std::string("Entry point not found: ") + entryPointName);
                }

                slang::IComponentType* components[] = { module, entryPoint.get() };
                Slang::ComPtr<slang::IComponentType> program;
                Slang::ComPtr<slang::IBlob> linkDiagnostics;

                session->createCompositeComponentType(components, 2, program.writeRef(), linkDiagnostics.writeRef());
                CheckSlangDiagnostics(linkDiagnostics.get(), std::string(entryPointName) + " Link");

                // Estrae il layout solo se richiesto (es. per il vertex)
                if (outLayout) {
                    *outLayout = program->getLayout();
                }

                Slang::ComPtr<slang::IBlob> spirvBlob;
                Slang::ComPtr<slang::IBlob> codeDiagnostics;
                
                // Slang assegna l'indice dell'entry point (0) nel programma composito appena creato
                program->getEntryPointCode(0, 0, spirvBlob.writeRef(), codeDiagnostics.writeRef());
                CheckSlangDiagnostics(codeDiagnostics.get(), std::string(entryPointName) + " CodeGen");

                if (!spirvBlob || spirvBlob->getBufferSize() == 0) {
                    throw std::runtime_error(std::string("Failed to generate SPIR-V for: ") + entryPointName);
                }

                // Estrazione pulita dei dati in un std::vector<uint32_t>
                const auto* buffer = static_cast<const uint32_t*>(spirvBlob->getBufferPointer());
                size_t wordCount = spirvBlob->getBufferSize() / sizeof(uint32_t);
                
                return std::vector<uint32_t>(buffer, buffer + wordCount);
            }

            inline static void CheckSlangDiagnostics(slang::IBlob* diagnostics, std::string_view context) 
            {
                if (diagnostics && diagnostics->getBufferSize() > 0) {
                    std::cerr << "Slang Log (" << context << "):\n" 
                            << static_cast<const char*>(diagnostics->getBufferPointer()) << "\n";
                }
            }


            inline static Slang::ComPtr<slang::IGlobalSession> globalSession;
            inline static Slang::ComPtr<slang::ISession> session;

    };

}