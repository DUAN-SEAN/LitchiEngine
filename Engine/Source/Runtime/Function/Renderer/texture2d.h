
#ifndef UNTITLED_TEXTURE2D_H
#define UNTITLED_TEXTURE2D_H

#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <glad/glad.h>
#include <rttr/rttr_enable.h>
namespace LitchiRuntime
{

	class TextureRes
	{

	public:
		TextureRes() {}

		std::string texture_name_;
		std::string texture_path_;

		RTTR_ENABLE()
	};

	class Texture2D
	{
	private:
		Texture2D() :mipmap_level_(0), width_(0), height_(0), gl_texture_format_(0), texture_handle_(0)
		{

		};

	public:

		~Texture2D();
		void UpdateSubImage(int x, int y, int width, int height, unsigned int client_format, unsigned int data_type, unsigned char* data, unsigned int data_size);
		void UpdateImage(int x, int y, int width, int height, unsigned int client_format, unsigned int data_type, unsigned char* data, unsigned int data_size);

		static Texture2D* LoadFromFile(std::string& image_file_path);//加载一个图片文件
	
		static Texture2D* Create(unsigned short width, unsigned short height, unsigned int server_format, unsigned int client_format, unsigned int data_type, unsigned char* data, unsigned int data_size);

	public:
		//cpt文件头
		struct CptFileHead
		{
			char type_[3];
			int mipmap_level_;
			int width_;
			int height_;
			int gl_texture_format_;
			int compress_size_;
		};

		int mipmap_level() { return mipmap_level_; }
		int width() { return width_; }
		int height() { return height_; }
		GLenum gl_texture_format() { return gl_texture_format_; }
		unsigned int texture_handle() { return texture_handle_; }
		void set_texture_handle(unsigned int texture_handle) { texture_handle_ = texture_handle; }
	private:
		int mipmap_level_;
		int width_;
		int height_;
		GLenum gl_texture_format_;
		unsigned int texture_handle_;//纹理ID
	};
}
#endif //UNTITLED_TEXTURE2D_H
