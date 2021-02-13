#include "bmp.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define sizeOfInt 4 // константа 

static void swapRB(Pixel* pixel) {
	char tempRGB = pixel->R;
	pixel->R = pixel->B;
	pixel->B = tempRGB;
}	

static void swapAllImgRB(BMPimage *img) {
	for (int j = 0; j < img->height; j++) {
		for (int i = 0; i < img->width; i++) {
			swapRB(&img->pict[j][i]);
		}
	}
}

static int allocMemoryImg(BMPimage *img) {
	img->pict = (Pixel **)malloc(sizeof(Pixel *) * img->height);
	if (img->pict == NULL) {
		printf("Memory allocation error!\n");
		return 1;
	}
	Pixel *allocated_array = (Pixel *)malloc(sizeof(Pixel *) * img->height * img->width);
	if (allocated_array == NULL) {
		free(img->pict);
		printf("Memory allocation error!\n");
		return 1;
	}
	for (int i = 0; i < img->height; i++) {
		img->pict[i] = allocated_array + i * img->width;
	}
	return 0;
}

void distructorPicture(BMPimage *img) {
	free(img->pict[0]); // free memory for image
	free(img->pict); // free memory for array of pointers
}

// size in bytes for one image line 1xW
static inline int oneLineSize(int w) {
	return (sizeof(Pixel) * w + (sizeOfInt - 1)) / sizeOfInt * sizeOfInt;
}

// size of trash bytes for one image line 1xW
static inline int LineTrashBytes(int w) { // (4 - (x % 4)) % 4
	return (sizeOfInt - (sizeof(Pixel) * w) % sizeOfInt) % sizeOfInt;
}


static void readTrash(FILE *imgFile, int w) {
	static char trash[sizeOfInt]; 
	int modBytes = LineTrashBytes(w);
	if (modBytes == 0) 
		return;
	fread(&trash, sizeof(char), modBytes, imgFile);
}

static void writeTrash(FILE *imgFile, int w) {
	static char trash[sizeOfInt] = {0, 0, 0, 0};
	int modBytes = LineTrashBytes(w);
	if (modBytes == 0) 
		return;
	fwrite(&trash, sizeof(char), modBytes, imgFile);
}

int loadBMP(char *fileName, BMPimage* img) {
	FILE *imgFile = fopen(fileName,"rb");
	if (imgFile == NULL) {
		printf("Failed to open file: %s\n", fileName);
		return 1;
	}
	//read headers
	fread(&img->headers.fileHeader, sizeof(BMPfileHeader), 1, imgFile);
	fread(&img->headers.infoHeader, sizeof(BMPinfoHeader), 1, imgFile);
	fseek(imgFile, img->headers.fileHeader.offset, SEEK_SET);

	img->width = img->headers.infoHeader.width, img->height = img->headers.infoHeader.height;
	if (allocMemoryImg(img) != 0) 
		return 1;
	for (int j = 0; j < img->height; j++) {
		fread(img->pict[img->height - 1 - j], sizeof(Pixel), img->width, imgFile);
		readTrash(imgFile, img->width);
	}
	swapAllImgRB(img);
	fclose(imgFile);
	return 0;
}

static int cropPicture(BMPimage *img, int x, int y, int w, int h, BMPimage *resImg) {
	if (w <= 0 || h <= 0 || x + w > img->width || y + h > img->height) {
		printf("Error! Out of bounds!\n");
		return 1;
	}
	resImg->width = w, resImg->height = h;
	if (allocMemoryImg(resImg) != 0) 
		return 1;
	for (int j = 0; j < h; j++) {
		memcpy(resImg->pict[j], &img->pict[y + j][x], sizeof(Pixel) * w);
	}
	resImg->headers = img->headers;
	return 0;
}

static void fixHeaders(int w, int h, Headers *headers) {
	int fileSize = sizeof(BMPfileHeader) + sizeof(BMPinfoHeader) + (oneLineSize(w) * h);
	headers->fileHeader.size = fileSize;
	headers->infoHeader.width = w, headers->infoHeader.height = h;
	headers->infoHeader.imagesize = h * oneLineSize(w);
}

int writeBMP(char *fileName, BMPimage *img) {
	fixHeaders(img->width, img->height, &(img->headers));
	FILE *imgFile = fopen(fileName, "wb");
	if (imgFile == NULL) {
		printf("Failed to open file: %s\n", fileName);
		return 1;
	}
	fwrite(&img->headers.fileHeader, sizeof(BMPfileHeader), 1, imgFile);
	fwrite(&img->headers.infoHeader, sizeof(BMPinfoHeader), 1, imgFile);
	swapAllImgRB(img);
	for (int j = img->height - 1; j >= 0; j--) {
		fwrite(img->pict[j], sizeof(Pixel), img->width, imgFile);
		writeTrash(imgFile, img->width);
	}
	fclose(imgFile);
	return 0;
}

int crop(char *imgFileName, char *resImgFileName, int x, int y, int w, int h) {
	BMPimage img, resImg;
	if (loadBMP(imgFileName, &img) != 0) 
		return 1;
	if (cropPicture(&img, x, y, w, h, &resImg) != 0) {
		distructorPicture(&img);
		return 1;
	}
	if (writeBMP(resImgFileName, &resImg) != 0) {
		distructorPicture(&img);
		distructorPicture(&resImg);
		return 1;
	}
	distructorPicture(&img);
	distructorPicture(&resImg);
	return 0;
}