 //***************************************************************
 // @file:    yuv420_graybar.c
 // @author:  dingfang
 // @date    2019-02-26 10:39:07
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数说明: 生成一个灰阶图
 * 参数		 w    图片的宽度
 *			 h    图片的高度
 *			 ymin 灰度开始的最小值
 *			 ymax 灰度结束的最大值
 *			 barnum 决定相邻灰度差值(ymax - ymin的值越小, barnum的值越大, 灰度差越小)
 *			 url  生成图片的url
 * */
int yuv420_graybar(int w, int h, int ymin, int ymax, int barnum, const char *url)
{
	FILE *fp = fopen(url, "wb+");

	unsigned char *pic = (unsigned char *)malloc(w * h * 3 / 2);

	float lun_inc = (float)(ymax - ymin) / (float)(barnum);

	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			pic[i * w + j] = (char)(j / barnum) * lun_inc + ymin;
		}
	}
	for (int i = w * h; i < w * h * 3 / 2; ++i)
	{
		pic[i] = 128;
	}
	fwrite(pic, 1, w * h * 3 / 2, fp);

	free(pic);
	pic = NULL;
	fclose(fp);

	return 0;
}

int main(void)
{
	yuv420_graybar(640, 360, 50, 200, 10, "./yuv420_graybar.yuv");

	return 0;
}
