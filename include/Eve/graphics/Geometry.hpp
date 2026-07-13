#pragma once

#include <cstdint>

namespace Eve::Graphics
{
    enum class Topology : uint16_t
    {
        TOPOLOGY_POINT_LIST = 0,
        TOPOLOGY_LINE_LIST = 1,
        TOPOLOGY_LINE_STRIP = 2,
        TOPOLOGY_TRIANGLE_LIST = 3
    };

    enum class PolygonMode : uint16_t
    {
        POLYGON_MODE_FILL = 0,
        POLYGON_MODE_LINE = 1,
        POLYGON_MODE_POINT = 2
    };

    enum class CullMode : uint16_t
    {
        CULL_MODE_NONE = 0,
        CULL_MODE_FRONT = 1,
        CULL_MODE_BACK = 2,
        CULL_MODE_FRONT_AND_BACK = 3
    };

    enum class DepthTest : uint16_t
    {
        DEPTH_COMPARE_ALWAYS = 0,
        DEPTH_COMPARE_LESS = 1,
        DEPTH_COMPARE_LESS_OR_EQUAL = 2,
        DEPTH_COMPARE_GREATER = 3,
        DEPTH_COMPARE_GREATER_OR_EQUAL = 4
    };
}