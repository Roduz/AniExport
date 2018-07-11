//Copyright (c) Brigido Rodriguez, all rights reserved.
#include "../header/frame_reader.h"
#include "../header/texture.h"
#include "../header/palette.h"
#include "../header/errors.h"
#include "../header/timer.hpp"
#include <algorithm>
#include <fstream>
#include <assert.h>

using namespace std;
using namespace image_lib;

namespace
aniexp
	{
	namespace
		{
		shared_ptr<texture> get_texture_frame(string name, string type, string ext)
			{
			timer_action time_it("Loaded",(name + type + ext).c_str());
			shared_ptr<texture> tf = load_texture_pcx((name + type + ext).c_str());
			if(tf)
				time_it.action_success();
			else
				throw(errors_lib::file_missing((name + type + ext).c_str()));

			return tf;
			}
		shared_ptr<texture> get_texture_frame(string name, string type, string ext, int width, int height)
			{
			if(fun_lib::file_exists((name + type + ext).c_str()))
				{
				timer_action time_it("Loaded",(name + type + ext).c_str());
				shared_ptr<texture> tf = load_texture_pcx((name + type + ext).c_str());
				if(tf)
					time_it.action_success();
				else
					throw(errors_lib::file_missing((name + type + ext).c_str()));

				return tf;
				}
			else
				{
				std::vector<unsigned char> pix(width*height*4, 255);
				return make_texture(pix, width, height, image_format::RGBA);
				}
			}

		vector<frame> read_ani(vector<string> & filelist, const char * anifile)
			{
			vector<frame> frames;
			timer_action time_it("Reading",anifile);
			ifstream file(anifile);
			int counter = 0;
			int def_rep = 0;
			for( string line; fun_lib::get_line(file,line,counter); )   
				{
				line = fun_lib::split_string_at(line, '#');
				if( fun_lib::find_strings(line, "AllFrames:", "allframes:"))
					{
					for( string result; fun_lib::get_line(file,result,counter); ) 
						{
						if( fun_lib::find_strings(result, "Modifier:", "modifier:"))
							{
							for( string check; fun_lib::get_line(file,check,counter); )
								{
								if( check.find("[end]") != string::npos )
									break;
								}
							}
						else if( fun_lib::find_strings(result, "Repeat:", "repeat:"))
							{
							result.erase(remove_if(result.begin(), result.end(), isspace), result.end());
							def_rep = fun_lib::get_int_string(result, false, ':');
							break;
							}
						else if( fun_lib::find_strings(result, "Repeats:", "repeats:"))
							{
							result.erase(remove_if(result.begin(), result.end(), isspace), result.end());
							def_rep = fun_lib::get_int_string(result, false, ':');
							break;
							}
						else if( result.find("[end]") != string::npos )
							break;
						}
					}
				else if( fun_lib::find_strings(line, "Frames:", "frames:"))
					{
					for( string result; fun_lib::get_line(file,result,counter); )
						{
						result = fun_lib::split_string_at(result, '#');
						result.erase(remove_if(result.begin(), result.end(), isspace), result.end());
						if( result.find("[end]") != string::npos )
							break;
						else if ( result.find("_colors") != string::npos )
							{
							result.erase(result.end()-6, result.end());
							string const path = fun_lib::get_directory(anifile);
							if(fun_lib::find_strings(result, "!base"))
								{
								string base = fun_lib::split_string_at(result, '/', 1);
								string tofile = result.erase(0,6);
								result = path.substr(0, size_t(path.find(base))) + tofile;
								}
							else
								result = path + result;

							filelist.push_back(result);
							}
						}
					}
				else if( fun_lib::find_strings(line, "Frame:", "frame:"))
					{		
					int num = 0;
					int rep = 0;
					for( string result; fun_lib::get_line(file,result,counter); ) 
						{
						result = fun_lib::split_string_at(result, '#');
						if(num && rep)
							{
							frames.push_back(frame(num,rep));
							break;
							}
						else if( fun_lib::find_strings(result, "Number:", "number:"))
							{
							result.erase(remove_if(result.begin(), result.end(), isspace), result.end());
							num = fun_lib::get_int_string(result, false, ':');
							}
						else if( fun_lib::find_strings(result, "Repeat:", "repeat:"))
							{
							result.erase(remove_if(result.begin(), result.end(), isspace), result.end());
							rep = fun_lib::get_int_string(result, false, ':');
							if(!rep)
								break;
							}
						else if( fun_lib::find_strings(result, "Repeats:", "repeats:"))
							{
							result.erase(remove_if(result.begin(), result.end(), isspace), result.end());
							rep = fun_lib::get_int_string(result, false, ':');
							if(!rep)
								break;
							}
						else if( result.find("[end]") != string::npos )
							{
							if(num && !rep)
								{
								if(def_rep)
									frames.push_back(frame(num,def_rep));
								else
									frames.push_back(frame(num,1));
								}
							break;
							}
						}
					}
				}
			file.close();
			time_it.action_success();
			return frames;
			}
		}

	vector<frame> read_data_files(reader_mode::enum_ const mode, unsigned const background, const char * anifile, const char * pmapfile,
		const char * cmapfile, const char * palfile, const char * dyefile, const char * homedir, bool palout)
		{
		if(!fun_lib::file_exists(anifile))
			throw(errors_lib::file_missing(anifile));

		vector<string> filelist;
		vector<frame> frames = read_ani(filelist, anifile);
		if(frames.empty())
			throw(errors_lib::bad_info_data("No frame files found in ani file!"));

		unsigned pal_col = 0;
		std::vector<unsigned short> pmap;
		if(mode==reader_mode::full || mode==reader_mode::old)
			{
			timer_action time_it("Reading",pmapfile);
			pmap = image_lib::read_pmap(pmapfile, pal_col);
			time_it.action_success();
			}
		std::vector<unsigned> colormap;
		if(mode==reader_mode::full || mode==reader_mode::old)
			{
			timer_action time_it("Reading",cmapfile);
			colormap = image_lib::read_colormap(cmapfile, pal_col, pmap);
			time_it.action_success();
			if (pmap.size() != colormap.size())
				throw(errors_lib::bad_info_data("PMAP color size is not the same as in the COLORMAP!"));
			}
		std::vector<unsigned> colorpal;
		if(mode==reader_mode::full || mode==reader_mode::old)
			{
			timer_action time_it("Reading",palfile);
			std::vector<unsigned> pal_data = image_lib::read_pal(palfile, pal_col);
			if (pal_col != pal_data.size())
				{
				string msg = string("Color amount ") + to_string(pal_col) + string(" is not the same as in the color palette!");
				throw(errors_lib::bad_info_data(msg.c_str()));
				}
			colorpal = image_lib::make_palette(pmap, pal_data);
			time_it.action_success();
			if (palout)
				{
				std::shared_ptr<texture> tpal = image_lib::make_texture(colorpal, 78, pmap.size(), image_format::RGBA);
				string outpng = string(homedir) + "colpal.png";
				save_texture_png(*tpal, outpng.c_str());
				}
			}
		std::vector<unsigned>dyepal;
		if(mode==reader_mode::full)
			{
			timer_action time_it("Reading",dyefile);
			std::vector<unsigned> pal_data = image_lib::read_pal(dyefile, pal_col);
			if (pal_col != pal_data.size())
				{
				string msg = string("Color amount ") + to_string(pal_col) + string(" is not the same as in the color palette!");
				throw(errors_lib::bad_info_data(msg.c_str()));
				}
			dyepal = image_lib::make_palette(pmap, pal_data);
			time_it.action_success();
			if (palout)
				{
				std::shared_ptr<texture> tpal = image_lib::make_texture(dyepal, 78, pmap.size(), image_format::RGBA);
				string outpng = string(homedir) + "dyepal.png";
				save_texture_png(*tpal, outpng.c_str());
				}
			}
		string const ext = ".pcx";
		string const _dye = "dye";
		string const _lines = "lines";
		string const _shade = "shade";
		string const _colors = "colors";
		vector<shared_ptr<texture> > textures;
		textures.reserve(filelist.size());
		for(std::vector<string>::iterator i = filelist.begin(), e = filelist.end(); i!=e; i++)
			{
			shared_ptr<texture> colors;
			shared_ptr<texture> lines;
			shared_ptr<texture> shade;
			shared_ptr<texture> dye;
			switch(mode)
				{
				case reader_mode::colors:
					{
					textures.push_back(get_texture_frame(*i, _colors, ext));
					break;
					}
				case reader_mode::lines:
					{
					textures.push_back(get_texture_frame(*i, _lines, ext));
					break;
					}
				case reader_mode::shade:
					{
					textures.push_back(get_texture_frame(*i, _shade, ext));
					break;
					}
				case reader_mode::dye:
					{
					textures.push_back(get_texture_frame(*i, _dye, ext));
					break;
					}
				case reader_mode::full:
					{
					colors = get_texture_frame(*i, _colors, ext);
					lines = get_texture_frame(*i, _lines, ext, colors->width_, colors->height_);
					shade = get_texture_frame(*i, _shade, ext, colors->width_, colors->height_);
					dye = get_texture_frame(*i, _dye, ext, colors->width_, colors->height_);
					timer_action time_it("Combined", (*i).c_str());
					textures.push_back(combine_all(*colors,*lines,*shade,*dye,colormap,colorpal,dyepal,background));
					time_it.action_success();
					break;
					}
				default:
					{
					assert(mode == reader_mode::old);
					colors = get_texture_frame(*i, _colors, ext);
					lines = get_texture_frame(*i, _lines, ext, colors->width_, colors->height_);
					shade = get_texture_frame(*i, _shade, ext, colors->width_, colors->height_);
					timer_action time_it("Combined", (*i).c_str());
					textures.push_back(combine_lcs(*colors,*lines,*shade,colormap,colorpal,background));
					time_it.action_success();
					break;
					}
				}
			}
		unsigned short const szck = frames.size();
		for(std::vector<frame>::iterator i = frames.begin(), e = frames.end(); i!=e; i++)
			{
			unsigned short const idx = i->number_-1;
			if (idx < szck)
				i->p_image_ = textures[idx];
			else
				throw(errors_lib::bad_info_data("Frame files are less then frame number used in the ani!"));
			}
		return frames;
		}
	}