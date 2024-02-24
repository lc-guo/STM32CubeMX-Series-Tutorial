#include "file_operate.h"


uint8_t workBuffer[FLASH_SECTOR_SIZE];

/*����FatFS�ļ�ϵͳ*/
void Mount_Fatfs(void)
{
	retUSER = f_mount(&USERFatFS, USERPath, 1);
		
	if(retUSER != FR_OK)
	{
		if(retUSER == FR_NO_FILESYSTEM)
		{
		
			printf("f_mount û���ļ�ϵͳ,��ʼ��ʽ��spi-flash\r\n");
			// �ش�СΪ2��������2*4096�ֽڣ�
			retUSER = f_mkfs(USERPath, FM_FAT, 2 * FLASH_SECTOR_SIZE, workBuffer, FLASH_SECTOR_SIZE);

			if(retUSER != FR_OK)
			{
				printf("f_mkfs ��ʽ��ʧ�ܣ�err = %d\r\n", retUSER);
			}
			else
			{
				printf("��ʽ���ɹ�����ʼ���¹���spi-flash\r\n");
				retUSER = f_mount(&USERFatFS, USERPath, 1);

				if(retUSER != FR_OK)
				{
					printf("f_mount ��������err = %d\r\n", retUSER);
				}
				else
				{
					printf("spi-flash�ļ�ϵͳ���سɹ�\r\n");
				}
			}
			}
			else
			{
				printf("f_mount ������������err = %d\r\n", retUSER);
			}
	}
	else
	{
		printf("spi-flash�ļ�ϵͳ���سɹ�\r\n");
	}
}



/* ��ȡ������Ϣ����LCD����ʾ  */
void fatTest_GetDiskInfo(void)
{
	FATFS *fs;
	DWORD fre_clust;  //ʣ��ظ���
	FRESULT res = f_getfree("0:", &fre_clust, &fs); //��Ҫ����ˢ��,fre_clust
	if(res != FR_OK)
	{
		printf("f_getfree() error\r\n");
		return;
	}

	printf("*** FAT disk info ***\r\n");

	DWORD  tot_sect = (fs->n_fatent - 2) * fs->csize;  //�ܵ���������
	DWORD  fre_sect = fre_clust * fs->csize;    //ʣ�����������=ʣ��ظ���*ÿ���ص���������
#if  _MAX_SS == _MIN_SS  //����SD ���� U ��, _MIN_SS=512�ֽ�
	//	SD����_MIN_SS�̶�Ϊ512������11λ�൱�ڳ���2048
	DWORD  freespace= (fre_sect>>11);   //ʣ��ռ��С����λ��MB,����SD����U��
	DWORD  totalSpace= (tot_sect>>11);  //�ܿռ��С����λ��MB,����SD����U��
#else		//Flash�洢����С����
	DWORD  freespace= (fre_sect*fs->ssize)>>10;   //ʣ��ռ��С����λ��KB
	DWORD  totalSpace= (tot_sect*fs->ssize)>>10;  //�ܿռ��С����λ��KB
#endif

	printf("FAT type = %d\r\n",fs->fs_type);
	printf("[1=FAT12,2=FAT16,3=FAT32,4=exFAT]\r\n");

	printf("Sector size(bytes)=");
#if  _MAX_SS == _MIN_SS  //SD ��, U ��
	printf("%d\r\n", _MIN_SS);
#else
	printf("%d\r\n", fs->ssize);
#endif

	printf("Cluster size(sectors) = %d\r\n", fs->csize);
	printf("Total cluster count = %ld\r\n", fs->n_fatent-2);
	printf("Total sector count = %ld\r\n", tot_sect);

#if  _MAX_SS == _MIN_SS  //SD ��, U ��
	printf("Total space(MB) = %ld\r\n", totalSpace);
#else
	printf("Total space(KB) = %ld\r\n", totalSpace);
#endif

	printf("Free cluster count = %ld\r\n",fre_clust);

	printf("Free sector count = %ld\r\n", fre_sect);

#if  _MAX_SS == _MIN_SS  //SD ��, U ��
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
		f_puts(headStr, &file); //д���ַ�������,��"\n"��β������"\0"

		UINT bw=0;  //ʵ��д���ֽ���
		f_write(&file, &pointCount, sizeof(uint32_t), &bw);  //���ݵ����
		f_write(&file, &sampFreq, sizeof(uint32_t), &bw);    //����Ƶ��

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
		f_gets(str,50, &file);	//��ȡ1���ַ���

		UINT  bw=0;  //ʵ�ʶ�ȡ�ֽ���
		uint32_t pointCount, sampFreq;  //����������ݵı���
		f_read(&file, &pointCount, sizeof(uint32_t), &bw);   //���ݵ����
		f_read(&file, &sampFreq, sizeof(uint32_t), &bw);  //����Ƶ��

		uint32_t  value;
		for(uint16_t i=0; i< pointCount; i++)
			f_read(&file, &value, sizeof(uint32_t), &bw);

		//LCD��ʾ
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


//�����ı��ļ�
void fatTest_WriteTXTFile(TCHAR *filename,uint16_t year, uint8_t month, uint8_t day)
{
	FIL	file;

	FRESULT res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
	printf("return:%d\r\n", res);
	if(res == FR_OK)
	{
		UINT fnum;                          							/* �ļ��ɹ���д���� */
		
		const TCHAR str[] = "Line1: Hello, FatFS\n";  		//�ַ��������л��з�"\n"
		f_write(&USERFile, str, sizeof(str), &fnum);
		printf("strд��ɹ���д���ֽ����ݣ�%d\r\n", fnum);
		
		const TCHAR str2[]="Line2: UPC, Qingdao\n";
		f_write(&USERFile, str2, sizeof(str2), &fnum);
		printf("str2д��ɹ���д���ֽ����ݣ�%d\r\n", fnum);

		printf("Write file OK: %s\r\n", filename);
	}
	else
		printf("Open file error\r\n");
		
	f_close(&file);
}

//��ȡһ���ı��ļ�������
void fatTest_ReadTXTFile(TCHAR *filename)
{
	printf("Reading TXT file: %s\r\n", filename);

	FIL	file;
	FRESULT res = f_open(&file,filename, FA_OPEN_EXISTING | FA_READ);   //��ֻ����ʽ���ļ�
	
	if(res == FR_OK)
	{
		UINT fnum;
		BYTE ReadBuffer[1024] = {0};        /* �������� */
		printf("�����ļ��ɹ���\r\n");
		retUSER = f_read(&file, ReadBuffer, sizeof(ReadBuffer), &fnum); 
    if(retUSER==FR_OK)
    {
      printf("���ļ���ȡ�ɹ�,�����ֽ����ݣ�%d\r\n",fnum);
      printf("����ȡ�õ��ļ�����Ϊ��\r\n%s \r\n", ReadBuffer);	
    }
    else
    {
      printf("�����ļ���ȡʧ�ܣ�(%d)\n",retUSER);
    }		
	}
	else
	{
		printf("�������ļ�ʧ�ܡ�\r\n");
	}
//	if(res == FR_OK)
//	{
//		TCHAR str[100];
//		
//		while (!f_eof(&file))
//		{
//			f_gets(str,100, &file);	//��ȡ1���ַ���,�Զ����Ͻ�������\0��
//			printf("%s\r\n", (uint8_t *)str);
//		}
//	}
//	else if (res==FR_NO_FILE)
//		printf("File does not exist\r\n");
//	else
//		printf("f_open() error\r\n");

	f_close(&file);
}



//ɨ�����ʾָ��Ŀ¼�µ��ļ���Ŀ¼
void fatTest_ScanDir(const TCHAR* PathName)
{
	DIR dir;		//Ŀ¼����
	FILINFO fno;	//�ļ���Ϣ
//	FRESULT res = f_opendir(&dir, "0:"); 	//��Ŀ¼
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
		res = f_readdir(&dir, &fno);    //��ȡĿ¼�µ�һ����
		if (res != FR_OK || fno.fname[0] == 0)
			break;  // �ļ���Ϊ�ձ�ʾû�ж����ɶ���

		if (fno.fattrib & AM_DIR)  		//��һ��Ŀ¼
		{
			printf("DIR: %s\r\n", fno.fname);
		}
		else  		// ��һ���ļ�
		{
			printf("FILE: %s\r\n",fno.fname);
		}
	}

	printf("Scan dir OK\r\n");
	f_closedir(&dir);
}


//��ȡһ���ļ����ļ���Ϣ
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
//		fat_GetTimeStamp(&fno, &sDate, &sTime);  //��ʱ���ת��Ϊ����ʾ��ʽ
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

