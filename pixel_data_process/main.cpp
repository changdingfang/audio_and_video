 //***************************************************************
 // @file:    main.cpp
 // @author:  dingfang
 // @date    2019-02-27 17:35:00
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>

typedef struct _T_BmpHead
{
	/*
	unsigned short int	bfType;
	unsigned int32_t	bfSize;
	unsigned short int	bfReserverd1;
	unsigned short int	bfReserverd2;
	unsigned int32_t	bfbfoffBits;
	*/

	int32_t imageSize;
	int32_t blank;
	int32_t startPosition;
}BmpHead_T;

typedef struct _T_InfoHead
{
	/*
	int32_t	biSize;
	int32_t	biWidth;
	int32_t	biHeight
	unsigned short int biPlanes;
	unsigned short int biBitcount;
	unsigned short int biCompression;
	int32_t	biSizeImage;
	int32_t	biXPelsPermeter;
	int32_t	biYPelsPermeter;
	int32_t	biClrUsed;
	int32_t	biClrImportant;
	*/

	int32_t length;
	int32_t width;
	int32_t height;
	unsigned short int colorPlane;
	unsigned short int bitColor;
	int32_t zipFormat;
	//unsigned short int zipFormat;
	int32_t realSize;
	int32_t xPels;
	int32_t yPels;
	int32_t colorUse;
	int32_t colorImportant;
}InfoHead_T;

using namespace std;

int main(void)
{
	cout << "BmpHead: " << sizeof(BmpHead_T) << endl;
	cout << "InfoHead: " << sizeof(InfoHead_T) << endl;

	return 0;
}
