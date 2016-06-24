/*
 * WAVE file info reader.
 *
 * Compile:
 * $ cc -o wav_info wav_info.c
 *
 * Usage:
 * $ ./wav_info <file>
 *
 * Examples:
 * $ ./wav_info /path/to/file.wav
 *
 * Copyright (C) 2010 Alessandro Ghedini <alessandro@ghedini.me>
 * --------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Alessandro Ghedini wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we
 * meet some day, and you think this stuff is worth it, you can
 * buy me a beer in return.
 * --------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// struct wav_header{
// 	char	id[4];			/* must be "RIFF" */
// 	int	size;			/* file size - 8 */
// 	char	fmt[8];			/* must be "WAVEfmt " */
// 	int	format;
// 	short	pcm;            	/* 1 if PCM */
// 	short	channels;	
// 	int	frequency;	
// 	int	bytes_per_second;
// 	short	bytes_by_capture;
// 	short	bits_per_sample;
// 	char	data[4];
// 	int	bytes_in_data;
// };
#pragma pack(push, 1)
struct CHUNK
{
	char	sig[4];      
	int     size ;  	/* file size - 8 */
};

struct WAVE_FILE_HEADER
{
	char	sig[4];      
	int     size ;  	/* file size - 8 */
	char    riffType[4];
};


struct WAVE_FMT
{
	short   pcm;
	short	channels;
	int		frequency;
	int     bytes_per_second;
	short   blockAlign;
	short   bits_per_sample;
	short   ExtraFormatBytes;
};

#pragma pack(pop)




int main(int argc, char **argv) {
	FILE *fd;

	struct WAVE_FILE_HEADER header;

	if (argc < 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return -1;
	}
	
	printf("name,channels,hz,bps,sampes, duration,size\n");
	int i=1;
	int total = argc;
	for(i=1;i<total;i++)
	{
		//printf("name %s\n",argv[i]);

		fd = fopen(argv[i], "rb");
		if(fd==NULL)
		{
			printf("open %s failed\n", argv[i]);
			continue;
		}

		int ret= fread(&header, sizeof(header), 1, fd);
		
		if (strncmp(header.sig, "RIFF", 4)) {
			printf("File \"%s\" not valid WAV.\n", argv[1]);
			fclose(fd);
			continue;
		}

		const char* indexOfLastPath = strrchr(argv[i],'/');
		const char*szName = argv[i];
		if(NULL!=indexOfLastPath)
		{
			szName = indexOfLastPath+1;
		}


		int hitCount=0;
		struct CHUNK chunk;
		struct WAVE_FMT fmt;
		int sampeDataSize=0;
		while(!feof(fd))
		{
		
			ret= fread(&chunk, sizeof(chunk), 1, fd);
			if(ret<=0)
				break;
			
			long begPos = ftell(fd);

			if(memcmp(chunk.sig,"fmt ",4)==0)
			{
				hitCount ++;
				ret= fread(&fmt, sizeof(fmt), 1, fd);
				if(ret<=0)
					break;
			}
			else if(memcmp(chunk.sig,"data",4)==0)
			{
				hitCount ++;
				sampeDataSize = chunk.size;
			}

			if(hitCount==2)
			{
				break;
			}

			fseek(fd, chunk.size + begPos, SEEK_SET);
		}

		double length = 0;
		int channels = 0;
		int frequency=0;
		int sampes = 0;
		int bps = 0;
	

		frequency = fmt.frequency;
		channels = fmt.channels;
		length   = (double)sampeDataSize/(double)fmt.bytes_per_second;
		sampes   = sampeDataSize*8/fmt.bits_per_sample;
		bps      = fmt.bytes_per_second*8; 

	
		printf("%s,%d,%d,%d, %d, %.3f, %uk\n",szName
	 					   ,channels,
		   				   frequency,
		   				   bps,
		   				   sampes,
	                       length,
	                       sampeDataSize/1024);
		
		fclose(fd);

	}
	
	return 0;
}

