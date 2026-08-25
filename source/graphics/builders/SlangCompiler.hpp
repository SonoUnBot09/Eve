#pragma once

#include <slang.h>
#include <slang-com-ptr.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <filesystem>

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

            inline static void Initialize()
            {
                if (SLANG_FAILED(slang::createGlobalSession(globalSession.writeRef()))) {
                    throw std::runtime_error("Errore GlobalSession");
                }

                slang::TargetDesc targetDesc = {};
                targetDesc.format = SLANG_SPIRV;
                targetDesc.profile = globalSession->findProfile("spirv_1_6");

                const char* searchPaths[]
                {
                    "C:/Users/sonou/Desktop/LearnVulkan/Eve/source/shaders"
                };

                slang::SessionDesc sessionDesc = {};
                sessionDesc.targets = &targetDesc;
                sessionDesc.targetCount = 1;
                sessionDesc.searchPaths = searchPaths;
                sessionDesc.searchPathCount = 1;

                if (SLANG_FAILED(globalSession->createSession(sessionDesc, session.writeRef()))) {
                    throw std::runtime_error("Errore Session");
                }
            }

            inline static ShaderBytecode CompileVertFrag(const char* shaderModule)
            {
                Slang::ComPtr<slang::IBlob> diagnostics;
                
                slang::IModule* module = session->loadModule(shaderModule, diagnostics.writeRef());

                if (diagnostics) {
                    std::cerr << "Slang Log (Module):\n" << (const char*)diagnostics->getBufferPointer() << "\n";
                }
                if (!module) {
                    throw std::runtime_error("Unable to get a valid Slang module");
                }

                ShaderBytecode result;

                // --- Compile Vertex ---
                {
                    Slang::ComPtr<slang::IEntryPoint> entryPoint;
                    module->findEntryPointByName("vertex", entryPoint.writeRef());
                    
                    slang::IComponentType* components[] = { module, entryPoint };
                    Slang::ComPtr<slang::IComponentType> program;
                    
                    Slang::ComPtr<slang::IBlob> linkDiagnostics;
                    session->createCompositeComponentType(components, 2, program.writeRef(), linkDiagnostics.writeRef());
                    if (linkDiagnostics) {
                        std::cerr << "Slang Log (Vertex Link):\n" << (const char*)linkDiagnostics->getBufferPointer() << "\n";
                    }

                    Slang::ComPtr<slang::IBlob> spirvBlob;
                    Slang::ComPtr<slang::IBlob> codeDiagnostics;
                    
                    program->getEntryPointCode(0, 0, spirvBlob.writeRef(), codeDiagnostics.writeRef());
                    if (codeDiagnostics) {
                        std::cerr << "Slang Log (Vertex CodeGen):\n" << (const char*)codeDiagnostics->getBufferPointer() << "\n";
                    }

                    if (!spirvBlob) {
                        throw std::runtime_error("Failed to generate SPIR-V for vertex shader.");
                    }

                    const uint32_t* buffer = (const uint32_t*)spirvBlob->getBufferPointer();
                    size_t wordCount = spirvBlob->getBufferSize() / sizeof(uint32_t);
                    result.vertex.assign(buffer, buffer + wordCount);
                }

                // --- Compile Fragment ---
                {
                    Slang::ComPtr<slang::IEntryPoint> entryPoint;
                    module->findEntryPointByName("fragment", entryPoint.writeRef());
                    
                    slang::IComponentType* components[] = { module, entryPoint };
                    Slang::ComPtr<slang::IComponentType> program;
                    
                    Slang::ComPtr<slang::IBlob> linkDiagnostics;
                    session->createCompositeComponentType(components, 2, program.writeRef(), linkDiagnostics.writeRef());
                    if (linkDiagnostics) {
                        std::cerr << "Slang Log (Fragment Link):\n" << (const char*)linkDiagnostics->getBufferPointer() << "\n";
                    }

                    Slang::ComPtr<slang::IBlob> spirvBlob;
                    Slang::ComPtr<slang::IBlob> codeDiagnostics;
                    
                    program->getEntryPointCode(0, 0, spirvBlob.writeRef(), codeDiagnostics.writeRef());
                    if (codeDiagnostics) {
                        std::cerr << "Slang Log (Fragment CodeGen):\n" << (const char*)codeDiagnostics->getBufferPointer() << "\n";
                    }

                    if (!spirvBlob) {
                        throw std::runtime_error("Failed to generate SPIR-V for fragment shader.");
                    }

                    const uint32_t* buffer = (const uint32_t*)spirvBlob->getBufferPointer();
                    size_t wordCount = spirvBlob->getBufferSize() / sizeof(uint32_t);
                    result.fragment.assign(buffer, buffer + wordCount);
                }

                return result;
            }

            inline static ShaderBytecode CompileCompute(const char* shaderModule)
            {
                Slang::ComPtr<slang::IBlob> diagnostics;
                
                slang::IModule* module = session->loadModule(shaderModule, diagnostics.writeRef());

                if (diagnostics) {
                    std::cerr << "Slang Log:\n" << (const char*)diagnostics->getBufferPointer() << "\n";
                }
                if (!module) {
                    throw std::runtime_error("Unable to get a valid Slang module");
                }

                ShaderBytecode result;

                // --- Compile Compute ---
                {
                    Slang::ComPtr<slang::IEntryPoint> entryPoint;
                    module->findEntryPointByName("compute", entryPoint.writeRef());
                    
                    slang::IComponentType* components[] = { module, entryPoint };
                    Slang::ComPtr<slang::IComponentType> program;
                    session->createCompositeComponentType(components, 2, program.writeRef(), nullptr);

                    Slang::ComPtr<slang::IBlob> spirvBlob;
                    program->getEntryPointCode(0, 0, spirvBlob.writeRef(), nullptr);

                    const uint32_t* buffer = (const uint32_t*)spirvBlob->getBufferPointer();
                    size_t wordCount = spirvBlob->getBufferSize() / sizeof(uint32_t);
                    result.compute.assign(buffer, buffer + wordCount);
                }

                return result;
            }

        private:

            inline static Slang::ComPtr<slang::IGlobalSession> globalSession;
            inline static Slang::ComPtr<slang::ISession> session;

    };

}