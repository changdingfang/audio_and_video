 //***************************************************************
 // @file:    udpParser.c
 // @author:  dingfang
 // @date    2019-03-11 11:25:21
 //***************************************************************
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdint.h>
#include <bits/sockaddr.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <error.h>

#pragma pack(1)

#define INVALID_SOCKET (-1)
#define RECV_DATA_SIZE 10240

typedef struct _T_RTP_FIXED_HEADER
{
	/* byte0 */
	unsigned char version: 2;
	unsigned char extensizon: 1;
	unsigned char padding: 1;
	unsigned char strLen: 4;

	/* byte 1 */
	unsigned char payload: 7;
	unsigned char marker: 1;

	/* bytes 2, 3 */
	unsigned short seqNo;

	/* bytes 4-7 */
	unsigned int timestamp;

	/* bytes 8-11 */
	unsigned int ssrc;
}RTP_FIXED_HEADER_T;

typedef struct _T_MpegTS_FIXED_HEADER
{
	unsigned char syncType: 8;
	unsigned char transportErrorIndicator: 1;
	unsigned char payloadUnitStartIndicator: 1;
	unsigned char transportPriority: 1;
	unsigned PID: 13;
	unsigned scramblingControl: 2;
	unsigned adaptationFieldExist: 2;
	unsigned continuityCounter: 4;
}MpegTS_FIXED_HEADER_T;

void switchPayload(char payload, char *payloadStr)
{
	switch (payload)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18: sprintf(payloadStr, "Audio");	return;
		case 31: sprintf(payloadStr, "H.261");	return;
		case 32: sprintf(payloadStr, "MPV");	return;
		case 33: sprintf(payloadStr, "MP2T");	return;
		case 34: sprintf(payloadStr, "H.263");	return;
		case 96: sprintf(payloadStr, "H.264");	return;
		default: sprintf(payloadStr, "other");	return;
	}
}

void mpegTSprocess(char *rtpData, int rtpDataSize, FILE *myout)
{
	MpegTS_FIXED_HEADER_T mpegtsHeader;
	int i;
	for (i = 0; i < rtpDataSize; i += 188)
	{
		if (rtpData[i] != 0x47)
		{
			fprintf(myout, "   [MPEGTS Pkt] count: %d\n", i);
			break;
		}
		//fprintf(myout, "   [MPEGTS Pkt]\n");
	}
	if (i == rtpDataSize)
	{
		fprintf(myout, "   [MPEGTS Pkt] count: %d\n", i / 188);
	}

	return ;
}

int initSocket(int port)
{
	int serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serSocket == INVALID_SOCKET)
	{
		printf("socket error !");
		return -1;
	}

	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(serSocket, (struct sockaddr *)&serAddr, sizeof(serAddr)) == -1)
	{
		printf("bind error");
		close(serSocket);
		return -1;
	}

	return serSocket;
}

int udpParser(int port)
{
	FILE *myout = stdout;
	FILE *fp = fopen("./out_udpParse.ts", "wb+");

	int serSocket = initSocket(port);
	struct sockaddr_in remoteAddr;
	int nAddrLen = sizeof(remoteAddr);

	int parseMpegTS	= 1;
	int cnt = 0;
	char recvData[RECV_DATA_SIZE] = { 0 };

	while (1)
	{
		int pktsize = recvfrom(serSocket, recvData, RECV_DATA_SIZE, 0, (struct sockaddr *)&remoteAddr, &nAddrLen);
		if (pktsize > 0)
		{
			char payloadStr[10] = { 0 };
			RTP_FIXED_HEADER_T rtpHeader;
			int rtpHeaderSize = sizeof(RTP_FIXED_HEADER_T);

			memcpy((void *)&rtpHeader, recvData, rtpHeaderSize);

			char payload = rtpHeader.payload;
			switchPayload(payload, payloadStr);

			unsigned int timestamp = ntohl(rtpHeader.timestamp);
			unsigned int seqNo = ntohs(rtpHeader.seqNo);

			fprintf(myout, "[RTP Pkt] %5d| %5s| %10u| %5d| %5d|\n", cnt, payloadStr, timestamp, seqNo, pktsize);

			char *rtpData = recvData + rtpHeaderSize;
			int rtpDataSize = pktsize - rtpHeaderSize;
			fwrite(rtpData, rtpDataSize, 1, fp);

			if (parseMpegTS != 0 && payload == 33)
			{
				mpegTSprocess(rtpData, rtpDataSize, myout);
			}

			if (cnt == 15)
			{
				break;
			}
			++cnt;
		}
	}

	close(serSocket);
	fclose(fp);

	return 0;
}

int main(void)
{
	udpParser(8877);

	return 0;
}
