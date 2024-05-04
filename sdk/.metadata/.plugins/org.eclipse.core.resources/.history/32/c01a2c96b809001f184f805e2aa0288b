/*

 * devmem2.c: Simple program to read/write from/to any location in memory.

 *

 *  Copyright (C) 2000, Jan-Derk Bakker (jdb@lartmaker.nl)

 *

 *

 * This software has been developed for the LART computing board

 * (http://www.lart.tudelft.nl/). The development has been sponsored by

 * the Mobile MultiMedia Communications (http://www.mmc.tudelft.nl/)

 * and Ubiquitous Communications (http://www.ubicom.tudelft.nl/)

 * projects.

 *

 *

 * This program is free software; you can redistribute it and/or modify

 * it under the terms of the GNU General Public License as published by

 * the Free Software Foundation; either version 2 of the License, or

 * (at your option) any later version.

 *

 * This program is distributed in the hope that it will be useful,

 * but WITHOUT ANY WARRANTY; without even the implied warranty of

 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the

 * GNU General Public License for more details.

 *

 * You should have received a copy of the GNU General Public License

 * along with this program; if not, write to the Free Software

 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <time.h>
#include <DriveCore.h>

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
  __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)

#define REGISTER_MAP_SIZE 1024*1024UL
#define REGISTER_MAP_MASK (REGISTER_MAP_SIZE - 1)
#define REGISTER_START_ADDR     0x80000000
#define DMA_MAP_SIZE 2048*1024*1024UL
#define DMA_MAP_MASK (DMA_MAP_SIZE - 1)
// dma phsycal address
#define DMA_START_ADDR          0x800000000
#define MAX_DMA_ADDR			DMA_START_ADDR + DMA_MAP_SIZE

//当板卡未打开时使用

/*错误处理*/

//int DriveCoreErrCode;

/* Last error information string */

//char DriveCoreLastErr[256];



typedef struct

{
    /*错误处理*/
    UINT32 DriveCoreErrCode;
    /* Last error information string */
    char DriveCoreLastErr[256];
}DriveError;
/// --------------------------------------------------------------------------------

/// <summary>

/// 设备结构体，存放与设备相关的信息，作为内部信息使用，保证设备相关信息的唯一性

/// </summary>

/// --------------------------------------------------------------------------------

typedef struct {

    //int DMAbufferSize;

    int fd;//设备句柄

    void *registerVirtualbaseAddr;//用于对寄存器进行读写，单次只允许操作4字节，，用户如果对一个非法地址，写入非法数据，将产生风险，需要有机制对其进行约束，只允许其向合法地址写入数据

    void *memoryVirtualbaseAddr;//用于对DDR进行读写

    DriveError devError;//设备错误信息


} _Drive_DEV, *_PDrive_DEV;



EXTERN_C DLL_API DriveDevice Drive_Init()

{

    int fd;

    _PDrive_DEV pDrive = NULL;

    //分配内存

    pDrive = (_PDrive_DEV)malloc(sizeof(_Drive_DEV));

    if (!pDrive)

    {

        pDrive->devError.DriveCoreErrCode = FAILED_ALLOCATE_MEMORRY;

        //DriveErrLog(pDrive, "Drive_Init: Error -Failed to allocate the memorry for pDrive!\n" "Error: 0x%lx", pDrive->devError.DriveCoreErrCode);

    }

    memset(pDrive, 0, sizeof(_Drive_DEV));    

    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;

        printf("/dev/mem opened.\n");



    pDrive->fd = fd;



    //map register

    pDrive->registerVirtualbaseAddr = mmap(0, REGISTER_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, REGISTER_START_ADDR & ~REGISTER_MAP_MASK);

    if(pDrive->registerVirtualbaseAddr == (void *) -1)

    {
        FATAL;
    }

    //printf("Memory mapped at address %p.\n", pDrive->registerVirtualbaseAddr);

    fflush(stdout);



    //map dma

    pDrive->memoryVirtualbaseAddr = mmap(0, DMA_MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, DMA_START_ADDR);

    if(pDrive->memoryVirtualbaseAddr == (void *) -1)

    {
        FATAL;
    }

    //printf("Memory mapped at address %p.\n",  pDrive->memoryVirtualbaseAddr);

    fflush(stdout);

    return  pDrive;

}



EXTERN_C DLL_API int Drive_Close(DriveDevice hDev)

{

    _PDrive_DEV pDrive = (_PDrive_DEV)hDev;

    //int status = 0;

    if (hDev == NULL) {
        return 0;// already been closed or unopened
    }  



    //unmap register

    if(munmap(pDrive->registerVirtualbaseAddr, REGISTER_MAP_SIZE) == -1) 

    {
        FATAL; 
    }

    //unmap dma

    if(munmap(pDrive->memoryVirtualbaseAddr,DMA_MAP_SIZE) == -1) 

    {
        FATAL; 
   }

    //close device

    close(pDrive->fd);



    //free memory

    free(pDrive);

    return 0;

}




EXTERN_C DLL_API int Drive_ReadReg(DriveDevice hDev, UINT32 offset, INT32 *regVal)

{

    _PDrive_DEV pDrive = (_PDrive_DEV)hDev;



	void  *virt_addr;



    virt_addr = (char*)pDrive->registerVirtualbaseAddr + (offset & REGISTER_MAP_MASK);



    *regVal = *((unsigned int *) virt_addr);



    return 0;

}



EXTERN_C DLL_API int Drive_WriteReg(DriveDevice hDev, UINT32 offset, INT32 regVal)

{

    _PDrive_DEV pDrive = (_PDrive_DEV)hDev;



	void  *virt_addr;



    virt_addr = ((char*)pDrive->registerVirtualbaseAddr + (offset & REGISTER_MAP_MASK));



    *((unsigned int *) virt_addr) = regVal;



    return 0;

}



EXTERN_C DLL_API int Drive_DMA_MM_ReadC2H(DriveDevice hDev, PVOID pUserBuf, UINT32 offset, UINT32 length, UINT32 *pnActualBytesRead)

{
	if(offset >= DMA_START_ADDR)
	{
		offset = offset - DMA_START_ADDR;
	}
    _PDrive_DEV pDrive = (_PDrive_DEV)hDev;
	void  *virt_addr;
    virt_addr = (char*)pDrive->memoryVirtualbaseAddr + (offset & DMA_MAP_MASK);
    memcpy(pUserBuf, virt_addr,length);//read
    //pUserBuf = virt_addr;
    *pnActualBytesRead  =  length;
    return 0;
}



EXTERN_C DLL_API int Drive_DMA_MM_WriteH2C(DriveDevice hDev,PVOID pUserBuf, UINT32 offset, UINT32 length, UINT32 *pnActualBytesWritten)

{
	if(offset >= DMA_START_ADDR)
	{
		offset = offset - DMA_START_ADDR;
	}
    _PDrive_DEV pDrive = (_PDrive_DEV)hDev;

	void  *virt_addr;

    virt_addr = (char*)pDrive->memoryVirtualbaseAddr + (offset & DMA_MAP_MASK);

    memcpy(virt_addr,pUserBuf,length);//write

    *pnActualBytesWritten  =  length;



    return 0;

}

int TestPerformance(DriveDevice hDev)

{

    struct timespec start, stop;

    double speed = 0;

    int elapsedMs=0;

    int size = 1024*1024;

    int status = 0;

    UINT32 actualBytes = 0;



    char  *testBuffer = (char *)malloc(size);



    clock_gettime(CLOCK_REALTIME, &start);

    for(int i=0; i<100; i++)

    {

        status = Drive_DMA_MM_WriteH2C(hDev, testBuffer, 0x30000000, size, &actualBytes);

        if (status)

        {

            //printf("Memory mapped at address %p.\n", map_base);

            return status;

        }

    }



    

    clock_gettime(CLOCK_REALTIME, &stop);

	elapsedMs = (int)((stop.tv_sec - start.tv_sec) * 1e3 + (stop.tv_nsec - start.tv_nsec) / 1e6);    // in milliseconds

	printf ("%d ms to write %d bytes\n", elapsedMs, size);

	speed = ((size * 100 /elapsedMs)/1000);

	printf ("%0.2f MBytes/s\n", speed);



    free(testBuffer);

    return 0;

}



int Testdata(DriveDevice hDev)

{

    int size = 1024*1024;

    int status = 0;

    UINT32 actualBytes = 0;

    int test_times=0;

    int base_addr = 0x30000000;

    char  *rdBuffer = (char *)malloc(size);

    char  *wdBuffer = (char *)malloc(size);

    do{

        for(int i=0; i< size; i++)

        {

        	wdBuffer[i] = (char)((i+test_times)%255);

        }

        status = Drive_DMA_MM_WriteH2C(hDev, wdBuffer, base_addr, size, &actualBytes);

        if (status)

        {

            //printf("Memory mapped at address %p.\n", map_base);

            return status;

        }



        status = Drive_DMA_MM_ReadC2H(hDev,rdBuffer, base_addr, size, &actualBytes);

        if (status)

        {

            //printf("Memory mapped at address %p.\n", map_base);

            return status;

        }

        for(int i=0; i< size; i++)

        {

        	if(rdBuffer[i]!=wdBuffer[i])

        	{

                //printf("Memory mapped at address %p.\n", map_base);

                return status;

        	}

        }

        Drive_WriteReg(hDev, 0x41220000, (test_times)%15);

        test_times++;

        base_addr = base_addr + size;

        if(base_addr>=0x40000000)

        	base_addr = 0x30000000;

    }while(test_times<1000000);



    free(rdBuffer);

    free(wdBuffer);

    return 0;

}


/*
int main()
{

    //int result =0 ;

    DriveDevice fd = 0;

    fd = Drive_Init(0,0,0);

    if (fd == NULL)

    {

        //result = -1;

        return -1;

    }

    TestPerformance(fd);

    Testdata(fd);

    for(int i=0; i<10000;i++)

    {

        Drive_WriteReg(fd, 0x41220000, i);

        sleep(1);

    }



    Drive_Close(fd);



}
*/
