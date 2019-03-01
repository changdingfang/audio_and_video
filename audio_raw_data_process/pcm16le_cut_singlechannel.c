 //***************************************************************
 // @file:    pcm16le_cut_singlechannel.c
 // @author:  dingfang
 // @date    2019-03-01 16:17:58
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>

/*
 * 函数说明: 从pcm16le单声道数据中截取一部分数据
 * 参数      url  pcm16le数据的url
 *           start_num   开始位置
 *           dur_len     截取长度
 * */
int pcm16le_cut_singlechannel(const char *url, int start_num, int dur_len)
{
	FILE *fp = fopen(url, "rb+");
	if (fp == NULL)
	{
		printf("pcm16数据文件打开失败!\n");
		return -1;
	}
	FILE *fp_out = fopen("./drum_cut.pcm", "wb+");

	unsigned char *sample = (unsigned char *)malloc(2);

	int cnt = 0;
	while (fread(sample, 1, 2, fp) != 0)
	{
		if (cnt >= start_num && cnt <= (start_num + dur_len))
		{
			fwrite(sample, 1, 2, fp_out);
		}
		cnt++;
	}

	free(sample);
	sample = NULL;
	fclose(fp);
	fclose(fp_out);

	return 0;
}

int main(void)
{
	pcm16le_cut_singlechannel("./drum.pcm", 4360, 15000);

	return 0;
}
