//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#include "../header/errors.h"

namespace
errors_lib
	{
	bad_command_line::
	bad_command_line( const char * msg ):
		msg_(msg)
		{
		}
	unable_to_save::
	unable_to_save( const char * msg ):
		msg_(msg)
		{
		}
	file_missing::
	file_missing( const char * msg ):
		msg_(msg)
		{
		}
	bad_info_data::
	bad_info_data( const char * msg ):
		msg_(msg)
		{
		}
	need_help::
	need_help()
		{
		}
	need_anifile_entry::
	need_anifile_entry()
		{
		}
	}
