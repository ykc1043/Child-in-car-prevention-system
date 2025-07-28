#include "file_ops.h"
#include <stdio.h>
#include <string.h>
#include "sdmmc_sd.h"//��ʹ��cubemx���ɵ�sdmmc.c
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
FATFS fs;  // FATFS �ļ�ϵͳ����


FRESULT 	MyFile_Res;    // ������� 
char SDPath[4];			// SD���߼�����·��

//	������FatFs_Check
//	���ܣ�����FatFs�ļ�ϵͳ�Ĺ���
//
void FatFs_Check(void)	//�ж�FatFs�Ƿ���سɹ�����û�д���FatFs���ʽ��SD��
{
	BYTE work[FF_MAX_SS]; 
	
	FATFS_LinkDriver(&SD_Driver, SDPath);		// ��ʼ������
	MyFile_Res = MOUNT_FILESYSTEM();	// ����SD��
	
	if (MyFile_Res == FR_OK)	//�ж��Ƿ���سɹ�
	{
		printf("\r\nSD file system mounted successfully\r\n");
	}
	else		
	{
		printf("SD card file system not created, starting format\r\n");
		return;
//		MyFile_Res = f_mkfs("0:", FM_FAT32, 0, work, sizeof work);		// ��ʽ��SD����FAT32����Ĭ�ϴ�С16K
//		
//		if (MyFile_Res == FR_OK)		//�ж��Ƿ��ʽ���ɹ�
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

//	/* ���� */
//	res=f_mount(&FS,"0:",1);		//����
//	if (res != FR_OK)
//	{
//		printf("FileSystem Mounted Failed (%d)\r\n", result);
//	}

//	res = f_getfree("0:", &fre_clust, &fs);  /* ��Ŀ¼ */
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
	
	
	

//	res=MOUNT_FILESYSTEM();		//����
	

	res = f_open(&file,SD_FileName,FA_OPEN_ALWAYS |FA_WRITE);
	if((res & FR_DENIED) == FR_DENIED)
	{
		printf("sdcard has over, writting failed!\r\n");		
	}
	
	f_lseek(&file, f_size(&file));//ȷ��д��д�벻�Ḳ��֮ǰ������
	if(res == FR_OK)
	{
		printf("open succeed/creat file succeed!\r\n");		
		res = f_write(&file,write_buff,bufSize,&Bw);		//д���ݵ�SD��
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
	
	f_close(&file);						//�ر��ļ�		
//	UNMOUNT_FILESYSTEM();		 //ȡ������
	
}
void file_cat(const char *filename) {
    FATFS fs;           // FATFS����
    FIL fil;            // �ļ�����
    FRESULT fr;        // �ļ����
    char buffer[128];  // ��ȡ������
    UINT bytesRead;    // ʵ�ʶ�ȡ���ֽ���

    // �����ļ�ϵͳ
//    fr = MOUNT_FILESYSTEM();
//    if (fr != FR_OK) {
//        printf("Error mounting filesystem: %d\n", fr);
//        return;
//    }

    // ���ļ�
    fr = f_open(&fil, filename, FA_READ);
    if (fr != FR_OK) {
        printf("Error opening file: %d\n", fr);
        f_mount(NULL, "", 1); // ж���ļ�ϵͳ
        return;
    }

    // ��ȡ����ӡ�ļ�����
    while (1) {
        fr = f_read(&fil, buffer, sizeof(buffer) - 1, &bytesRead);
        if (fr != FR_OK) {
            printf("Error reading file: %d\n", fr);
            break;
        }
        if (bytesRead == 0) {
            break; // EOF
        }

        buffer[bytesRead] = '\0'; // ȷ���ַ����� '\0' ��β
        printf("%s", buffer);      // ��ӡ��ȡ������
    }

    // �ر��ļ�
    f_close(&fil);
    // ж���ļ�ϵͳ
//    UNMOUNT_FILESYSTEM();
}

// �г�Ŀ¼�µ��ļ�
FRESULT file_list(const char *path) {
    FRESULT res;
    DIR dir;
    FILINFO fno;

    // �����ļ�ϵͳ
    MOUNT_FILESYSTEM();

    // ��Ŀ¼
    res = f_opendir(&dir, path);
    if (res == FR_OK) {
        // ��ȡĿ¼�µ��ļ�
        printf("File list:\r\n");

        while ((res = f_readdir(&dir, &fno)) == FR_OK && fno.fname[0]) {
            if (fno.fattrib & AM_DIR) {
                // ��ӡĿ¼��ʹ�ö���
                printf(" <DIR>  %s\r\n", fno.fname);
            } else {
                // ��ӡ�ļ���ʹ�ö���
                printf(" <FILE> %-20s (%lu bytes)\r\n", fno.fname, fno.fsize);
            }
        }
    }

    UNMOUNT_FILESYSTEM();
    return res;  // ���ؽ��
}

FRESULT file_remove(const char *filename) {
    FRESULT res;
    // �����ļ�ϵͳ
//    MOUNT_FILESYSTEM();

    // ɾ���ļ�
    res = f_unlink(filename);
    if (res == FR_OK) {
        printf("File '%s' deleted successfully.\n", filename);
    } else {
        // ���ݲ�ͬ��������ṩ��ϸ��Ϣ
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

    // ж���ļ�ϵͳ
//    UNMOUNT_FILESYSTEM();
    return res;
}
