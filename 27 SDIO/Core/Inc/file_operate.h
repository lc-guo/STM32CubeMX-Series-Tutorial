#ifndef FILE_OPERATE_H
#define FILE_OPERATE_H

#include "main.h"
#include "fatfs.h"
#include "w25flash.h" 
#include "stdio.h" 



void Mount_Fatfs(void);
void fatTest_GetDiskInfo(void);
void fatTest_ScanDir(const TCHAR* PathName);
void fatTest_ReadTXTFile(TCHAR *filename);
void fatTest_WriteTXTFile(TCHAR *filename,uint16_t year, uint8_t month, uint8_t day);
void fatTest_ReadBinFile(TCHAR *filename);
void fatTest_WriteBinFile(TCHAR *filename, uint32_t pointCount, uint32_t sampFreq);
void fatTest_GetFileInfo(TCHAR *filename);



#endif
