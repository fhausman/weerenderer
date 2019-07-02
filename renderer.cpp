#include "renderer.hpp"
#include <algorithm>

float_t Renderer::CalculateLightIntensity(const Triangle& triangle)
{
    const auto&[v0, v1, v2] = triangle;
    const auto n = cross(v2 - v0, v1 - v0);
    const auto normalized = normalize(n);

    return dot(m_lightVector, normalized);
}

BoundingBox Renderer::CalculateBoundingBox(const Triangle& triangle, const ImageSize& size)
{
    const auto&[width, height] = size;

    vec2f bbmax{ 0.f, 0.f };
    vec2f bbmin{ static_cast<float>(width - 1), static_cast<float>(height - 1) };
    vec2f clamp{ static_cast<float>(width - 1), static_cast<float>(height - 1) };

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

std::optional<vec3f> Renderer::CalculateBarycentric(const Point& p, const Triangle& triangle)
{
    const auto is_degenerate = [](const auto& u) { return std::abs(get_z(u)) < 1.f; };

    const auto&[a, b, c] = triangle;
    const vec3f x_part = vec3f{ get_x(c) - get_x(a), get_x(b) - get_x(a), get_x(a) - get_x(p) };
    const vec3f y_part = vec3f{ get_y(c) - get_y(a), get_y(b) - get_y(a), get_y(a) - get_y(p) };
    const auto u = cross(x_part, y_part);

    if (is_degenerate(u))
    {
        return std::nullopt;
    }

    return vec3f{ 1.f - (get_x(u) + (get_y(u))) / get_z(u), get_y(u) / get_z(u), get_x(u) / get_z(u) };
}

std::unique_ptr<IColor> Renderer::GetColorFromTexture(const vec3f & barycentric, const TexCoords & texture_coords, const IImg & texture)
{
    const auto[width, height] = texture.GetImageSize();
    const auto p_uv =
        texture_coords[0] * barycentric[0] +
        texture_coords[1] * barycentric[1] +
        texture_coords[2] * barycentric[2];

    const int tex_x = static_cast<int>(width - get_x(p_uv)*width);
    const int tex_y = static_cast<int>(height - get_y(p_uv)*height);
    return texture.GetPixelColor(tex_x, tex_y);
}

void Renderer::RenderModel(const IModel& model, IImg& texture, IImg& out_image)
{
    const auto[width, height] = out_image.GetImageSize();
    const auto triangle_to_screen_coords =
        [width, height](const auto&... v) -> Triangle {
        const auto calc_img_coord = [](const auto obj_coord, const auto image_dimension) {
            return static_cast<int>((obj_coord + 1.f) * image_dimension / 2.f + .5f);
        };

        return {
            vec3f{ static_cast<float>(calc_img_coord(get_x(v), width)),
                   static_cast<float>(calc_img_coord(get_y(v), height)),
                   get_z(v) }...
        };
    };

    m_zBuffer.resize(width*height);
    std::fill(m_zBuffer.begin(), m_zBuffer.end(), -std::numeric_limits<float_t>::max());
    while (const auto shape = model.GetNextShape())
    {
        while (const auto& polygon = shape->GetNextPolygon())
        {
            const auto&[v0, v1, v2] = polygon->vertices;
            const auto&[t0, t1, t2] = polygon->textureCoordinates;

            const auto intensity = CalculateLightIntensity({ v0, v1, v2 });
            if (intensity > 0)
            {
                RenderTriangle(triangle_to_screen_coords(v0, v1, v2),
                    { t0, t1, t2 }, intensity, out_image, texture);
            }
        }
    }
}

void Renderer::RenderLine(const vec2i& v0, const vec2i& v1, IImg& image, const IColor& color)
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
        image.SetPixelColor(px_x, px_y, 1.0f, color);
    }
}

void Renderer::RenderTriangle(const Triangle & triangle, const TexCoords & texture_coords, const float_t intensity, IImg & out_image, IImg & texture)
{
    const auto size = out_image.GetImageSize();
    const auto[width, height] = size;
    const auto buffer_idx = [&](const auto& x, const auto& y) {
        return size_t(x + y * width);
    };

    const auto is_inside_triangle = [](const auto& barycentric)
    {
        return !(get_x(barycentric) < 0.f ||
            get_y(barycentric) < 0.f ||
            get_z(barycentric) < 0.f);
    };

    const auto bbox = CalculateBoundingBox(triangle, size);
    for (auto x = get_x(bbox.min); x <= get_x(bbox.max); ++x)
    {
        for (auto y = get_y(bbox.min); y <= get_y(bbox.max); ++y)
        {
            const auto barycentric = CalculateBarycentric({ x,y,0.f }, triangle);
            if (barycentric)
            {
                if (!is_inside_triangle(*barycentric))
                    continue;

                float z = 0.f;
                for (size_t i = 0; i < triangle.size(); ++i)
                {
                    z += get_z(triangle[i])*barycentric->at(i);
                }
                if (m_zBuffer[buffer_idx(x, y)] < z)
                {
                    m_zBuffer[buffer_idx(x, y)] = z;
                    out_image.SetPixelColor(
                        static_cast<int32_t>(x),
                        static_cast<int32_t>(y),
                        intensity,
                        *GetColorFromTexture(*barycentric, texture_coords, texture));
                }
            }
        }
    }
}

