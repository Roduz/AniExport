//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#ifndef _FILE_FUN_H_
#define _FILE_FUN_H_

#include <sys/stat.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <cctype>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace
reader_mode
	{
	enum enum_
		{
		none,
		full,
		old,
		colors,
		lines,
		shade,
		dye
		};
	}

namespace
fun_lib
	{
	using namespace std;

	inline bool file_exists(const char * name)
		{
		struct stat buffer;   
		return (stat (name, &buffer) == 0); 
		}
	inline string find_file(string & path, const char * ext)
		{
		WIN32_FIND_DATA FindFileData;
		string p = path + "*." + ext;
		LPCSTR param = p.c_str();
		HANDLE hFind = FindFirstFile(param, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE)
			return "";

		FindClose(hFind);
		string r = FindFileData.cFileName;
		return r;
		}
	inline bool get_line(ifstream & file, string & line, int & count)
		{
		count++;
		return (!getline(file,line).fail());
		}
	inline bool find_strings(string const & line, const char * st1)
		{
		return (line.find(st1) != string::npos);
		}
	inline bool find_strings(string const & line, const char * st1, const char * st2)
		{
		return ((line.find(st1) != string::npos) || (line.find(st2) != string::npos));
		}
	inline string trim_string(string const & s)
		{
		auto wsfront=find_if_not(s.begin(),s.end(),[](int c){return isspace(c);});
		auto wsback=find_if_not(s.rbegin(),s.rend(),[](int c){return isspace(c);}).base();
		return (wsback<=wsfront ? string() : string(wsfront,wsback));
		}
	inline string get_filename(string const & s)
		{
		return s.substr(size_t(s.find_last_of("/\\"))+1, s.length()-size_t(s.find_last_of("/\\")));
		}
	inline string get_directory(string const & s)
		{
		return s.substr(0, size_t(s.find_last_of("/\\"))) + "/";
		}
	inline string remove_extension(string const s)
		{
		return s.substr(0, size_t(s.find_last_of('.')));
		}
	inline string file_search(string const & dir, const char * search, const char * filetype)
		{
		string parse = dir;
		int size = parse.length();
		for(int i = size; i>=2; i--)
			{
			if(parse[i] == '/' || parse[i] == '\\')
				{
				string check = parse.substr(0, i) + "/" + search + "/";
				string ret = find_file(check, filetype);
				if(!ret.empty())
					return check + ret;
				}
			}
		return "";
		}
	inline string split_string_at(string const & s, char c, short at = 0)
		{
		int len = s.length();
		for(int i=0, b=0, e=0, p=0; i<len; i++, e++)
			{
			if(i == len-1)
				e++;

			if(s[i] == c || i == len-1)
				{
				if(p == at)
					return s.substr(b, e);
				else
					{
					p++;
					i++;
					b=i;
					e=0;
					}
				}
			}
		return s;
		}
	inline bool int_in_string(string & s)
		{
		short const len = s.length();
		for(int i=0; i<len; i++)
			{
			if(!(s[i] >= '0' && s[i] <= '9'))
				return false;
			}
		return true;
		}
	inline unsigned short get_int_string(string & s, bool start, const char c)
		{
		string num;
		int len = s.length();
		bool scan = false;
		for(int i=0; i<len; i++)
			{
			if(start)
				{
				if (!scan)
					scan = true;

				if ((s[i] == c) && scan)
					break;
				}
			else
				{
				if ((s[i] == c) && !scan)
					scan = true;
				}
			if(scan)
				{
				if(s[i]>='0' && s[i]<='9')
					num += s[i];
				else if (num.length()>0)
					break;
				else if (s[i] == 35)
					break;
				}
			}

		if (num.length()>0)
			return stoi(num);
		else
			return 0;
		}
	inline int get_RGBA_channel_pixel(string & s)
		{
		int pix = 0;
		int len = s.length();
		unsigned char nums[4] = {255,255,255,255};
		string tmp;
		for(int i=0, c=0; i<len; i++)
			{
			if(s[i]>='0' && s[i]<='9')
				{
				tmp += s[i];
				if (i==len-1)
					{
					nums[c] = (char)stoi(tmp);
					tmp.clear();
					break;
					}
				}
			else if(s[i]=='#' || s[i]=='/0')
				{
				if (tmp.empty())
					break;
				else if (c<4)
					{
					nums[c] = (char)stoi(tmp);
					tmp.clear();
					break;
					}
				}
			else
				{
				if (tmp.empty())
					continue;
				else if (c<4)
					{
					nums[c] = (char)stoi(tmp);
					tmp.clear();
					c++;
					}
				}
			}
		pix = (nums[0] << 0) | (nums[1] << 8) | (nums[2] << 16) | (nums[3] << 24);
		return pix;
		}
	}
#endif
