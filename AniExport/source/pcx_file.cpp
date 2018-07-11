//Copyright (c) Brigido Rodriguez, all rights reserved.
#include "../header/pcx_file.h"
#include <sys/stat.h>
#include <assert.h>

int pcx_read(std::vector<unsigned char> & out, unsigned & width, unsigned & height, const char * filename)
	{
	struct pcx_header
		{
		char ID;
		char version;
		char encoding;
		char bits_per_pixel;
		short int x_min;
		short int y_min;
		short int x_max;
		short int y_max;
		short x_resolution;
		short y_resolution;
		char palette[48];
		char reserved;
		char number_of_bit_planes;
		short bytes_per_line;
		short palette_type;
		short x_screen_size;
		short y_screen_size;
		char filler[54];
		} pcx_header;

	struct stat input_file_stat;
	stat (filename, &input_file_stat);
	int input_file_size=input_file_stat.st_size;
	unsigned char * input_file_buffer=(unsigned char*)malloc(input_file_size);
		{
		if (input_file_buffer==NULL)
			return 0;

		FILE *input_file;
		fopen_s(&input_file,filename,"rb");
		if (input_file==NULL)
			return 0;

		fread(input_file_buffer,input_file_size,1,input_file);
		fclose(input_file);
		}
	memcpy(&pcx_header,input_file_buffer,128);
	if (pcx_header.bits_per_pixel!=8)
		return 0;

	unsigned char pcx_image_palette[768];
	for (int i=0;i<768;i++)
		pcx_image_palette[i]=(input_file_buffer[input_file_size-768+i]);

	width = pcx_header.x_max-pcx_header.x_min+((pcx_header.x_max%2)? 1 : 2);
	height = pcx_header.y_max-pcx_header.y_min+1;
	int const pcx_buffer_size=width*height;
	std::vector<unsigned char> pcx_buffer(pcx_buffer_size);
	for(int i=0, c=0, ofs=0; c<pcx_buffer_size; i++)
		{
		int current_byte=input_file_buffer[i+128];
		if (current_byte>192)
			{
			int run_count = current_byte-192;
			for (int pos=0; pos!=run_count; pos++)
				{
				if (c>=pcx_buffer_size)
					break;
				int tmp = ofs+pos;
				assert(tmp<pcx_buffer_size);
				pcx_buffer[tmp]=input_file_buffer[i+128+1];
				c++;
				}	
			ofs+=run_count;
			i++;
			}
		else
			{
			if(pcx_buffer_size>ofs)
				{
				assert(ofs<pcx_buffer_size);
				pcx_buffer[ofs]=current_byte;
				c++;
				ofs++;
				}
			}
		}
	free(input_file_buffer);
	std::vector<unsigned char> out_pixels(pcx_buffer_size*4);
	for(int i=0, c=0; i<pcx_buffer_size; i++)
		{
		int pos = pcx_buffer[i]*3;
		out_pixels[c] = pcx_image_palette[pos]; c++;
		out_pixels[c] = pcx_image_palette[pos+1]; c++;
		out_pixels[c] = pcx_image_palette[pos+2]; c++;
		out_pixels[c] = 255; c++;
		}
	out_pixels.swap(out);
	return 1;
	}
