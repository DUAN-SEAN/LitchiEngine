
#include "texture2d.h"
#include <fstream>

#include "gpu_resource_mapper.h"
#include "Runtime/Core/App/application.h"
#include "Runtime/Core/Log/debug.h"

using std::ifstream;
using std::ios;
namespace LitchiRuntime
{
	Texture2D::~Texture2D() {
		if (texture_handle_ > 0) {
			GLuint* texture_id_array = new GLuint[1];
			texture_id_array[0] = GPUResourceMapper::GetTexture(texture_handle_);
			glDeleteTextures(1, texture_id_array); __CHECK_GL_ERROR__
			delete[] texture_id_array;
		}
	}

	Texture2D* Texture2D::LoadFromFile(std::string& image_file_path)
	{
		Texture2D* texture2d = new Texture2D();

		//读取 cpt 压缩纹理文件
		ifstream input_file_stream(Application::GetDataPath() + image_file_path, ios::in | ios::binary);
		CptFileHead cpt_file_head;
		input_file_stream.read((char*)&cpt_file_head, sizeof(CptFileHead));

		unsigned char* data = (unsigned char*)malloc(cpt_file_head.compress_size_);
		input_file_stream.read((char*)data, cpt_file_head.compress_size_);
		input_file_stream.close();

		texture2d->gl_texture_format_ = cpt_file_head.gl_texture_format_;
		texture2d->width_ = cpt_file_head.width_;
		texture2d->height_ = cpt_file_head.height_;
		texture2d->texture_handle_ = GPUResourceMapper::GenerateTextureHandle();


		//1. 通知显卡创建纹理对象，返回句柄;
		GLuint texture_id;
		glGenTextures(1, &texture_id);

		//2. 将纹理绑定到特定纹理目标;
		glBindTexture(GL_TEXTURE_2D, texture_id);

		{
			//3. 将压缩纹理数据上传到GPU;
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, texture2d->gl_texture_format_, texture2d->width_, texture2d->height_, 0, cpt_file_head.compress_size_, data);
		}

		//4. 指定放大，缩小滤波方式，线性滤波，即放大缩小的插值方式;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		delete (data);

		//将主线程中产生的纹理句柄 映射到 纹理
		GPUResourceMapper::MapTexture(texture2d->texture_handle_, texture_id);

		return texture2d;
	}



	void Texture2D::UpdateSubImage(int x, int y, int width, int height, unsigned int client_format, unsigned int data_type,
		unsigned char* data, unsigned int data_size) {
		if (width <= 0 || height <= 0) {
			return;
		}
		glBindTexture(GL_TEXTURE_2D, texture_handle_); __CHECK_GL_ERROR__
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); __CHECK_GL_ERROR__
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, client_format, data_type, data); __CHECK_GL_ERROR__
		
	}

	void Texture2D::UpdateImage(int x, int y, int width, int height, unsigned int client_format, unsigned int data_type,
		unsigned char* data, unsigned int data_size) {
		if (width <= 0 || height <= 0) {
			return;
		}
		glBindTexture(GL_TEXTURE_2D, texture_handle_); __CHECK_GL_ERROR__
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); __CHECK_GL_ERROR__
		glTexImage2D(GL_TEXTURE_2D, 0, gl_texture_format_, width, height, 0, client_format, data_type, data); __CHECK_GL_ERROR__
	}

	Texture2D* Texture2D::Create(unsigned short width, unsigned short height, unsigned int server_format, unsigned int client_format,
		unsigned int data_type, unsigned char* data, unsigned int data_size) {
		Texture2D* texture2d = new Texture2D();
		texture2d->gl_texture_format_ = server_format;
		texture2d->width_ = width;
		texture2d->height_ = height;
		texture2d->texture_handle_ = GPUResourceMapper::GenerateTextureHandle();
		GLuint texture_id;

		//1. 通知显卡创建纹理对象，返回句柄;
		glGenTextures(1, &texture_id); __CHECK_GL_ERROR__

		//2. 将纹理绑定到特定纹理目标;
		glBindTexture(GL_TEXTURE_2D, texture_id); __CHECK_GL_ERROR__

		//3. 将图片rgb数据上传到GPU;
		glTexImage2D(GL_TEXTURE_2D, 0, texture2d->gl_texture_format_, width, height, 0, client_format, data_type, data); __CHECK_GL_ERROR__

		//4. 指定放大，缩小滤波方式，线性滤波，即放大缩小的插值方式;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); __CHECK_GL_ERROR__
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); __CHECK_GL_ERROR__

		//将主线程中产生的纹理句柄 映射到 纹理
		GPUResourceMapper::MapTexture(texture2d->texture_handle_, texture_id);
		return texture2d;
	}

}