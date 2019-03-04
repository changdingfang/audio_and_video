 //***************************************************************
 // @file:    h264_parser.c
 // @author:  dingfang
 // @date    2019-03-03 11:29:22
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
	/* 0  未使用 */
	NALU_TYPE_SLICE		= 1,	/* 不分区, 非IDR的片 */
	NALU_TYPE_DPA		= 2,	/* 片分区A */
	NALU_TYPE_DPB		= 3,	/* 片分区B */
	NALU_TYPE_DPC		= 4,	/* 片分区C */
	NALU_TYPE_IDR		= 5,	/* IDR图像中的片 */
	NALU_TYPE_SEI		= 6,	/* 补充增强信息单元(SEI) */
	NALU_TYPE_SPS		= 7,	/* 序列参数集 */
	NALU_TYPE_PPS		= 8,	/* 图像参数集 */
	NALU_TYPE_AUD		= 9,	/* 分解符 */
	NALU_TYPE_EOSEQ		= 10,	/* 序列结束 */
	NALU_TYPE_EOSTREAM	= 11,	/* 码流结束 */
	NALU_TYPE_FILL		= 12,	/* 填充 */
	/*  13-23  保留 */
	/*  24-31  不保留 */
}NaluType_E;

typedef enum
{
	NALU_PRIORITY_DISPOSABLE	= 0,
	NALU_PRIORITY_LOW			= 1,
	NALU_PRIORITY_HIGH			= 2,
	NALU_PRIORITY_HIGHEST		= 3,
}NaluPriority_E;

typedef struct _T_NALU
{
	int		startcodeprefixLen;
	unsigned int len;
	unsigned int maxSize;
	char	forbiddenBit;
	char	nalReferenceIdc;
	char	nalUnitType;
	char	*buf;
}NALU_T;

static int findStartCode3b(unsigned char *buf)
{
	return (buf[2] != 1 || buf[0] !=0 || buf[1] != 0) ? 0 : 1;//0x000001
}

static int findStartCode4b(unsigned char *buf)
{
	return (buf[3] != 1 || buf[2] != 0 || buf[0] !=0 || buf[1] != 0) ? 0 : 1;//0x00000001
}

int getAnnexbNalu(NALU_T *pNalu, FILE *fp)
{
	unsigned char *buff = (unsigned char *)calloc(pNalu->maxSize, sizeof(char));
	if (buff == NULL)
	{
		printf("buff calloc error !\n");
		return -1;
	}
	int pos = 0;
	if (fread(buff, 1, 3, fp) != 3)
	{
		printf("fread len != 3\n");
		free(buff);
		return -1;
	}
	if (findStartCode3b(buff) != 1)
	{
		if (fread(buff + 3, 1, 1, fp) != 1)
		{
			printf("fread len != 1\n");
			free(buff);
			return -1;
		}
		if (findStartCode4b(buff) != 1)
		{
			free(buff);
			return -2;
		}
		else
		{
			pos = 4;
			pNalu->startcodeprefixLen = 4;
		}
	}
	else
	{
		pos = 3;
		pNalu->startcodeprefixLen = 3;
	}

	int rewind = 0;

	do 
	{
		/* 
		 * 直到寻找到下一个起始码 
		 * 在寻找到下一个startcode(起始码)之前的数据都是NALU 
		 * */
		if (feof(fp))
		{
			pNalu->len = (pos - 1) - pNalu->startcodeprefixLen;
			memcpy(pNalu->buf, &buff[pNalu->startcodeprefixLen], pNalu->len);
			pNalu->forbiddenBit		= pNalu->buf[0] & 0x80;
			pNalu->nalReferenceIdc	= pNalu->buf[0] & 0x60;
			pNalu->nalUnitType		= pNalu->buf[0] & 0x1f;
			free(buff);
			buff = NULL;
			return pos - 1;
		}
		buff[pos++] = fgetc(fp);
	} while (!(1 == findStartCode4b(&buff[pos - 4]) || 1 == findStartCode3b(&buff[pos - 3])));

	rewind = (1 == findStartCode4b(&buff[pos - 4])) ? -4 : -3;

	if (fseek(fp, rewind, SEEK_CUR) != 0)
	{
		free(buff);
		printf("getAnnexbNalu: fseek error\n");
	}

	pNalu->len = (pos + rewind) - pNalu->startcodeprefixLen;
	memcpy(pNalu->buf, &buff[pNalu->startcodeprefixLen], pNalu->len);
	/* 
	 * forbiddenBit, nalReferenceIdc, nalUnitType共占1字节大小
	 * 0x80  1000 0000
	 * 0x60  0110 0000
	 * 0x1f  0001 1111
	 * */
	pNalu->forbiddenBit		= pNalu->buf[0] & 0x80; //1bit
	pNalu->nalReferenceIdc	= pNalu->buf[0] & 0x60; //2bit
	pNalu->nalUnitType		= pNalu->buf[0] & 0x1f; //5bit
	free(buff);

	return pos + rewind;
}

int h264_parser(const char *url)
{
	//stdout	//标准输出
	//stdin		//标准输入
	//stderr	//标准错误
	FILE *myout = stdout;
	FILE *fp = fopen(url, "rb+");
	if (fp == NULL)
	{
		printf("open h264 file error\n");
		return -1;
	}
	NALU_T *pNalu = (NALU_T *)calloc(1, sizeof(NALU_T));
	if (pNalu == NULL)
	{
		printf("pNalu calloc error\n");
		fclose(fp);
		return -1;
	}
	int buffersize = 100000;
	pNalu->maxSize = buffersize;
	pNalu->buf = (char *)calloc(buffersize, sizeof(char));
	if (pNalu->buf == NULL)
	{
		printf("pNalu->buf calloc error !\n");
		free(pNalu);
		fclose(fp);
		return -1;
	}

	int dataOffset = 0;
	int nalNum = 0;
	printf("-----+---------+-- NALU Table --+---------+---------+\n");
	printf(" NUM |   POS   |   IDC  |  TYPE |   LEN   | DATALEN |\n");
	printf("-----+---------+--------+-------+---------+---------+\n");
	while (!feof(fp))
	{
		int dataLenth = getAnnexbNalu(pNalu, fp);
		if (dataLenth < 0)
		{
			printf("h264 parser: dataLenth < 0\n");
			return -1;
		}
		char typeStr[10] = { 0 };

		switch (pNalu->nalUnitType)
		{
			case NALU_TYPE_SLICE:	sprintf(typeStr, "SLICE");		break;
			case NALU_TYPE_DPA:		sprintf(typeStr, "DPA");		break;
			case NALU_TYPE_DPB:		sprintf(typeStr, "DPB");		break;
			case NALU_TYPE_DPC:		sprintf(typeStr, "DPC");		break;
			case NALU_TYPE_IDR:		sprintf(typeStr, "IDR");		break;
			case NALU_TYPE_SEI:		sprintf(typeStr, "SEI");		break;
			case NALU_TYPE_SPS:		sprintf(typeStr, "SPS");		break;
			case NALU_TYPE_PPS:		sprintf(typeStr, "PPS");		break;
			case NALU_TYPE_AUD:		sprintf(typeStr, "AUD");		break;
			case NALU_TYPE_EOSEQ:	sprintf(typeStr, "EOSEQ");		break;
			case NALU_TYPE_EOSTREAM:sprintf(typeStr, "EOSTREAM");	break;
			case NALU_TYPE_FILL:	sprintf(typeStr, "FILL");		break;
		}
		char idcStr[10] = { 0 };
		switch (pNalu->nalReferenceIdc >> 5)
		{
			case NALU_PRIORITY_DISPOSABLE:	sprintf(idcStr, "DISPOS");	break;
			case NALU_PRIORITY_LOW:			sprintf(idcStr, "LOW");		break;
			case NALU_PRIORITY_HIGH:		sprintf(idcStr, "HIGH");	break;
			case NALU_PRIORITY_HIGHEST:		sprintf(idcStr, "HIGHEST");	break;
		}
		if (nalNum < 30)
		{
			fprintf(myout, "%5d| %8d| %7s| %6s| %8d| %8d|\n", nalNum, dataOffset, idcStr, typeStr, pNalu->len, dataLenth);
		}
		dataOffset += dataLenth;
		++nalNum;
	}


	if (pNalu)
	{
		if (pNalu->buf)
		{
			free(pNalu->buf);
			pNalu->buf = NULL;
		}
		free(pNalu);
		pNalu = NULL;
	}
	fclose(myout);
	fclose(fp);

	return 0;
}

int main(void)
{
	h264_parser("./sintel.h264");

	return 0;
}
