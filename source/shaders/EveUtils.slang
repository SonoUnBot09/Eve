module eve_utils;

[vk::binding(0, 0)]
internal Texture2D globalTextures[];

[vk::binding(1, 0)]
internal SamplerState globalSamplers[];

[vk::binding(2, 0)]
internal StructuredBuffer<uint64_t> globalBufferAddresses;


public Texture2D GetTexture(uint index)
{
    return globalTextures[index];
}

public SamplerState GetSampler(uint index)
{
    return globalSamplers[index];
}

public T* GetBuffer<T>(uint slotIndex)
{
    return (T*)globalBufferAddresses[slotIndex];
}