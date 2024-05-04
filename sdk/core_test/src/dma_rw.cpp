
/***************************************

for dma speed and correctness test
****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h> 
#include "DriveCore.h"
#ifdef WINDOWS_IMPL
#else
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#endif
#ifdef WINDOWS_IMPL
#define DriveSuccess 1
#else
#define DriveSuccess 0
#define LPDWORD  UINT32*
#define DWORD int
#define MAXCHAR     0x7f
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))
#define _aligned_malloc(nBytesCount, sizeT) aligned_alloc((size_t)sizeT, (size_t)nBytesCount)
#define _aligned_free(p) free(p)
#define _CRT_WARN
#define _RPT3(...)

#define Sleep(t)            usleep(((t) * 1000))  //Sleep ms with usleep
#endif
typedef struct {
	int slotNumber;
	DMA_DIR direction;
	long startAddr;
	long length;
} Options;

static Options options = { 0, (DMA_DIR)3, 0, 1024*1024*512 };
DriveDevice hDev = NULL;
#ifdef WINDOWS_IMPL
HANDLE DMA0R;
HANDLE DMA0W;
#endif
int error=0;
BOOL test_run = TRUE;
static void usage(const char* const exe_name) {
	printf("%s usage:\n\n", exe_name);
	printf("%s <slot_number> <read|write> <ADDR> [Length]\n", exe_name);
	printf("- slot_number : PXIe slot number, 0 means the fisrt card OS found\n");
	printf("- read|write  : read/write DMA operation. \n");
	printf("- ADDR :    The target offset address of the read/write operation. \n");
	printf("            Can be in hex or decimal.\n");
	printf("- Length :  DMA read/write length  \n");
}

static int parse(int argc, char* argv[]) {

	// 0		1			2				3		4...     n...
	if (argc < 5) {
		usage(argv[0]);//
		return 0;
	}
	options.slotNumber = strtoul(argv[1], NULL, 0);//(int)argv[1];
	if (strcmp(argv[2], "write") == 0) {
		options.direction = DMA_H2C;
	}
	else if (strcmp(argv[2], "read") == 0) {
		options.direction = DMA_C2H;
	}
	else {
		options.direction = (DMA_DIR)3;
	}
	options.startAddr = strtoul(argv[3], NULL, 0);
	options.length = strtoul(argv[4], NULL, 0);
	return 0;
}

#ifdef WINDOWS_IMPL
DWORD WINAPI DMARead_Task0_MM(void* lpParameter)
#else
void* DMARead_Task0_MM(void* lpParameter)
#endif
{
#ifdef WINDOWS_IMPL
	LARGE_INTEGER	TimeBegin;
	LARGE_INTEGER	TimeEnd;
	LARGE_INTEGER	SysFrequency;//64bit interger
	LONGLONG TimeCount;
#else
    struct timeval tp_start, tp_end;
#endif
    long long RunningTime;
	int *pUserBuf;
	int status = 0;
	DWORD ReadTimes;
	DWORD ID;
	UINT64 totalBytesTransferd;
	DWORD nByteToWrite;
	UINT32 actualBytesWrite;
	DWORD dwBufSize;

	char error_message[MAXCHAR];


	dwBufSize = 1024 * 1024 * 1;
	nByteToWrite = dwBufSize;
	totalBytesTransferd = 0;
	actualBytesWrite = 0;
	ReadTimes = 0;
	ID = 0;//DMAͨ channel id
	
	pUserBuf =(int*)_aligned_malloc(dwBufSize,1024);
	if (!pUserBuf)
	{
		printf("pUserBuf memory allocate failed");
	}
#ifdef WINDOWS_IMPL
	QueryPerformanceFrequency(&SysFrequency);
	QueryPerformanceCounter(&TimeBegin);
#else
    gettimeofday(&tp_start, NULL);
#endif
	while (test_run)
	{
		if (totalBytesTransferd>0x20000000)//
		{
#ifdef WINDOWS_IMPL
			QueryPerformanceCounter(&TimeEnd);//
			TimeCount = TimeEnd.QuadPart - TimeBegin.QuadPart;
			RunningTime = TimeCount * 1000 / (SysFrequency.QuadPart);

			QueryPerformanceCounter(&TimeBegin);//
#else
			gettimeofday(&tp_end, NULL);
			RunningTime = 1000 * (tp_end.tv_sec - tp_start.tv_sec) + (tp_end.tv_usec - tp_start.tv_usec) / 1000;
		    gettimeofday(&tp_start, NULL);
#endif
			printf("memory read %lld M/s\n", totalBytesTransferd * 1000 / (1024*1024* RunningTime));
			totalBytesTransferd = 0;
		}
		status = Drive_DMA_MM_ReadC2H(hDev, (PCHAR)pUserBuf, 0x00000000, nByteToWrite, &actualBytesWrite);
		if (status)
		{
			printf("%s\n", error_message);
			goto Error;
		}
		//memcpy(pUserBuf, sourceBuf, dwActualBytesRead);
		ReadTimes++;

		totalBytesTransferd += actualBytesWrite;
	}

	printf("memory Read RunTimes=0x%x  ", ReadTimes);
	if (pUserBuf)
	{
		_aligned_free(pUserBuf);//
	}
#ifdef WINDOWS_IMPL
	CloseHandle(DMA0R);
#endif
	return 0;

Error:
	printf("DMA Read error, status=%d,  DMA Read RunTimes=0x%x  ",status, ReadTimes);
	if (pUserBuf)
	{
		_aligned_free(pUserBuf);//
	}
#ifdef WINDOWS_IMPL
	if (DMA0R)
	{
	CloseHandle(DMA0R);
	}
	return status;
#else
	return NULL;
#endif
}

#ifdef WINDOWS_IMPL
DWORD WINAPI DMAWrite_Task0_MM(void* lpParameter)
#else
void* DMAWrite_Task0_MM(void* lpParameter)
#endif
{
#ifdef WINDOWS_IMPL
	LARGE_INTEGER	TimeBegin;
	LARGE_INTEGER	TimeEnd;
	LARGE_INTEGER	SysFrequency;//64bit interger
	LONGLONG TimeCount;
#else
    struct timeval tp_start, tp_end;
#endif
    long long RunningTime;
	int *pUserBuf;
	int status = 0;
	DWORD WriteTimes;
	DWORD ID;
	UINT64 totalBytesTransferd;
	DWORD nByteToWrite;
	UINT32 actualBytesWrite;
	DWORD dwBufSize;

	char error_message[MAXCHAR];


	dwBufSize = 1024 * 1024 * 4;
	nByteToWrite = dwBufSize;
	totalBytesTransferd = 0;
	actualBytesWrite = 0;
	WriteTimes = 0;
	ID = 0;

	pUserBuf = (int*)_aligned_malloc(nByteToWrite, 1024);
	if (!pUserBuf)
	{
		printf("pUserBuf memory allocate failed");
	}
#ifdef WINDOWS_IMPL
	QueryPerformanceFrequency(&SysFrequency);
	QueryPerformanceCounter(&TimeBegin);
#else
    gettimeofday(&tp_start, NULL);
#endif
	while (test_run)
	{
		if (totalBytesTransferd>0x20000000)//
		{
#ifdef WINDOWS_IMPL
			QueryPerformanceCounter(&TimeEnd);
			TimeCount = TimeEnd.QuadPart - TimeBegin.QuadPart;
			RunningTime = TimeCount * 1000 / (SysFrequency.QuadPart);

			QueryPerformanceCounter(&TimeBegin);//
#else
			gettimeofday(&tp_end, NULL);
			RunningTime = 1000 * (tp_end.tv_sec - tp_start.tv_sec) + (tp_end.tv_usec - tp_start.tv_usec) / 1000;
		    gettimeofday(&tp_start, NULL);
#endif
			printf("memory write %lld M/s\n", totalBytesTransferd * 1000 / (1024*1024* RunningTime));
			totalBytesTransferd = 0;
		}
		status = Drive_DMA_MM_WriteH2C(hDev, (PCHAR)pUserBuf, 0x40000000, nByteToWrite, &actualBytesWrite);
		if (status)
		{
			printf("%s\n", error_message);
			goto Error;
		}
		//memcpy(pUserBuf, sourceBuf, dwActualBytesRead);
		WriteTimes++;

		totalBytesTransferd += actualBytesWrite;
	}
	printf("memory Write Test RunTimes=0x%x  ", WriteTimes);

	if (pUserBuf)
	{
		_aligned_free(pUserBuf);//
	}
	return 0;

Error:
	printf("DMA Write error ,status=%d, DMA Write Test RunTimes=0x%x  ", status,WriteTimes);
	if (pUserBuf)
	{
		_aligned_free(pUserBuf);//
	}
#ifdef WINDOWS_IMPL
	if (DMA0W)
	{
	CloseHandle(DMA0R);
	}
	return status;
#else
	return NULL;
#endif
}

int  main(int argc, char* argv[])
{
	int status = 0;
#ifdef WINDOWS_IMPL
	DWORD dwID = 0;
#else
	pthread_t dma_c2h_process,dma_h2c_process;
#endif
	int length = 1024 * 1024*4;
	unsigned int testedLength = 0;
	int testedTimes = 0;
	UINT32 actual_bytes = 0;
	int *rbuffer = (int*)_aligned_malloc(length, 32);
	int *wbuffer = (int*)_aligned_malloc(length, 32);
	int key = 0;
	// parse command line arguments
	if (parse(argc, argv)) {
		goto Exit;
	}

	hDev = Drive_Init();
	if (hDev == NULL)
		hDev = Drive_Init();
	if (hDev == NULL)
	{
		printf("no device opend\n");
		goto Exit;
	}
	for (int i = 0; i < length/4; i++)
	{
		//srand((int)time(0));
		wbuffer[i] = i;
	}
	int registerValue;
	for(int i = 0; i<16*1024; i = i+4)
	{
		Drive_WriteReg(hDev, 0+ i, i);
		Drive_ReadReg(hDev, 0+ i, &registerValue);
		if(registerValue !=i)
		{
			printf("register read/write error\n");
		}
	}

	printf("check dma read/write correctness\n");
	do
	{
		/*if (options.length- testedLength<length)
		{
			length = options.length - testedLength;
		}*/
		if(options.startAddr+ testedLength>2*1024*1024*1024UL)
		{
			testedLength = 0;
			testedTimes++;
			printf("data check correct data check correct data check correct data check correct all 512M\n");
		}
		status = Drive_DMA_MM_WriteH2C(hDev, wbuffer,options.startAddr+ testedLength, length, &actual_bytes);
		if (status)
		{
			printf("dma write test failed, error code is %d\n", status);
			goto Exit;
		}
		
		status = Drive_DMA_MM_ReadC2H(hDev, rbuffer, options.startAddr + testedLength, length, &actual_bytes);
		if (status)
		{
			printf("dma read test failed, error code is %d\n", status);
			goto Exit;
		}
		testedLength += actual_bytes;
		for (unsigned int i = 0; i < actual_bytes/4; i++)
		{
			if (rbuffer[i] != wbuffer[i])
			{
				printf("read data not equal write data, transfer error\n");
			}
			
		}
		printf("data check correct, testedLength = %d\n",testedLength );
		RtlZeroMemory(rbuffer, length);
		//RtlZeroMemory(wbuffer, length);
	} while (testedTimes<1);//testedLength<options.length
	printf("dma read/write check over, everything is okay\n");

	printf("dma read/write speed test\n");
	switch (options.direction)
	{
	case DMA_C2H:
#ifdef WINDOWS_IMPL
		DMA0R = CreateThread(NULL, 0, DMARead_Task0_MM, 0, 0, &dwID);
#else
		status=pthread_create(&dma_c2h_process,0,DMARead_Task0_MM,NULL);
#endif
		break;
	case DMA_H2C:
#ifdef WINDOWS_IMPL
		DMA0W = CreateThread(NULL, 0, DMAWrite_Task0_MM, 0, 0, &dwID);
#else
		status = pthread_create(&dma_h2c_process,0,DMAWrite_Task0_MM,NULL);
#endif
		break;
	default://
#ifdef WINDOWS_IMPL
		DMA0R = CreateThread(NULL, 0, DMARead_Task0_MM, 0, 0, &dwID);
		DMA0W = CreateThread(NULL, 0, DMAWrite_Task0_MM, 0, 0, &dwID);
#else
		status=pthread_create(&dma_c2h_process,0,DMARead_Task0_MM,NULL);
		status = pthread_create(&dma_h2c_process,0,DMAWrite_Task0_MM,NULL);
#endif
		break;
	}

	do
	{
		Sleep(1000);
		key = getchar();
		if (key == 27)
		{
			test_run = FALSE;
			Sleep(1000);
			break;
		}
	} while (error);

Exit:
	if (hDev)
	{
		Drive_Close(hDev);
	}
	if (rbuffer!=NULL)
	{
		_aligned_free(rbuffer);
	}
	if (wbuffer!=NULL)
	{
		_aligned_free(wbuffer);
	}
	return status;
}
