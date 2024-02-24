#ifndef FILE_OPERATE_H
#define FILE_OPERATE_H

#include "main.h"
#include "FatFs.h"
#include "stdio.h"
#include "usbh_msc.h"

/*�����Լ��Ĵ洢�豸*/
/*�û��洢�豸�����ֽ���*/
#define User_Sector 512
/*�û��洢�豸FatFS����*/
#define User_FatFs 	USBHFatFS
/*�û��洢�豸��·��*/
#define User_SDPath USBHPath
/*�û��洢�豸��ʼ������*/
#define User_FatType FM_FAT32

/*��������*/
void Mount_FatFs(void);
void FatFs_GetDiskInfo(void);
void FatFs_ScanDir(const TCHAR* PathName);
void FatFs_ReadTXTFile(TCHAR *filename);
void FatFs_WriteTXTFile(TCHAR *filename,uint16_t year, uint8_t month, uint8_t day);
void FatFs_GetFileInfo(TCHAR *filename);
void FatFs_DeleteFile(TCHAR *filename);
void FatFs_PrintfFileDate(WORD date, WORD time);
void USBDisk_ShowInfo(void);


#endif
