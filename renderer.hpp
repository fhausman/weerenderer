#pragma once

#include <optional>
#include <variant>
#include "hola/hola.hpp"
#include "img/tgaimage.h"

using namespace hola;

using Triangle = std::array<vec3f, 3>;
using Point = vec3f;

//other image formats may be supported in the future
using Image = std::variant<TGAImage>;
using Color = std::variant<TGAColor>;

struct ScreenResolution
{
    int width;
    int height;
};

struct BoundingBox
{
    vec2f min;
    vec2f max;
};

BoundingBox calculate_bounding_box(const Triangle& triangle, const ScreenResolution& screen);

std::optional<vec3f> calculate_barycentric(const Point& p, const Triangle& triangle);

void draw_line(const vec2i& v0, const vec2i& v1, Image& image, const Color& color);
