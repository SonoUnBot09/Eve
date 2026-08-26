#pragma once

#include <vulkan/vulkan.hpp>
#include <vulkan/vk_enum_string_helper.h>
#include <ExecutablePath.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace Eve::Graphics
{

    static constexpr uint32_t errorCount = 22;
    inline static VkResult errors[]
    {
        VK_ERROR_OUT_OF_HOST_MEMORY,
        VK_ERROR_OUT_OF_DEVICE_MEMORY,
        VK_ERROR_INITIALIZATION_FAILED,
        VK_ERROR_DEVICE_LOST,
        VK_ERROR_MEMORY_MAP_FAILED,
        VK_ERROR_LAYER_NOT_PRESENT,
        VK_ERROR_EXTENSION_NOT_PRESENT,
        VK_ERROR_FEATURE_NOT_PRESENT,
        VK_ERROR_INCOMPATIBLE_DRIVER,
        VK_ERROR_TOO_MANY_OBJECTS,
        VK_ERROR_FORMAT_NOT_SUPPORTED,
        VK_ERROR_FRAGMENTED_POOL,
        VK_ERROR_UNKNOWN,
        VK_ERROR_VALIDATION_FAILED,
        VK_ERROR_OUT_OF_POOL_MEMORY,
        VK_ERROR_INVALID_EXTERNAL_HANDLE,
        VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS,
        VK_ERROR_FRAGMENTATION,
        VK_ERROR_NOT_PERMITTED,
        VK_ERROR_SURFACE_LOST_KHR,
        VK_ERROR_NATIVE_WINDOW_IN_USE_KHR,
        VK_ERROR_INCOMPATIBLE_DISPLAY_KHR
    };

    inline static std::string errorMessages[]
    {
        "ERROR 101: Unable to allocate memory on RAM, no space available",
        "ERROR 102: Unable to allocate memory on VRAM, no space available",
        "ERROR 103: Unable to initialize a driver object",
        "ERROR 104: GPU Driver not available",
        "ERROR 105: VRAM Mapping Failed",
        "ERROR 106: Validation Layer requested not available",
        "ERROR 107: Extension not supported",
        "ERROR 108: Feature not supported",
        "ERROR 109: Incompatible driver",
        "ERROR 110: Too many objects, driver error",
        "ERROR 111: Format not supported",
        "ERROR 112: Fragment pool memory, unable to allocate data",
        "ERROR 113: Unknown driver error",
        "ERROR 114: Invalid operation",
        "ERROR 115: Out of pool memory",
        "ERROR 116: Invalid external handle",
        "ERROR 117: Invalid requested GPU memory address",
        "ERROR 118: VRAM is fragmented, unable to allocate memory",
        "ERROR 119: Operation denied",
        "ERROR 120: Rendering surface lost",
        "ERROR 121: Window already in use",
        "ERROR 122: Incompatible display"
    };

    inline static bool FindError(VkResult error, uint32_t& errorIndex)
    {
        bool found = false;
        for(uint32_t i = 0; i < errorCount; i++)
        {
            if(errors[i] == error)
            {
                found = true;
                errorIndex = i;
                break;
            }
        }

        return found;
    }

    inline static std::string GetCurrentTimestamp() 
    {
        auto now = std::chrono::system_clock::now();
        
        return std::format("{:%Y-%m-%d %H:%M:%S}", now);
    }

    inline static void LogError(std::string message)
    {
        fs::path path = GetExecutableDirectory();
        std::string time = GetCurrentTimestamp();

        try 
        {
            fs::path logDir = GetExecutableDirectory() / "logs";
            fs::create_directories(logDir);

            std::ofstream logFile(logDir / time, std::ios::app);
            if (logFile.is_open()) {
                logFile << message << std::endl;
                logFile.close();
            }
        } 
        catch (const std::exception& e) {
            std::cerr << "Unable to write the log: " << e.what() << std::endl;
        }
    }

    inline static void HandleError(VkResult error, std::string fileName, int row)
    {
        uint32_t errorIndex;
        bool foundError = FindError(error, errorIndex);

        const char* errName = string_VkResult(error);
        std::string errorName = std::to_string(*errName);

        if(foundError)
        {
            std::string message = errorMessages[errorIndex];
            message = message + " || Vulkan error: " + errorName;

            LogError(message);
        }
        else 
        {
            std::string message = "Unknown error || Vulkan error: " + errorName;

            LogError(message);
        }
    }

    #define VK_CHECK(x) \
        VkResult res = (f); \
        if (res < 0) { \
            std::string file = __FILE__\
            int row = __LINE__\
            HandleError(error, file, row);\
        }
}