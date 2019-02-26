 //***************************************************************
 // @file:    yuv420_border.c
 // @author:  dingfang
 // @date    2019-02-26 09:31:30
 //***************************************************************

#include <stdio.h>
#include <stdlib.h>

/*
 * 函数说明: 将yuv420像素数据的周围添加边框
 * 参数:     url  图片的路径
 *			 w    图片的宽
 *			 h    图片的高
 *			 border  边框的宽度
 * */
int yuv420_border(const char *url, int w, int h, int border)
{
	FILE *fp_b	= fopen("./yuv420_border.yuv", "wb+");
	FILE *fp	= fopen(url, "rb+");
	if (fp == NULL)
	{
		printf("图片文件打开失败\n");
		return -1;
	}
	unsigned char *pic = (unsigned char *)malloc(w * h * 3 / 2);

	fread(pic, 1, w * h * 3 / 2, fp);
	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			if (i <= border || j <= border || j >= w - border || i >= h - border)
			{
				pic[i * w + j] = 0;
			}
		}
	}
	fwrite(pic, 1, w * h * 3 / 2, fp_b);

	free(pic);
	pic = NULL;
	fclose(fp_b);
	fclose(fp);


	return 0;
}

int main(void)
{
	yuv420_border("./yuv420.yuv", 256, 256, 40);

	return 0;
}
