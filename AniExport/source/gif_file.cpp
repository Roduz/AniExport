//Permission is hereby granted by Brigido Rodriguez to use as is without limitations.
#include "../header/gif_file.h"
#include "../header/octree.h"

#include <string.h>
#include <assert.h>

// Define these macros to hook into a custom memory allocator.
// TEMP_MALLOC and TEMP_FREE will only be called in stack fashion - frees in the reverse order of mallocs
// and any temp memory allocated by a function will be freed before it exits.
// MALLOC and FREE are used only by GifBegin and GifEnd respectively (to allocate a buffer the size of the image, which
// is used to find changed pixels for delta-encoding.)

#ifndef GIF_TEMP_MALLOC
#include <stdlib.h>
#define GIF_TEMP_MALLOC malloc
#endif

#ifndef GIF_TEMP_FREE
#include <stdlib.h>
#define GIF_TEMP_FREE free
#endif

#ifndef GIF_MALLOC
#include <stdlib.h>
#define GIF_MALLOC malloc
#endif

struct GifPalette
	{
    int bitDepth;
    uint8_t r[256];
    uint8_t g[256];
    uint8_t b[256];
	GifPalette(int bd, const uint32_t * palette):
		bitDepth(bd)
		{
		for(int i=0; i<256; i++)
			{
			r[i] = palette[i] >> 0;
			g[i] = palette[i] >> 8;
			b[i] = palette[i] >> 16;
			}
		}
	};

// Simple structure to write out the LZW-compressed portion of the image one bit at a time
struct GifBitStatus
	{
    uint8_t bitIndex;  // how many bits in the partial byte written so far
    uint8_t byte;      // current partial byte  
    uint32_t chunkIndex;
    uint8_t chunk[256];   // bytes are written in here until we have 256 of them, then written to the file
	};

// The LZW dictionary is a 256-ary tree constructed as the file is encoded, this is one node
struct GifLzwNode
	{
    uint16_t m_next[256];
	};

const int kGifTransIndex = 0;

int GifIMax(int l, int r) { return l>r?l:r; }
int GifIMin(int l, int r) { return l<r?l:r; }
int GifIAbs(int i) { return i<0?-i:i; }

void GifSwapPixels(uint8_t* image, int pixA, int pixB)
	{
    uint8_t rA = image[pixA*4];
    uint8_t gA = image[pixA*4+1];
    uint8_t bA = image[pixA*4+2];
    uint8_t aA = image[pixA*4+3];
    
    uint8_t rB = image[pixB*4];
    uint8_t gB = image[pixB*4+1];
    uint8_t bB = image[pixB*4+2];
    uint8_t aB = image[pixA*4+3];
    
    image[pixA*4] = rB;
    image[pixA*4+1] = gB;
    image[pixA*4+2] = bB;
    image[pixA*4+3] = aB;
    
    image[pixB*4] = rA;
    image[pixB*4+1] = gA;
    image[pixB*4+2] = bA;
    image[pixB*4+3] = aA;
	}

// Picks palette colors for the image using simple thresholding, no dithering
void GifThresholdImage( const uint8_t* lastFrame, uint8_t* nextFrame, const uint32_t width, const uint32_t height )
	{
	assert(lastFrame);
    uint32_t numPixels = width*height;
    for( uint32_t ii=0; ii<numPixels; ii++ )
		{
        // if a previous color is available, and it matches the current color, set the pixel to transparent
        if((lastFrame[0] == nextFrame[0]) && (lastFrame[1] == nextFrame[1]) && (lastFrame[2] == nextFrame[2]))
            nextFrame[3] = kGifTransIndex;

		nextFrame += 4;
        if(lastFrame)
			lastFrame += 4;
		}
	}

// insert a single bit
void GifWriteBit( GifBitStatus& stat, uint32_t bit )
	{
    bit = bit & 1;
    bit = bit << stat.bitIndex;
    stat.byte |= bit;
    ++stat.bitIndex;
    if( stat.bitIndex > 7 )
		{
        // move the newly-finished byte to the chunk buffer 
        stat.chunk[stat.chunkIndex++] = stat.byte;
        // and start a new byte
        stat.bitIndex = 0;
        stat.byte = 0;
		}
	}

// write all bytes so far to the file
void GifWriteChunk( FILE* f, GifBitStatus& stat )
	{
    fputc(stat.chunkIndex, f);
    fwrite(stat.chunk, 1, stat.chunkIndex, f);
    stat.bitIndex = 0;
    stat.byte = 0;
    stat.chunkIndex = 0;
	}

void GifWriteCode( FILE* f, GifBitStatus& stat, uint32_t code, uint32_t length )
	{
    for( uint32_t ii=0; ii<length; ii++ )
		{
        GifWriteBit(stat, code);
        code = code >> 1;
        if( stat.chunkIndex == 255 )
            GifWriteChunk(f, stat);

		}
	}

// write a 256-color (8-bit) image palette to the file
void GifWritePalette( const GifPalette* pPal, FILE* f )
	{
    fputc(0, f);  // first color: transparency
    fputc(0, f);
    fputc(0, f);
	int end = (1 << pPal->bitDepth);
	int cnt = 0;
    for(int ii=1; ii<(1 << pPal->bitDepth); cnt++, ii++)
		{
        uint32_t r = pPal->r[ii];
        uint32_t g = pPal->g[ii];
        uint32_t b = pPal->b[ii];
        fputc(r, f);
        fputc(g, f);
        fputc(b, f);
		}
	}

// write the image header, LZW-compress and write out the image
void GifWriteLzwImage(FILE* f, uint8_t* image, uint32_t left, uint32_t top,  uint32_t width, uint32_t height, uint32_t delay, GifPalette* pPal)
	{
    // graphics control extension
    fputc(0x21, f);
    fputc(0xf9, f);
    fputc(0x04, f);
    fputc(0x05, f); // leave prev frame in place, this frame has transparency
    fputc(delay & 0xff, f);
    fputc((delay >> 8) & 0xff, f);
    fputc(kGifTransIndex, f); // transparent color index
    fputc(0, f);
    fputc(0x2c, f); // image descriptor block
    fputc(left & 0xff, f);           // corner of image in canvas space
    fputc((left >> 8) & 0xff, f);
    fputc(top & 0xff, f);
    fputc((top >> 8) & 0xff, f);
    fputc(width & 0xff, f);          // width and height of image
    fputc((width >> 8) & 0xff, f);
    fputc(height & 0xff, f);
    fputc((height >> 8) & 0xff, f);
    //fputc(0, f); // no local color table, no transparency
    //fputc(0x80, f); // no local color table, but transparency
    fputc(0x80 + pPal->bitDepth-1, f); // local color table present, 2 ^ bitDepth entries
    GifWritePalette(pPal, f);
    const int minCodeSize = pPal->bitDepth;
    const uint32_t clearCode = 1 << pPal->bitDepth;
    fputc(minCodeSize, f); // min code size 8 bits
    GifLzwNode* codetree = (GifLzwNode*)GIF_TEMP_MALLOC(sizeof(GifLzwNode)*4096);
    memset(codetree, 0, sizeof(GifLzwNode)*4096);
    int32_t curCode = -1;
    uint32_t codeSize = minCodeSize+1;
    uint32_t maxCode = clearCode+1;
    GifBitStatus stat;
    stat.byte = 0;
    stat.bitIndex = 0;
    stat.chunkIndex = 0;
    GifWriteCode(f, stat, clearCode, codeSize);  // start with a fresh LZW dictionary
    for(uint32_t yy=0; yy<height; yy++)
		{
        for(uint32_t xx=0; xx<width; xx++)
			{
            uint8_t nextValue = image[(yy*width+xx)*4+3];
            // "loser mode" - no compression, every single code is followed immediately by a clear
            //WriteCode( f, stat, nextValue, codeSize );
            //WriteCode( f, stat, 256, codeSize );
            if( curCode < 0 )
				{
                // first value in a new run
                curCode = nextValue;
				}
            else if( codetree[curCode].m_next[nextValue] )
				{
                // current run already in the dictionary
                curCode = codetree[curCode].m_next[nextValue];
				}
            else
				{
                // finish the current run, write a code
                GifWriteCode( f, stat, curCode, codeSize );
                // insert the new run into the dictionary
                codetree[curCode].m_next[nextValue] = ++maxCode;
                if( maxCode >= (1ul << codeSize) )
					{
                    // dictionary entry count has broken a size barrier,
                    // we need more bits for codes
                    codeSize++;
					}
                if( maxCode == 4095 )
					{
                    // the dictionary is full, clear it out and begin anew
                    GifWriteCode(f, stat, clearCode, codeSize); // clear tree
                    
                    memset(codetree, 0, sizeof(GifLzwNode)*4096);
                    curCode = -1;
                    codeSize = minCodeSize+1;
                    maxCode = clearCode+1;
					}
                curCode = nextValue;
				}
			}
		}
    // compression footer
    GifWriteCode( f, stat, curCode, codeSize );
    GifWriteCode( f, stat, clearCode, codeSize );
    GifWriteCode( f, stat, clearCode+1, minCodeSize+1 );
    // write out the last partial chunk
    while( stat.bitIndex ) GifWriteBit(stat, 0);
    if( stat.chunkIndex ) GifWriteChunk(f, stat);
    fputc(0, f); // image block terminator
    GIF_TEMP_FREE(codetree);
	}

// Creates a gif file.
// The input GIFWriter is assumed to be uninitialized.
// The delay value is the time between frames in hundredths of a second - note that not all viewers pay much attention to this value.
bool GifBegin( GifWriter* writer, const char* filename, uint32_t width, uint32_t height, bool loop )
	{
	writer->f = 0;
    fopen_s(&writer->f, filename, "wb");
    if(!writer->f) return false;
	//writer->oldImage = NULL;
	writer->started = false;
    // allocate 
    //writer->oldImage = (uint8_t*)GIF_MALLOC(width*height*4);
    writer->oldImage.reserve(width*height*4);
    fputs("GIF89a", writer->f);
    // screen descriptor
    fputc(width & 0xff, writer->f);
    fputc((width >> 8) & 0xff, writer->f);
    fputc(height & 0xff, writer->f);
    fputc((height >> 8) & 0xff, writer->f);
    fputc(0xf0, writer->f);  // there is an unsorted global color table of 2 entries
    fputc(0, writer->f);     // background color
    fputc(0, writer->f);     // pixels are square (we need to specify this because it's 1989)
    // now the "global" palette (really just a dummy palette)
    // color 0: black
    fputc(0, writer->f);
    fputc(0, writer->f); 
    fputc(0, writer->f);
    // color 1: also black
    fputc(0, writer->f);
    fputc(0, writer->f);
    fputc(0, writer->f);
    if(loop)
		{
        // animation header
        fputc(0x21, writer->f); // extension
        fputc(0xff, writer->f); // application specific
        fputc(11, writer->f); // length 11
        fputs("NETSCAPE2.0", writer->f); // yes, really
        fputc(3, writer->f); // 3 bytes of NETSCAPE2.0 data
        fputc(1, writer->f); // JUST BECAUSE
        fputc(0, writer->f); // loop infinitely (byte 0)
        fputc(0, writer->f); // loop infinitely (byte 1)
        fputc(0, writer->f); // block terminator
		}
	return true;
	}

// Writes out a new frame to a GIF in progress.
// The GIFWriter should have been created by GIFBegin.
// AFAIK, it is legal to use different bit depths for different frames of an image -
// this may be handy to save bits in animations that don't change much.
bool GifWriteFrame( GifWriter * writer, std::vector<uint8_t> & image, uint32_t const width, uint32_t const height, uint32_t const delay )
	{
	std::vector<unsigned> palette;
	std::vector<unsigned char> pixels = image_lib::color_quant(image, palette, 256);
	uint32_t const bitDepth = 8;
    if(!writer->f)
		return false;

	GifPalette fpal(bitDepth, palette.data());
	if(writer->started)
		GifThresholdImage(writer->oldImage.data(), pixels.data(), width, height);
	else
		writer->started = true;

	writer->oldImage = pixels;
	GifWriteLzwImage(writer->f, writer->oldImage.data(), 0, 0, width, height, delay, &fpal);
	return true;
	}

// Writes the EOF code, closes the file handle, and frees temp memory used by a GIF.
// Many if not most viewers will still display a GIF properly if the EOF code is missing,
// but it's still a good idea to write it out.
bool GifEnd( GifWriter* writer )
	{
    if(!writer->f) return false;
    fputc(0x3b, writer->f); // end of file
    fclose(writer->f);
    writer->f = NULL;
    return true;
	}
