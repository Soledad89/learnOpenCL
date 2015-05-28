#ifndef __BMPFUNCS__
#define __BMPFUNCS__
#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uchar;

float* readImage(const char *filename, int* widthOut, int* heightOut);
void storeImage(float *imageOut, const char *filename, int rows, int cols, 
                const char* refFilename);

#ifdef __cplusplus
}
#endif
#endif
