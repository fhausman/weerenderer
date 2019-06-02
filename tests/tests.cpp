#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../hola/hola.hpp"

using namespace hola;

using BoundingBox = std::tuple<vec2i, vec2i>;

struct ScreenResolution
{
    int width;
    int height;
};

BoundingBox get_bounding_box(
    const std::array<vec2i, 3>& triangle, const ScreenResolution& screen)
{
    vec bbmax{0, 0};
    vec bbmin{screen.width, screen.height};
    vec clamp{screen.width, screen.height};

    for (const auto& vertex : triangle)
    {
        for (size_t i = 0; i < vertex.size(); ++i)
        {
            bbmin[i] = std::max(0, std::min(bbmin[i], vertex[i]));
            bbmax[i] = std::min(clamp[i], std::max(bbmax[i], vertex[i]));
        }
    }

    return {bbmin, bbmax};
}

TEST_CASE("Calculating bounding box", "[bounding box]")
{
    std::array<vec2i, 3> triangle = {{{10, 10}, {30, 30}, {20, 40}}};
    const auto bb = get_bounding_box(triangle, {1920, 1080});

    REQUIRE(BoundingBox{{10, 10}, {30, 30}} == bb);
}
