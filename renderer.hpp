#pragma once

#include <optional>
#include <variant>
#include <vector>
#include "img.hpp"
#include "hola/hola.hpp"
#include "img/tgaimage.h"

using namespace hola;

using Triangle = std::array<vec3f, 3>;
using TexCoords = const std::array<vec2f, 3>;
using Point = vec3f;

struct BoundingBox
{
    vec2f min;
    vec2f max;
};

BoundingBox calculate_bounding_box(const Triangle& triangle, const Width width, const Height height);

std::optional<vec3f> calculate_barycentric(const Point& p, const Triangle& triangle);

void draw_line(const vec2i& v0, const vec2i& v1, Image& image, const Color& color);

void draw_triangle(const Triangle& triangle, const TexCoords& texture_coords, const float_t intensity, std::vector<float>& z_buffer, Image& image, Image& texture);
