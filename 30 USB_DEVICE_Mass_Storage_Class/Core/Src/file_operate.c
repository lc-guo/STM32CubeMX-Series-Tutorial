#include "file_operate.h"

//�������ڸ�ʽ���Ĺ���������
BYTE workBuffer[4*User_Sector];

/*����FatFs�ļ�ϵͳ*/
void Mount_FatFs(void)
{
	//�����ļ�ϵͳ
	FRESULT retUSER = f_mount(&User_FatFs, User_SDPath, 1);
	//��������
	if(retUSER != FR_OK)
	{
		//û���ļ�ϵͳ����Ҫ��ʽ��
		if(retUSER == FR_NO_FILESYSTEM)
		{
			printf("\r\nû���ļ�ϵͳ,��ʼ��ʽ��\r\n");
			//�����ļ�ϵͳ
			retUSER = f_mkfs(User_SDPath, FM_FAT32, 0, workBuffer, 4*User_Sector);
			//��ʽ��ʧ��
			if(retUSER != FR_OK)
			{
				printf("��ʽ��ʧ�ܣ�������� = %d\r\n", retUSER);
			}
			//��ʽ���ɹ�
			else
			{
				printf("��ʽ���ɹ�����ʼ���¹���\r\n");
				//���ļ�ϵͳ�����¹���
				retUSER = f_mount(&User_FatFs, User_SDPath, 1);
				//����ʧ��
				if(retUSER != FR_OK)
				{
					printf("�������󣬴������ = %d\r\n", retUSER);
				}
				//���سɹ�
				else
				{
					printf("*** �ļ�ϵͳ���سɹ� ***\r\n");
				}
			}
		}
		//����û���ļ�ϵͳ�����Ƿ�����������
		else
		{
			printf("�����������󣬴������ = %d\r\n", retUSER);
		}
	}
	//���ļ�ϵͳֱ�ӹ��ڳɹ�
	else
	{
		printf("�ļ�ϵͳ���سɹ�\r\n");
	}
}

/*��ȡ������Ϣ����LCD����ʾ*/
void FatFs_GetDiskInfo(void)
{
    FATFS *fs;
	//����ʣ��ظ�������
    DWORD fre_clust; 
	//��ȡʣ��ظ���
    FRESULT res = f_getfree("0:", &fre_clust, &fs); 
	//��ȡʧ��
    if(res != FR_OK)
    {
        printf("f_getfree() error\r\n");
        return;
    }
    printf("\r\n*** FAT disk info ***\r\n");
		
	//�ܵ���������
    DWORD tot_sect = (fs->n_fatent - 2) * fs->csize;  
		
	//ʣ����������� = ʣ��ظ��� * ÿ���ص���������
    DWORD fre_sect = fre_clust * fs->csize;    
		
	//����SD����U��, _MIN_SS=512�ֽ�
#if  _MAX_SS == _MIN_SS  
    //SD����_MIN_SS�̶�Ϊ512������11λ�൱�ڳ���2048
	//ʣ��ռ��С����λ��MB������SD����U��
    DWORD freespace= (fre_sect>>11); 
		//�ܿռ��С����λ��MB������SD����U��		
    DWORD totalSpace= (tot_sect>>11);  
#else
	//Flash�洢����С����
	//ʣ��ռ��С����λ��KB
    DWORD freespace= (fre_sect*fs->ssize)>>10;   
	//�ܿռ��С����λ��KB
    DWORD totalSpace= (tot_sect*fs->ssize)>>10;  
#endif

	//FAT����
    printf("FAT type = %d\r\n",fs->fs_type);
    printf("[1=FAT12,2=FAT16,3=FAT32,4=exFAT]\r\n");
		
	//������С����λ�ֽ�
    printf("Sector size(bytes) = ");
	//SD���̶�512�ֽ�
#if  _MAX_SS == _MIN_SS 
    printf("%d\r\n", _MIN_SS);
#else
	//FLASH�洢��
    printf("%d\r\n", fs->ssize);
#endif
		
    printf("Cluster size(sectors) = %d\r\n", fs->csize);
    printf("Total cluster count = %ld\r\n", fs->n_fatent-2);
    printf("Total sector count = %ld\r\n", tot_sect);
		
	//�ܿռ�
#if  _MAX_SS == _MIN_SS 
    printf("Total space(MB) = %ld\r\n", totalSpace);
#else
    printf("Total space(KB) = %ld\r\n", totalSpace);
#endif
		
	//���д�����
    printf("Free cluster count = %ld\r\n",fre_clust);
	//������������
    printf("Free sector count = %ld\r\n", fre_sect);
		
	//���пռ�
#if  _MAX_SS == _MIN_SS 
    printf("Free space(MB) = %ld\r\n", freespace);
#else
    printf("Free space(KB) = %ld\r\n", freespace);
#endif

    printf("Get FAT disk info OK\r\n");
}

/*�����ı��ļ�*/
void FatFs_WriteTXTFile(TCHAR *filename,uint16_t year, uint8_t month, uint8_t day)
{
	FIL	file;
	printf("\r\n*** Creating TXT file: %s ***\r\n", filename);
	
	FRESULT res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
	//��/�����ļ��ɹ�
	if(res == FR_OK)
	{
		//�ַ��������л��з�"\n"
		TCHAR str[]="Line1: Hello, FatFs***\n";  
		//����д�������"\0"
		f_puts(str, &file); 
		
		printf("Write file OK: %s\r\n", filename);
	}
	else
	{
		printf("Open file error��error code: %d\r\n", res);
	}
	//ʹ����Ϲر��ļ�
	f_close(&file);
}

/*��ȡһ���ı��ļ�������*/
void FatFs_ReadTXTFile(TCHAR *filename)
{
	printf("\r\n*** Reading TXT file: %s ***\r\n", filename);

	FIL	file;
	//��ֻ����ʽ���ļ�
	FRESULT res = f_open(&file, filename, FA_READ);  
	//�򿪳ɹ�
	if(res == FR_OK)
	{
		//��ȡ����
		TCHAR str[100];
		//û�ж����ļ�����ĩβ
		while(!f_eof(&file))
		{
			//��ȡ1���ַ���,�Զ����Ͻ�������\0��
			f_gets(str,100, &file);	
			printf("%s", str);
		}
		printf("\r\n");
	}
	//���û�и��ļ�
	else if(res == FR_NO_FILE)
		printf("File does not exist\r\n");
	//��ʧ��
	else
		printf("f_open() error��error code: %d\r\n", res);
	//�ر��ļ�
	f_close(&file);
}

/*ɨ�����ʾָ��Ŀ¼�µ��ļ���Ŀ¼*/
void FatFs_ScanDir(const TCHAR* PathName)
{
	DIR dir;					//Ŀ¼����
	FILINFO fno;				//�ļ���Ϣ
	//��Ŀ¼
	FRESULT res = f_opendir(&dir, PathName);
	//��ʧ��
	if(res != FR_OK)
	{
		//�ر�Ŀ¼��ֱ���˳�����
		f_closedir(&dir);
		printf("\r\nf_opendir() error��error code: %d\r\n", res);
		return;
	}
	
	printf("\r\n*** All entries in dir: %s ***\r\n", PathName);
	//˳���ȡĿ¼�е��ļ�
	while(1)
	{
		//��ȡĿ¼�µ�һ����
		res = f_readdir(&dir, &fno);    
		//�ļ���Ϊ�ձ�ʾû�ж����ɶ���
		if(res != FR_OK || fno.fname[0] == 0)
			break;  
		//�����һ��Ŀ¼
		if(fno.fattrib & AM_DIR)  		
		{
			printf("DIR: %s\r\n", fno.fname);
		}
		//�����һ���ļ�
		else  		
		{
			printf("FILE: %s\r\n",fno.fname);
		}
	}
	//ɨ����ϣ��ر�Ŀ¼
	printf("Scan dir OK\r\n");
	f_closedir(&dir);
}

/*��ȡһ���ļ����ļ���Ϣ*/
void FatFs_GetFileInfo(TCHAR *filename)
{
	printf("\r\n*** File info of: %s ***\r\n", filename);

	FILINFO fno;
	//����ļ�����Ŀ¼�Ƿ����
	FRESULT fr = f_stat(filename, &fno);
	//������ڴ�fno�ж�ȡ�ļ���Ϣ
	if(fr == FR_OK)
	{
		printf("File size(bytes) = %ld\r\n", fno.fsize);
		printf("File attribute = 0x%x\r\n", fno.fattrib);
		printf("File Name = %s\r\n", fno.fname);
		//�������/�޸��ļ�ʱ��ʱ���
		FatFs_PrintfFileDate(fno.fdate, fno.ftime);
	}
	//���û�и��ļ�
	else if (fr == FR_NO_FILE)
		printf("File does not exist\r\n");
	//������������
	else
		printf("f_stat() error��error code: %d\r\n", fr);
}

/*ɾ���ļ�*/
void FatFs_DeleteFile(TCHAR *filename)
{
	printf("\r\n*** Delete File: %s ***\r\n", filename);
	FIL	file;
	//���ļ�
	FRESULT res = f_open(&file, filename, FA_OPEN_EXISTING);  
	if(res == FR_OK)
	{
		//�ر��ļ�
		f_close(&file);
		printf("open successfully!\r\n");
	}
	//ɾ���ļ�
	res = f_unlink(filename);
	//ɾ���ɹ�
	if(res == FR_OK)
	{
		printf("The file was deleted successfully!\r\n");
	}
	//ɾ��ʧ��
	else
	{
		printf("File deletion failed, error code��%d\r\n", res);
	}
}

/*��ӡ����ļ�����*/
void FatFs_PrintfFileDate(WORD date, WORD time)
{
	printf("File data = %d/%d/%d\r\n", ((date>>9)&0x7F)+1980, (date>>5)&0xF, date&0x1F);
	printf("File time = %d:%d:%d\r\n", (time>>11)&0x1F, (time>>5)&0x3F, time&0x1F);
}
