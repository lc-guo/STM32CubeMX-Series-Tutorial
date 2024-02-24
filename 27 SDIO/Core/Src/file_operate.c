#include "file_operate.h"


uint8_t workBuffer[FLASH_SECTOR_SIZE];

/*挂载FatFS文件系统*/
void Mount_Fatfs(void)
{
	retUSER = f_mount(&USERFatFS, USERPath, 1);
		
	if(retUSER != FR_OK)
	{
		if(retUSER == FR_NO_FILESYSTEM)
		{
		
			printf("f_mount 没有文件系统,开始格式化spi-flash\r\n");
			// 簇大小为2个扇区（2*4096字节）
			retUSER = f_mkfs(USERPath, FM_FAT, 2 * FLASH_SECTOR_SIZE, workBuffer, FLASH_SECTOR_SIZE);

			if(retUSER != FR_OK)
			{
				printf("f_mkfs 格式化失败，err = %d\r\n", retUSER);
			}
			else
			{
				printf("格式化成功，开始重新挂载spi-flash\r\n");
				retUSER = f_mount(&USERFatFS, USERPath, 1);

				if(retUSER != FR_OK)
				{
					printf("f_mount 发生错误，err = %d\r\n", retUSER);
				}
				else
				{
					printf("spi-flash文件系统挂载成功\r\n");
				}
			}
			}
			else
			{
				printf("f_mount 发生其他错误，err = %d\r\n", retUSER);
			}
	}
	else
	{
		printf("spi-flash文件系统挂载成功\r\n");
	}
}



/* 获取磁盘信息并在LCD上显示  */
void fatTest_GetDiskInfo(void)
{
	FATFS *fs;
	DWORD fre_clust;  //剩余簇个数
	FRESULT res = f_getfree("0:", &fre_clust, &fs); //需要调用刷新,fre_clust
	if(res != FR_OK)
	{
		printf("f_getfree() error\r\n");
		return;
	}

	printf("*** FAT disk info ***\r\n");

	DWORD  tot_sect = (fs->n_fatent - 2) * fs->csize;  //总的扇区个数
	DWORD  fre_sect = fre_clust * fs->csize;    //剩余的扇区个数=剩余簇个数*每个簇的扇区个数
#if  _MAX_SS == _MIN_SS  //对于SD 卡和 U 盘, _MIN_SS=512字节
	//	SD卡的_MIN_SS固定为512，右移11位相当于除以2048
	DWORD  freespace= (fre_sect>>11);   //剩余空间大小，单位：MB,用于SD卡，U盘
	DWORD  totalSpace= (tot_sect>>11);  //总空间大小，单位：MB,用于SD卡，U盘
#else		//Flash存储器，小容量
	DWORD  freespace= (fre_sect*fs->ssize)>>10;   //剩余空间大小，单位：KB
	DWORD  totalSpace= (tot_sect*fs->ssize)>>10;  //总空间大小，单位：KB
#endif

	printf("FAT type = %d\r\n",fs->fs_type);
	printf("[1=FAT12,2=FAT16,3=FAT32,4=exFAT]\r\n");

	printf("Sector size(bytes)=");
#if  _MAX_SS == _MIN_SS  //SD 卡, U 盘
	printf("%d\r\n", _MIN_SS);
#else
	printf("%d\r\n", fs->ssize);
#endif

	printf("Cluster size(sectors) = %d\r\n", fs->csize);
	printf("Total cluster count = %ld\r\n", fs->n_fatent-2);
	printf("Total sector count = %ld\r\n", tot_sect);

#if  _MAX_SS == _MIN_SS  //SD 卡, U 盘
	printf("Total space(MB) = %ld\r\n", totalSpace);
#else
	printf("Total space(KB) = %ld\r\n", totalSpace);
#endif

	printf("Free cluster count = %ld\r\n",fre_clust);

	printf("Free sector count = %ld\r\n", fre_sect);

#if  _MAX_SS == _MIN_SS  //SD 卡, U 盘
	printf("Free space(MB) = %ld\r\n", freespace);
#else
	printf("Free space(KB) = %ld\r\n", freespace);
#endif

	printf("Get FAT disk info OK\r\n");
}


void fatTest_WriteBinFile(TCHAR *filename, uint32_t pointCount, uint32_t sampFreq)
{
	FIL file;
	FRESULT res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
	
	if(res == FR_OK)
	{
		TCHAR headStr[]="ADC1-IN5\n";
		f_puts(headStr, &file); //写入字符串数据,以"\n"结尾，不带"\0"

		UINT bw=0;  //实际写入字节数
		f_write(&file, &pointCount, sizeof(uint32_t), &bw);  //数据点个数
		f_write(&file, &sampFreq, sizeof(uint32_t), &bw);    //采样频率

		uint32_t  value=1000;
		for(uint16_t i=0; i<pointCount; i++,value++)
			f_write(&file, &value, sizeof(uint32_t), &bw);

		printf("Write file OK: %s\r\n", filename);
	}

	f_close(&file);
}

void fatTest_ReadBinFile(TCHAR *filename)
{
	printf("Reading BIN file: %s\r\n", filename);

	FIL file;
	FRESULT res=f_open(&file,filename, FA_READ);
	
	if(res == FR_OK)
	{
		TCHAR str[50];
		f_gets(str,50, &file);	//读取1个字符串

		UINT  bw=0;  //实际读取字节数
		uint32_t pointCount, sampFreq;  //保存读出数据的变量
		f_read(&file, &pointCount, sizeof(uint32_t), &bw);   //数据点个数
		f_read(&file, &sampFreq, sizeof(uint32_t), &bw);  //采样频率

		uint32_t  value;
		for(uint16_t i=0; i< pointCount; i++)
			f_read(&file, &value, sizeof(uint32_t), &bw);

		//LCD显示
		printf("%s\r\n",str);
		printf("Sampling freq = %d\r\n",sampFreq);
		printf("Point count = %d\r\n",pointCount);
	}
	else if (res==FR_NO_FILE)
		printf("File does not exist\r\n");
	else
		printf("f_open() error\r\n");

	f_close(&file);
}


//创建文本文件
void fatTest_WriteTXTFile(TCHAR *filename,uint16_t year, uint8_t month, uint8_t day)
{
	FIL	file;

	FRESULT res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
	printf("return:%d\r\n", res);
	if(res == FR_OK)
	{
		UINT fnum;                          							/* 文件成功读写数量 */
		
		const TCHAR str[] = "Line1: Hello, FatFS\n";  		//字符串必须有换行符"\n"
		f_write(&USERFile, str, sizeof(str), &fnum);
		printf("str写入成功，写入字节数据：%d\r\n", fnum);
		
		const TCHAR str2[]="Line2: UPC, Qingdao\n";
		f_write(&USERFile, str2, sizeof(str2), &fnum);
		printf("str2写入成功，写入字节数据：%d\r\n", fnum);

		printf("Write file OK: %s\r\n", filename);
	}
	else
		printf("Open file error\r\n");
		
	f_close(&file);
}

//读取一个文本文件的内容
void fatTest_ReadTXTFile(TCHAR *filename)
{
	printf("Reading TXT file: %s\r\n", filename);

	FIL	file;
	FRESULT res = f_open(&file,filename, FA_OPEN_EXISTING | FA_READ);   //以只读方式打开文件
	
	if(res == FR_OK)
	{
		UINT fnum;
		BYTE ReadBuffer[1024] = {0};        /* 读缓冲区 */
		printf("》打开文件成功。\r\n");
		retUSER = f_read(&file, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(retUSER==FR_OK)
    {
      printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
      printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("！！文件读取失败：(%d)\n",retUSER);
    }		
	}
	else
	{
		printf("！！打开文件失败。\r\n");
	}
//	if(res == FR_OK)
//	{
//		TCHAR str[100];
//		
//		while (!f_eof(&file))
//		{
//			f_gets(str,100, &file);	//读取1个字符串,自动加上结束符”\0”
//			printf("%s\r\n", (uint8_t *)str);
//		}
//	}
//	else if (res==FR_NO_FILE)
//		printf("File does not exist\r\n");
//	else
//		printf("f_open() error\r\n");

	f_close(&file);
}



//扫描和显示指定目录下的文件和目录
void fatTest_ScanDir(const TCHAR* PathName)
{
	DIR dir;		//目录对象
	FILINFO fno;	//文件信息
//	FRESULT res = f_opendir(&dir, "0:"); 	//打开目录
	FRESULT res = f_opendir(&dir, PathName);
	if (res != FR_OK)
	{
		f_closedir(&dir);
		return;
	}

	printf("All entries in dir\r\n");
	printf("%s\r\n", PathName);

	while(1)
	{
		res = f_readdir(&dir, &fno);    //读取目录下的一个项
		if (res != FR_OK || fno.fname[0] == 0)
			break;  // 文件名为空表示没有多的项可读了

		if (fno.fattrib & AM_DIR)  		//是一个目录
		{
			printf("DIR: %s\r\n", fno.fname);
		}
		else  		// 是一个文件
		{
			printf("FILE: %s\r\n",fno.fname);
		}
	}

	printf("Scan dir OK\r\n");
	f_closedir(&dir);
}


//获取一个文件的文件信息
void fatTest_GetFileInfo(TCHAR *filename)
{
	printf("File info of: %s\r\n", filename);
	
	FILINFO  fno;
	FRESULT fr=f_stat(filename, &fno);
	if(fr==FR_OK)
	{
		printf("File size(bytes) = %ld\r\n", fno.fsize);

		printf("File attribute = %x\r\n", fno.fattrib);

		printf("File Name = %s\r\n", fno.fname);

//		RTC_DateTypeDef sDate;
//		RTC_TimeTypeDef sTime;
//		fat_GetTimeStamp(&fno, &sDate, &sTime);  //将时间戳转换为易显示格式
//		char str[40];

//		sprintf(str,"File Date= %d-%2d-%2d",2000+sDate.Year,sDate.Month,sDate.Date);
//		printf("%s\r\n", (uint8_t *)str);

//		sprintf(str,"File Time= %2d:%2d:%2d",sTime.Hours, sTime.Minutes,sTime.Seconds);
//		printf("%s\r\n", (uint8_t *)str);

	}
	else if (fr==FR_NO_FILE)
		printf("File does not exist\r\n");
	else
		printf("f_stat() error\r\n");
}

