/* 文件： w25flash.h
 * 功能描述： Flash 存储器W25Q128的驱动程序
 * 作者：王维波
 * 修改日期：2019-06-05
 * W25Q128 芯片参数： 16M字节，24位地址线
 * 分为256个Block，每个Block 64K字节
 * 一个Block又分为16个Sector，共4096个Sector，每个Sector 4K字节
 * 一个Sector又分为16个Page，共65536个Page，每个Page 256字节
 * 写数据操作的基本单元是Page，一次连续写入操作不能超过一个Page的范围。写的Page必须是擦除过的。
 */

#ifndef _W25FLASH_H
#define _W25FLASH_H

#include 	"stm32f4xx_hal.h"
#include	"spi.h"		//使用其中的变量 hspi1，表示SPI1接口

/*  W25Q128硬件接口相关的部分：CS引脚和SPI接口 ，若电路不同，更改这部分配置即可   */
// Flash_CS -->PB14, 片选信号CS操作的宏定义函数
#define CS_PORT		GPIOB
#define	CS_PIN		GPIO_PIN_14
#define	SPI_HANDLE		hspi1		//SPI接口对象，使用spi.h中的变量 hspi1

#define	__Select_Flash()		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET)	//CS=0
#define	__Deselect_Flash()		HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET)	//CS=1

//===========Flash存储芯片W25Q128的存储容量参数================
#define		FLASH_PAGE_SIZE			256		//一个Page是256字节

#define		FLASH_SECTOR_SIZE		4096	//一个Sector是4096字节

#define		FLASH_SECTOR_COUNT		4096	//总共4096个 Sector

//=======1. SPI 基本发送和接收函数，阻塞式传输============
HAL_StatusTypeDef	SPI_TransmitOneByte(uint8_t	byteData);	//SPI接口发送一个字节
HAL_StatusTypeDef	SPI_TransmitBytes(uint8_t* pBuffer, uint16_t byteCount);	//SPI接口发送多个字节

uint8_t	SPI_ReceiveOneByte(void);	//SPI接口接收一个字节
HAL_StatusTypeDef	SPI_ReceiveBytes(uint8_t* pBuffer, uint16_t byteCount);	//SPI接口接收多个字节

//=========2. W25Qxx 基本控制指令==========
// 0xEF17,表示芯片型号为W25Q128, Winbond,用过
// 0xC817，表示芯片型号为GD25Q128,ELM，用过
// 0x1C17，表示芯片型号为EN25Q128,台湾EON
// 0xA117，表示芯片型号为FM25Q128，复旦微电子
// 0x2018，表示芯片型号为N25Q128，美光
// 0x2017，表示芯片型号为XM25QH128，武汉新芯，用过

uint16_t  Flash_ReadID(void); 	// Command=0x90, Manufacturer/Device ID

uint64_t  Flash_ReadSerialNum(uint32_t* High32,  uint32_t* Low32); //Command=0x4B, Read Unique ID, 64-bit

HAL_StatusTypeDef Flash_WriteVolatile_Enable(void);  	//Command=0x50: Write Volatile Enable

HAL_StatusTypeDef Flash_Write_Enable(void);  	//Command=0x06: Write Enable,    使WEL=1
HAL_StatusTypeDef Flash_Write_Disable(void);	//Command=0x04, Write Disable，	  使WEL=0

uint8_t	 Flash_ReadSR1(void);  	//Command=0x05:  Read Status Register-1,	返回寄存器SR1的值
uint8_t	 Flash_ReadSR2(void);  	//Command=0x35:  Read Status Register-2,	返回寄存器SR2的值

void Flash_WriteSR1(uint8_t SR1);  //Command=0x01:  Write Status Register,	只写SR1的值,禁止写状态寄存器

uint32_t Flash_Wait_Busy(void);  	//读状态寄存器SR1，等待BUSY变为0,返回值是等待时间
void Flash_PowerDown(void);   		//Command=0xB9: Power Down
void Flash_WakeUp(void);  			//Command=0xAB: Release Power Down

//========3. 计算地址的辅助功能函数========
//根据Block  绝对编号获取地址,共256个Block
uint32_t	Flash_Addr_byBlock(uint8_t BlockNo);
//根据Sector 绝对编号获取地址,共4096个Sector
uint32_t	Flash_Addr_bySector(uint16_t  SectorNo);
//根据Page  绝对编号获取地址，共65536个Page
uint32_t	Flash_Addr_byPage(uint16_t  PageNo);

//根据Block编号，和内部Sector编号计算地址，一个Block有16个Sector,
uint32_t	Flash_Addr_byBlockSector(uint8_t BlockNo, uint8_t SubSectorNo);
//根据Block编号，内部Sector编号，内部Page编号计算地址
uint32_t	Flash_Addr_byBlockSectorPage(uint8_t BlockNo, uint8_t SubSectorNo, uint8_t  SubPageNo);
//将24位地址分解为3个字节
void		Flash_SpliteAddr(uint32_t globalAddr, uint8_t* addrHigh, uint8_t* addrMid,uint8_t* addrLow);

//=======4. chip、Block，Sector擦除函数============
//Command=0xC7: Chip Erase, 擦除整个器件,大约25秒
void Flash_EraseChip(void);

//Command=0xD8: Block Erase(64KB) 擦除整个Block, globalAddr是全局地址，耗时大约150ms
void Flash_EraseBlock64K(uint32_t globalAddr);

//Command=0x20: Sector Erase(4KB) 扇区擦除, globalAddr是扇区的全局地址，耗时大约30ms
void Flash_EraseSector(uint32_t globalAddr);

//=========5. 数据读写函数=============
//Command=0x03,  读取一个字节，任意全局地址
uint8_t Flash_ReadOneByte(uint32_t globalAddr);

//Command=0x03,  连续读取多个字节，任意全局地址
void Flash_ReadBytes(uint32_t globalAddr, uint8_t* pBuffer,  uint16_t byteCount);

//Command=0x0B,  高速连续读取多个字节，任意全局地址， 速度大约是常规读取的2倍
void Flash_FastReadBytes(uint32_t globalAddr, uint8_t* pBuffer,  uint16_t byteCount);

//Command=0x02: Page program 对一个Page写入数据（最多256字节）, globalAddr是初始位置的全局地址，耗时大约3ms
void Flash_WriteInPage(uint32_t globalAddr, uint8_t* pBuffer, uint16_t byteCount);

//从某个Sector的起始地址开始写数据，数据可能跨越多个Page,甚至跨越Sector，总字节数byteCount不能超过64K，也就是一个Block的大小
void Flash_WriteSector(uint32_t globalAddr,  uint8_t* pBuffer, uint16_t byteCount);

#endif
