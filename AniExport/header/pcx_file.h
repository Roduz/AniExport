//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#ifndef _PCX_FILE_H_
#define _PCX_FILE_H_

#include <vector>

int pcx_read(std::vector<unsigned char> & out, unsigned & width, unsigned & height, const char * filename);

#endif