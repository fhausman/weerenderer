#include "tgaimpl.hpp"

RGBA TgaColor::ToRgba() const
{
    return { m_color.bgra[0], m_color.bgra[1], m_color.bgra[2], m_color.bgra[3] };
}

void TgaImage::CreateImage(const Width width, const Height height)
{
    m_image = TGAImage{ static_cast<int>(width), static_cast<int>(height), TGAImage::RGB };
}

void TgaImage::ReadImage(const std::filesystem::path& path_to_img)
{
    if (path_to_img.extension() != ".tga")
        throw std::runtime_error("Invalid file provided");

    m_image.read_tga_file(path_to_img.string().c_str());
}

void TgaImage::WriteImage(const std::filesystem::path& path_to_write)
{
    m_image.flip_vertically();
    if(!m_image.write_tga_file(path_to_write.string().c_str()))
        throw std::runtime_error("Couldn't save file");
}

std::tuple<Width, Height> TgaImage::GetImageSize() const
{
    return { m_image.get_width(), m_image.get_height() };
}

std::unique_ptr<IColor> TgaImage::GetPixelColor(const int32_t x, const int32_t y) const
{
    const auto color = m_image.get(x, y);
    return std::make_unique<TgaColor>(
        color.bgra[0], color.bgra[1], color.bgra[2], color.bgra[3]);
}

void TgaImage::SetPixelColor(const int32_t x, const int32_t y, const float_t intensity, const IColor& color)
{
    const auto rgba = color.ToRgba();
    m_image.set(x, y, TGAColor{ rgba.r, rgba.g, rgba.b, rgba.a }*intensity);
}
