//
//#include "FontLoader.h"
//
//#include <fstream>
//#include <glad/glad.h>
//
//#include "TextureLoader.h"
//#include "freetype/ftbitmap.h"
//#include "Runtime/Function/Renderer/Font/font.h"
//#include "Runtime/Function/Renderer/Rendering/Color.h"
//#include "Runtime/Function/Renderer/Rendering/Font/Font.h"
//#include "spdlog/spdlog.h"
//
//using std::ifstream;
//using std::ios;
//
//using namespace LitchiRuntime;
//
//Font* Loaders::FontLoader::Create(const std::string& p_filepath, unsigned short p_font_size)
//{
//    //读取 ttf 字体文件
//    ifstream input_file_stream(p_filepath, ios::in | ios::binary);
//    input_file_stream.seekg(0, std::ios::end);
//    int len = input_file_stream.tellg();
//    input_file_stream.seekg(0, std::ios::beg);
//    char* font_file_buffer = new char[len];
//    input_file_stream.read(font_file_buffer, len);
//
//    //将ttf 传入FreeType解析
//    FT_Library ft_library = nullptr;
//    FT_Face ft_face = nullptr;
//    FT_Init_FreeType(&ft_library);//FreeType初始化;
//    FT_Error error = FT_New_Memory_Face(ft_library, (const FT_Byte*)font_file_buffer, len, 0, &ft_face);
//    if (error != 0) {
//        spdlog::error("FT_New_Memory_Face return error {}!", error);
//        return nullptr;
//    }
//
//    FT_Select_Charmap(ft_face, FT_ENCODING_UNICODE);
//
//    FT_F26Dot6 ft_size = (FT_F26Dot6)(p_font_size * (1 << 6));
//
//    FT_Set_Char_Size(ft_face, ft_size, 0, 72, 72);
//
//    if (ft_face == nullptr) {
//        spdlog::error("FT_Set_Char_Size error!");
//        return nullptr;
//    }
//
//    //创建Font实例，保存Freetype解析字体结果。
//    auto font = new Font();
//    font->font_size_ = p_font_size;
//    font->font_file_buffer_ = font_file_buffer;
//    font->ft_library_ = ft_library;
//    font->ft_face_ = ft_face;
//
//    //创建空白的、仅Alpha通道纹理，用于生成文字。
//    unsigned char* pixels = (unsigned char*)malloc(font->font_texture_size_ * font->font_texture_size_);
//    memset(pixels, 0, font->font_texture_size_ * font->font_texture_size_);
//    font->font_texture_ = TextureLoader::CreateFromMemory(pixels,font->font_texture_size_, font->font_texture_size_, ETextureFilteringMode::LINEAR,ETextureFilteringMode::LINEAR,false,GL_RED, GL_RED, GL_UNSIGNED_BYTE);
//    free(pixels);
//
//    return font;
//}
