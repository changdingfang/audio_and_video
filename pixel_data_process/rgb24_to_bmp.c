 //***************************************************************
 // @file:    rgb24_to_bmp.c
 // @author:  dingfang
 // @date    2019-02-27 13:42:02
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

typedef struct _T_BmpHead
{
#if 0 
	unsigned short int	bfType;
	unsigned int32_t	bfSize;
	unsigned short int	bfReserverd1;
	unsigned short int	bfReserverd2;
	unsigned int32_t	bfbfoffBits;
#endif

	int32_t imageSize;
	int32_t blank;
	int32_t startPosition;
}BmpHead_T;

typedef struct _T_InfoHead
{
#if 0 
	int32_t	biSize;
	int32_t	biWidth;
	int32_t	biHeight
	unsigned short int biPlanes;
	unsigned short int biBitcount;
	int32_t	biCompression;
	int32_t	biSizeImage;
	int32_t	biXPelsPermeter;
	int32_t	biYPelsPermeter;
	int32_t	biClrUsed;
	int32_t	biClrImportant;
#endif

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

void swap_char(unsigned char *a, unsigned char *b)
{
	char tmp = *a;
	*a = *b;
	*b = tmp;
	return ;
}

/*
 * 函数说明: 将rgb24像素数据封装成bmp图像
 * 参数      rgb24path  rgb24的路径
 *           w   数据的宽
 *           h   数据的高
 *           bmppath    bmp图像的路径
 * */
int reg24_to_bmp(const char *rgb24path, int w, int h, const char *bmppath)
{
	FILE *fp_rgb24 = fopen(rgb24path, "rb+");
	if (fp_rgb24 == NULL)
	{
		printf("rgb24文件打开失败\n");
		return -1;
	}
	FILE *fp_bmp = fopen(bmppath, "wb+");

	BmpHead_T	bmpHeader	= { 0 };
	InfoHead_T	infoHeader	= { 0 };
	char bfType[2] = {'B', 'M'};
	int headerSize = sizeof(bfType) + sizeof(BmpHead_T) + sizeof(InfoHead_T);

	unsigned char *rgb24_buffer = (unsigned char *)malloc(w * h * 3);
	fread(rgb24_buffer, 1, w * h * 3, fp_rgb24);

	bmpHeader.imageSize = w * h * 3 + headerSize;
	bmpHeader.startPosition = headerSize;

	infoHeader.length	= sizeof(InfoHead_T);
	infoHeader.width	= w;
	infoHeader.height	= h;
	infoHeader.realSize	= 3 * w * h;
	infoHeader.colorPlane	= 1;
	infoHeader.bitColor		= 24;

	fwrite(bfType, 1, sizeof(bfType), fp_bmp);
	fwrite(&bmpHeader, 1, sizeof(bmpHeader), fp_bmp);
	fwrite(&infoHeader, 1, sizeof(infoHeader), fp_bmp);

#if 1
	for (int i = 0; i < w * h * 3 - i; ++i)
	{
		swap_char(&rgb24_buffer[i], &rgb24_buffer[w * h * 3 - i - 1]);
	}
#if 0
	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < 3 * w; ++j)
		{
			swap_char(&rgb24_buffer[i * w * 3 + j], &rgb24_buffer[(i + 1) * w * 3 - j - 1]);
		}
	}
#endif
#endif
	/*
	 * 该方法生成的bmp图像是倒立的
	 * for (int i = 0; i < h; ++i)
	 * {
	 * 	for (int j = 0; j < w; ++j)
	 * 	{
	 * 		swap_char(&rgb24_buffer[(i * w + j) * 3 + 2], &rgb24_buffer[(i * w + j) * 3]);
	 * 	}
	 * }
	 * */
	//fwrite(rgb24_buffer, w * h * 3, 1, fp_bmp);
	fwrite(rgb24_buffer, 1, w * h * 3, fp_bmp);

	free(rgb24_buffer);
	rgb24_buffer = NULL;
	fclose(fp_rgb24);
	fclose(fp_bmp);

	return 0;
}

int main(void)
{
	reg24_to_bmp("./rgb24_256x256.rgb", 256, 256, "./bmp_256x256.bmp");

	return 0;
}
