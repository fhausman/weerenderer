#pragma once

#include <optional>
#include "hola/hola.hpp"

using namespace hola;

using Triangle = std::array<vec2i, 3>;
using Point = vec2i;

struct ScreenResolution
{
    int width;
    int height;
};

struct BoundingBox
{
    vec2i min;
    vec2i max;
};

BoundingBox calculate_bounding_box(const Triangle& triangle, const ScreenResolution& screen);

std::optional<vec3f> calculate_barycentric(const Point& p, const Triangle& triangle);
