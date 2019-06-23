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
