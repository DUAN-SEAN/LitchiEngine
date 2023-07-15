
#pragma once

#include <iostream>
#include <unordered_map>
#include "freetype/ftglyph.h"

namespace LitchiRuntime
{
    class Texture2D;
    class Font {
    public:
        Texture2D* font_texture() { return font_texture_; }

        /// freetype为字符生成bitmap
        /// \param c
        void LoadCharacter(char ch);

        /// 记录单个字符在图集上的坐标、宽高，用于生成同尺寸的顶点数据，1：1渲染。
        struct Character {
            float left_top_x_;
            float left_top_y_;
            float right_bottom_x_;
            float right_bottom_y_;
            Character(float left_top_x, float left_top_y, float right_bottom_x, float right_bottom_y) {
                left_top_x_ = left_top_x;
                left_top_y_ = left_top_y;
                right_bottom_x_ = right_bottom_x;
                right_bottom_y_ = right_bottom_y;
            }
        };

        /// 为字符串生成bitmap，返回字符串每个字符的Character数据。
        /// \param str
        /// \return
        std::vector<Character*> LoadStr(std::string str);

    public:
    	const std::string path;
        unsigned short font_size_ = 20;//默认字体大小
        char* font_file_buffer_ = nullptr;//ttf字体文件加载到内存
        FT_Library ft_library_;
        FT_Face ft_face_;
        Texture2D* font_texture_;
        unsigned short font_texture_size_ = 1024;
        unsigned short font_texture_fill_x = 0;//
        unsigned short font_texture_fill_y = 0;
    // private:
        std::unordered_map<char, Character*> character_map_;//已经生成bitmap的字符
    };
	
}


