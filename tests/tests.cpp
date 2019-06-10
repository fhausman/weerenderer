#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <optional>
#include "../hola/hola.hpp"

using namespace hola;

using BoundingBox = std::tuple<vec2i, vec2i>;
using Triangle = std::array<vec2i, 3>;
using Point = vec2i;

struct ScreenResolution
{
    int width;
    int height;
};

BoundingBox get_bounding_box(
    const Triangle& triangle, const ScreenResolution& screen)
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

SCENARIO("Bounding box calculation", "[renderer]")
{
    GIVEN("triangle inside screen")
    {
        const Triangle triangle = {{{10, 30}, {15, 40}, {20, 20}}};
        WHEN("calculating bounding box")
        {
            const auto [bbmin, bbmax] =
                get_bounding_box(triangle, {1980, 1080});
            THEN("bounding box should consist furthest triangle coordinates")
            {
                REQUIRE(bbmin == vec{10, 20});
                REQUIRE(bbmax == vec{20, 40});
            }
        }
    }

    GIVEN("triangle partially outside screen")
    {
        const Triangle triangle = {{{-10, -30}, {40, 40}, {-20, -50}}};
        WHEN("calculating bounding box")
        {
            const auto [bbmin, bbmax] =
                get_bounding_box(triangle, {1980, 1080});
            THEN("bounding box should be clamped to screen resolution")
            {
                REQUIRE(bbmin == vec{0, 0});
                REQUIRE(bbmax == vec{40, 40});
            }
        }
    }

    GIVEN("triangle fully outside screen")
    {
        const Triangle triangle = {{{-10, -30}, {-40, -100}, {-20, -50}}};
        WHEN("calculating bounding box")
        {
            const auto [bbmin, bbmax] =
                get_bounding_box(triangle, {1980, 1080});
            THEN("bounding box should be zeroed")
            {
                REQUIRE(bbmin == vec{0, 0});
                REQUIRE(bbmax == vec{0, 0});
            }
        }
    }

    GIVEN("triangle on the edge of screen")
    {
        const Triangle triangle = {{{0, 0}, {1980, 1080}, {0, 1080}}};
        WHEN("calculating bounding box")
        {
            const auto [bbmin, bbmax] =
                get_bounding_box(triangle, {1980, 1080});
            THEN("bounding box should be fullscreen")
            {
                REQUIRE(bbmin == vec{0, 0});
                REQUIRE(bbmax == vec{1980, 1080});
            }
        }
    }
}

std::optional<vec3f> calculate_barycentric(
    const Point& p, const Triangle& triangle)
{
    const auto is_degenerate = [](const auto& u) { return std::abs(get_z(u)) < 0; };

    const auto& [a, b, c] = triangle;
    const vec3f x_part = convert_to<vec3f>(vec3i{ get_x(c) - get_x(a), get_x(b) - get_x(a), get_x(a) - get_x(p) });
    const vec3f y_part = convert_to<vec3f>(vec3i{ get_y(c) - get_y(a), get_y(b) - get_y(a), get_y(a) - get_y(p) });
    const auto u = cross(x_part, y_part);

    if (is_degenerate(u))
    {
        return std::nullopt;
    }

    return vec3f{1.f - (get_x(u) + (get_y(u)))/get_z(u), get_y(u)/get_z(u), get_x(u)/get_z(u)};
}

bool is_inside_triangle(const Point& p, const Triangle& triangle)
{
    if (auto barycentric = calculate_barycentric(p, triangle); barycentric)
    {
        return get_x(*barycentric) < 0.f ||
            get_y(*barycentric) < 0.f ||
            get_z(*barycentric) < 0.f;
    }

    return false;
}

SCENARIO("Calculating point barycentric coordinates in relation to triangle", "[renderer]")
{
    GIVEN("non-degenerate triangle")
    {
        const Triangle triangle = {{{10,10}, {50,10}, {30,50}}};
        WHEN("P is point on one of the triangle vertices")
        {
            const Point p1{ 10,10 };
            const Point p2{ 50,10 };
            const Point p3{ 30,50 };

            const auto bar1 = calculate_barycentric(p1, triangle);
            const auto bar2 = calculate_barycentric(p2, triangle);
            const auto bar3 = calculate_barycentric(p3, triangle);
            THEN("barycentric of P is unit vector")
            {
                REQUIRE(bar1 == vec{ 1.f, 0.f, 0.f });
                REQUIRE(bar2 == vec{ 0.f, 1.f, 0.f });
                REQUIRE(bar3 == vec{ 0.f, 0.f, 1.f });
            }
        }

        WHEN("P is inside triangle")
        {
            const Point p{ 30, 10 };

            const auto bar = *calculate_barycentric(p, triangle);
            THEN("all barycentric coords are positive")
            {
                REQUIRE(get_x(bar) >= 0.f);
                REQUIRE(get_y(bar) >= 0.f);
                REQUIRE(get_z(bar) >= 0.f);
            }
        }

        WHEN("P is outside triangle")
        {
            const Point p{ 100, 100 };

            const auto bar = *calculate_barycentric(p, triangle);
            THEN("one of barycentric coords is negative")
            {
                REQUIRE((get_x(bar) < 0.f || get_y(bar) < 0.f || get_z(bar) < 0.f));
            }
        }
    }
}
