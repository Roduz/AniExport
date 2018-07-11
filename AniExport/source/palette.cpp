//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#include "../header/palette.h"
#include "../header/errors.h"
#include "../header/rgba_fun.hpp"
#include "../header/file_fun.hpp"
#include <fstream>
#include <sstream>
#include <assert.h>

namespace image_lib
	{
	using namespace std;

	vector<unsigned short> read_pmap(const char * filename, unsigned & amount)
		{
		if(!fun_lib::file_exists(filename))
			throw errors_lib::file_missing(filename);

		vector<unsigned short> count;
			{
			ifstream file(filename);
			int c_count = 0;
			for( string line; getline(file,line); )
				{
				string result = fun_lib::trim_string(line);
				char res0 = result[0];
				if ((res0 == '#') || (result.length() <= 2))
					continue;
				else if (fun_lib::find_strings(result, "light:", "Light:"))
					{
					int ret = fun_lib::get_int_string(result, false, ':');
					if (ret)
						{
						c_count += ret;
						count.push_back(ret);
						}
					else
						break;
					}
				else if (fun_lib::find_strings(result, "colors", "Colors"))
					amount = fun_lib::get_int_string(result, true, ' ');
				}
			if (c_count != amount)
				throw errors_lib::bad_info_data("PMAP color amount is not the same as colors counted in the pmap file");
			}
		return count;
		}

	vector<unsigned> read_colormap(const char * filename, unsigned const & amount, vector<unsigned short> const & pmap)
		{
		if(!fun_lib::file_exists(filename))
			throw errors_lib::file_missing(filename);

        vector<unsigned> count(pmap.size());
			{
			ifstream file(filename);
			vector<unsigned> hold;
			for( string line; getline(file,line); )
				{
				string result = fun_lib::trim_string(line);
				char res0 = result[0];
				if ((res0 == '#') || (result.length() <= 2))
					continue;
				else if (fun_lib::find_strings(result, "Shadow:", "shadow:"))
					continue;
				else if (fun_lib::find_strings(result, "Entries", "entries"))
					{
					int c_count = fun_lib::get_int_string(result, true, ' ');
					if (c_count != amount)
						throw errors_lib::bad_info_data("COLORMAP color amount is not the same as colors counted in the colormap file");
					}
				else
					hold.push_back(fun_lib::get_RGBA_channel_pixel(result)); 
				}
			unsigned const empty = (0 << 0) | (0 << 8) | (0 << 16) | (255 << 24);
			int id = 0, ic = 0;
			for(vector<unsigned short>::const_iterator i=pmap.begin(), e=pmap.end(); i!=e; ic++, i++)
				{
				unsigned entry = empty;
				for( int t=0; t<*i ; id++, t++)
					{
					if ((hold[id] != entry) && (hold[id] != empty))
						entry = hold[id];
					}
				count[ic] = entry;
				}
			}
        return count;
		}

	vector<unsigned> read_pal(const char * filename, unsigned const & amount)
		{
		if(!fun_lib::file_exists(filename))
			throw errors_lib::file_missing(filename);

        vector<unsigned> count;
			{
			ifstream file(filename);
			for( string line; getline(file,line); )
				{
				string result = fun_lib::trim_string(line);
				char res0 = result[0];
				if ((res0 == '#') || (result.length() <= 2))
					continue;
				else if (fun_lib::find_strings(result, "Shadow:", "shadow:"))
					continue;
				else if (fun_lib::find_strings(result, "Entries", "entries"))
					{
					int c_count = fun_lib::get_int_string(result, true, ' ');
					if (c_count != amount)
						throw errors_lib::bad_info_data("PALETTE color amount is not the same as colors counted in the palette file");
					}
				else
					{
					unsigned const ret = fun_lib::get_RGBA_channel_pixel(result);
					count.push_back(ret);
					}
				}
			}
        return count;
		}
	
	vector<unsigned> make_palette(vector<unsigned short> const & pmap, vector<unsigned> pal)
		{
		vector<unsigned> palout(pmap.size()*fun_lib::pal_res);
		int pcount = 0;
		int mcount = 0;
		for(vector<unsigned short>::const_iterator i=pmap.begin(), e=pmap.end(); i!=e; mcount++, i++)
			{
			if(*i < 1)
				throw errors_lib::bad_info_data("PMAP has color band entry of 0 colors");

			if(*i == 1)
				{
				pal[pcount];
				vector<unsigned> band(fun_lib::pal_res, pal[pcount]);
				pcount++;

				for(int c=0, p = mcount*fun_lib::pal_res; c < fun_lib::pal_res; c++)
					palout[p+c] = band[c];
	
				}
			else
				{
				int step = 0;
				vector<unsigned> strip(*i);
				for(int step = 0 ; step < *i; step++, pcount++ )
					strip[step] = pal[pcount];

				vector<unsigned> band = make_color_band(strip);
				for(int c=0, p = mcount*fun_lib::pal_res; c < fun_lib::pal_res; c++)
					palout[p+c] = band[c];

				}
			}
		return palout;
		}

	vector<unsigned> make_color_band(vector<unsigned> const & strip)
		{
        vector<unsigned> baseColors(fun_lib::pal_res, 0);
		int const num = strip.size();
		float const mod = (float)(fun_lib::pal_res-1)/(num-1);
		vector<short> key_index(num);
		for(int i = 0; i < num; i++)
			key_index[i] = (int)((mod*i)+0.5f);

		for(int i = 0; i < num-1; i++)
			{
			for(int k = key_index[i]; k < key_index[i+1]; k++)
				{
				int count = key_index[i+1]-key_index[i];
				int div = k-key_index[i];
				float rate = (float)div/count;
				baseColors[k] = fun_lib::pixel_lerp(strip[i], strip[i + 1], rate);
				}
			}
		baseColors[fun_lib::pal_res-1] = strip[num-1];
		return baseColors;
		}

	}
