 //***************************************************************
 // @file:    rgb24_to_yuv420.c
 // @author:  dingfang
 // @date    2019-02-28 09:31:20
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数说明: 将一个rgb24像素数据转成yuv420像素数据
 * 参数      rgbUrl   rgb数据的url
 *           w        数据的宽
 *           h        数据的高
 *           yuvUrl   yuv420数据的url
 * */
int rgb24_to_yuv420(const char *rgbUrl, int w, int h, const char *yuvUrl)
{
	FILE *fp_rgb = fopen(rgbUrl, "rb+");
	if (fp_rgb == NULL)
	{
		printf("rgb24数据打开失败!\n");
		return -1;
	}
	FILE *fp_yuv = fopen(yuvUrl, "wb+");

	unsigned char *rgb = (unsigned char *)malloc(w * h * 3);
	unsigned char *yuv_y = (unsigned char *)malloc(w * h);
	unsigned char *yuv_u = (unsigned char *)malloc(w * h / 4);
	unsigned char *yuv_v = (unsigned char *)malloc(w * h / 4);

	fread(rgb, 1, w * h * 3, fp_rgb);

	unsigned char y, u, v;

	for(int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			int idx = i * w + j;
#if 0
			y = (unsigned char)((rgb[idx * 3] * 66 + rgb[idx * 3 + 1] * 129 + rgb[idx * 3 + 2] * 25 + 128) >> 8) + 16;
			u = (unsigned char)((rgb[idx * 3] * -38 - rgb[idx * 3 + 1] * 74 + rgb[idx * 3 + 2] * 112 + 128) >> 8) + 128;
			v = (unsigned char)((rgb[idx * 3] * 112 - rgb[idx * 3 + 1] * 94 - rgb[idx * 3 + 2] * 18 + 128) >> 8) + 128;

			//*(py++) = y;
			yuv_y[idx] = y;
			if (i % 2 == 0 && j % 2 == 0)
			{
				yuv_u[(i / 2 * w + j) / 2] = u;
			}
			else
			{
				if (j % 2 == 0)
				{
					yuv_v[(i / 2 * w + j) / 2] = v;
				}
			}
#endif
#if 1
			yuv_y[idx] = (unsigned char)(rgb[idx * 3] * 0.299 + rgb[idx * 3 + 1] * 0.587 + rgb[idx * 3 + 2] * 0.114) + 16;
			if (i % 2 == 0 && j % 2 == 0)
			{
				yuv_u[(i / 2 * w + j) / 2] = (unsigned char)(rgb[idx * 3] * -0.147 + rgb[idx * 3 + 1] * -0.289 + rgb[idx * 3 + 2] * 0.436) + 128;
			}
			else
			{
				if (j % 2 == 0)
				{
					yuv_v[(i / 2 * w + j) / 2] = (unsigned char)(rgb[idx * 3] * 0.615 + rgb[idx * 3 + 1] * -0.515 + rgb[idx * 3 + 2] * -0.100) + 128;
				}
			}
#endif
		}
	}
	fwrite(yuv_y, 1, w * h, fp_yuv);
	fwrite(yuv_u, 1, w * h / 4, fp_yuv);
	fwrite(yuv_v, 1, w * h / 4, fp_yuv);

	free(rgb);
	rgb = NULL;
	free(yuv_y);
	yuv_y = NULL;
	free(yuv_u);
	yuv_u = NULL;
	free(yuv_v);
	yuv_v = NULL;
	fclose(fp_rgb);
	fclose(fp_yuv);

	return 0;
}

int main (void)
{
	rgb24_to_yuv420("./rgb24_256x256.rgb", 256, 256, "./rgb_to_yuv420.yuv");

	return 0;
}
