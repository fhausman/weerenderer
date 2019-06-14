#include "renderer.hpp"

namespace renderer_
{
struct SetPixel
{
    const int x;
    const int y;
    const Color& color;

    void operator()(TGAImage& image)
    {
        image.set(x, y, std::get<TGAColor>(color));
    }
};
}

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


void draw_line(const vec2i& v0, const vec2i& v1, Image& image, const Color& color)
{
    const auto dx = std::abs(get_x(v1) - get_x(v0));
    const auto dy = std::abs(get_y(v1) - get_y(v0));
    const auto steps_nr = dx >= dy ? dx : dy;

    const auto xi = get_x(v0) > get_x(v1) ? -1 : 1;
    const auto yi = get_y(v0) > get_y(v1) ? -1 : 1;

    const auto calculate_next_pixel =
        [steps_nr](const auto start, const auto difference, const auto step) {
        return static_cast<int>(
            start + step * (difference / static_cast<float>(steps_nr)));
    };

    for (int step = 0; step < steps_nr; ++step)
    {
        const auto px_x = calculate_next_pixel(get_x(v0), dx * xi, step);
        const auto px_y = calculate_next_pixel(get_y(v0), dy * yi, step);
        std::visit(renderer_::SetPixel{px_x, px_y, color}, image);
    }
}

