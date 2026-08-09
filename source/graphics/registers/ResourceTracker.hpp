#pragma once

#include <Eve/graphics/Texture.hpp>
#include <Eve/graphics/Buffer.hpp>
#include <graphics/Resources.hpp>

namespace Eve::Graphics
{
    class ResourceTracker
    {
        public:
            static void RegisterTextureState(TextureHandle handle);
            static void RegisterTextureState(TransientTextureHandle handle);
            static void RegisterBufferState(BufferHandle handle);
            static void RegisterBufferState(TransientBufferHandle handle);

            inline static TextureState& GetTextureState(TextureHandle handle) { return textureStates[handle.Id]; }
            inline static BufferState& GetBufferState(BufferHandle handle) { return bufferStates[handle.Id]; }
            inline static void SetTextureState(TextureHandle handle, TextureState& state) { textureStates[handle.Id] = state; }
            inline static void SetBufferState(BufferHandle handle, BufferState& state) { bufferStates[handle.Id] = state; }

        private:
            inline static std::vector<TextureState> textureStates;
            inline static std::vector<BufferState> bufferStates;
    };
}