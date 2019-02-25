 //***************************************************************
 // @file:    yuv420_split.c
 // @author:  dingfang
 // @date    2019-02-25 14:16:02
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

//yuv420
/*
 * 函数说明: 传入yuv图片url和yuv图片的长宽分离出图片的y,u,v分量
 * 参数: url 图片的地址
 *		 w   yuv图片的宽
 *		 h   yuv图片的高
 * */
int yuv420_split(const char *url, int w, int h)
{
	FILE *fp	= fopen(url, "rb+");
	if (fp == NULL)
	{
		printf("图片文件打开错误\n");
		return -1;
	}
	FILE *fp_y	= fopen("420_y.y", "wb+");
	FILE *fp_u	= fopen("420_u.y", "wb+");
	FILE *fp_v	= fopen("420_v.y", "wb+");

	unsigned char *pic = (unsigned char *)malloc(w * h * 3 / 2);

	fread(pic, 1, w * h * 3 / 2, fp);
	fwrite(pic, 1, w * h, fp_y);						//像素: w * h 
	fwrite(pic + w * h, 1, w * h / 4, fp_u);			//像素: w/2 * h/2
	fwrite(pic + w * h * 5 / 4, 1, w * h / 4, fp_v);	//像素: w/2 * h/2

	free(pic);
	pic = NULL;
	fclose(fp);
	fclose(fp_y);
	fclose(fp_u);
	fclose(fp_v);

	return 0;
}

int main(void)
{
	yuv420_split("./yuv420.yuv", 256, 256);

	return 0;
}
