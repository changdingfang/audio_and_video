 //***************************************************************
 // @file:    yuv420_gray.c
 // @author:  dingfang
 // @date    2019-02-25 15:51:08
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 函数说明: 把yuv420图片去掉颜色(变成灰度图)
 * 参数: url 图片的地址
 *		 w   yuv图片的宽
 *		 h   yuv图片的高
 * */
int yuv420_gray(const char *url, int w, int h)
{
	FILE *fp	= fopen(url, "rb+");
	FILE *fp_g	= fopen("yuv420_gray.yuv", "wb+");
	if (fp == NULL)
	{
		printf("图片文件打开错误\n");
		return -1;
	}
	unsigned char *pic = (unsigned char *)malloc(w * h * 3 / 2);

	fread(pic, 1, w * h * 3 / 2, fp);
	memset(pic + w * h, 128, w * h / 2);	//128对应的是无色//把u, v分量设置成128即成为灰度图像
	fwrite(pic, 1, w * h * 3 / 2, fp_g);

	free(pic);
	pic = NULL;
	fclose(fp);
	fclose(fp_g);

	return 0;
}

int main(void)
{
	yuv420_gray("./yuv420.yuv", 256, 256);

	return 0;
}
