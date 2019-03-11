 //***************************************************************
 // @file:    rgb24_split.c
 // @author:  dingfang
 // @date    2019-02-27 09:06:36
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数说明: 分离rgb24像素数据的r,g,b分量
 * 参数:     url  图片的rul
 *           w    图片的宽
 *           h    图片的高
 * */
int rgb24_split(const char *url, int w, int h)
{
	FILE *fp	= fopen(url, "rb+");
	FILE *fp_r	= fopen("rgb24_r.y", "wb+");
	FILE *fp_g	= fopen("rgb24_g.y", "wb+");
	FILE *fp_b	= fopen("rgb24_b.y", "wb+");

	unsigned char *pic = (unsigned char *)malloc(w * h * 3);

	fread(pic, 1, w * h * 3, fp);
	for (int i = 0; i < w * h * 3; i += 3)
	{
		//R
		fwrite(pic + i, 1, 1, fp_r);
		//G
		fwrite(pic + i + 1, 1, 1, fp_g);
		//B
		fwrite(pic + i + 2, 1, 1, fp_b);
	}

	free(pic);
	pic = NULL;
	fclose(fp);
	fclose(fp_r);
	fclose(fp_g);
	fclose(fp_b);

	return 0;
}

int main(void)
{
	rgb24_split("./rgb24_500x500.rgb", 500, 500);
	//rgb24_split("./rgb24_256x256.rgb", 256, 256);

	return 0;
}
