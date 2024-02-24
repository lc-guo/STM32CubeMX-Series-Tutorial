/* 文件： w25flash.c
 * 功能描述： Flash 存储器W25Q128的驱动程序
 * 作者：王维波
 * 修改日期：2019-06-05
 */


#include "w25flash.h"

#define MAX_TIMEOUT   200		//SPI轮询操作时的最大等待时间,ms

//SPI接口发送一个字节,byteData是需要发送的数据
HAL_StatusTypeDef	SPI_TransmitOneByte(uint8_t	byteData)
{
	return HAL_SPI_Transmit(&SPI_HANDLE, &byteData, 1, MAX_TIMEOUT);
}

//SPI接口发送多个字节, pBuffer是发送数据缓存区指针，byteCount是发送数据字节数，byteCount最大256
HAL_StatusTypeDef	SPI_TransmitBytes(uint8_t* pBuffer, uint16_t byteCount)
{
	return HAL_SPI_Transmit(&SPI_HANDLE, pBuffer, byteCount, MAX_TIMEOUT);
}

//SPI接口接收一个字节， 返回接收的一个字节数据
uint8_t	SPI_ReceiveOneByte()
{
	uint8_t	byteData=0;
	HAL_SPI_Receive(&SPI_HANDLE, &byteData, 1, MAX_TIMEOUT);
	return  byteData;
}

//SPI接口接收多个字节， pBuffer是接收数据缓存区指针，byteCount是需要接收数据的字节数
HAL_StatusTypeDef	SPI_ReceiveBytes(uint8_t* pBuffer, uint16_t byteCount)
{
	return HAL_SPI_Receive(&SPI_HANDLE, pBuffer, byteCount, MAX_TIMEOUT);
}

//Command=0x05:  Read Status Register-1,返回寄存器SR1的值
uint8_t Flash_ReadSR1(void)
{  
	uint8_t byte=0;
	__Select_Flash();	//CS=0
	SPI_TransmitOneByte(0x05); //Command=0x05:  Read Status Register-1
	byte=SPI_ReceiveOneByte();
	__Deselect_Flash();	//CS=1
	return byte;   
} 

//Command=0x35:  Read Status Register-2,返回寄存器SR2的值
uint8_t Flash_ReadSR2(void)
{
	uint8_t byte=0;
	__Select_Flash();	//CS=0
	SPI_TransmitOneByte(0x35); //Command=0x35:  Read Status Register-2
	byte=SPI_ReceiveOneByte();	//读取一个字节
	__Deselect_Flash();	//CS=1
	return byte;
}


//Command=0x01:  Write Status Register,	只写SR1的值
//耗时大约10-15ms
void Flash_WriteSR1(uint8_t SR1)
{   
	Flash_Write_Enable();       //必须使 WEL=1

	__Select_Flash();	//CS=0
	SPI_TransmitOneByte(0x01);   //Command=0x01:  Write Status Register,	只写SR1的值
	SPI_TransmitOneByte(0x00);    //SR1的值
//	SPI_WriteOneByte(0x00);    //SR2的值, 只发送SR1的值，而不发送SR2的值， QE和CMP将自动被清零
	__Deselect_Flash();	//CS=1

	Flash_Wait_Busy(); 	   //耗时大约10-15ms
}  

HAL_StatusTypeDef Flash_WriteVolatile_Enable(void)  	//Command=0x50: Write Volatile Enable
{
	__Select_Flash();	//CS=0
	HAL_StatusTypeDef result=SPI_TransmitOneByte(0x50);
	__Deselect_Flash();	//CS=1
	return result;
}


//Command=0x06: Write Enable,    使WEL=1
HAL_StatusTypeDef Flash_Write_Enable(void)
{
	__Select_Flash();	//CS=0
	HAL_StatusTypeDef result=SPI_TransmitOneByte(0x06);  //Command=0x06: Write Enable,    使WEL=1
	__Deselect_Flash();	//CS=1
	Flash_Wait_Busy(); 	//等待操作完成
	return result;
} 

//Command=0x04, Write Disable，	  使WEL=0
HAL_StatusTypeDef Flash_Write_Disable(void)
{  
	__Select_Flash();	//CS=0
	HAL_StatusTypeDef result=SPI_TransmitOneByte(0x04); 	//Command=0x04, Write Disable，	  使WEL=0
	__Deselect_Flash();	//CS=1
	Flash_Wait_Busy(); 	   //
	return result;
} 

//根据Block绝对编号获取地址, 共256个Block， BlockNo 取值范围0-255
//每个块64K字节，16位地址，块内地址范围0x0000-0xFFFF。
uint32_t	Flash_Addr_byBlock(uint8_t	BlockNo)
{
//	uint32_t addr=BlockNo*0x10000;

	uint32_t addr=BlockNo;
	addr=addr<<16; //左移16位，等于乘以0x10000
	return addr;
}

//根据Sector绝对编号获取地址, 共4096个Sector, SectorNo取值范围0-4095
//每个扇区4K字节，12位地址，扇区内地址范围0x000-0xFFF
uint32_t	Flash_Addr_bySector(uint16_t  SectorNo)
{
	if (SectorNo>4095)	//不能超过4095
		SectorNo=0;
//	uint32_t addr=SectorNo*0x1000;

	uint32_t addr=SectorNo;
	addr=addr<<12;		//左移12位,等于乘以0x1000
	return addr;
}

//根据Page绝对编号获取地址，共65536个Page,  PageNo取值范围0-65535
//每个页256字节，8位地址，页内地址范围0x00—0xFF
uint32_t	Flash_Addr_byPage(uint16_t  PageNo)
{
//	uint32_t addr=PageNo*0x100;

	uint32_t addr=PageNo;
	addr=addr<<8;		//左移8位，等于乘以0x100
	return addr;
}

//根据Block编号和内部Sector编号计算地址，一个Block有16个Sector
//BlockNo取值范围0-255，  内部SubSectorNo取值范围0-15
uint32_t	Flash_Addr_byBlockSector(uint8_t BlockNo, uint8_t SubSectorNo)
{
	if (SubSectorNo>15)	 //不能超过15
		SubSectorNo=0;

//	uint32_t addr=BlockNo*0x10000;	//先计算Block的起始地址
	uint32_t addr=BlockNo;
	addr=addr<<16;	//先计算Block的起始地址

//	uint32_t offset=SubSectorNo*0x1000;	//计算Sector的偏移地址
	uint32_t offset=SubSectorNo;	//计算Sector的偏移地址
	offset=offset<<12;	//计算Sector的偏移地址

	addr += offset;

	return addr;
}

//根据Block编号，内部Sector编号，内部Page编号获取地址
//BlockNo取值范围0-255
//一个Block有16个Sector， 内部SubSectorNo取值范围0-15
//一个Sector有16个Page , 内部SubPageNo取值范围0-15
uint32_t	Flash_Addr_byBlockSectorPage(uint8_t BlockNo, uint8_t SubSectorNo, uint8_t  SubPageNo)
{
	if (SubSectorNo>15)	//不能超过15
		SubSectorNo=0;

	if (SubPageNo>15)	//不能超过15
		SubPageNo=0;

//	uint32_t addr=BlockNo*0x10000;		//先计算Block的起始地址
	uint32_t addr=BlockNo;
	addr=addr<<16;		//先计算Block的起始地址

//	uint32_t offset=SubSectorNo*0x1000;	//计算Sector的偏移地址
	uint32_t offset=SubSectorNo;	//计算Sector的偏移地址
	offset=offset<<12;	//计算Sector的偏移地址
	addr += offset;

//	offset=SubPageNo*0x100;	//计算Page的偏移地址
	offset=SubPageNo;
	offset=offset<<8;	//计算Page的偏移地址

	addr += offset;		//Page的起始地址
	return addr;
}

//将24位地址分解为3个字节
//globalAddr是全局24位地址, 返回 addrHigh高字节，addrMid中间字节，addrLow低字节
void  Flash_SpliteAddr(uint32_t globalAddr, uint8_t* addrHigh, uint8_t* addrMid,uint8_t* addrLow)
{
	*addrHigh= (globalAddr>>16);	//addrHigh=高字节

	globalAddr =globalAddr & 0x0000FFFF;	//屏蔽高字节
	*addrMid= (globalAddr>>8);	//addrMid=中间字节

	*addrLow =globalAddr & 0x000000FF;	//屏蔽中间字节， 只剩低字节，addrLow=低字节
}


//读取芯片ID
//返回值如下:				   
// 0xEF17,表示芯片型号为W25Q128, Winbond,用过
// 0xC817，表示芯片型号为GD25Q128,ELM，用过
// 0x1C17，表示芯片型号为EN25Q128,台湾EON
// 0xA117，表示芯片型号为FM25Q128，复旦微电子
// 0x2018，表示芯片型号为N25Q128，美光
// 0x2017，表示芯片型号为XM25QH128，武汉新芯，用过

//读取芯片的制造商和器件ID，高字节是Manufacturer ID,低字节是Device ID
uint16_t Flash_ReadID(void)
{
	uint16_t Temp = 0;
	__Select_Flash();	//CS=0

	SPI_TransmitOneByte(0x90);		 //指令码，0x90=Manufacturer/Device ID
	SPI_TransmitOneByte(0x00);		 //dummy
	SPI_TransmitOneByte(0x00);		 //dummy
	SPI_TransmitOneByte(0x00);		 //0x00
	Temp =SPI_ReceiveOneByte()<<8; //Manufacturer ID
	Temp|=SPI_ReceiveOneByte();	 	 //Device ID, 与具体器件相关

	__Deselect_Flash();	//CS=1
	
	return Temp;
}

// 参数High32和Low32分别返回64位序列号的高32位和低32位的值
// 函数返回值为64位序列号的值
uint64_t  Flash_ReadSerialNum(uint32_t* High32,  uint32_t* Low32)//读取64位序列号，
{
	uint8_t Temp = 0;
	uint64_t SerialNum=0;
	uint32_t High=0,Low=0;

	__Select_Flash();	//CS=0
	SPI_TransmitOneByte(0x4B);		//发送指令码, 4B=read Unique ID
	SPI_TransmitOneByte(0x00);		//发送4个Dummy字节数据
	SPI_TransmitOneByte(0x00);
	SPI_TransmitOneByte(0x00);
	SPI_TransmitOneByte(0x00);

	for(uint8_t i=0; i<4; i++)  //高32位
	{
		Temp =SPI_ReceiveOneByte();
		High = (High<<8);
		High = High | Temp;  //按位或
	}

	for(uint8_t i=0; i<4; i++)	//低32位
	{
		Temp =SPI_ReceiveOneByte();
		Low = (Low<<8);
		Low = Low | Temp;  //按位或
	}
	__Deselect_Flash();	//CS=1

	*High32 = High;
	*Low32=Low;

	SerialNum = High;
	SerialNum = SerialNum<<32;  //高32位
	SerialNum=SerialNum | Low;

	return SerialNum;
}


//在任意地址读取一个字节的数据,返回读取的字节数据
// globalAddr是24位全局地址
uint8_t Flash_ReadOneByte(uint32_t globalAddr)
{
	uint8_t byte2, byte3, byte4;
	Flash_SpliteAddr(globalAddr, &byte2, &byte3, &byte4);	//24位地址分解为3个字节

	__Select_Flash();	//CS=0
	SPI_TransmitOneByte(0x03);      //Command=0x03, read data
	SPI_TransmitOneByte(byte2);		//发送24位地址
	SPI_TransmitOneByte(byte3);
	SPI_TransmitOneByte(byte4);
	byte2 = SPI_ReceiveOneByte();	//接收1个字节
	__Deselect_Flash();	//CS=1

	return byte2;
}


//从任何地址开始读取指定长度的数据
//globalAddr：开始读取的地址(24bit)， pBuffer：数据存储区指针，byteCount:要读取的字节数
void Flash_ReadBytes(uint32_t globalAddr, uint8_t* pBuffer, uint16_t byteCount)
{ 
	uint8_t byte2, byte3, byte4;
	Flash_SpliteAddr(globalAddr, &byte2, &byte3, &byte4);	//24位地址分解为3个字节

	__Select_Flash();	//CS=0
	SPI_TransmitOneByte(0x03);      //Command=0x03, read data
	SPI_TransmitOneByte(byte2);		//发送24位地址
	SPI_TransmitOneByte(byte3);
	SPI_TransmitOneByte(byte4);
	SPI_ReceiveBytes(pBuffer, byteCount);	//接收byteCount个字节数据
	__Deselect_Flash();	//CS=1
}  

//Command=0x0B,  高速连续读取flash多个字节，任意全局地址， 速度大约是常规读取的2倍
void Flash_FastReadBytes(uint32_t globalAddr, uint8_t* pBuffer,  uint16_t byteCount)
{
// 	uint16_t i;
	uint8_t byte2, byte3, byte4;
	Flash_SpliteAddr(globalAddr, &byte2, &byte3, &byte4);	//24位地址分解为3个字节

	__Select_Flash();	//CS=0

	SPI_TransmitOneByte(0x0B);      //Command=0x0B, fast read data
	SPI_TransmitOneByte(byte2);		//发送24位地址
	SPI_TransmitOneByte(byte3);
	SPI_TransmitOneByte(byte4);
	SPI_TransmitOneByte(0x00);		//Dummy字节

	SPI_ReceiveBytes(pBuffer, byteCount);	//接收byteCount个字节数据
	__Deselect_Flash();	//CS=1

}

//Command=0xC7: Chip Erase, 擦除整个器件
// 擦除后，所有存储区内容为0xFF,耗时大约25秒
void Flash_EraseChip(void)
{                                   
	Flash_Write_Enable();   //使 WEL=1
	Flash_Wait_Busy();   	//等待空闲

	__Select_Flash();		//CS=0
	SPI_TransmitOneByte(0xC7);  // Command=0xC7: Chip Erase, 擦除整个器件
	__Deselect_Flash();		//CS=1

	Flash_Wait_Busy();   //等待芯片擦除结束,大约25秒
}   

// Command=0x02: Page program, 对一个页（256字节）编程, 耗时大约3ms,
// globalAddr是写入初始地址，全局地址
// pBuffer是要写入数据缓冲区指针，byteCount是需要写入的数据字节数
// 写入的Page必须是前面已经擦除过的，如果写入地址超出了page的边界，就从Page的开头重新写
void Flash_WriteInPage(uint32_t globalAddr, uint8_t* pBuffer, uint16_t byteCount)
{
	uint8_t byte2, byte3, byte4;
	Flash_SpliteAddr(globalAddr, &byte2, &byte3, &byte4);	//24位地址分解为3个字节

	Flash_Write_Enable();   //SET WEL
 	Flash_Wait_Busy();

	__Select_Flash();	//CS=0
	SPI_TransmitOneByte(0x02);      //Command=0x02: Page program 对一个扇区编程
	SPI_TransmitOneByte(byte2);	//发送24位地址
	SPI_TransmitOneByte(byte3);
	SPI_TransmitOneByte(byte4);
	SPI_TransmitBytes(pBuffer, byteCount); 		//发送byteCount个字节的数据
//	for(uint16_t i=0; i<byteCount; i++)
//	{
//		byte2=pBuffer[i];
//		SPI_WriteOneByte(byte2);	//要写入的数据
//	}
	__Deselect_Flash();	//CS=1

	Flash_Wait_Busy(); 	   //耗时大约3ms
}

//从某个Sector的起始位置开始写数据，数据可能跨越多个Page,甚至跨越Sector,不必提前擦除
// globalAddr是写入初始地址，全局地址,是扇区的起始地址，
// pBuffer是要写入数据缓冲区指针
// byteCount是需要写入的数据字节数，byteCount不能超过64K，也就是一个Block（16个扇区）的大小,但是可以超过一个Sector(4K字节)
// 如果数据超过一个Page，自动分成多个Page，调用EN25Q_WriteInPage分别写入
void Flash_WriteSector(uint32_t globalAddr,  uint8_t* pBuffer, uint16_t byteCount)
{
//需要先擦除扇区，可能是重复写文件
	uint8_t secCount= (byteCount / FLASH_SECTOR_SIZE);	//数据覆盖的扇区个数
	if ((byteCount % FLASH_SECTOR_SIZE) >0)
		secCount++;

	uint32_t startAddr=globalAddr;
	for (uint8_t k=0; k<secCount; k++)
	{
		Flash_EraseSector(startAddr);	//擦除扇区
		startAddr += FLASH_SECTOR_SIZE;	//移到下一个扇区
	}

//分成Page写入数据,写入数据的最小单位是Page
	uint16_t  leftBytes=byteCount % FLASH_PAGE_SIZE;  //非整数个Page剩余的字节数，即最后一个Page写入的数据
	uint16_t  pgCount=byteCount/FLASH_PAGE_SIZE;  //前面整数个Page
	uint8_t* buff=pBuffer;
	for(uint16_t i=0; i<pgCount; i++)	//写入前面pgCount个Page的数据，
	{
		Flash_WriteInPage(globalAddr, buff, FLASH_PAGE_SIZE);  //写一整个Page的数据
		globalAddr += FLASH_PAGE_SIZE;	//地址移动一个Page
		buff += FLASH_PAGE_SIZE;		//数据指针移动一个Page大小
	}

	if (leftBytes>0)
		Flash_WriteInPage(globalAddr, buff, leftBytes);  //最后一个Page，不是一整个Page的数据
}

//Command=0xD8: Block Erase(64KB) 擦除整个Block, globalAddr是全局地址
//清除后存储区内容全部为0xFF,  耗时大概150ms
void Flash_EraseBlock64K(uint32_t globalAddr)
{
 	Flash_Write_Enable();   //SET WEL
 	Flash_Wait_Busy();

	uint8_t byte2, byte3, byte4;
	Flash_SpliteAddr(globalAddr, &byte2, &byte3, &byte4);	//24位地址分解为3个字节

	__Select_Flash();	//CS=0

	SPI_TransmitOneByte(0xD8);      //Command=0xD8, Block Erase(64KB)
	SPI_TransmitOneByte(byte2);	//发送24位地址
	SPI_TransmitOneByte(byte3);
	SPI_TransmitOneByte(byte4);

	__Deselect_Flash();	//CS=1
	Flash_Wait_Busy(); 	   //耗时大概150ms
}


//擦除一个扇区(4KB字节)，Command=0x20, Sector Erase(4KB)
//globalAddr: 扇区的绝对地址，24位地址0x00XXXXXX
//擦除后，扇区内全部内容为0xFF, 耗时大约30ms，
void Flash_EraseSector(uint32_t globalAddr)
{  
 	Flash_Write_Enable();   //SET WEL
 	Flash_Wait_Busy();
	uint8_t byte2, byte3, byte4;
	Flash_SpliteAddr(globalAddr, &byte2, &byte3, &byte4);	//24位地址分解为3个字节

	__Select_Flash();	//CS=0

	SPI_TransmitOneByte(0x20);      //Command=0x20, Sector Erase(4KB)
	SPI_TransmitOneByte(byte2);		//发送24位地址
	SPI_TransmitOneByte(byte3);
	SPI_TransmitOneByte(byte4);

	__Deselect_Flash();		//CS=1
	Flash_Wait_Busy(); 	   //大约30ms
}

//检查寄存器SR1的BUSY位，直到BUSY位为0
uint32_t Flash_Wait_Busy(void)
{   
	uint8_t	SR1=0;
	uint32_t  delay=0;
	SR1=Flash_ReadSR1();	//读取状态寄存器SR1
	while((SR1 & 0x01)==0x01)
	{
		HAL_Delay(1);	//延时1ms
		delay++;
		SR1=Flash_ReadSR1();	//读取状态寄存器SR1
	}
	return delay;
}

//进入掉电模式
//Command=0xB9: Power Down
void Flash_PowerDown(void)
{ 
	__Select_Flash();	//CS=0
	SPI_TransmitOneByte(0xB9);  //Command=0xB9: Power Down
	__Deselect_Flash();	//CS=1
    HAL_Delay(1); //等待TPD
}   

//唤醒
//Command=0xAB: Release Power Down
void Flash_WakeUp(void)
{  
	__Select_Flash();	//CS=0
	SPI_TransmitOneByte(0xAB);		//Command=0xAB: Release Power Down
	__Deselect_Flash();	//CS=1
	HAL_Delay(1);     //等待TRES1
}
