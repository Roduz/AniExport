//Copyright (c) Brigido Rodriguez, all rights reserved.
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <memory>
#include <vector>

namespace
image_lib
	{
	namespace
	image_format
		{
		enum enum_
			{
			RGBA,
			BGRA,
			ARGB,
			ABGR
			};
		}

	struct texture
		{
		int const width_;
		int const height_;
		int const channels_;
		image_format::enum_ const format_;
		std::vector<unsigned char> pixels_;
		unsigned int id_;
		texture(int w, int h, image_format::enum_ f):
			width_(w),
			height_(h),
			channels_(4),
			format_(f)
			{
			}
		texture(std::vector<unsigned char> p, int w, int h, image_format::enum_ f):
			width_(w),
			height_(h),
			channels_(4),
			format_(f)
			{
			pixels_.swap(p);
			}
		~texture()
			{
			}
		};

	std::shared_ptr<texture> make_texture(std::vector<unsigned> pixels, int width, int height, image_format::enum_ format);
	std::shared_ptr<texture> make_texture(std::vector<unsigned char> pixels, int width, int height, image_format::enum_ format);
	std::shared_ptr<texture> load_texture_png(const char * name);
	std::shared_ptr<texture> load_texture_pcx(const char * name);

	std::shared_ptr<texture> combine_all(texture const & colors, texture const & lines, texture const & shade, texture const & dye,
		std::vector<unsigned> & cm, std::vector<unsigned> & colpal, std::vector<unsigned> & dyepal, unsigned bg);
	std::shared_ptr<texture> combine_lcs(texture const & colors, texture const & lines, texture const & shade,
		std::vector<unsigned> & cm, std::vector<unsigned> & colpal, unsigned bg);

	bool save_texture_png(texture const & p, const char * name);
	}

#endif