//Copyright (c) Brigido Rodriguez, all rights reserved.
#ifndef _PALETTE_H_
#define _PALETTE_H_

#include <vector>

namespace
image_lib
	{
	std::vector<unsigned short> read_pmap(const char * name, unsigned & amount);
	std::vector<unsigned> read_colormap(const char * name, unsigned const & amount, std::vector<unsigned short> const & pmap);
	std::vector<unsigned> read_pal(const char * name, unsigned const & amount);
	std::vector<unsigned> make_palette(std::vector<unsigned short> const & pmap, std::vector<unsigned> pal);
	std::vector<unsigned> make_color_band(std::vector<unsigned> const & strip);
	}

#endif