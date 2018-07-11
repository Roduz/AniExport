//Copyright (c) Brigido Rodriguez, all rights reserved.
#ifndef _RGBA_FUN_H_
#define _RGBA_FUN_H_

namespace
fun_lib
	{
	int const pal_res = 78;
	unsigned const white = 4294967295;

	struct
	rgb_pixel
		{
		unsigned char r_;
		unsigned char g_;
		unsigned char b_;
		rgb_pixel(){}
		rgb_pixel(unsigned char r, unsigned char g, unsigned char b):
			r_(r),
			g_(g),
			b_(b)
			{
			}
		};
	inline bool operator==(const rgb_pixel &lhs, const rgb_pixel &rhs)
		{
		return ((lhs.r_ == rhs.r_)&&(lhs.g_ == rhs.g_)&&(lhs.b_ == rhs.b_));
		}

	inline void pixel_to_chars(unsigned const & data, unsigned char & r, unsigned char & g, unsigned char & b, unsigned char & a)
		{
		r = data >> 0;
		g = data >> 8;
		b = data >> 16;
		a = data >> 24;
		}
	inline unsigned pixel_to_int(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
		{
		return ((r << 0) | (g << 8) | (b << 16) | (a << 24));
		}

	inline unsigned pixel_lerp(unsigned a, unsigned b, float t)
		{
		unsigned char ac[4] = {a>>0, a>>8, a>>16, a>>24};
		unsigned char bc[4] = {b>>0, b>>8, b>>16, b>>24};
		unsigned char out[4] =
			{(ac[0]==bc[0])? ac[0] : (unsigned char)(ac[0]+(bc[0]-ac[0])*t), (ac[1]==bc[1])? ac[1] : (unsigned char)(ac[1]+(bc[1]-ac[1])*t),
			(ac[2]==bc[2])? ac[2] : (unsigned char)(ac[2]+(bc[2]-ac[2])*t), (ac[3]==bc[3])? ac[3] : (unsigned char)(ac[3]+(bc[3]-ac[3])*t)};

		return pixel_to_int(out[0],out[1],out[2],out[3]);
		}

	inline unsigned pixel_blend(unsigned front, unsigned mid, unsigned back)
		{
		unsigned char f[4] = {front>>0, front>>8, front>>16, front>>24};
		unsigned char m[4] = {mid>>0, mid>>8, mid>>16, mid>>24};
		unsigned char b[4] = {back>>0, back>>8, back>>16, back>>24};

		float prate = 1.0f - (float)f[3]/255.0f;
		unsigned char p[4] =
			{(f[0]==m[0])? f[0] : (unsigned char)(f[0]+(m[0]-f[0])*prate), (f[1]==m[1])? f[1] : (unsigned char)(f[1]+(m[1]-f[1])*prate),
			(f[2]==m[2])? f[2] : (unsigned char)(f[2]+(m[2]-f[2])*prate), (f[3]>=m[3])? f[3] : m[3]};

		if(!b[3])
			return pixel_to_int(p[0],p[1],p[2],p[3]);

		float srate = 1.0f - (float)p[3]/255.0f;
		unsigned char s[4] =
			{(p[0]==b[0])? p[0] : (unsigned char)(p[0]+(b[0]-p[0])*srate), (p[1]==b[1])? p[1] : (unsigned char)(p[1]+(b[1]-p[1])*srate),
			(p[2]==b[2])? p[2] : (unsigned char)(p[2]+(b[2]-p[2])*srate), (p[3]>=b[3])? p[3] : b[3]};

		return pixel_to_int(s[0],s[1],s[2],s[3]);
		}

	inline int get_color_amount(std::vector<unsigned char> const & pixels)
		{
		std::vector<rgb_pixel> colors;
		colors.reserve(512);
		for(std::vector<unsigned char>::const_iterator i=pixels.begin(), e=pixels.end(); i!=e; i+=4)
			{
			rgb_pixel pix(*i, *(i+1), *(i+2));
			bool found = false;
			for(std::vector<rgb_pixel>::iterator pi=colors.begin(), pe=colors.end(); pi!=pe; pi++)
				{
				if(pix == *pi)
					{
					found = true;
					break;
					}
				}
			if(!found)
				colors.push_back(pix);

			}
		return colors.size();
		}
	}

#endif