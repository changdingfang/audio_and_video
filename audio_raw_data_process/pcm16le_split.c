 //***************************************************************
 // @file:    pcm16le_split.c
 // @author:  dingfang
 // @date    2019-03-01 10:15:46
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数说明: 分离pcm16le双声道音频采样数据的左声道和右声道
 * 参数      url  pcm16le数据的url
 * */
int pcm16le_split(const char *url)
{
	FILE *fp = fopen(url, "rb+");
	if (fp == NULL)
	{
		printf("打开音频文件失败\n");
		return -1;
	}
	FILE *fp_l = fopen("./pcm16le_L.pcm", "wb+");
	FILE *fp_r = fopen("./pcm16le_R.pcm", "wb+");

	unsigned char *sample = (unsigned char *)malloc(4);

	while (!feof(fp))
	{
		fread(sample, 1, 4, fp);
		//L
		fwrite(sample, 1, 2, fp_l);
		//R
		fwrite(sample + 2, 1, 2, fp_r);
	}

	free(sample);
	sample = NULL;
	fclose(fp);
	fclose(fp_l);
	fclose(fp_r);

	return 0;
}

int main(void)
{
	pcm16le_split("./44.1k_s16le.pcm");

	return 0;
}
