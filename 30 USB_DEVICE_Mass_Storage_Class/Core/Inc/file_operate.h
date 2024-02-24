#ifndef FILE_OPERATE_H
#define FILE_OPERATE_H

#include "main.h"
#include "FatFs.h"
#include "stdio.h"

/*�����Լ��Ĵ洢�豸*/
/*�û��洢�豸�����ֽ���*/
#define User_Sector 512
/*�û��洢�豸FatFs����*/
#define User_FatFs 	SDFatFS
/*�û��洢�豸��·��*/
#define User_SDPath SDPath

/*��������*/
void Mount_FatFs(void);
void FatFs_GetDiskInfo(void);
void FatFs_ScanDir(const TCHAR* PathName);
void FatFs_ReadTXTFile(TCHAR *filename);
void FatFs_WriteTXTFile(TCHAR *filename,uint16_t year, uint8_t month, uint8_t day);
void FatFs_GetFileInfo(TCHAR *filename);
void FatFs_DeleteFile(TCHAR *filename);
void FatFs_PrintfFileDate(WORD date, WORD time);

#endif
