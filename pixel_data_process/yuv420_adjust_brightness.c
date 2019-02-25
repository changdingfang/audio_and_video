 //***************************************************************
 // @file:    yuv420_adjust_brightness.c
 // @author:  dingfang
 // @date    2019-02-25 16:46:05
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数说明: 调节yuv420图片的亮度
 * 参数: url  图片的路径
 *		 w    图片的宽
 *		 h    图片的高
 *		 weight  调节亮度的权重//取值范围0~1
 * */
int yuv420_adjust_brightness(const char *url, int w, int h, float weight)
{
	FILE *fp	= fopen(url, "rb+");
	FILE *fp_out= fopen("yuv420_ab.yuv", "wb+");
	if (fp == NULL)
	{
		printf("图片文件打开失败\n");
		return -1;
	}
	unsigned char * pic = (unsigned char *)malloc(w * h * 3 / 2);

	fread(pic, 1, w * h * 3 / 2, fp);
	for (int i = 0; i < w * h; ++i)
	{
		unsigned char tmp = pic[i] * weight;
		pic[i] = tmp;
	}
	fwrite(pic, 1, w * h * 3 / 2, fp_out);

	free(pic);
	pic = NULL;
	fclose(fp);
	fclose(fp_out);
}

int main(void)
{
	yuv420_adjust_brightness("./yuv420.yuv", 256, 256, 0.5);

	return 0;
}
