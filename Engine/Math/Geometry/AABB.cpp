#include "AABB.hpp"

namespace Math
{
    Vector3 AABB::Center() const noexcept
    {
        return (Min + Max) / 2.0f;
    }

    Vector3 AABB::Extents() const noexcept
    {
        return (Max - Min) / 2.0f;
    }
}