 //***************************************************************
 // @file:    aacParser.c
 // @author:  dingfang
 // @date    2019-03-05 09:10:34
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getADTSframe(const unsigned char *pBuffer, int buffSize, unsigned char *pData, int *pDataSize)
{
	int size = 0;

	if (!pBuffer || !pData || !pDataSize)
	{
		printf("pBuffer == NULL || pData == NULL || pDataSize == NULL\n");
		return -1;
	}
	while (1)
	{
		if (buffSize < 7)
		{
			printf("buffSzie < 7\n");
			return -1;
		}
		/* 寻找 syncword: 0xFFF 占12位 1111 1111 1111*/
		if (0xFF == pBuffer[0] && 0xF0 == (pBuffer[1] & 0xF0))
		{
			/* 第31~43bit表示标头长度 */
			size |= (pBuffer[3] & 0x03) << 11;  //第31~32bit
			size |= pBuffer[4] << 3;            //第33~40bit
			size |= (pBuffer[5] & 0xE0) >> 5;   //第41~43bit
			break;
		}
		--buffSize;
		++pBuffer;
	}
	if (buffSize < size)
	{
		printf("\n");
		return 1;
	}
	memcpy(pData, pBuffer, size);
	*pDataSize = size;

	return 0;
}

/*
 * 函数说明: 解析aac原始码流
 * */
int aacParser(const char *url)
{
	int dataSize	= 0;
	int size		= 0;
	int cnt			= 0;
	int offset		= 0;

	//stdout //标准输出
	FILE *myout = stdout;

	unsigned char *pAacFrame = (unsigned char *)malloc(1024 * 5);
	unsigned char *pAacBuffer = (unsigned char *)malloc(1024 * 1024);

	FILE *fp = fopen(url, "rb");
	if (fp == NULL)
	{
		printf("open fp error !\n");
		return -1;
	}

	printf("-----+- ADTS Frame Table -+------+\n");
	printf(" NUM | Profile | Frequency| Size |\n");
	printf("-----+---------+----------+------+\n");

	while (!feof(fp))
	{
		dataSize = fread(pAacBuffer + offset, 1, 1024 * 1024 - offset, fp);
		unsigned char *inputData = pAacBuffer;

		while (1)
		{
			int ret = getADTSframe(inputData, dataSize, pAacFrame, &size);
			if (-1 == ret)
			{
				break;
			}
			else if (1 == ret)
			{
				memcpy(pAacBuffer, inputData, dataSize);
				offset = dataSize;
				break;
			}

			char profileStr[10]		= { 0 };
			char frequenceStr[10]	= { 0 };

			unsigned char profile = pAacFrame[2] & 0xC0;
			profile >>= 6;
			
			switch (profile)
			{
				case 0: sprintf(profileStr, "Main");	break;
				case 1: sprintf(profileStr, "LC");		break;
				case 2: sprintf(profileStr, "SSR");		break;
				default: sprintf(profileStr, "unknown");	break;
			}

			/* 第19~22bit 表示采样频率, 禁止15*/
			unsigned char samplingFrequencyIndex = pAacFrame[2] & 0x3C;
			samplingFrequencyIndex >>= 2;

			switch (samplingFrequencyIndex)
			{
				case 0: sprintf(frequenceStr, "96000Hz");	break;
				case 1: sprintf(frequenceStr, "88200Hz");	break;
				case 2: sprintf(frequenceStr, "64000Hz");	break;
				case 3: sprintf(frequenceStr, "48000Hz");	break;
				case 4: sprintf(frequenceStr, "44100Hz");	break;
				case 5: sprintf(frequenceStr, "32000Hz");	break;
				case 6: sprintf(frequenceStr, "24000Hz");	break;
				case 7: sprintf(frequenceStr, "22050Hz");	break;
				case 8: sprintf(frequenceStr, "16000Hz");	break;
				case 9: sprintf(frequenceStr, "12000Hz");	break;
				case 10: sprintf(frequenceStr, "11025Hz");	break;
				case 11: sprintf(frequenceStr, "8000Hz");	break;
				default: sprintf(frequenceStr, "unknown");	break;
			}

			fprintf(myout, "%5d| %8s| %8s| %5d|\n", cnt, profileStr, frequenceStr, size);
			dataSize -= size;
			inputData += size;
			if (cnt > 30)
			{
				return 2;
			}
			++cnt;
		}
	}

	free(pAacBuffer);
	pAacBuffer = NULL;
	free(pAacFrame);
	pAacFrame = NULL;
	fclose(fp);

	return 0;
}

int main(void)
{
	aacParser("./nocturne.aac");

	return 0;
}
