 //***************************************************************
 // @file:    yuv420_psnr.c
 // @author:  dingfang
 // @date    2019-02-26 17:24:45
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 * 函数说明: 计算两个yuv420像素数据的PSRN
 * 参数:	 url1  图片1的url
 *			 url2  图片2的url
 *			 w	   图片的宽
 *			 h     图片的高
 * */
int yuv420_psnr(const char *url1, const char *url2, int w, int h)
{
	FILE *fp1	= fopen(url1, "rb+");
	FILE *fp2	= fopen(url2, "rb+");
	if (fp1 == NULL || fp2 == NULL)
	{
		printf("图片文件打开失败 !");
		return -1;
	}
	unsigned char *pic1 = (unsigned char *)malloc(w * h);
	unsigned char *pic2 = (unsigned char *)malloc(w * h);

	fread(pic1, 1, w * h, fp1);
	fread(pic2, 1, w * h, fp2);

	double mse		= 0;
	double psnr		= 0;
	double mse_sum	= 0;
	for (int i = 0; i < w * h; ++i)
	{
		mse_sum = pow((double)(pic1[i] - pic2[i]), 2);
	}
	mse =  mse_sum / (w * h);
	psnr = 10 * log10(255.0 * 255.0 / mse);
	printf("%lf\n", psnr);

	free(pic1);
	pic1 = NULL;
	free(pic2);
	pic2 = NULL;
	fclose(fp1);
	fclose(fp2);
}

int main(void)
{
	yuv420_psnr("./yuv420.yuv", "./yuv420_distort.yuv", 256, 256);

	return 0;
}
