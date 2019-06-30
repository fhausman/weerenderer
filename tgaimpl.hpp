#pragma once
#include "img.hpp"
#include "img/tgaimage.h"

class TgaColor : public IColor
{
    TGAColor m_color;
public:
    TgaColor(const uint8_t r = 0,
        const uint8_t g = 0,
        const uint8_t b = 0,
        const uint8_t a = 255)
        : m_color(r, g, b, a)
    {}

    virtual RGBA ToRgba() const override;
};

class TgaImage : public IImg
{
    TGAImage m_image;
public:
    virtual void CreateImage(const Width width, const Height height) override;
    virtual void ReadImage(const std::filesystem::path& path_to_img) override;
    virtual void WriteImage(const std::filesystem::path& path_to_write) override;
    virtual std::tuple<Width, Height> GetImageSize() const override;
    virtual std::unique_ptr<IColor> GetPixelColor(const int32_t x, const int32_t y) const override;
    virtual void SetPixelColor(const int32_t x, const int32_t y, const float_t intensity, const IColor& color) override;
};
