 //***************************************************************
 // @file:    pcm16le_halfvolumeleft.c
 // @author:  dingfang
 // @date    2019-03-01 11:24:23
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数说明: 将pcm16le双声道音频数据左声道的音量降一半
 * 参数      url  pcm16le数据的url
 * */
int halfvolumeleft(const char *url)
{
	FILE *fp = fopen(url, "rb+");
	if (fp == NULL)
	{
		printf("pcm16le数据文件打开失败\n");
		return -1;
	}
	FILE *fp_out = fopen("./44.1k_s16le_halfvolumeleft.pcm", "wb+");

	unsigned char *sample = (unsigned char *)malloc(4);

	while (!feof(fp))
	{
		short *sampleNum = NULL;
		fread(sample, 1, 4, fp);
		sampleNum = (short *)sample;
		*sampleNum /= 2;

		//L
		fwrite(sample, 1, 2, fp_out);
		//R
		fwrite(sample + 2, 1, 2, fp_out);
	}

	free(sample);
	sample = NULL;
	fclose(fp);
	fclose(fp_out);

	return 0;
}

int main(void)
{
	halfvolumeleft("./44.1k_s16le.pcm");

	return 0;
}
