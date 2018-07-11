//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#include "../header/exporter.h"
#include "../header/frame_reader.h"
#include "../header/errors.h"
#include "../header/gif_file.h"
#include "../header/texture.h"
#include "../header/timer.hpp"
#include "../header/file_fun.hpp"
#include <assert.h>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

using namespace std;

namespace
	{
	struct
	arguments
		{
		string exepath_;
		string anifile_;
		string pmapfile_;
		string cmapfile_;
		string palfile_;
		string dyefile_;
		string outfile_;
		int crop_;
		bool png_out_;
		bool mov_out_;
		bool gif_out_;
		bool pal_out_;
		unsigned background_;
		reader_mode::enum_ mode_;
		arguments():
			crop_(0),
			png_out_(false),
			mov_out_(false),
			gif_out_(false),
			pal_out_(false),
			background_(4294967295),
			mode_(reader_mode::none)
			{
			}
		};

	void search_palette_filenames(reader_mode::enum_ & mode, string const & ani, string & pmap, string & cmap, string & pal, string & dye)
		{
		if (mode == reader_mode::lines || mode == reader_mode::colors || mode == reader_mode::shade || mode == reader_mode::dye)
			return;

		if(pmap.empty())
			pmap = fun_lib::file_search(fun_lib::get_directory(ani), "palettes", "pmap");

		if (!fun_lib::file_exists(pmap.c_str()))
			throw(errors_lib::file_missing(pmap.c_str()));

		string ppath = fun_lib::get_directory(pmap);
		string pname = fun_lib::remove_extension(fun_lib::get_filename(pmap));
		if(cmap.empty())
			cmap = ppath + pname + "_colormap.pal";

		if (!fun_lib::file_exists(cmap.c_str()))
			throw(errors_lib::file_missing(cmap.c_str()));

		if(pal.empty())
			pal = ppath + pname + "_1p.pal";

		if (!fun_lib::file_exists(pal.c_str()))
			throw errors_lib::file_missing(pal.c_str());

		if(dye.empty())
			{
			string tdye = ppath + pname + "_1p_dye.pal";
			if (fun_lib::file_exists(tdye.c_str()))
				{
				mode = reader_mode::full;
				dye = tdye;
				}
			else
				mode = reader_mode::old;
			}
		else
			mode = fun_lib::file_exists(dye.c_str()) ? reader_mode::full : mode = reader_mode::old;

		if(!dye.empty())
			if (!fun_lib::file_exists(dye.c_str()))
				throw errors_lib::file_missing(dye.c_str());

		}
	arguments
	parse_arguments( int argc, char const * argv[] )
		{
		arguments p;
		assert( argc > 0 );
		if(argc==1)
			throw errors_lib::need_help();
		else if (argc==2)
			{
			if( !strcmp( argv[1], "-h" ) || !strcmp( argv[1], "-help" ) )
				throw errors_lib::need_help();
			else if(fun_lib::find_strings(argv[1], "-"))
				throw errors_lib::bad_command_line(argv[1]);

			p.anifile_ = argv[1];
			if (!fun_lib::find_strings(p.anifile_, ".ani"))
				throw errors_lib::bad_command_line(argv[1]);

			if (!fun_lib::file_exists(p.anifile_.c_str()))
				throw errors_lib::file_missing(p.anifile_.c_str());

			p.gif_out_ = true;
			//p.png_out_ = true;
			//p.mov_out_ = true;
			search_palette_filenames(p.mode_, p.anifile_, p.pmapfile_, p.cmapfile_, p.palfile_, p.dyefile_);
			}
		else
			{
			for( int i = 1; i < argc; ++i )
				{
				if( !strcmp( argv[i], "-h" ) || !strcmp( argv[i], "-help" ) )
					throw errors_lib::need_help();
				else if( !strcmp( "-ani", argv[i] ) )
					{
					if( ++i == argc )
						throw errors_lib::bad_command_line(argv[i]);
					if( p.anifile_.empty() )
						p.anifile_ = argv[i];
					}
				else if( !strcmp( "-out", argv[i] ) )
					{
					if( ++i == argc )
						throw errors_lib::bad_command_line(argv[i]);
					if( p.outfile_.empty() )
						p.outfile_ = argv[i];
					}
				else if( !strcmp( "-cmap", argv[i] ) )
					{
					if( ++i == argc )
						throw errors_lib::bad_command_line(argv[i]);
					if( p.pmapfile_.empty() )
						p.pmapfile_ = argv[i];
					}
				else if( !strcmp( "-pmap", argv[i] ) )
					{
					if( ++i == argc )
						throw errors_lib::bad_command_line(argv[i]);
					if( p.pmapfile_.empty() )
						p.pmapfile_ = argv[i];
					}
				else if( !strcmp( "-pal", argv[i] ) )
					{
					if( ++i == argc )
						throw errors_lib::bad_command_line(argv[i]);
					if( p.palfile_.empty() )
						p.palfile_ = argv[i];
					}
				else if( !strcmp( "-dyepal", argv[i] ) )
					{
					if( ++i == argc )
						throw errors_lib::bad_command_line(argv[i]);
					if( p.dyefile_.empty() )
						p.dyefile_ = argv[i];
					}
				else if( !strcmp( "-crop", argv[i] ) )
					{
					if( ++i == argc )
						throw( errors_lib::bad_command_line(argv[i]) );
					p.crop_ = stoi(argv[i]);
					}
				else if( !strcmp( "-bg", argv[i] ) )
					{
					if( ++i == argc )
						throw errors_lib::bad_command_line(argv[i]);

					unsigned char rgba[4] = {255,255,255,255};
					for(int c = 0 ; i < argc; i++)
						{
						string argu = argv[i];
						if(fun_lib::int_in_string(argu))
							{
							rgba[c] = (unsigned char)stoi(argv[i]);
							c++;
							}
						else if(c < 3)
							throw errors_lib::bad_command_line(argv[i]);
						else
							{
							i--;
							break;
							}
						}
					p.background_ = (rgba[0] << 0) | (rgba[1] << 8) | (rgba[2] << 16) | (rgba[3] << 24);
					}
				else if(!strcmp( "-gif", argv[i]))
					p.gif_out_ = true;
				else if(!strcmp( "-png", argv[i]))
					p.png_out_ = true;
				else if(!strcmp( "-movie", argv[i]))
					p.mov_out_ = true;
				else if(!strcmp( "-lines", argv[i]))
					p.mode_ = reader_mode::lines;
				else if(!strcmp( "-colors", argv[i]))
					p.mode_ = reader_mode::colors;
				else if(!strcmp( "-shade", argv[i]))
					p.mode_ = reader_mode::shade;
				else if(!strcmp( "-dye", argv[i]))
					p.mode_ = reader_mode::dye;
				else if(!strcmp( "-palout", argv[i]))
					p.pal_out_ = true;
				}
			if(p.anifile_.empty())
				throw(errors_lib::need_anifile_entry());

			if (!fun_lib::file_exists(p.anifile_.c_str()))
				throw(errors_lib::file_missing(p.anifile_.c_str()));

			if(p.mode_==reader_mode::none)
				search_palette_filenames(p.mode_, p.anifile_, p.pmapfile_, p.cmapfile_, p.palfile_, p.dyefile_);
			}
		p.exepath_ = fun_lib::get_directory(argv[0]);
		if(p.outfile_.empty())
			{
			if(argc>2)
				p.outfile_ = fun_lib::remove_extension(fun_lib::get_filename(p.anifile_));
			else
				p.outfile_ = p.exepath_ + fun_lib::remove_extension(fun_lib::get_filename(p.anifile_));

			switch(p.mode_)
				{
				case reader_mode::colors:
					p.outfile_ += "_colors";
					break;
				case reader_mode::lines:
					p.outfile_ += "_lines";
					break;
				case reader_mode::shade:
					p.outfile_ += "_shade";
					break;
				case reader_mode::dye:
					p.outfile_ += "_dye";
					break;
				}
			}
		if(!p.gif_out_ && !p.png_out_ && !p.mov_out_)
			p.gif_out_ = true;

		assert(!p.anifile_.empty()); 
		assert(!p.outfile_.empty());
		return p;
		}

	int show_help()
		{
		cout << "\nRequired File Entry: -ani path/file.ani"
		<< "\n"
		<< "\nOptional File Entries: -out path/file"
		<< "\n-pmap path/file.pmap -cmap path/file_colormap.pal"
		<< "\n-pal path/file_1p.pal -dyepal path/file_1p_dye.pal"
		<< "\n"
		<< "\nOptional Values:"
		<< "\n-bg r g b a <or> -bg r g b (sets background color range 0 - 255)"
		//<< "\n-crop value (crops excessive pixels leaving an amount of buffer pixels)"
		<< "\n"
		<< "\nOptional Flags:"
		<< "\n-gif (output will be a gif file)"
		<< "\n-png (output will be a series of png files)"
		<< "\n-movie (output will be a series of png frame to used for a movie)"
		<< "\n-lines (will only process lines layer files)"
		<< "\n-colors (will only process colors layer files)"
		<< "\n-shade (will only process shade layer files)"
		<< "\n-dye (will only process dye layer files)"
		<< "\n-palout (outputs the palette data as a png file)"
		<< "\n-help <or> -h (calls help)"
		<< "\n"
		<< "\nOptional values will be filled out with defaults and existing files.\nOutput will be set at the exe location.\n";
		return 1;
		}
	}

namespace
aniexp
	{
	using namespace image_lib;
	using namespace errors_lib;
	int exporter( int argc, char const * argv[ ] )
		{
		cout << "Animation Exporter Version 0.170105\n";
		arguments cfg;
		try
			{
			if(argc==1)
				throw need_help();
			else if (argc==2)
				cout <<"Running in AUTO mode!\n";
			else
				cout <<"Running in MANUAL mode!\n";

			cfg = parse_arguments(argc,argv);
			vector<frame> ani_out = read_data_files(cfg.mode_, cfg.background_, cfg.anifile_.c_str(), cfg.pmapfile_.c_str(), cfg.cmapfile_.c_str(),
				cfg.palfile_.c_str(), cfg.dyefile_.c_str(), cfg.exepath_.c_str(), cfg.pal_out_);			
			assert(!ani_out.empty());
			if(cfg.png_out_)
				{
				int c = 0;
				CreateDirectory(cfg.outfile_.c_str(), 0);
				for(vector<frame>::iterator i = ani_out.begin(), e = ani_out.end(); i!=e; i++, c++)
					{
					std::string num = std::to_string(c);
					std::string cnt;
					for(int j=num.length(); j!=2; ++j)
						cnt += "0";
					cnt += num;
					int time = (int)((i->repeats_ * 1.667f) + 0.5f)*10;
					std::string tmp = cfg.outfile_ + "/" + cnt + "_" + to_string(time) + "_ms.png";
					timer_action time_it("Saved", tmp);
					if (save_texture_png(*i->p_image_, tmp.c_str()))
						time_it.action_success();
					else
						throw errors_lib::unable_to_save(tmp.c_str());
					}
				}
			if(cfg.mov_out_)
				{
				int c = 0;
				string mov_dir = cfg.outfile_ + "_movie";
				CreateDirectory(mov_dir.c_str(), 0);
				for(vector<frame>::iterator i = ani_out.begin(), e = ani_out.end(); i!=e; i++)
					{
					for(int rep=0; rep<i->repeats_; rep++)
						{
						std::string cnt;
							{
							std::string num = std::to_string(c++);
							for(int j=num.length(); j!=4; ++j)
								cnt += "0";
							cnt += num;
							}
						std::string tmp = mov_dir + "/" + cnt + ".png";
						timer_action time_it("Saved", tmp);
						if (save_texture_png(*i->p_image_, tmp.c_str()))
							time_it.action_success();
						else
							throw errors_lib::unable_to_save(tmp.c_str());
						}
					}
				}
			if(cfg.gif_out_)
				{
				string outfile = cfg.outfile_ + ".gif";
				timer_action time_it("Saved",outfile);
				GifWriter gifw;
				GifBegin( &gifw, outfile.c_str(), ani_out[0].p_image_->width_, ani_out[0].p_image_->height_, false);
				std::string gstrt = "Going to write " + to_string(ani_out.size()) + " frames to gif.";
				cout << gstrt << endl;
				unsigned count = 0;
				for(vector<frame>::iterator i = ani_out.begin(), e = ani_out.end(); i!=e; i++)
					{
					int time = (int)((i->repeats_ * 1.667f) + 0.5f);
					GifWriteFrame(&gifw, i->p_image_->pixels_, i->p_image_->width_, i->p_image_->height_, time);
					std::string gmsg = "Writing frame: " + to_string(++count);
					cout << gmsg << endl;
					}
				GifEnd( &gifw );
				time_it.action_success();
				}
			return 1;
			}
		catch(errors_lib::need_help)
			{
			return show_help();
			}
		catch(errors_lib::need_anifile_entry)
			{
			cerr << "No ANI file was specified!\n";
			return 2;
			}
		catch(errors_lib::bad_command_line & e)
			{
			cerr << "Bad command line arguments!\n" << e.msg_ << endl;
			return 3;
			}
		catch(errors_lib::unable_to_save & e)
			{
			cerr << "Unable to save file!\n" << e.msg_ << endl;
			return 4;
			}
		catch(errors_lib::file_missing & e)
			{
			cerr << "Unable to find file! Try manually checking if file exist!\n" << e.msg_ << endl;
			return 5;
			}
		catch(errors_lib::bad_info_data & e)
			{
			cerr << e.msg_ << endl;
			return 6;
			}
		catch(...)
			{
			cerr << "Unhandled exception!\n";
			return 9;
			}
		}
	}
