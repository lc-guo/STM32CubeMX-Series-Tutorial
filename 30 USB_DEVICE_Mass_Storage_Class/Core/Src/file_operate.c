#include "file_operate.h"

//定义用于格式化的工作区缓存
BYTE workBuffer[4*User_Sector];

/*挂载FatFs文件系统*/
void Mount_FatFs(void)
{
	//挂载文件系统
	FRESULT retUSER = f_mount(&User_FatFs, User_SDPath, 1);
	//发生错误
	if(retUSER != FR_OK)
	{
		//没有文件系统，需要格式化
		if(retUSER == FR_NO_FILESYSTEM)
		{
			printf("\r\n没有文件系统,开始格式化\r\n");
			//创建文件系统
			retUSER = f_mkfs(User_SDPath, FM_FAT32, 0, workBuffer, 4*User_Sector);
			//格式化失败
			if(retUSER != FR_OK)
			{
				printf("格式化失败，错误代码 = %d\r\n", retUSER);
			}
			//格式化成功
			else
			{
				printf("格式化成功，开始重新挂载\r\n");
				//有文件系统后重新挂载
				retUSER = f_mount(&User_FatFs, User_SDPath, 1);
				//挂载失败
				if(retUSER != FR_OK)
				{
					printf("发生错误，错误代码 = %d\r\n", retUSER);
				}
				//挂载成功
				else
				{
					printf("*** 文件系统挂载成功 ***\r\n");
				}
			}
		}
		//不是没有文件系统，而是发生其他错误
		else
		{
			printf("发生其他错误，错误代码 = %d\r\n", retUSER);
		}
	}
	//有文件系统直接挂在成功
	else
	{
		printf("文件系统挂载成功\r\n");
	}
}

/*获取磁盘信息并在LCD上显示*/
void FatFs_GetDiskInfo(void)
{
    FATFS *fs;
	//定义剩余簇个数变量
    DWORD fre_clust; 
	//获取剩余簇个数
    FRESULT res = f_getfree("0:", &fre_clust, &fs); 
	//获取失败
    if(res != FR_OK)
    {
        printf("f_getfree() error\r\n");
        return;
    }
    printf("\r\n*** FAT disk info ***\r\n");
		
	//总的扇区个数
    DWORD tot_sect = (fs->n_fatent - 2) * fs->csize;  
		
	//剩余的扇区个数 = 剩余簇个数 * 每个簇的扇区个数
    DWORD fre_sect = fre_clust * fs->csize;    
		
	//对于SD卡和U盘, _MIN_SS=512字节
#if  _MAX_SS == _MIN_SS  
    //SD卡的_MIN_SS固定为512，右移11位相当于除以2048
	//剩余空间大小，单位：MB，用于SD卡，U盘
    DWORD freespace= (fre_sect>>11); 
		//总空间大小，单位：MB，用于SD卡，U盘		
    DWORD totalSpace= (tot_sect>>11);  
#else
	//Flash存储器，小容量
	//剩余空间大小，单位：KB
    DWORD freespace= (fre_sect*fs->ssize)>>10;   
	//总空间大小，单位：KB
    DWORD totalSpace= (tot_sect*fs->ssize)>>10;  
#endif

	//FAT类型
    printf("FAT type = %d\r\n",fs->fs_type);
    printf("[1=FAT12,2=FAT16,3=FAT32,4=exFAT]\r\n");
		
	//扇区大小，单位字节
    printf("Sector size(bytes) = ");
	//SD卡固定512字节
#if  _MAX_SS == _MIN_SS 
    printf("%d\r\n", _MIN_SS);
#else
	//FLASH存储器
    printf("%d\r\n", fs->ssize);
#endif
		
    printf("Cluster size(sectors) = %d\r\n", fs->csize);
    printf("Total cluster count = %ld\r\n", fs->n_fatent-2);
    printf("Total sector count = %ld\r\n", tot_sect);
		
	//总空间
#if  _MAX_SS == _MIN_SS 
    printf("Total space(MB) = %ld\r\n", totalSpace);
#else
    printf("Total space(KB) = %ld\r\n", totalSpace);
#endif
		
	//空闲簇数量
    printf("Free cluster count = %ld\r\n",fre_clust);
	//空闲扇区数量
    printf("Free sector count = %ld\r\n", fre_sect);
		
	//空闲空间
#if  _MAX_SS == _MIN_SS 
    printf("Free space(MB) = %ld\r\n", freespace);
#else
    printf("Free space(KB) = %ld\r\n", freespace);
#endif

    printf("Get FAT disk info OK\r\n");
}

/*创建文本文件*/
void FatFs_WriteTXTFile(TCHAR *filename,uint16_t year, uint8_t month, uint8_t day)
{
	FIL	file;
	printf("\r\n*** Creating TXT file: %s ***\r\n", filename);
	
	FRESULT res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
	//打开/创建文件成功
	if(res == FR_OK)
	{
		//字符串必须有换行符"\n"
		TCHAR str[]="Line1: Hello, FatFs***\n";  
		//不会写入结束符"\0"
		f_puts(str, &file); 
		
		printf("Write file OK: %s\r\n", filename);
	}
	else
	{
		printf("Open file error，error code: %d\r\n", res);
	}
	//使用完毕关闭文件
	f_close(&file);
}

/*读取一个文本文件的内容*/
void FatFs_ReadTXTFile(TCHAR *filename)
{
	printf("\r\n*** Reading TXT file: %s ***\r\n", filename);

	FIL	file;
	//以只读方式打开文件
	FRESULT res = f_open(&file, filename, FA_READ);  
	//打开成功
	if(res == FR_OK)
	{
		//读取缓存
		TCHAR str[100];
		//没有读到文件内容末尾
		while(!f_eof(&file))
		{
			//读取1个字符串,自动加上结束符”\0”
			f_gets(str,100, &file);	
			printf("%s", str);
		}
		printf("\r\n");
	}
	//如果没有该文件
	else if(res == FR_NO_FILE)
		printf("File does not exist\r\n");
	//打开失败
	else
		printf("f_open() error，error code: %d\r\n", res);
	//关闭文件
	f_close(&file);
}

/*扫描和显示指定目录下的文件和目录*/
void FatFs_ScanDir(const TCHAR* PathName)
{
	DIR dir;					//目录对象
	FILINFO fno;				//文件信息
	//打开目录
	FRESULT res = f_opendir(&dir, PathName);
	//打开失败
	if(res != FR_OK)
	{
		//关闭目录，直接退出函数
		f_closedir(&dir);
		printf("\r\nf_opendir() error，error code: %d\r\n", res);
		return;
	}
	
	printf("\r\n*** All entries in dir: %s ***\r\n", PathName);
	//顺序读取目录中的文件
	while(1)
	{
		//读取目录下的一个项
		res = f_readdir(&dir, &fno);    
		//文件名为空表示没有多的项可读了
		if(res != FR_OK || fno.fname[0] == 0)
			break;  
		//如果是一个目录
		if(fno.fattrib & AM_DIR)  		
		{
			printf("DIR: %s\r\n", fno.fname);
		}
		//如果是一个文件
		else  		
		{
			printf("FILE: %s\r\n",fno.fname);
		}
	}
	//扫描完毕，关闭目录
	printf("Scan dir OK\r\n");
	f_closedir(&dir);
}

/*获取一个文件的文件信息*/
void FatFs_GetFileInfo(TCHAR *filename)
{
	printf("\r\n*** File info of: %s ***\r\n", filename);

	FILINFO fno;
	//检查文件或子目录是否存在
	FRESULT fr = f_stat(filename, &fno);
	//如果存在从fno中读取文件信息
	if(fr == FR_OK)
	{
		printf("File size(bytes) = %ld\r\n", fno.fsize);
		printf("File attribute = 0x%x\r\n", fno.fattrib);
		printf("File Name = %s\r\n", fno.fname);
		//输出创建/修改文件时的时间戳
		FatFs_PrintfFileDate(fno.fdate, fno.ftime);
	}
	//如果没有该文件
	else if (fr == FR_NO_FILE)
		printf("File does not exist\r\n");
	//发生其他错误
	else
		printf("f_stat() error，error code: %d\r\n", fr);
}

/*删除文件*/
void FatFs_DeleteFile(TCHAR *filename)
{
	printf("\r\n*** Delete File: %s ***\r\n", filename);
	FIL	file;
	//打开文件
	FRESULT res = f_open(&file, filename, FA_OPEN_EXISTING);  
	if(res == FR_OK)
	{
		//关闭文件
		f_close(&file);
		printf("open successfully!\r\n");
	}
	//删除文件
	res = f_unlink(filename);
	//删除成功
	if(res == FR_OK)
	{
		printf("The file was deleted successfully!\r\n");
	}
	//删除失败
	else
	{
		printf("File deletion failed, error code：%d\r\n", res);
	}
}

/*打印输出文件日期*/
void FatFs_PrintfFileDate(WORD date, WORD time)
{
	printf("File data = %d/%d/%d\r\n", ((date>>9)&0x7F)+1980, (date>>5)&0xF, date&0x1F);
	printf("File time = %d:%d:%d\r\n", (time>>11)&0x1F, (time>>5)&0x3F, time&0x1F);
}
