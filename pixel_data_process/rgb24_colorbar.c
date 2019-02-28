 //***************************************************************
 // @file:    rgb24_colorbar.c
 // @author:  dingfang
 // @date    2019-02-28 18:28:15
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数说明: 生成一个rgb24格式的彩条图
 * 参数      w     rgb数据的宽
 *           h     rgb数据的高
 *           url   生成的rgb数据的url
 * */
int rgb24_colorbar(int w, int h, const char *url)
{
	FILE *fp = fopen(url, "wb+");
	unsigned char *pic = (unsigned char *)malloc(w * h * 3);

	/*--------------------白    黄   青   绿   品   红   蓝  黑  ---*/
	unsigned char r[8] = {255, 255,   0,   0, 255, 255,   0,  0};
	unsigned char g[8] = {255, 255, 255, 255,   0,   0,   0,  0};
	unsigned char b[8] = {255,   0, 255,   0, 255,   0, 255,  0};

	int barW = w / 8;
	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			pic[(i * w + j) * 3] = r[j / barW];
			pic[(i * w + j) * 3 + 1] = g[j / barW];
			pic[(i * w + j) * 3 + 2] = b[j / barW];
		}
	}
	fwrite(pic, 1, w * h * 3, fp);
	
	free(pic);
	pic = NULL;
	fclose(fp);
}

int main(void)
{
	rgb24_colorbar(640, 360, "./rgb24_colorbar.rgb");

	return 0;
}
