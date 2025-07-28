#include "file_ops.h"
#include <stdio.h>
#include <string.h>
#include "sdmmc_sd.h"//不使用cubemx生成的sdmmc.c
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
FATFS fs;  // FATFS 文件系统对象


FRESULT 	MyFile_Res;    // 操作结果 
char SDPath[4];			// SD卡逻辑驱动路径

//	函数：FatFs_Check
//	功能：进行FatFs文件系统的挂载
//
void FatFs_Check(void)	//判断FatFs是否挂载成功，若没有创建FatFs则格式化SD卡
{
	BYTE work[FF_MAX_SS]; 
	
	FATFS_LinkDriver(&SD_Driver, SDPath);		// 初始化驱动
	MyFile_Res = MOUNT_FILESYSTEM();	// 挂载SD卡
	
	if (MyFile_Res == FR_OK)	//判断是否挂载成功
	{
		printf("\r\nSD file system mounted successfully\r\n");
	}
	else		
	{
		printf("SD card file system not created, starting format\r\n");
		return;
//		MyFile_Res = f_mkfs("0:", FM_FAT32, 0, work, sizeof work);		// 格式化SD卡，FAT32，簇默认大小16K
//		
//		if (MyFile_Res == FR_OK)		//判断是否格式化成功
//			printf("SD card formatted successfully!\r\n");
//		else
//			printf("Formatting failed, please check or replace the SD card!\r\n");
	}
}
//uint8_t Get_SDCard_Capacity(void)
//{
//	FRESULT result;
//	FATFS FS;
//	FATFS *fs;
//	DWORD fre_clust,AvailableSize,UsedSize;  
//	uint16_t TotalSpace;
//	uint8_t res;

//	/* 挂载 */
//	res=f_mount(&FS,"0:",1);		//挂载
//	if (res != FR_OK)
//	{
//		printf("FileSystem Mounted Failed (%d)\r\n", result);
//	}

//	res = f_getfree("0:", &fre_clust, &fs);  /* 根目录 */
//	if ( res == FR_OK ) 
//	{
//		TotalSpace=(uint16_t)(((fs->n_fatent - 2) * fs->csize ) / 2 /1024);
//		AvailableSize=(uint16_t)((fre_clust * fs->csize) / 2 /1024);
//		UsedSize=TotalSpace-AvailableSize;              
//		/* Print free space in unit of MB (assuming 512 bytes/sector) */
//		printf("\r\n%d MB total drive space.\r\n""%d MB available.\r\n""%d MB  used.\r\n",TotalSpace, AvailableSize,UsedSize);
//	}
//	else 
//	{
//		printf("Get SDCard Capacity Failed (%d)\r\n", result);
//	}		
//	return res;
//} 

void file_write(char SD_FileName[],BYTE write_buff[],uint8_t bufSize)
{
	FATFS fs;
	FIL file;
	uint8_t res=0;
	UINT Bw;	
	
	
	

//	res=MOUNT_FILESYSTEM();		//挂载
	

	res = f_open(&file,SD_FileName,FA_OPEN_ALWAYS |FA_WRITE);
	if((res & FR_DENIED) == FR_DENIED)
	{
		printf("sdcard has over, writting failed!\r\n");		
	}
	
	f_lseek(&file, f_size(&file));//确保写词写入不会覆盖之前的数据
	if(res == FR_OK)
	{
		printf("open succeed/creat file succeed!\r\n");		
		res = f_write(&file,write_buff,bufSize,&Bw);		//写数据到SD卡
		if(res == FR_OK)
		{
			printf("file write success! \r\n");			
		}
		else
		{
			printf("file write failed \r\n");
		}		
	}
	else
	{
		printf("open file failed!\r\n");
	}	
	
	f_close(&file);						//关闭文件		
//	UNMOUNT_FILESYSTEM();		 //取消挂载
	
}
void file_cat(const char *filename) {
    FATFS fs;           // FATFS对象
    FIL fil;            // 文件对象
    FRESULT fr;        // 文件结果
    char buffer[128];  // 读取缓冲区
    UINT bytesRead;    // 实际读取的字节数

    // 挂载文件系统
//    fr = MOUNT_FILESYSTEM();
//    if (fr != FR_OK) {
//        printf("Error mounting filesystem: %d\n", fr);
//        return;
//    }

    // 打开文件
    fr = f_open(&fil, filename, FA_READ);
    if (fr != FR_OK) {
        printf("Error opening file: %d\n", fr);
        f_mount(NULL, "", 1); // 卸载文件系统
        return;
    }

    // 读取并打印文件内容
    while (1) {
        fr = f_read(&fil, buffer, sizeof(buffer) - 1, &bytesRead);
        if (fr != FR_OK) {
            printf("Error reading file: %d\n", fr);
            break;
        }
        if (bytesRead == 0) {
            break; // EOF
        }

        buffer[bytesRead] = '\0'; // 确保字符串以 '\0' 结尾
        printf("%s", buffer);      // 打印读取的内容
    }

    // 关闭文件
    f_close(&fil);
    // 卸载文件系统
//    UNMOUNT_FILESYSTEM();
}

// 列出目录下的文件
FRESULT file_list(const char *path) {
    FRESULT res;
    DIR dir;
    FILINFO fno;

    // 挂载文件系统
    MOUNT_FILESYSTEM();

    // 打开目录
    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        // 读取目录下的文件
        printf("File list:\r\n");

        while ((res = f_readdir(&dir, &fno)) == FR_OK && fno.fname[0]) {
            if (fno.fattrib & AM_DIR) {
                // 打印目录，使用对齐
                printf(" <DIR>  %s\r\n", fno.fname);
            } else {
                // 打印文件，使用对齐
                printf(" <FILE> %-20s (%lu bytes)\r\n", fno.fname, fno.fsize);
            }
        }
    }

    UNMOUNT_FILESYSTEM();
    return res;  // 返回结果
}

FRESULT file_remove(const char *filename) {
    FRESULT res;
    // 挂载文件系统
//    MOUNT_FILESYSTEM();

    // 删除文件
    res = f_unlink(filename);
    if (res == FR_OK) {
        printf("File '%s' deleted successfully.\n", filename);
    } else {
        // 根据不同错误代码提供详细信息
        switch (res) {
            case FR_NO_FILE:
                printf("Error: File '%s' not found.\n", filename);
                break;
            case FR_DENIED:
                printf("Error: Access denied while deleting '%s'.\n", filename);
                break;
            case FR_NOT_READY:
                printf("Error: File system not ready.\n");
                break;
            case FR_INVALID_NAME:
                printf("Error: Invalid file name '%s'.\n", filename);
                break;
            case FR_DISK_ERR:
                printf("Error: Disk I/O error occurred while deleting '%s'.\n", filename);
                break;
            default:
                printf("Error: Failed to delete file '%s', error code: %d.\n", filename, res);
                break;
        }
    }

    // 卸载文件系统
//    UNMOUNT_FILESYSTEM();
    return res;
}
