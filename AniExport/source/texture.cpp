//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#include "../header/texture.h"
#include "../header/errors.h"
#include "../header/pcx_file.h"
#include "../header/png_file.h"
#include "../header/rgba_fun.hpp"
#include "../header/file_fun.hpp"
#include <assert.h>

namespace
image_lib
	{
	namespace
		{
		unsigned int lookup[] =
			{
			0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,
			0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
			0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,
			0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
			0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,
			0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
			0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,
			0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
			0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,
			0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
			0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,
			0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
			0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,
			0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
			0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,
			0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
			0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,
			0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
			0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,
			0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
			0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,
			0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
			0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,
			0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
			0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,
			0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
			0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,
			0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
			0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,
			0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
			0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,
			0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D
			};
		void input(unsigned int * crc, unsigned char message_element)
			{
			*crc = (*crc >> 8) ^ lookup[message_element ^ (*crc & 0xFF)];
			}
		int get_crc32_number(std::vector<unsigned char> pix, int length)
			{
			unsigned int crc32;
			unsigned char *array = &pix[0];
			for (; length; length--, array++)
				input(&crc32, *array);

			return crc32;
			}
		}

	std::shared_ptr<texture> make_texture(std::vector<unsigned> p, int w, int h, image_format::enum_ f)
		{
		int const psize = p.size();
		std::vector<unsigned char> pixels(p.size()*4);
		for(int i = 0; i < psize; i++)
			{
			int mod = i * 4;
			fun_lib::pixel_to_chars(p[i],pixels[mod],pixels[mod+1],pixels[mod+2],pixels[mod+3]);
			}
		std::shared_ptr<texture> pt(new texture(pixels, w, h, f));
		//pt->id_ = get_crc32_number(pt->pixels_,pt->width_*pt->height_*pt->channels_);
		return pt;
		}

	std::shared_ptr<texture> make_texture(std::vector<unsigned char> p, int w, int h, image_format::enum_ f)
		{
		std::shared_ptr<texture> pt(new texture(p, w, h, f));
		//pt->id_ = get_crc32_number(pt->pixels_,pt->width_*pt->height_*pt->channels_);
		return pt;
		}

	std::shared_ptr<texture> load_texture_png(const char * n)
		{
		if(!fun_lib::file_exists(n))
			return 0;
		std::vector<unsigned char> pixels;
		unsigned width = 0, height = 0;
		unsigned error = lodepng::decode(pixels, width, height, n);
		assert(!pixels.empty());
		return make_texture(pixels, width, height, image_format::RGBA);
		}

	std::shared_ptr<texture> load_texture_pcx(const char * n)
		{
		if(!fun_lib::file_exists(n))
			return 0;
		std::vector<unsigned char> pixels;
		unsigned width = 0, height = 0;
		int code = pcx_read(pixels, width, height, n);
		assert(!pixels.empty());
		if(code)
			return make_texture(pixels, width, height, image_format::RGBA);
		else
			return 0;
		}

	bool save_texture_png(texture const & p, const char * name)
		{
		std::vector<unsigned char> buffer;
		lodepng::State state;
		state.encoder.auto_convert = false;
		unsigned error = lodepng::encode(buffer, p.pixels_, p.width_, p.height_, state);
		if(error)
			return false;

		lodepng::save_file(buffer, name);
		return true;
		}

	std::shared_ptr<texture> combine_all(texture const & c, texture const & l, texture const & s, texture const & d,
		std::vector<unsigned> & colmap, std::vector<unsigned> & colpal, std::vector<unsigned> & dyepal, unsigned background)
		{
		int width = 0;
		int height = 0;
		if((c.width_ == l.width_) && (l.width_ == s.width_) && (s.width_ == d.width_))
			width = c.width_;
		else
			throw errors_lib::bad_info_data("Width of all frames must be the same!");

		if((c.height_ == l.height_) && (l.height_ == s.height_) && (s.height_ == d.height_))
			height = c.height_;
		else
			throw errors_lib::bad_info_data("Height of all frames must be the same!");

		int const map_sz = colmap.size();
		unsigned buffer_size = width*height*4;
		std::vector<unsigned char> outbuffer(buffer_size);
		int count = 0;
		for(std::vector<unsigned char>::const_iterator ic=c.pixels_.begin(), il=l.pixels_.begin(), is=s.pixels_.begin(), id=d.pixels_.begin(), ec=c.pixels_.end(); ic!=ec; ic+=4,is+=4,il+=4,id+=4)
			{
			if ((*il != *(il+1)) && (*il != *(il+2)))
				throw errors_lib::bad_info_data("Lines frame has color value!");

			unsigned color = 0;
			int c_idx = -1;
			int s_idx = -1;
				{
				unsigned const ckey = fun_lib::pixel_to_int(*ic,*(ic+1),*(ic+2),*(ic+3));
				if (ckey != fun_lib::white)
					{
					int idx = 0;
					for(std::vector<unsigned>::const_iterator i = colmap.begin(), e = colmap.end(); i!=e; idx++, i++)
						{
						if(ckey == *i)
							{
							c_idx = idx;
							break;
							}
						}
					if(c_idx >= map_sz)
						throw errors_lib::bad_info_data("Color pixel did not match any colormap data entries!");

					if ((*is != *(is+1)) && (*is != *(is+2)))
						throw errors_lib::bad_info_data("Shade frame has color value!");

					if (*is < 102)
						s_idx = (256 - 102) / 2;
					else
						s_idx = (256 - *is) / 2;

					if(s_idx >= fun_lib::pal_res)
						throw errors_lib::bad_info_data("Shade index is larger than color band resolution!");

					if (c_idx >= 0)
						color = colpal[(c_idx * fun_lib::pal_res) + s_idx];
					}
				}
			unsigned line = 0;
			int d_idx = -1;
			int l_idx = -1;
			if(*il != 255)
				{
				float lidx = ((float)*il / 255.0f) * ((float)fun_lib::pal_res-1.0f);
				l_idx = (int)lidx;
				unsigned const key = fun_lib::pixel_to_int(*id, *(id+1), *(id+2), *(id+3));
				int idx = 0;
				if(key != fun_lib::white)
					{
					for(std::vector<unsigned>::const_iterator i = colmap.begin(), e = colmap.end(); i!=e; idx++, i++)
						{
						if(key == *i)
							{
							d_idx = idx;
							break;
							}
						}
					if(d_idx >= map_sz)
						throw errors_lib::bad_info_data("Dye pixel did not match any colormap data entries!");
					}
				if (d_idx == -1)
					line = fun_lib::pixel_to_int(0,0,0,255-*il);
				else
					{
					unsigned char lin[4];
					fun_lib::pixel_to_chars(dyepal[(d_idx * fun_lib::pal_res) + l_idx], lin[0], lin[1], lin[2], lin[3]);
					line = fun_lib::pixel_to_int(lin[0],lin[1],lin[2],255-*il);
					}
				}
			unsigned char pix[4] = {0,0,0,0};
			fun_lib::pixel_to_chars(fun_lib::pixel_blend(line, color, background), pix[0], pix[1], pix[2], pix[3]);
			outbuffer[count]=pix[0];
			count++;
			outbuffer[count]=pix[1];
			count++;
			outbuffer[count]=pix[2];
			count++;
			outbuffer[count]=pix[3];
			count++;
			}
		return make_texture(outbuffer, width, height, image_format::RGBA);
		}

	std::shared_ptr<texture> combine_lcs(texture const & c, texture const & l, texture const & s,
		std::vector<unsigned> & colmap, std::vector<unsigned> & colpal, unsigned background)
		{
		int width = 0;
		int height = 0;
		if((c.width_ == l.width_) && (l.width_ == s.width_))
			width = c.width_;
		else
			throw errors_lib::bad_info_data("Width of all frames must be the same!");

		if((c.height_ == l.height_) && (l.height_ == s.height_))
			height = c.height_;
		else
			throw errors_lib::bad_info_data("Height of all frames must be the same!");

		int const map_sz = colmap.size();
		unsigned buffer_size = width*height*4;
		std::vector<unsigned char> outbuffer(buffer_size);
		int count = 0;
		for(std::vector<unsigned char>::const_iterator ic=c.pixels_.begin(), il=l.pixels_.begin(), is=s.pixels_.begin(), ec=c.pixels_.end(); ic!=ec; ic+=4,is+=4,il+=4)
			{
			if ((*il != *(il+1)) && (*il != *(il+2)))
				throw errors_lib::bad_info_data("Lines frame has color value!");

			unsigned color = 0;
			int c_idx = -1;
			int s_idx = -1;
				{
				unsigned const ckey = fun_lib::pixel_to_int(*ic,*(ic+1),*(ic+2),*(ic+3));
				if (ckey != fun_lib::white)
					{
					int idx = 0;
					for(std::vector<unsigned>::const_iterator i = colmap.begin(), e = colmap.end(); i!=e; idx++, i++)
						{
						if(ckey == *i)
							{
							c_idx = idx;
							break;
							}
						}
					if(c_idx >= map_sz)
						throw errors_lib::bad_info_data("Color pixel did not match any colormap data entries!");

					if ((*is != *(is+1)) && (*is != *(is+2)))
						throw errors_lib::bad_info_data("Shade frame has color value!");

					if (*is < 102)
						s_idx = (256 - 102) / 2;
					else
						s_idx = (256 - *is) / 2;

					if(s_idx >= fun_lib::pal_res)
						throw errors_lib::bad_info_data("Shade index is larger than color band resolution!");

					if (c_idx >= 0)
						color = colpal[(c_idx * fun_lib::pal_res) + s_idx];
					}
				}
			unsigned line = fun_lib::pixel_to_int(0,0,0,255-*il);
			unsigned char pix[4] = {0,0,0,0};
			fun_lib::pixel_to_chars(fun_lib::pixel_blend(line, color, background), pix[0], pix[1], pix[2], pix[3]);
			outbuffer[count]=pix[0];
			count++;
			outbuffer[count]=pix[1];
			count++;
			outbuffer[count]=pix[2];
			count++;
			outbuffer[count]=pix[3];
			count++;
			}
		return make_texture(outbuffer, width, height, image_format::RGBA);
		}
	}
