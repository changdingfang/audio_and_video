 //***************************************************************
 // @file:    flvParser.c
 // @author:  dingfang
 // @date    2019-03-06 13:54:02
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)

#define TAG_TYPE_AUDIO		8
#define TAG_TYPE_VIDEO		9
#define TAG_TYPE_SCRIPT		18

typedef unsigned int uint32_t;

typedef struct _T_FLV_HEADER
{
	char		signature[3];
	char		version;
	char		flags;
	uint32_t	dataOffset;
}FLV_HEADER_T;

typedef struct _T_TAG_HEADER
{
	char	tagType;
	char	dataSize[3];
	char	timestamp[3];
	char	timestampEx;
	char	reserved[3];
}TAG_HEADER_T;

/*
 * 函数说明: 大端转小端
 * 参数: p  需要转换的字符串
 *       c  转换的字节数
 * */
uint32_t reverseChar(unsigned char *p, int c)
{/* 大端转小端 */
	int r = 0;
	for (int i = 0; i < c; ++i)
	{
		r |= *(p + i) << (c - 1) * 8 - 8 * i;
	}

	return r;
}

/*
 * 处理音频数据
 * 参数:   fp  flv文件描述符
 *         myout  输出到终端
 *         fp_a   音频文件
 *         pTagHeader    tag头
 * */
void processAudio(FILE *fp, FILE *myout, FILE *fp_a, TAG_HEADER_T *pTagHeader)
{
	char tagDataFirstByte = fgetc(fp);
	int format = (tagDataFirstByte & 0xF0) >> 4;
	char audioTagStr[100] = { 0 };
	strcat(audioTagStr, "| ");
	switch (format)
	{
		case 0: strcat(audioTagStr, "Linear PCM, platform endian");	break;
		case 1: strcat(audioTagStr, "ADPCM");						break;
		case 2: strcat(audioTagStr, "MP3");							break;
		case 3: strcat(audioTagStr, "Linear PCM, little endian");	break;
		case 4: strcat(audioTagStr, "Nellymoser 16-kHz mono");		break;
		case 5: strcat(audioTagStr, "Nellymoser 8-kHz mono");		break;
		case 6: strcat(audioTagStr, "Nellymoser");					break;
		case 7: strcat(audioTagStr, "G.711 A-law logarithmic PCM");	break;
		case 8: strcat(audioTagStr, "G.711 mu-law logarithmic PCM");break;
		case 9: strcat(audioTagStr, "reserved");					break;
		case 10: strcat(audioTagStr, "AAC");						break;
		case 11: strcat(audioTagStr, "Speex");						break;
		case 14: strcat(audioTagStr, "MP3 8-Khz");					break;
		case 15: strcat(audioTagStr, "Device-specific sound");		break;
		default: strcat(audioTagStr, "unknown");					break;
	}
	strcat(audioTagStr, "| ");
	int samplingRate = (tagDataFirstByte & 0x0C) >> 2;
	switch (samplingRate)
	{
		case 0: strcat(audioTagStr, "5.5-kHz");	break;
		case 1: strcat(audioTagStr, "11-kHz");	break;
		case 2: strcat(audioTagStr, "22-kHz");	break;
		case 3: strcat(audioTagStr, "44-kHz");	break;
		default: strcat(audioTagStr, "unknown");break;
	}
	strcat(audioTagStr, "| ");
	int accuracy = (tagDataFirstByte & 0x02) >> 1;
	switch (accuracy)
	{
		case 0: strcat(audioTagStr, "snd8bit");	break;
		case 1: strcat(audioTagStr, "snd16bit");break;
		default: strcat(audioTagStr, "unknown");break;
	}
	strcat(audioTagStr, "| ");
	int audioType = tagDataFirstByte & 0x01;
	switch (audioType)
	{
		case 0: strcat(audioTagStr, "sndMono");	break;
		case 1: strcat(audioTagStr, "sndStereo");break;
		default: strcat(audioTagStr, "unknown");break;
	}
	fprintf(myout, "%s", audioTagStr);
	uint32_t dataSize = reverseChar((unsigned char *)pTagHeader->dataSize, sizeof(pTagHeader->dataSize)) - 1;
	for (int i = 0; i < dataSize; ++i)
	{
		fputc(fgetc(fp), fp_a);
	}

	return ;
}

/*
 * 处理视频数据
 * 参数:   fp  flv文件描述符
 *         myout  输出到终端
 *         fp_v   视频文件
 *         pTagHeader    tag头
 * */
void processVideo(FILE *fp, FILE *myout, FILE *fp_v, TAG_HEADER_T *pTagHeader)
{
	char videoTagStr[100] = { 0 };
	strcat(videoTagStr, "| ");
	char tagDataFirstByte;
	tagDataFirstByte = fgetc(fp);
	int frameType = (tagDataFirstByte & 0xF0) >> 4;
	switch (frameType)
	{
		case 1: strcat(videoTagStr, "key frame");				break;
		case 2: strcat(videoTagStr, "inter frame");				break;
		case 3: strcat(videoTagStr, "disposable inter frame");	break;
		case 4: strcat(videoTagStr, "generated keyframe");		break;
		case 5: strcat(videoTagStr, "video info/command frame");break;
		default: strcat(videoTagStr, "unknown");				break;
	}
	strcat(videoTagStr, "| ");
	int codeID = tagDataFirstByte & 0x0F;
	switch (codeID)
	{
		case 1: strcat(videoTagStr, "JPEG");						break;
		case 2: strcat(videoTagStr, "Sorenson H.263");				break;
		case 3: strcat(videoTagStr, "Screen video");				break;
		case 4: strcat(videoTagStr, "On2 VP6");						break;
		case 5: strcat(videoTagStr, "On2 VP6 with alpha channel");	break;
		case 6: strcat(videoTagStr, "Screen video version 2");		break;
		case 7: strcat(videoTagStr, "AVC");							break;
		default: strcat(videoTagStr, "unknown");					break;
	}
	fprintf(myout, "%s", videoTagStr);
	//因为保存的视频扔为flv格式
	//把tagData的首字节留下
	fseek(fp, -1, SEEK_CUR);
	//把最后一个preiousTagSize留下
	uint32_t dataSize = reverseChar((unsigned char *)pTagHeader->dataSize, sizeof(pTagHeader->dataSize)) + 4;
	fwrite((char *)pTagHeader, 1, sizeof(TAG_HEADER_T), fp_v);
	for (int i = 0; i < dataSize; ++i)
	{
		fputc(fgetc(fp), fp_v);
	}
	fseek(fp, -4, SEEK_CUR);

	return ;
}

/*
 * 处理script数据
 * 参数:   fp  flv文件描述符
 *         myout  输出到终端
 *         pTagHeader    tag头
 * */
void processScript(FILE *fp, FILE *myout, TAG_HEADER_T *pTagHeader)
{
	fseek(fp, reverseChar((unsigned char *)pTagHeader->dataSize, sizeof(TAG_HEADER_T)), SEEK_CUR);

	return ;
}

/*
 * 函数说明: 解析flv文件
 * 参数: url   flv文件的url
 * */
int flvParser(const char *url)
{
	FILE *fp = fopen(url, "rb+");
	if (fp == NULL)
	{
		printf("flv file open error!\n");
		return -1;
	}
	FILE *fp_v = fopen("./video.flv", "wb+");
	FILE *fp_a = fopen("./audio.mp3", "wb+");
	FILE *myout = stdout;

	FLV_HEADER_T flvHeader;
	TAG_HEADER_T tagHeader;
	uint32_t previousTagSize = 0;

	unsigned char *pData = (unsigned char *)malloc(1024 * 1024);

	fread(&flvHeader, 1, sizeof(flvHeader), fp);

	fprintf(myout, "--------->FLV Header<---------\n");
	fprintf(myout, "Signature:	%c %c %c\n", flvHeader.signature[0], flvHeader.signature[1], flvHeader.signature[2]);
	fprintf(myout, "Version:	0x%02x\n", flvHeader.version);
	fprintf(myout, "Flags:		0x%02x\n", flvHeader.flags);
	fprintf(myout, "DataOffset:	0x%08x\n", reverseChar((unsigned char *)(&flvHeader.dataOffset), sizeof(flvHeader.dataOffset)));
	fprintf(myout, "--------->FLV End<---------\n");
	fwrite((char *)&flvHeader, 1, sizeof(flvHeader), fp_v);
	fwrite((char *)&previousTagSize, 1, sizeof(previousTagSize), fp_v);

	int cnt = 0;
	while (!feof(fp))
	{
		if (cnt++ > 30)
		{
			break;
		}
		fread(&previousTagSize, 1, 4, fp);
		memset(&tagHeader, 0, sizeof(TAG_HEADER_T));
		fread((void *)&tagHeader, 1, sizeof(TAG_HEADER_T), fp);
		if (feof(fp))
		{
			break;
		}
		uint32_t tagHeaderDataSize	= reverseChar((unsigned char *)tagHeader.dataSize, 3);
		uint32_t tagHeaderTimeStamp	= reverseChar((unsigned char *)tagHeader.timestamp, 3);

		char tagType[10] = { 0 };
		switch (tagHeader.tagType)
		{
			case TAG_TYPE_AUDIO:	sprintf(tagType, "AUDIO");	break;
			case TAG_TYPE_VIDEO:	sprintf(tagType, "VIDEO");	break;
			case TAG_TYPE_SCRIPT:	sprintf(tagType, "SCRIPT");	break;
			default: sprintf(tagType, "unknown tagType");	break;
		}
		fprintf(myout, "[%6s] %6u %6u |", tagType, tagHeaderDataSize, tagHeaderTimeStamp);
		switch (tagHeader.tagType)
		{
			case TAG_TYPE_AUDIO: processAudio(fp, myout, fp_a, &tagHeader);	break;
			case TAG_TYPE_VIDEO: processVideo(fp, myout, fp_v, &tagHeader);	break;
			case TAG_TYPE_SCRIPT: processScript(fp, myout, &tagHeader);		break;
			default: fprintf(myout, "unknow!----");	break;
		}
		fprintf(myout, "\n");
	}

	return 0;
}

int main(void)
{
	flvParser("./cuc_ieschool.flv");

	return 0;
}
