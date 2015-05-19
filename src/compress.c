#include <setjmp.h>
#include <stdio.h>
#include <jpeglib.h>
#include <turbojpeg.h>
#include <stdlib.h>

#include "compress.h"
#include "utils.h"

//TODO Error handling

void cclt_compress(char* output_file, unsigned char* image_buffer, cclt_compress_parameters* pars) {
	FILE* fp;
	tjhandle tjCompressHandle;
	unsigned char* output_buffer;
	int status;
	unsigned long output_size = 0;

	fp = fopen(output_file, "wb");

	//Check for errors
	//TODO Use UNIX error messages
	if (fp == NULL) {
    	printf("INPUT: Failed to open output \"%s\"\n", output_file);
    	return;
    }

    output_buffer = NULL;
    tjCompressHandle = tjInitCompress();

    status = tjCompress2(tjCompressHandle,
    	image_buffer,
    	pars->width,
    	0,
    	pars->height,
    	pars->color_space,
    	&output_buffer,
    	&output_size,
    	pars->subsample,
    	pars->quality,
    	pars->dct_method);


    fwrite(output_buffer, 1, output_size, fp);

    fclose(fp);
    tjDestroy(tjCompressHandle);
    tjFree(output_buffer);

}

unsigned char* cclt_decompress(char* fileName, cclt_compress_parameters* pars) {

    FILE *file = NULL;
    int res = 0;
    long int sourceJpegBufferSize = 0;
    unsigned char* sourceJpegBuffer = NULL;
    tjhandle tjDecompressHandle;
    int fileWidth = 0, fileHeight = 0, jpegSubsamp = 0;

    file = fopen(fileName, "rb");
    res = fseek(file, 0, SEEK_END);
    sourceJpegBufferSize = ftell(file);
    sourceJpegBuffer = tjAlloc(sourceJpegBufferSize);

    res = fseek(file, 0, SEEK_SET);
    res = fread(sourceJpegBuffer, (long)sourceJpegBufferSize, 1, file);
    tjDecompressHandle = tjInitDecompress();
    res = tjDecompressHeader2(tjDecompressHandle, sourceJpegBuffer, sourceJpegBufferSize, &fileWidth, &fileHeight, &jpegSubsamp);

    int destWidth = fileWidth;
    int destHeight = fileHeight;
    pars->subsample = jpegSubsamp;



    unsigned char* temp = tjAlloc(destHeight * destWidth * tjPixelSize[TJPF_RGB]);


    res = tjDecompress2(tjDecompressHandle,
                                 sourceJpegBuffer,
                                 sourceJpegBufferSize,
                                 temp,
                                 destWidth,
                                 0,
                                 destHeight,
                                 pars->color_space,
                                 TJFLAG_ACCURATEDCT);

    tjDestroy(tjDecompressHandle);

    pars->width = destWidth;
    pars->height = destHeight;

    return temp;
}