//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#ifndef _GIF_FILE_H_
#define _GIF_FILE_H_

#include <stdio.h>
#include <stdint.h>
#include <vector>

struct GifWriter
	{
    FILE* f;
    std::vector<uint8_t> oldImage;
    bool started;
	};

bool GifBegin(GifWriter* writer, const char* filename, uint32_t width, uint32_t height, bool loop);
bool GifWriteFrame( GifWriter * writer, std::vector<uint8_t> & image, uint32_t const width, uint32_t const height, uint32_t const delay);
bool GifEnd(GifWriter* writer);

#endif
