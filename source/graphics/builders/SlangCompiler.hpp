#pragma once

#include <slang.h>
#include <slang-com-ptr.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <ExecutablePath.hpp>
#include <memory>

namespace Eve::Graphics
{

    struct ShaderBytecode
    {
        std::vector<uint32_t> vertex;
        std::vector<uint32_t> fragment;
        std::vector<uint32_t> compute;
    };

    struct MaterialProperties
    {
        std::vector<std::string> names;
        std::vector<size_t> offsets;
    };

    struct Shader
    {
        ShaderBytecode bytecode;
        MaterialProperties properties;
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

               slang::CompilerOptionEntry entry{};
                entry.name = slang::CompilerOptionName::DownstreamArgs;
                entry.value.kind = slang::CompilerOptionValueKind::String;
                entry.value.stringValue0 = "-fvk-use-scalar-layout";

                slang::TargetDesc targetDesc = {};
                targetDesc.format = SLANG_SPIRV;
                targetDesc.profile = globalSession->findProfile("spirv_1_5");
                targetDesc.compilerOptionEntryCount = 1;
                targetDesc.compilerOptionEntries = &entry;

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

            inline static Shader CompileVertFrag(const char* shaderModule)
            {
                Slang::ComPtr<slang::IBlob> diagnostics;
                slang::IModule* module = session->loadModule(shaderModule, diagnostics.writeRef());
                CheckSlangDiagnostics(diagnostics.get(), "Module Load");

                if (!module) {
                    throw std::runtime_error("Unable to get a valid Slang module");
                }

                ShaderBytecode result;
                Slang::ComPtr<slang::IComponentType> program;

                result.vertex = CompileEntryPoint(session, module, "vertex", std::addressof(program));
                result.fragment = CompileEntryPoint(session, module, "fragment");

                MaterialProperties properties;
                if (program) 
                {
                    properties = SearchProperties(program->getLayout());
                }

                return Shader{result, properties};
            }

            inline static Shader CompileCompute(const char* shaderModule)
            {
                Slang::ComPtr<slang::IBlob> diagnostics;
                slang::IModule* module = session->loadModule(shaderModule, diagnostics.writeRef());
                CheckSlangDiagnostics(diagnostics.get(), "Compute Module Load");

                if (!module) 
                {
                    throw std::runtime_error("Unable to get a valid Slang module");
                }

                ShaderBytecode result;
                Slang::ComPtr<slang::IComponentType> program;
                
                result.compute = CompileEntryPoint(session, module, "compute", std::addressof(program));

                MaterialProperties properties;
                if (program) 
                {
                    properties = SearchProperties(program->getLayout()); 
                }

                return Shader{result, properties};
            }

        private:

            inline static MaterialProperties SearchProperties(slang::ProgramLayout* programLayout)
            {
                MaterialProperties properties{};

                slang::TypeReflection* propertiesType = programLayout->findTypeByName("Properties");
                if(propertiesType == nullptr) { return properties; }
                if(propertiesType->getKind() != slang::TypeReflection::Kind::Struct) { return properties; }

                slang::TypeLayoutReflection* propertiesStruct = programLayout->getTypeLayout(propertiesType);
                
                if (propertiesStruct == nullptr) 
                { 
                    return properties; 
                }

                uint32_t fieldCount = propertiesStruct->getFieldCount();

                uint32_t offset = 0;
                for(uint32_t i = 0; i < fieldCount; i++)
                {
                    slang::VariableLayoutReflection* fieldLayout = propertiesStruct->getFieldByIndex(i);
                    slang::VariableReflection* field = fieldLayout->getVariable();

                    slang::TypeLayoutReflection* typeLayout = fieldLayout->getTypeLayout();
                    
                    uint32_t size = typeLayout->getSize();
                    uint32_t alignment = CalculateAlignment(typeLayout);

                    offset = std::ceil(offset / alignment) * alignment;

                    if(offset + size > 16 * 1024)
                    {
                        std::cout << "WARNING: Material properties struct exceed the 16 KB limit, not all field will be mapped." << std::endl;
                        return properties;
                    }

                    const char* name = field->getName();
            
                    properties.names.push_back(name);
                    properties.offsets.push_back(offset);

                    offset += size;
                }

                return properties;
            }

            inline static std::vector<uint32_t> CompileEntryPoint(
                slang::ISession* session, slang::IModule* module, 
                const char* entryPointName, Slang::ComPtr<slang::IComponentType>* outProgram = nullptr) 
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

                Slang::ComPtr<slang::IBlob> spirvBlob;
                Slang::ComPtr<slang::IBlob> codeDiagnostics;
                
                program->getEntryPointCode(0, 0, spirvBlob.writeRef(), codeDiagnostics.writeRef());
                CheckSlangDiagnostics(codeDiagnostics.get(), std::string(entryPointName) + " CodeGen");

                if (!spirvBlob || spirvBlob->getBufferSize() == 0) {
                    throw std::runtime_error(std::string("Failed to generate SPIR-V for: ") + entryPointName);
                }

                if (outProgram) {
                    *outProgram = program;
                }

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

            inline static uint32_t CalculateAlignment(slang::TypeLayoutReflection* typeLayout)
            {
                slang::TypeReflection* type = typeLayout->getType();

                switch (type->getScalarType()) 
                {
                    case slang::TypeReflection::ScalarType::Int8 :
                    case slang::TypeReflection::ScalarType::UInt8 :
                    case slang::TypeReflection::ScalarType::Bool :
                        return 1;
                    case slang::TypeReflection::ScalarType::Int16 :
                    case slang::TypeReflection::ScalarType::UInt16 :
                    case slang::TypeReflection::ScalarType::Float16 :
                        return 2;
                    case slang::TypeReflection::ScalarType::Int32 :
                    case slang::TypeReflection::ScalarType::UInt32 :
                    case slang::TypeReflection::ScalarType::Float32 :
                        return 4;
                    case slang::TypeReflection::ScalarType::Int64 :
                    case slang::TypeReflection::ScalarType::UInt64 :
                    case slang::TypeReflection::ScalarType::Float64 :
                        return 8;
                    default:
                        return 4;
                }
            }


            inline static Slang::ComPtr<slang::IGlobalSession> globalSession;
            inline static Slang::ComPtr<slang::ISession> session;

    };

}