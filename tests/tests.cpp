#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../renderer.hpp"
#include "../hola/hola.hpp"

SCENARIO("Bounding box calculation", "[renderer]")
{
    Renderer renderer;
    GIVEN("triangle inside screen")
    {
        const Triangle triangle = {{{10.f, 30.f}, {15.f, 40.f}, {20.f, 20.f}}};
        WHEN("calculating bounding box")
        {
            auto [bbmin, bbmax] =
                renderer.CalculateBoundingBox(triangle, { 1920, 1080 });
            THEN("bounding box should consist furthest triangle coordinates")
            {
                REQUIRE(bbmin == vec2f{10.f, 20.f});
                REQUIRE(bbmax == vec2f{20.f, 40.f});
            }
        }
    }

    GIVEN("triangle partially outside screen")
    {
        const Triangle triangle = {{{-10.f, -30.f}, {40.f, 40.f}, {-20.f, -50.f}}};
        WHEN("calculating bounding box")
        {
            auto [bbmin, bbmax] =
                renderer.CalculateBoundingBox(triangle, { 1920, 1080 });
            THEN("bounding box should be clamped to screen resolution")
            {
                REQUIRE(bbmin == vec2f{0.f, 0.f});
                REQUIRE(bbmax == vec2f{40.f, 40.f});
            }
        }
    }

    GIVEN("triangle fully outside screen")
    {
        const Triangle triangle = {{{-10.f, -30.f}, {-40.f, -100.f}, {-20.f, -50.f}}};
        WHEN("calculating bounding box")
        {
            auto [bbmin, bbmax] =
                renderer.CalculateBoundingBox(triangle, { 1920, 1080 });
            THEN("bounding box should be zeroed")
            {
                REQUIRE(bbmin == vec2f{0.f, 0.f});
                REQUIRE(bbmax == vec2f{0.f, 0.f});
            }
        }
    }

    GIVEN("triangle on the edge of screen")
    {
        const Triangle triangle = {{{0.f, 0.f}, {1920.f, 1080.f}, {0.f, 1080.f}}};
        WHEN("calculating bounding box")
        {
            auto [bbmin, bbmax] =
                renderer.CalculateBoundingBox(triangle, { 1920, 1080 });
            THEN("bounding box should be fullscreen (max is -1)")
            {
                REQUIRE(bbmin == vec{0.f, 0.f});
                REQUIRE(bbmax == vec{1919.f, 1079.f});
            }
        }
    }
}

SCENARIO("Calculating point barycentric coordinates in relation to triangle", "[renderer]")
{
    Renderer renderer;
    GIVEN("non-degenerate triangle")
    {
        const Triangle triangle = {{{10.f,10.f}, {50.f,10.f}, {30.f,50.f}}};
        WHEN("P is point on one of the triangle vertices")
        {
            const Point p1{ 10.f,10.f };
            const Point p2{ 50.f,10.f };
            const Point p3{ 30.f,50.f };

            const auto bar1 = renderer.CalculateBarycentric(p1, triangle);
            const auto bar2 = renderer.CalculateBarycentric(p2, triangle);
            const auto bar3 = renderer.CalculateBarycentric(p3, triangle);
            THEN("barycentric of P is unit vector")
            {
                REQUIRE(bar1 == vec{ 1.f, 0.f, 0.f });
                REQUIRE(bar2 == vec{ 0.f, 1.f, 0.f });
                REQUIRE(bar3 == vec{ 0.f, 0.f, 1.f });
            }
        }

        WHEN("P is inside triangle")
        {
            const Point p{ 30.f, 10.f };

            const auto bar = *renderer.CalculateBarycentric(p, triangle);
            THEN("all barycentric coords are positive")
            {
                REQUIRE(get_x(bar) >= 0.f);
                REQUIRE(get_y(bar) >= 0.f);
                REQUIRE(get_z(bar) >= 0.f);
            }
        }

        WHEN("P is outside triangle")
        {
            const Point p{ 100.f, 100.f };

            const auto bar = *renderer.CalculateBarycentric(p, triangle);
            THEN("at least one of barycentric coords is negative")
            {
                REQUIRE((get_x(bar) < 0.f || get_y(bar) < 0.f || get_z(bar) < 0.f));
            }
        }
    }
}
