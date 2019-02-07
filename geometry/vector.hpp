#pragma once

#include <array>
#include <tuple>

namespace geo
{
namespace vector
{
namespace _vector
{
template <typename T, size_t SIZE>
struct VecBase
{
    using type = T;

    std::array<T, SIZE> coordinates{0};

    constexpr auto operator[](size_t const i) { return coordinates[i]; }

    constexpr auto begin() const { return coordinates.begin(); }

    constexpr auto end() const { return coordinates.end(); }
};

template <typename T, size_t SIZE = 2>
struct Vec2 : public VecBase<T, SIZE>
{
    constexpr Vec2(T const& x, T const& y) : VecBase<T, SIZE>{{x, y}} {}

    constexpr T x() { return coordinates[0]; }

    constexpr T y() { return coordinates[1]; }
};

template <typename T, size_t SIZE = 3>
struct Vec3 : public VecBase<T, SIZE>
{
    constexpr Vec3(T const& x, T const& y, T const& z)
        : VecBase<T, SIZE>{{x, y, z}}
    {
    }

    constexpr T x() { return coordinates[0]; }

    constexpr T y() { return coordinates[1]; }

    constexpr T z() { return coordinates[2]; }
};

template <typename T>
struct CoordPairIt : std::tuple<T, T>
{
    CoordPairIt(T const& it0, T const& it1) : std::tuple<T, T>{it0, it1} {};

    constexpr CoordPairIt operator++()
    {
        return {++std::get<0>(*this), ++std::get<1>(*this)};
    }
};

template <typename T>
CoordPairIt(T const&, T const&)->CoordPairIt<T>;

template <typename TVec>
struct VecPair
{
    TVec const& v0;
    TVec const& v1;

    VecPair(TVec const& v0, TVec const& v1) : v0(v0), v1(v1) {}

    constexpr auto begin() const { return CoordPairIt{v0.begin(), v1.begin()}; }

    constexpr auto end() const { return CoordPairIt{v0.end(), v1.end()}; }
};
}  // namespace _vector

template <typename T, size_t SIZE>
using Vector = _vector::VecBase<T, SIZE>;

using Vec2i = _vector::Vec2<int>;

using Vec2f = _vector::Vec2<float>;

using Vec3i = _vector::Vec3<int>;

using Vec3f = _vector::Vec3<float>;

}  // namespace vector
}  // namespace geo
