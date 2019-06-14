#include "renderer.hpp"

namespace renderer_
{
struct SetPixel
{
    const int x;
    const int y;
    const Color& color;
    const float intensity = 1;

    void operator()(TGAImage& image)
    {
        image.set(x, y, std::get<TGAColor>(color));
    }
};

struct GetPixelColor
{
    const int x;
    const int y;

    Color operator()(TGAImage& image)
    {
        return image.get(x, y);
    }
};

struct ImageSize
{
    ImageResolution operator()(TGAImage& image)
    {
        return { image.get_width(), image.get_height() };
    }
};
}

BoundingBox calculate_bounding_box(const Triangle& triangle, const ImageResolution& screen)
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

bool is_inside_triangle(const vec3f& barycentric)
{
    return !(get_x(barycentric) < 0.f ||
        get_y(barycentric) < 0.f ||
        get_z(barycentric) < 0.f);
}

Color get_color(const vec3f& barycentric, const TexCoords& texture_coords, Image& texture)
{
    const auto tex_size = std::visit(renderer_::ImageSize{}, texture);
    const auto p_uv =
        texture_coords[0]*barycentric[0] +
        texture_coords[1]*barycentric[1] +
        texture_coords[2]*barycentric[2];

    const int tex_x = tex_size.width - get_x(p_uv)*tex_size.width;
    const int tex_y = tex_size.height - get_y(p_uv)*tex_size.height;
    return std::visit(renderer_::GetPixelColor{tex_x, tex_y}, texture);
}

void draw_triangle(const Triangle& triangle, const TexCoords& texture_coords, const float_t intensity, std::vector<float>& z_buffer, Image& image, Image& texture)
{
    const auto img_size = std::visit(renderer_::ImageSize{}, image);
    const auto buffer_idx = [&](const auto& x, const auto& y) {
        return size_t(x + y * img_size.width);
    };

    const auto bbox = calculate_bounding_box(
        triangle, {img_size.width, img_size.height});

    for (auto x = get_x(bbox.min); x <= get_x(bbox.max); ++x)
    {
        for(auto y = get_y(bbox.min); y <= get_y(bbox.max); ++y)
        {
            const auto barycentric = calculate_barycentric({ x,y,0.f }, triangle);
            if (barycentric)
            {
                if (!is_inside_triangle(*barycentric))
                    continue;

                float z = 0.f;
                for(size_t i = 0; i < triangle.size(); ++i)
                {
                    z += get_z(triangle[i])*barycentric->at(i);
                }
                if(z_buffer[buffer_idx(x,y)] < z)
                {
                    z_buffer[buffer_idx(x,y)] = z;
                    std::visit(renderer_::SetPixel{ x, y, get_color(*barycentric, texture_coords, texture), intensity}, image);
                }
            }
        }
    }
}


