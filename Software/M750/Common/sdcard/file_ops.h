#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "ff.h"  // FATFSͷ�ļ�
#include "stdint.h"
// ���ƺ꣬������ÿ����������غ�ж���ļ�ϵͳ
#define ENABLE_MOUNT_UNMOUNT  // ����˺������ù��غ�ж��

// �ļ�ϵͳ����
extern FATFS fs;

// �궨��
#ifdef ENABLE_MOUNT_UNMOUNT
    #define MOUNT_FILESYSTEM()    f_mount(&fs, "0:", 1)  // �����ļ�ϵͳ
    #define UNMOUNT_FILESYSTEM()  (void)0 // ж���ļ�ϵͳ
#else
    #define MOUNT_FILESYSTEM()    FR_OK
    #define UNMOUNT_FILESYSTEM()  FR_OK
#endif

// ��������
void FatFs_Check(void);
//uint8_t Get_SDCard_Capacity(void);
//FRESULT file_copy(const char *source, const char *destination);
//FRESULT file_move(const char *source, const char *destination);
//FRESULT file_remove(const char *filename);
void file_write(char SD_FileName[],BYTE write_buff[],uint8_t bufSize);
void file_cat(const char *filename) ;
FRESULT file_list(const char *path);
FRESULT file_remove(const char *filename);

#endif // FILE_OPS_H
