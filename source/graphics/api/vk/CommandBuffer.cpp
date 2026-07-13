#include <Eve/graphics/CommandBuffer.hpp>
#include <graphics/api/vk/ResourceMapper.hpp>

using namespace Eve::Graphics;

void CommandBufferHandle::MapImage(ImageHandle handle)
{
    ResourceMapper::ScheduleImageMapping(handle);
}

void CommandBufferHandle::MapSampler(SamplerHandle handle)
{
    ResourceMapper::ScheduleSamplerMapping(handle);
}

void CommandBufferHandle::MapBuffer(BufferHandle handle)
{
    ResourceMapper::ScheduleBufferMapping(handle);
}