#include "bmp.h"
#include "stego.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static const int oneCharCodeSize = 5;

static int charToInt(char c) {
	if ('A' <= c && c <= 'Z') 
		return c - 'A';
	if (c == ' ')
		return 26;
	if (c == '.')
		return 27;
	if (c == ',')
		return 28;
	return -1;
}

static char intToChar(int num) {
	if (num < 26) 
		return 'A' + num;
	if (num == 26)
		return ' ';
	if (num == 27)
		return '.';
	if (num == 28)
		return ',';
	return '\0';
}

static int getMsgLength(const char *msgFileName) {
	FILE *msgFile = fopen(msgFileName, "r");
	char c;
	int len = 0;
	while (1) {
		int count = fscanf(msgFile, "%c", &c);
		if (count != 1)
			break;
		if(charToInt(c) != -1) 
			len++;
	}
	fclose(msgFile);
	return len;
}

static int getKeyLength(const char *keyFileName) {
	FILE *keyFile = fopen(keyFileName, "r");
	int x, y, len = 0;
	char col;
	while(1) {
		int count = fscanf(keyFile, "%d %d %c", &x, &y, &col);
		if (count != 3) 
			break;
		len++;
	}
	fclose(keyFile);
	return len;
}



static char *readMsg(const char *msgFileName, int msgLen) {
	FILE *msgFile = fopen(msgFileName, "r");
	char *msg = (char *)malloc(sizeof(char) * (msgLen + 1));
	int pos = 0;
	char c;
	while (1) {
		int count = fscanf(msgFile, "%c", &c);
		if (count != 1)
			break;
		if(charToInt(c) != -1) {
			msg[pos++] = c;
		}
	}
	msg[pos] = '\0';
	fclose(msgFile);
	return msg;
}

static CodeList *readKey(const char *keyFileName, int keyLen) {
	FILE *keyFile = fopen(keyFileName, "r");
	CodeList *key = (CodeList *)malloc(sizeof(CodeList) * keyLen);
	int pos = 0;
	while (1) {
		int count = fscanf(keyFile, "%d %d %c", &key[pos].x, &key[pos].y, &key[pos].colour);
		if (count != 3) 
			break;
		pos++;
	}
	fclose(keyFile);
	return key;
}

void codeOneBit(BMPimage *img, CodeList key, int bit) {
	if (key.colour == 'R') {
		img->pict[key.y][key.x].R = img->pict[key.y][key.x].R / 2 * 2 + bit;
	}
	if (key.colour == 'G') {
		img->pict[key.y][key.x].G = img->pict[key.y][key.x].G / 2 * 2 + bit;
	}
	if (key.colour == 'B') {
		img->pict[key.y][key.x].B = img->pict[key.y][key.x].B / 2 * 2 + bit;
	}
}

int decodeOneBit(BMPimage *img, CodeList key) {
	if (key.colour == 'R')
		return (img->pict[key.y][key.x].R & 1);
	if (key.colour == 'G')
		return (img->pict[key.y][key.x].G & 1);
	if (key.colour == 'B')
		return (img->pict[key.y][key.x].B & 1);
	return 0;
}

void encrypt(char *ImgFile, char *msgFile, char *keyFile, char *resImgFile) {
	int msgLen = getMsgLength(msgFile);
	char *msg = readMsg(msgFile, msgLen);
	CodeList *keys = readKey(keyFile, getKeyLength(keyFile));
	BMPimage img;
	loadBMP(ImgFile, &img);
	int pos = 0;
	for (int i = 0; i < msgLen; i++) {
		int charCode = charToInt(msg[i]);
		for (int j = 0; j < oneCharCodeSize; j++) {
			int curBit = (charCode & (1 << j)) >> j;
			codeOneBit(&img, keys[pos++], curBit);
		}
	}
	writeBMP(resImgFile, &img);
	
	distructorPicture(&img);
	free(keys);
	free(msg);
}

static void writeMsg(char *msgFileName, char *msg) {
	FILE *msgFile = fopen(msgFileName, "w");
	fprintf(msgFile, "%s\n", msg);
	fclose(msgFile);
}

void decrypt(char *ImgFile, char *keyFile, char *msgFile) {
	BMPimage img;
	loadBMP(ImgFile, &img);
	int keyLen = getKeyLength(keyFile);
	CodeList *keys = readKey(keyFile, keyLen);
	int msgLen = keyLen / 5;
	char *msg = (char *)malloc(msgLen * sizeof(char) + 1);
	int pos = 0;
	for (int i = 0; i < msgLen; i++) {
		int curSymb = 0;
		for (int j = 0; j < oneCharCodeSize; j++) {
			int curBit = decodeOneBit(&img, keys[pos++]);
			curSymb |= curBit * (1 << j);
		}
		msg[i] = intToChar(curSymb);
	}
	msg[msgLen] = '\0';
	writeMsg(msgFile, msg);

	distructorPicture(&img);
	free(keys);
	free(msg);
}