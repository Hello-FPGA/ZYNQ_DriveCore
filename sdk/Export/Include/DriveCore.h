/// --------------------------------------------------------------------------------
/// <summary>
/// </summary>
/// --------------------------------------------------------------------------------

#ifndef  _Drive_H_
#define  _Drive_H_
#if defined(_WIN32) || defined(WIN32)        /**Windows*/
#define WINDOWS_IMPL
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(BSD)    /**Linux*/
#define LINUX_IMPL
#endif

#ifdef WINDOWS_IMPL
#include <windows.h>
#else

#endif

//Macro
#ifndef TRUE
#define TRUE 1
#endif // !TRUE
#ifndef FALSE
#define FALSE 0
#endif // !FALSE

#ifndef NULL
#define NULL (void*)0
#endif // !NULL

typedef int                BOOL;
typedef void*              PVOID;
typedef void*              HANDLE;
typedef unsigned char      BOOLEAN;
typedef char*              PCHAR;

typedef signed char        INT8;
typedef unsigned char      UINT8;
typedef signed short       INT16;
typedef unsigned short     UINT16;
typedef signed int         INT32;
typedef unsigned int       UINT32;

typedef long long INT64;
typedef unsigned long long UINT64;

#ifdef WINDOWS_IMPL
#ifdef Drive_API_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#else
#ifdef __cplusplus
#define EXTERN_C  extern "C"
#else
#define EXTERN_C  extern
#endif
#define DLL_API
#endif

typedef  void *DriveDevice;


/// <summary>DMA方向</summary>
typedef enum {
	DMA_C2H, //client to host - read
	DMA_H2C	 // host to client - write
}DMA_DIR;


// ********************************************************************************
/// <summary>
/// 打开并初始化设备，返回设备handle
/// <returns>成功： DEVICE HANDLE; 失败：NULL</returns>
// ********************************************************************************
EXTERN_C DLL_API DriveDevice Drive_Init();

/// ********************************************************************************
/// <summary>
/// 关闭指定设备，释放资源
/// </summary>
/// <param name="hDev">Drive设备HANDLE</param>
/// <returns>成功：0；错误：错误码</returns>
/// ******************************************************************************** 
EXTERN_C DLL_API int Drive_Close(DriveDevice hDev);

/// ********************************************************************************
/// <summary>
/// 写入指定偏移地址寄存器
/// </summary>
/// <param name="hDev">Drive设备HANDLE</param>
/// <param name="offset">偏移地址，必须是4的整数倍</param>
/// <param name="regVal">要写入寄存器的值</param>
/// <returns>成功：0；错误：错误码</returns>
/// ********************************************************************************
EXTERN_C DLL_API int Drive_WriteReg(DriveDevice hDev, UINT32 offset, INT32 regVal);

/// ********************************************************************************
/// <summary>
/// 读取指定偏移地址寄存器,单次读取4个字节
/// </summary>
/// <param name="hDev">Drive设备HANDLE</param>
/// <param name="offset">偏移地址，必须是4的整数倍</param>
/// <param name="regVal">寄存器的读出值</param>
/// <returns>成功：0；错误：错误码</returns>
/// ********************************************************************************
EXTERN_C DLL_API int Drive_ReadReg(DriveDevice hDev, UINT32 offset, INT32 *regVal);

// ********************************************************************************
/// <summary>
/// 在Memory map方式下从FPGA侧缓存指定地址处读取数据
/// </summary>
/// <param name="hDev">设备handle</param>
/// <param name="pUserBuf">用户内存，对应起始地址建议向32字节对齐，不对齐会影响读取速度，C++可以使用_aligned_malloc(nByteToWrite, 32)分配</param>
/// <param name="offset">读取数据起始偏移地址，取值范围为4GB以内</param>
/// <param name="length">请求读取数据长度</param>
/// <param name="pnActualBytesRead">实际读取数据量，单位是byte</param>
/// <returns>成功 0，失败 错误码</returns>
// ********************************************************************************
EXTERN_C DLL_API int Drive_DMA_MM_ReadC2H(DriveDevice hDev, PVOID pUserBuf, UINT32 offset, UINT32 length, UINT32 *pnActualBytesRead);

// ********************************************************************************
/// <summary>
///  在Memory map方式下向FPGA侧缓存指定地址处写入数据
/// </summary>
/// <param name="hDev">设备handle</param>
/// <param name="pUserBuf">用户内存</param>
/// <param name="offset">写入地址偏移量。支持32bit地址范围</param>
/// <param name="length">请求写入数据长度</param>
/// <param name="pnActualBytesWriten">实际写入数据量</param>
/// <returns>成功 0，失败 错误码</returns>
// ********************************************************************************
EXTERN_C DLL_API int Drive_DMA_MM_WriteH2C(DriveDevice hDev, PVOID pUserBuf, UINT32 offset, UINT32 length, UINT32* pnActualBytesWriten);

//错误码定义，用于查找错误原因
#define ERRO_ADDRESS_OFFSET -5000
#define FAILED_OPEN_USER_DEV		ERRO_ADDRESS_OFFSET+0
#define FAILED_OPEN_CONTROL_DEV		ERRO_ADDRESS_OFFSET+1
#define INVALID_DEVICE_HANDLE		ERRO_ADDRESS_OFFSET+2
#define FAILED_OPEN_DMA				ERRO_ADDRESS_OFFSET+3
#define FAILED_READ_FROM_DMA		ERRO_ADDRESS_OFFSET+4
#define FAILED_WRITE_TO_DMA			ERRO_ADDRESS_OFFSET+5
#define DMA_CHANNEL_NOT_EXIST		ERRO_ADDRESS_OFFSET+6
#define PID_IS_ILLEAGAL				ERRO_ADDRESS_OFFSET+7
#define FAILED_ENUM_DEV				ERRO_ADDRESS_OFFSET+8
#define VID_IS_ILLEGAL				ERRO_ADDRESS_OFFSET+9
#define SLOT_NUM_IS_ILLEGAL			ERRO_ADDRESS_OFFSET+10
#define LENGTH_GRANULARTY			ERRO_ADDRESS_OFFSET+11
#define FAILED_ALLOCATE_MEMORRY		ERRO_ADDRESS_OFFSET+12
#define DMA_BUFEER_OVERFLOW			ERRO_ADDRESS_OFFSET+13
#define DMA_MODE_IS_WRONG			ERRO_ADDRESS_OFFSET+14
#define DMA_IS_NOT_RUNNING			ERRO_ADDRESS_OFFSET+15
#define REQUEST_MODE_IS_UNSUPPORTED	ERRO_ADDRESS_OFFSET+16		
#define CRITICAL_SECTION_UNALLOCATE	ERRO_ADDRESS_OFFSET+17
#define USERBUF_NOT_ALIGNED_CRITICAL_WARNING ERRO_ADDRESS_OFFSET+18
#define TRIAL_WAS_EXPIRED			ERRO_ADDRESS_OFFSET+19
#define FAILED_UPDATE_DEVICE_INFO   ERRO_ADDRESS_OFFSET+20
#define DATABASE_INIT_FAILED        ERRO_ADDRESS_OFFSET+21
#define LICENCE_TRIAL_EXPIRED		ERRO_ADDRESS_OFFSET+22
#define NO_AVALIABLE_DEVICES		ERRO_ADDRESS_OFFSET+23
#define Open_USB_DEVICE_FAILED      ERRO_ADDRESS_OFFSET+24
#define Not_Support_For_USB         ERRO_ADDRESS_OFFSET+25
#define LICENCE_UN_ACTIVATED		ERRO_ADDRESS_OFFSET+26
#define LICENCE_ACTIVATE_FAILED		ERRO_ADDRESS_OFFSET+27
#define INVALID_INIT_STRING         ERRO_ADDRESS_OFFSET+28
#define DEVICE_EXCLUSIVE_LOCKED     ERRO_ADDRESS_OFFSET+29
#define UPDATE_DEVICE_TIMEOUT       ERRO_ADDRESS_OFFSET+30
#define EEPROM_INTERFACE_UNSUPPORTED    ERRO_ADDRESS_OFFSET+31
#define EEPROM_IP_MODE_OPERATE_FAILED    ERRO_ADDRESS_OFFSET+32
#define NO_DEVICES_FOUND			ERRO_ADDRESS_OFFSET+33

#endif
