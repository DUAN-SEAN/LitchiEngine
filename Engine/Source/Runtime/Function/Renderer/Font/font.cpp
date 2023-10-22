//
//#include "font.h"
//
//#include <glad/glad.h>
//#include <fstream>
//
//#include "Runtime/Function/Renderer/Resources/Texture.h"
//#include "spdlog/spdlog.h"
//
//using namespace LitchiRuntime;
//
//using std::ifstream;
//using std::ios;
//
//void Font::LoadCharacter(char ch) {
//    if(character_map_.find(ch) != character_map_.end())
//    {
//        return;
//    }
//
//    //加载这个字的字形,加载到 m_FTFace上面去;Glyph：字形，图形字符 [glif];
//    FT_Load_Glyph(ft_face_, FT_Get_Char_Index(ft_face_, ch), FT_LOAD_DEFAULT);
//
//    //从 FTFace上面读取这个字形  到 ft_glyph 变量;
//    FT_Glyph ft_glyph;
//    FT_Get_Glyph(ft_face_->glyph, &ft_glyph);
//    //渲染为256级灰度图
//    FT_Glyph_To_Bitmap(&ft_glyph, ft_render_mode_normal, 0, 1);
//
//    FT_BitmapGlyph ft_bitmap_glyph = (FT_BitmapGlyph)ft_glyph;
//    FT_Bitmap& ft_bitmap = ft_bitmap_glyph->bitmap;
//
//    //计算新生成的字符，在图集中的排列。
//    if(font_texture_fill_x+ft_bitmap.width>=font_texture_size_){//从左上角往右上角填充，满了就换一行。
//        font_texture_fill_x=0;
//        font_texture_fill_y+=font_size_;
//    }
//    if(font_texture_fill_y+font_size_>=font_texture_size_){
//        spdlog::error("{} is out of font_texture y",ch);
//        return;
//    }
//    font_texture_->UpdateSubImage(font_texture_fill_x, font_texture_fill_y, ft_bitmap.width, ft_bitmap.rows,  GL_RED, GL_UNSIGNED_BYTE, ft_bitmap.buffer);
//
//    //存储字符信息
//    Character* character=new Character(font_texture_fill_x*1.0f/font_texture_size_,font_texture_fill_y*1.0f/font_texture_size_,(font_texture_fill_x+ft_bitmap.width)*1.0f/font_texture_size_,(font_texture_fill_y+ft_bitmap.rows)*1.0f/font_texture_size_);
//    character_map_[ch] = character;
//
//    font_texture_fill_x+=ft_bitmap.width;
//}
//
//std::vector<Font::Character*> Font::LoadStr(std::string str) {
//    //生成所有的字符 bitmap
//    for(auto ch : str){
//        LoadCharacter(ch);
//    }
//    //返回所有字符信息
//    std::vector<Character*> character_vec;
//    for(auto ch : str){
//        auto character=character_map_[ch];
//        if(character==nullptr){
//            spdlog::error("LoadStr error,no bitmap,ch:{}",ch);
//            continue;
//        }
//        character_vec.push_back(character);
//    }
//    return character_vec;
//}
//
//
//
