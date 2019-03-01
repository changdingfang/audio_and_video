 //***************************************************************
 // @file:    pcm16le_to_pcm8.c
 // @author:  dingfang
 // @date    2019-03-01 14:11:57
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数说明: 将pcm16le双声道音频数据转换成pcm8音频数据
 * 参数      url  pcm16le数据文件url
 * */
int pcm16le_to_pcm8(const char *url)
{
	FILE *fp = fopen(url, "rb+");
	if (fp == NULL)
	{
		printf("pcm16le数据文件打开失败!\n");
		return -1;
	}
	FILE *fp_out = fopen("./pcm8.pcm", "wb+");

	unsigned char *sample = (unsigned char *)malloc(4);

	while (fread(sample, 1, 4, fp) != 0)
	{
		short *sampleNum16 = NULL;
		char sampleNum8 = 0;
		unsigned char sampleUNum8 = 0;
		sampleNum16 = (short *)sample;
		for (int i = 0; i < 2; ++i, ++sampleNum16)
		{
			sampleNum8 = (*sampleNum16) >> 8;
			//sampleUNum8 = sampleNum8 + 128;
			//fwrite(&sampleUNum8, 1, 1, fp_out);
			fwrite(&sampleNum8, 1, 1, fp_out);
		}
	}

	free(sample);
	sample = NULL;
	fclose(fp);
	fclose(fp_out);

	return 0;
}

int main(void)
{
	pcm16le_to_pcm8("./44.1k_s16le.pcm");

	return 0;
}
