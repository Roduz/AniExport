//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#ifndef _FRAME_READER_H_
#define _FRAME_READER_H_

#include "../header/file_fun.hpp"
#include <vector>
#include <string>
#include <memory>

namespace image_lib { struct texture; };

namespace
aniexp
	{
	struct frame
		{
		int const number_;
		int const repeats_;
		std::shared_ptr<image_lib::texture> p_image_;
		frame(int n, int r):
			number_(n),
			repeats_(r)
			{
			}
		};

	std::vector<frame> read_data_files(reader_mode::enum_ const mode, unsigned const bg, const char * ani, const char * pmap,
		const char * cmap, const char * pal, const char * dye, const char * homedir, bool palout );
	}
#endif