#include "renderer.hpp"
#include "img.hpp"
#include "tgaimpl.hpp"
#include "model.hpp"
#include "objimpl.hpp"
#include "hola/hola.hpp"

#include <iostream>

using namespace hola;

using ImgPtr = std::unique_ptr<IImg>;
using ColorPtr = std::unique_ptr<IColor>;
using ModelPtr = std::unique_ptr<IModel>;

int main(int argc, const char* argv[])
{
    ImgPtr out_image = std::make_unique<TgaImage>();
    out_image->CreateImage(1600, 1600);

    ImgPtr texture = std::make_unique<TgaImage>();
    texture->ReadImage("african_head_diffuse.tga");

    ModelPtr model = std::make_unique<Obj>();
    model->ReadModel("head.obj");

    Renderer renderer;
    renderer.SetLightVector({ 0,0,-1 });
    renderer.RenderModel(*model, *texture, *out_image);

    out_image->WriteImage("output.tga");

    return 0;
}
