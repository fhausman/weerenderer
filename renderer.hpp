#pragma once

#include <optional>
#include <variant>
#include <vector>
#include "img.hpp"
#include "model.hpp"
#include "hola/hola.hpp"

using namespace hola;

using Triangle = std::array<vec3f, 3>;
using TexCoords = std::array<vec2f, 3>;
using ZBuffer = std::vector<float_t>;
using Point = vec3f;

struct BoundingBox
{
    vec2f min;
    vec2f max;
};

class Renderer
{
    vec3f m_lightVector;
    ZBuffer m_zBuffer;

public:
    void SetLightVector(const vec3f& light_vector) { m_lightVector = light_vector; }
    void RenderModel(const IModel& model, IImg& texture, IImg& out_image);
    void RenderLine(const vec2i& v0, const vec2i& v1, IImg& image, const IColor& color);
    void RenderTriangle(const Triangle& triangle,
        const TexCoords& texture_coords,
        const float_t intensity,
        IImg& out_image,
        IImg& texture);
    std::unique_ptr<IColor> GetColorFromTexture(const vec3f& barycentric,
        const TexCoords& texture_coords,
        const IImg& texture);
    std::optional<vec3f> CalculateBarycentric(const Point& p, const Triangle& triangle);
    BoundingBox CalculateBoundingBox(const Triangle& triangle, const ImageSize& size);
    float_t CalculateLightIntensity(const Triangle& triangle);
};
