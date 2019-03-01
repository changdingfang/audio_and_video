 //***************************************************************
 // @file:    pcm16le_to_wave.c
 // @author:  dingfang
 // @date    2019-03-01 16:39:21
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _T_WAVE_HEADER
{
	char fccID[4];
	unsigned int dwSize;
	char fccType[4];
}WAVE_HEADER_T;

typedef struct _T_WAVE_FMT
{
	char fccID[4];
	unsigned int	dwSize;
	unsigned short	wFormatTag;
	unsigned short	wChannels;
	unsigned int	dwSamplesPerSec;
	unsigned int	dwAvgBytesPerSec;
	unsigned short	wBlockAlign;
	unsigned short	uiBitsPerSample;
}WAVE_FMT_T;

typedef struct _T_WAVE_DATA
{
	char fccID[4];
	unsigned int dwSize;
}WAVE_DATA_T;

/*
 * 函数说明: 把pcm16le双声道数据转换成wave格式的音频数据
 * 参数      pcmUrl   pcm16le数据的url
 *           channels   
 *           sample_rate  pcm采样率
 *           waveUrl   wave音频数据url
 * */
int pcm16le_to_wave(const char *pcmUrl, int channels, int sample_rate, const char *waveUrl)
{
	FILE *fp = fopen(pcmUrl, "rb+");
	if (fp == NULL)
	{
		printf("pcm数据文件打开失败!\n");
		return -1;
	}
	FILE *fp_wave = fopen(waveUrl, "wb+");

	if (channels == 0 || sample_rate == 0)
	{
		channels = 2;
		sample_rate = 44100;
	}

	WAVE_HEADER_T	waveHeader	= { 0 };
	WAVE_FMT_T		waveFmt		= { 0 };
	WAVE_DATA_T		waveData	= { 0 };

	memcpy(waveHeader.fccID, "RIFF", strlen("RIFF"));
	memcpy(waveHeader.fccType, "WAVE", strlen("WAVE"));
	fseek(fp_wave, sizeof(WAVE_HEADER_T), 1);

	unsigned short pcmData;
	int bits = 16;
	waveFmt.dwSamplesPerSec	= sample_rate;
	waveFmt.dwAvgBytesPerSec= waveFmt.dwSamplesPerSec * sizeof(pcmData);
	waveFmt.uiBitsPerSample = bits;
	memcpy(waveFmt.fccID, "fmt ", strlen("fmt "));
	waveFmt.dwSize = 16;
	waveFmt.wBlockAlign = 2;
	waveFmt.wChannels = channels;
	waveFmt.wFormatTag = 1;

	fwrite(&waveFmt, 1, sizeof(WAVE_FMT_T), fp_wave);

	memcpy(waveData.fccID, "data", strlen("data"));
	waveData.dwSize = 0;

	fseek(fp_wave, sizeof(WAVE_DATA_T), SEEK_CUR);

	
	while (fread(&pcmData, 1, sizeof(unsigned short), fp) != 0)
	{
		waveData.dwSize += 2;
		fwrite(&pcmData, 1, sizeof(unsigned short), fp_wave);
	}

	waveHeader.dwSize = 44 + waveData.dwSize;
	rewind(fp_wave);
	fwrite(&waveHeader, 1, sizeof(WAVE_HEADER_T), fp_wave);
	fseek(fp_wave, sizeof(WAVE_FMT_T), SEEK_CUR);
	fwrite(&waveData, 1, sizeof(WAVE_DATA_T), fp_wave);


	fclose(fp);
	fclose(fp_wave);

	return 0;
}

int main(void)
{
	pcm16le_to_wave("./44.1k_s16le.pcm", 2, 44100, "./pcm16le_to_wave.wav");

	return 0;
}
