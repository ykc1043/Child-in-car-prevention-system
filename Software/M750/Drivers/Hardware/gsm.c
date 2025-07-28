#include "gsm.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include <stdint.h>
#include "string.h"
#include "DataProc.h"

#include "sdmmc_sd.h" //��ʹ��cubemx���ɵ�sdmmc.c
#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"

int k = 0, j = 0, gsm_flag1 = 0, gsm_flag2 = 0;

void convert_to_ucs2(const char *ascii_phone, char *ucs2_phone)
{
	size_t len = strlen(ascii_phone);
	for (size_t i = 0; i < len; i++)
	{
		sprintf(&ucs2_phone[i * 4], "003%c", ascii_phone[i]);
	}
	ucs2_phone[len * 4] = '\0';
}
// �������֧�ֵ� GBK �ַ�������
#define MAX_GBK_STR_LEN 256
#define MAX_UNICODE_STR_LEN 512

// "��" �� Unicode ����
#define UNICODE_NOT_FOUND 0x53E3

// ������ת��Ϊ GBK ����ĺ������򻯰汾����ʾ����
uint16_t char_to_gbk(const char *ch) {
    uint16_t gbk_code = 0;
    if (strlen(ch) == 2) { // ���� ch �� GBK ˫�ֽڱ���
        gbk_code = (uint8_t)ch[0] << 8 | (uint8_t)ch[1];
    }
    return gbk_code;
}

// �� FATFS �豸��ȡ bin ��ʽӳ������� Unicode ����
uint16_t read_bin_mapping(const char *bin_file, uint16_t gbk_code) {
    FIL file;
    uint16_t gbk, unicode;
    FRESULT res = f_open(&file, bin_file, FA_READ);
    if (res != FR_OK) {
        printf("Error opening bin file\n");
        return UNICODE_NOT_FOUND; // ����Ĭ��ֵ
    }

    while (f_read(&file, &gbk, 2, NULL) == FR_OK && f_read(&file, &unicode, 2, NULL) == FR_OK) {
        if (gbk == gbk_code) {
            f_close(&file);
            return unicode;
        }
    }
    f_close(&file);
    return UNICODE_NOT_FOUND; // δ�ҵ�ƥ����
}

// GBK �ַ���ת��Ϊ Unicode �ַ��������װ� str1
void gbk_to_unicode(char *gbk_str, char *str1, const char *bin_file) {
    int i = 0, offset = 0;
    char temp[3] = {0}; // ������ȡ���� GBK �ַ�

    while (gbk_str[i] != '\0') {
        uint16_t gbk_code, unicode_code;

        if ((uint8_t)gbk_str[i] >= 0x81) { // ˫�ֽ� GBK �ַ�
            temp[0] = gbk_str[i];
            temp[1] = gbk_str[i + 1];
            gbk_code = char_to_gbk(temp);

            // ���� Unicode ����
            unicode_code = read_bin_mapping(bin_file, gbk_code);
            i += 2;
        } else { // ���ֽ� ASCII �ַ� (Ӣ�ġ����֡�����)
            unicode_code = (uint8_t)gbk_str[i]; // ASCII ֵ��Ϊ Unicode ֵ
            i++;
        }

        // װ� str1
        offset += sprintf(&str1[offset], "%04X", unicode_code);
    }

    str1[offset] = '\0'; // ����ַ���������
}

#define USE_PRECONVERT_GBK_STR //ʹ��Ԥת��unicode�ַ���

char ucs2_phone[64];
char gbk_str[MAX_GBK_STR_LEN] = "������������ϵͳ�����ĳ���������������Ա���뼰ʱ�鿴��λ��:"; // ʾ�� GBK �����ַ���
char str1[MAX_UNICODE_STR_LEN] = {0};          // ���ڴ�� Unicode ������
//unicode�ַ��� "������������ϵͳ�����ĳ���������������Ա���뼰ʱ�鿴��λ��:"
char *unicode_str="30108F668F8696326EDE75597CFB7EDF301160A876848F66518575914F3C6709905775594EBA5458FF0C8BF753CA65F667E5770BFF0C4F4D7F6E003A";
void gbkconvert(void)
{
#ifdef USE_PRECONVERT_GBK_STR
    uint32_t a, b;
    a = HAL_GetTick();
	printf("use preconvert gbk string\r\n");
    printf("gbk convert start in %d ms\r\n", a);

    char tempstr[32] = {0};
    snprintf(tempstr, sizeof(tempstr), "%.8gE,%.8gN", sysConfig.latitude, sysConfig.longitude);

    // ʹ�ö���Ļ��������⸲������
    char temp_buf[MAX_UNICODE_STR_LEN] = {0};
    gbk_to_unicode(tempstr, str1, "gbk_to_unicode.bin");
    snprintf(temp_buf, sizeof(temp_buf), "%s%s", unicode_str, str1);
    strncpy(str1, temp_buf, MAX_UNICODE_STR_LEN - 1);

    b = HAL_GetTick();
    printf("gbk convert stop in %d ms, use %d ms\r\n", b, b - a);

#else
    uint32_t a, b;
    a = HAL_GetTick();
	printf("read sd file to convert gbk string\r\n");
    printf("gbk convert start in %d ms\r\n", a);

	sprintf(gbk_str,"%s%.8gE,%.8gN",gbk_str, sysConfig.latitude, sysConfig.longitude);
    gbk_to_unicode(gbk_str, str1, "gbk_to_unicode.bin");

    b = HAL_GetTick();
    printf("gbk convert stop in %d ms, use %d ms\r\n", b, b - a);
#endif    
	printf("%s\r\n", str1);

}

void gsm_send1(void)
{


	switch(k)
	{
		case 0: USART_Printf(&huart8,"AT\r\n");k++;break;//��������
		case 1: USART_Printf(&huart8,"AT+CMGF=1\r\n");k++;break;//���ö�����Ϣ��ʽΪ�ı�ģʽ
		case 2: USART_Printf(&huart8,"AT+CSCS=\"UCS2\"\r\n");k++;break;//���ö�����Ϣ��ʽΪ�ı�ģʽ
		case 3: USART_Printf(&huart8,"AT+CSCS?\r\n");k++;break;//���ö�����Ϣ��ʽΪ�ı�ģʽ
		case 4: USART_Printf(&huart8,"AT+CSMP=17,167,0,25\r\n");k++;break;//���ݱ����ʽ��24����Ӣ�ģ�25��������
		case 5: convert_to_ucs2(sysConfig.phone_number, ucs2_phone);USART_Printf(&huart8, "AT+CMGS=\"%s\"\r\n", ucs2_phone);k++;break;
		case 6: gbkconvert();HAL_UART_Transmit(&huart8,(uint8_t *)str1,strlen((char *)str1),HAL_MAX_DELAY);k++;break;
		case 7: HAL_UART_Transmit(&huart8, (uint8_t[]){0x1A}, 1, HAL_MAX_DELAY);k++;break;
		case 8: gsm_flag1=1;break;
	}
}
//void gsm_send2(void)
//{
//	switch (j)
//	{
//	case 0:
//		USART_Printf(&huart8, "AT+CPIN?\r\n");
//		j++;
//		break; // ��������
//	case 1:
//		USART_Printf(&huart8, "AT+CREG?\r\n");
//		j++;
//		break; // ���ö�����Ϣ��ʽΪ�ı�ģʽ
//	case 2:
//		USART_Printf(&huart8, "AT+CMGF=1\r\n");
//		j++;
//		break; // ���ö�����Ϣ��ʽΪ�ı�ģʽ
//	case 3:
//		USART_Printf(&huart8, "AT+CSCS=\"GSM\"\r\n");
//		j++;
//		break; // ���ö�����Ϣ��ʽΪ�ı�ģʽ
//	case 4:
//		USART_Printf(&huart8, "ATD17397397484;\r\n");
//		j++;
//		break; // ���ݱ����ʽ��24����Ӣ�ģ�25��������
//	case 5:
//		gsm_flag2 = 1;
//		break;
//	}
//}

// ʹ�ú���ģ��
// while(gsm_flag2==0)//����绰
//			{
//				gsm_send2();
//             delay_ms(500);
//			}
