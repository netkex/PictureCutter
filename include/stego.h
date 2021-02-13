#pragma once

typedef struct {
	int x, y;
	char colour;
} CodeList;


void encrypt(char *ImgFile, char *msgFile, char *keyFile, char *resImgFile);
void decrypt(char *ImgFile, char *keyFile, char *msgFile);