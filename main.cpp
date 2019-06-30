#include "renderer.hpp"
#include "img.hpp"
#include "tgaimpl.hpp"
#include "model.hpp"
#include "objimpl.hpp"
#include "hola/hola.hpp"
#include "Clara/include/clara.hpp"

#include <iostream>

using ImgPtr = std::unique_ptr<IImg>;
using ColorPtr = std::unique_ptr<IColor>;
using ModelPtr = std::unique_ptr<IModel>;

struct Config
{
    bool help = false;
    std::string output_filename;
    std::string texture_filename;
    std::string model_filename;
    uint32_t width;
    uint32_t height;
};

Config ParseCmdline(int argc, const char* argv[])
{
    using namespace clara;

    Config config;
    auto cli =  Opt(config.output_filename, "output file").required()
                    ["-o"]
                    ("Path to output file") |
                Opt(config.texture_filename, "texture file").required()
                    ["-t"]
                    ("Path to texture file") |
                Opt(config.model_filename, "model file").required()
                    ["-m"]
                    ("Path to model file") |
                Opt(config.width, "width").required()
                    ["-w"]
                    ("Width of output file") |
                Opt(config.height, "height").required()
                    ["-h"]
                    ("Height of output file") |
                Opt(config.help)
                    ["-?"]["--help"]
                    ("Displays help");
    
    auto result = cli.parse(Args(argc, argv));
    if (!result)
    {
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
        std::exit(-1);
    }

    if (config.help)
    {
        std::cerr << cli << std::endl;
        std::exit(-1);
    }

    return config;
}

int main(int argc, const char* argv[])
{
    Config config = ParseCmdline(argc, argv);

    ImgPtr out_image = std::make_unique<TgaImage>();
    out_image->CreateImage(config.width, config.height);

    ImgPtr texture = std::make_unique<TgaImage>();
    texture->ReadImage(config.texture_filename);

    ModelPtr model = std::make_unique<Obj>();
    model->ReadModel(config.model_filename);

    Renderer renderer;
    renderer.SetLightVector({ 0,0,-1 });
    renderer.RenderModel(*model, *texture, *out_image);

    out_image->WriteImage(config.output_filename);

    return 0;
}
