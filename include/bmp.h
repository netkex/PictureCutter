#include <stdint.h>
#pragma once

#pragma pack(push, 1)
/** structures comments */
typedef struct {
    uint16_t type;                 /* identifier            */
    uint32_t size;                       /* File size in bytes          */
    uint32_t reserved;
    uint32_t offset;                     /* Offset to image data, bytes */
} BMPfileHeader;


typedef struct {
    uint32_t size;               /* Header size in bytes      */
    int32_t width;
    int32_t height;                /* Width and height of image */
    uint16_t planes;       /* Number of colour planes   */
    uint16_t bits;         /* Bits per pixel            */
    uint32_t compression;        /* Compression type          */
    uint32_t imagesize;          /* Image size in bytes       */
    int32_t xresolution, yresolution;     /* Pixels per meter          */
    uint32_t ncolours;           /* Number of colours         */
    uint32_t importantcolours;   /* Important colours         */
} BMPinfoHeader;

typedef struct
{
    BMPfileHeader fileHeader;
    BMPinfoHeader infoHeader;
} Headers;

typedef struct 
{
    unsigned char R, G, B;
} Pixel;

#pragma pack(pop)

typedef struct {
    Headers headers;
    int width, height;
    Pixel **pict;
} BMPimage;


int loadBMP(char *imageName, BMPimage* img);
int writeBMP(char *fileName, BMPimage* img);
void distructorPicture(BMPimage* img);
int crop(char *imgFileName, char *resImgFileName, int x, int y, int w, int h);