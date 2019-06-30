#pragma once

#include "img/tgaimage.h"
#include <variant>
#include <tuple>
#include <filesystem>

//other image formats may be supported in the future
using Image = std::variant<TGAImage>;
using Color = std::variant<TGAColor>;

//todo: would be perfect to change to strong types
using Width = size_t;
using Height = size_t;

std::tuple<Width, Height> get_image_size(Image& image);

void set_pixel(Image& image, const int32_t x, const int32_t y, const float_t intensity, const Color& color);

Color get_pixel(Image& image, const int32_t x, const int32_t y);

Image read_image(const std::filesystem::path& path_to_img);

void write_image(Image& image, const std::filesystem::path& path_to_write);

struct RGBA
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct IColor
{
    virtual RGBA ToRgba() const = 0;
    virtual ~IColor() = default;
};

struct IImg
{
    virtual void CreateImage(const Width width, const Height height) = 0;
    virtual void ReadImage(const std::filesystem::path& path_to_img) = 0;
    virtual void WriteImage(const std::filesystem::path& path_to_write) = 0;
    virtual std::tuple<Width, Height> GetImageSize() const = 0;
    virtual std::unique_ptr<IColor> GetPixelColor(const int32_t x, const int32_t y) const = 0;
    virtual void SetPixelColor(const int32_t x, const int32_t y, const float_t intensity, const IColor& color) = 0;
    virtual ~IImg() = default;
};
