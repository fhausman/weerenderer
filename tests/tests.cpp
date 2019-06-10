#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../renderer.hpp"
#include "../hola/hola.hpp"

SCENARIO("Bounding box calculation", "[renderer]")
{
    GIVEN("triangle inside screen")
    {
        const Triangle triangle = {{{10, 30}, {15, 40}, {20, 20}}};
        WHEN("calculating bounding box")
        {
            auto [bbmin, bbmax] =
                calculate_bounding_box(triangle, {1980, 1080});
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
            auto [bbmin, bbmax] =
                calculate_bounding_box(triangle, {1980, 1080});
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
            auto [bbmin, bbmax] =
                calculate_bounding_box(triangle, {1980, 1080});
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
            auto [bbmin, bbmax] =
                calculate_bounding_box(triangle, {1980, 1080});
            THEN("bounding box should be fullscreen")
            {
                REQUIRE(bbmin == vec{0, 0});
                REQUIRE(bbmax == vec{1980, 1080});
            }
        }
    }
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
