# NBOS
小型学习用OS  
硬件平台：QEMU模拟的ARMv8 A57虚拟机  

ARMv8 裸机程序引用  
https://github.com/MaciekBielski/bareMetalAarch64  
https://github.com/NienfengYao/armv8-bare-metal  

参考  
https://github.com/s-matyukevich/raspberry-pi-os  
https://elixir.bootlin.com/linux/v2.6.24/source  
https://github.com/labrick/linux-0.11  


NBOS开发文档  
## 〇、总览
### 0.1 简介
NBOS是一个以Linux为参照的小型OS
硬件开发环境选用QEMU模拟器中的ARM64的virt machine 和 cortex-a57 CPU  
现在的配置QEMU启动后内部Bootloader做了一部分初始化，然后从0x4000 0000这个地址开始执行，并跳转到0x4008 0000，二进制镜像被加载到0x4008 0000地址处运行，默认是EL1  
### 0.2 已完成
基于EL0/EL1异常级别的任务简单调度  
中断控制器，定时器中断，串口打印  
任务控制块，静态定义了init任务  
堆的管理  
系统调用  
printf  
MMU及虚存管理  
### 0.3 下一步待完成
软件定时器  
系统调用  
delay  
## 一、运行NBOS
### 1.1 搭建开发环境

服务器环境 ubuntu 16.04  

安装QEMU模拟器  
sudo apt-get install qemu-system-arm  
安装完后会有qemu-system-aarch64可执行文件  

安装交叉编译器  
https://releases.linaro.org/components/toolchain/binaries/5.4-2017.01/aarch64-elf  
解压后添加至环境变量即可  

### 1.2 运行模拟器

#### a.获得项目  
git clone git@github.com:zhulangpi/NBOS.git  

#### b.编译并运行NBOS  

#编译产生NBOS.elf  
make  
#利用QEMU加载NBOS  
make run  

#qemu退出  

ctrl+a then x  

#### c.利用gdb调试  

#主机终端执行  
make gdb_srv  
#gdb终端执行，ctrl+x then a 退出reg layout布局  
make gdb_cli  

### 1.3 QEMU提供的硬件环境

获得虚拟机器的设备树dts  

在工程etc目录下产生设备树文件  
make dts  

以下qemu源码展示了virt虚拟机器的硬件配置  
https://github.com/qemu/qemu/blob/master/hw/arm/virt.c  


## 二、硬件结构
### 2.1 物理地址空间
由于ARM是IO与MEM统一编址，罗列地址空间方便查看  
主要从设备树获取  

|         属性        |          地址范围         |       大小       |
|---------------------|---------------------------|------------------|
| GICD(Distributor)   |  0x800 0000 - 0x801 0000  |     0x1 0000     |
| GICC(CPU interface) |  0x801 0000 - 0x802 0000  |     0x1 0000     |
|     PL011(串口)     |  0x900 0000 - 0x900 1000  |      0x1000      |
|         DRAM        | 0x4000 0000 - 0x8000 0000 | 0x4000 0000 (1G) |


## 三、任务管理
### 3.1 数据结构
#### 3.1.1 任务描述符

用于描述并管理每一个任务，作为调度的基本单位  
```c
/* 任务描述符 */  
struct task_struct{
    struct cpu_context cpu_context; //must be the first position
    int state;
    unsigned long preempt_count;
    struct mm_struct *mm;
};
```
cpu_context:   用于存放任务上下文，目前主要是CPU寄存器组的内容  
state:         任务状态  
preempt_count: 抢占计数  
mm           : 地址空间描述符  

任务的内核栈，满递减栈，地址最低处保存对应任务描述符的地址。  
如此，通过sp可以间接获得任务描述符  
### 3.2 任务调度
暂时是一个基于周期定时器和数组遍历的简单时间片轮转  

## 四、内存管理
参考资料：  
https://www.cnblogs.com/v-July-v/archive/2011/01/06/1983695.html  
https://github.com/tina0405/raspberry-pi3-mini-os/blob/a5b4bfcc1211f3a7e073ead3b80d4d509e563277/9.ldaxr%2Bcache_on/include/mm.h  

### 4.1 地址空间映射
内核空间  
0xffff 0000 0000 0000 - 0xffff 0000 4000 0000  
采用2MB的block映射  

| 页表项号 | 页表项存放PA |            VA         |      PA     | 属性  |
|----------|--------------|-----------------------|-------------|-------|
|     0    | 0x4008 9000  | 0xffff 0000 0000 0000 | 0x4000 0000 | 0x405 |
|     1    | 0x4008 9008  | 0xffff 0000 0020 0000 | 0x4020 0000 |  ...  |
|    ...   |      ...     |           ...         |      ...    |  ...  |
|    509   | 0x4008 9fe8  | 0xffff 0000 3fa0 0000 | 0x7fa0 0000 | 0x405 |
|    510   | 0x4008 9ff0  | 0xffff 0000 3fc0 0000 | 0x0900 0000 | 0x401 |
|    511   |              | 0xffff 0000 3fe0 0000 |             |       |


## 五、异常处理

## 六、设备驱动
### 6.0 引用前人代码
https://github.com/MaciekBielski/bareMetalAarch64  
https://github.com/NienfengYao/armv8-bare-metal  

### 6.1 定时器
核内定时器，产生周期中断，用于调度  
### 6.2 中断控制器GIC

### 6.3 串口




