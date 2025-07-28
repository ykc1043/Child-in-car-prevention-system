#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "ff.h"  // FATFS头文件
#include "stdint.h"
// 控制宏，开启后每个操作会挂载和卸载文件系统
#define ENABLE_MOUNT_UNMOUNT  // 定义此宏以启用挂载和卸载

// 文件系统对象
extern FATFS fs;

// 宏定义
#ifdef ENABLE_MOUNT_UNMOUNT
    #define MOUNT_FILESYSTEM()    f_mount(&fs, "0:", 1)  // 挂载文件系统
    #define UNMOUNT_FILESYSTEM()  (void)0 // 卸载文件系统
#else
    #define MOUNT_FILESYSTEM()    FR_OK
    #define UNMOUNT_FILESYSTEM()  FR_OK
#endif

// 函数声明
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
