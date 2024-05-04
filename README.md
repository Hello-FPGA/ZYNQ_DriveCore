# ZYNQ_DriveCore



## 概述

Xilinx ZYNQ平台 Linux底层驱动包，实现PS与PL 内存交互。

* 测试平台 ALINX Z19， FPGA XCZU19EG-2FFVC1760I，实际上代码适用于任意xilinx zynq 平台。
* VIVADO 2019.1， SDK2019.1
* 寄存器交互参考 https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/18842412/Accessing+BRAM+In+Linux#AccessingBRAMInLinux-1.1.1.2AccessingRegisters 
* DMA参考 https://github.com/ikwzm/udmabuf



## 路径

* fpga目录存放fpga工程相关文件，工程通过脚本方式管理。在script目录下，运行build.sh即可重建demo工程，注意使用前修改build.sh中的VIVADO安装目录。

  ![image-20240413193345121](README.assets/image-20240413193345121.png)



## 测试

使用dma_rw.cpp 对memory 读写进行测试，测试结果如下所示，其中读速度只有100MB，写速度只有790MB。这是因为当前的方式并非真正的DMA，而是内存映射的方式，无需内核驱动的支持。这种方式虽然与用户交互的速度慢，但是实现简单。实际上ZYNQ的应用里面，数据处理都会放在PL完成，PS主要完成流程控制、交互，因此这里的memory读写速度实际上并不影响应用，需要DMA的场合只需要在PL侧完成即可。

![image-20240504223822825](README.assets/image-20240504223822825.png)