 //***************************************************************
 // @file:    pcm16le_doublespeed.c
 // @author:  dingfang
 // @date    2019-03-01 13:37:33
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数说明: 将pcm16le双声道音频数据的声音速度提高一倍
 * 参数:     url   pcm16le数据的url
 * */
int pcm16le_doublespeed(const char *url)
{
	FILE *fp = fopen(url, "rb+");
	if (fp == NULL)
	{
		printf("pcm16le数据文件打开失败!\n");
		return -1;
	}
	FILE *fp_out = fopen("./44.1k_pcm16le_doublespeed.pcm", "wb+");

	unsigned char *sample = (unsigned char *)malloc(4);

	int cnt = 0;
	while (!feof(fp))
	{
		fread(sample, 1, 4, fp);
		if (cnt++ % 2 == 0)
		{
			fwrite(sample, 1, 4, fp_out);
		}
	}

	printf("cnt : %d\n", cnt);
	free(sample);
	sample = NULL;
	fclose(fp);
	fclose(fp_out);

	return 0;
}

int main(void)
{
	pcm16le_doublespeed("./44.1k_s16le.pcm");

	return 0;
}
