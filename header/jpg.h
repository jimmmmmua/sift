#ifndef __JPG__
#define __JPG__
#include "mat.h"
Mat* read_jpg(char* path);
int write_bmp(Mat* image, char* path);

typedef struct RGBQUAD{
    U8 R;
    U8 G;
    U8 B;
    U8 reserver;
}RGBQUAD;

#endif
