#include "obj.hpp"

#include <fstream>
#include <string>
#include <charconv>
#include <variant>
#include <tuple>
#include <algorithm>

namespace _obj
{
auto remove_found = [](const auto str, const auto delimiter_index) {
    return str.substr(delimiter_index + 1, str.length());
};

std::vector<std::string_view> Split(std::string_view str, const char delimiter)
{
    std::vector<std::string_view> tokens;

    auto delimiter_index = str.find(delimiter);
    while (delimiter_index != std::string_view::npos)
    {
        tokens.emplace_back(str.data(), delimiter_index);
        str = remove_found(str, delimiter_index);
        delimiter_index = str.find(delimiter);
    }
    tokens.push_back(str);

    return tokens;
}

auto ParseLine(const std::string_view str)
{
    const auto delimiter = ' ';
    auto get_identifier = [delimiter](auto str) {
        auto delimiter_index = str.find(delimiter);
        return std::tuple(std::string_view{str.data(), delimiter_index},
            remove_found(str, delimiter_index));
    };

    auto [identifier, rest] = get_identifier(str);
    auto tokens = Split(rest, delimiter);
    return std::tuple(identifier, std::move(tokens));
}

template <typename ToType, typename StrType>
ToType StringTo(const StrType& str)
{
    ToType result;
    std::from_chars(str.data(), str.data() + str.size(), result);

    return result;
}

template <typename T, typename... Args>
T Create(const Args&... arg)
{
    return T{StringTo<T::data_type>(arg)...};
}

using ParseResultVariant = std::variant<std::monostate, VertexIndices,
    VertexNormals, VertexTextureCoordinates, FaceElements>;

ParseResultVariant CreateObjField(const std::string_view& identifier,
    const std::vector<std::string_view>& tokens)
{
    if (identifier == "v")
    {
        enum coords : size_t
        {
            x,
            y,
            z,
            w
        };

        auto has_specified_w = [&tokens]() { return tokens.size() == 4; };
        auto vertex_indices = Create<VertexIndices>(
            tokens[coords::x], tokens[coords::y], tokens[coords::z]);

        if (has_specified_w())
            vertex_indices.w = StringTo<float>(tokens[coords::w]);

        return vertex_indices;
    }

    if (identifier == "vt")
    {
        enum coords : size_t
        {
            u,
            v,
            w
        };

        auto has_specified_v = [&tokens]() { return tokens.size() > 1; };
        auto has_specified_w = [&tokens]() { return tokens.size() > 2; };

        auto texture_coordinates =
            Create<VertexTextureCoordinates>(tokens[coords::u]);
        if (has_specified_v())
        {
            texture_coordinates.v = StringTo<float>(tokens[coords::v]);
            if (has_specified_w())
            {
                texture_coordinates.w = StringTo<float>(tokens[coords::w]);
            }
        }

        return texture_coordinates;
    }

    if (identifier == "vn")
    {
        enum coords : size_t
        {
            x,
            y,
            z,
        };

        return Create<VertexNormals>(
            tokens[coords::x], tokens[coords::y], tokens[coords::z]);
    }

    if (identifier == "f")
    {
        enum coords : size_t
        {
            v,
            vt,
            vn
        };

        using data_type = FaceElementCoords::data_type;
        using data_type_opt = FaceElementCoords::data_type_opt;

        FaceElements face_elements;

        auto convert_opt = [](const auto& elem) -> data_type_opt {
            if (elem.empty()) return std::nullopt;

            return StringTo<data_type>(elem);
        };

        auto convert = [](const auto& elem) -> data_type {
            return StringTo<data_type>(elem);
        };

        auto parse_token = [&face_elements, convert, convert_opt](
                               const auto& token) {

            const auto elems = _obj::Split(token, '/');
            switch (elems.size())
            {
                case 1:
                {
                    face_elements.add(convert(elems[coords::v]));
                    break;
                }
                case 2:
                {
                    face_elements.add(
                        convert(elems[coords::v]), convert(elems[coords::vt]));
                    break;
                }
                case 3:
                {
                    face_elements.add(convert(elems[coords::v]),
                        convert_opt(elems[coords::vt]),
                        convert(elems[coords::vn]));
                    break;
                }
            }
        };

        std::for_each(tokens.begin(), tokens.end(), parse_token);
        return face_elements;
    }

    return ParseResultVariant{};
}
}  // namespace _obj

template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

Obj Obj::CreateObjModel(const std::string_view file_dir)
{
    Obj model;
    auto model_visitor = overloaded{

        [&model](VertexIndices& vi) {
            model.vertex_indices.push_back(std::move(vi));
        },
        [&model](VertexTextureCoordinates& vt) {
            model.vertex_texture_coordinates.push_back(std::move(vt));
        },
        [&model](VertexNormals& vn) {
            model.vertex_normals.push_back(std::move(vn));
        },
        [&model](
            FaceElements& f) { model.face_elements.push_back(std::move(f)); },
        [](std::monostate) { /* Do nothing */ }

    };

    std::ifstream input_stream;
    input_stream.open(file_dir.data(), std::fstream::in);
    if (input_stream.fail()) return Obj();

    for (std::string line; !input_stream.eof();)
    {
        std::getline(input_stream, line);
        const auto line_view = std::string_view(line);
        const auto [identifier, tokens] = _obj::ParseLine(line_view);

        auto part = _obj::CreateObjField(identifier, tokens);
        std::visit(model_visitor, part);
    }

    return model;
}
