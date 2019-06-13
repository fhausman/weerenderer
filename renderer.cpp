#include "renderer.hpp"

BoundingBox calculate_bounding_box(const Triangle& triangle, const ScreenResolution& screen)
{
    vec2f bbmax{ 0.f, 0.f };
    vec2f bbmin{ static_cast<float>(screen.width - 1), static_cast<float>(screen.height - 1) };
    vec2f clamp{ static_cast<float>(screen.width - 1), static_cast<float>(screen.height - 1) };

    for (const auto& vertex : triangle)
    {
        for (size_t i = 0; i < vec2f::size; ++i)
        {
            bbmin[i] = std::max(0.f, std::min(bbmin[i], vertex[i]));
            bbmax[i] = std::min(clamp[i], std::max(bbmax[i], vertex[i]));
        }
    }

    return { bbmin, bbmax };
}

std::optional<vec3f> calculate_barycentric(const Point& p, const Triangle& triangle)
{
    const auto is_degenerate = [](const auto& u) { return std::abs(get_z(u)) < 0; };

    const auto&[a, b, c] = triangle;
    const vec3f x_part = convert_to<vec3f>(vec3f{ get_x(c) - get_x(a), get_x(b) - get_x(a), get_x(a) - get_x(p) });
    const vec3f y_part = convert_to<vec3f>(vec3f{ get_y(c) - get_y(a), get_y(b) - get_y(a), get_y(a) - get_y(p) });
    const auto u = cross(x_part, y_part);

    if (is_degenerate(u))
    {
        return std::nullopt;
    }

    return vec3f{ 1.f - (get_x(u) + (get_y(u))) / get_z(u), get_y(u) / get_z(u), get_x(u) / get_z(u) };
}
