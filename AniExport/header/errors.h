//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#ifndef _ERRORS_H_
#define _ERRORS_H_

#include <exception>
#include <string>

namespace
errors_lib
	{
	struct bad_command_line: public std::exception { std::string msg_; bad_command_line( const char * msg ); };
	struct unable_to_save: public std::exception { std::string msg_; unable_to_save( const char * msg ); };
	struct file_missing: public std::exception { std::string msg_; file_missing( const char * msg ); };
	struct bad_info_data: public std::exception { std::string msg_; bad_info_data( const char * msg ); };
	struct need_help: public std::exception { need_help(); };
	struct need_anifile_entry: public std::exception { need_anifile_entry(); };
	}

#endif