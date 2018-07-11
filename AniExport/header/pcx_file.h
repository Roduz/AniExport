//Copyright (c) Brigido Rodriguez, all rights reserved.
#ifndef _PCX_FILE_H_
#define _PCX_FILE_H_

#include <vector>

int pcx_read(std::vector<unsigned char> & out, unsigned & width, unsigned & height, const char * filename);

#endif