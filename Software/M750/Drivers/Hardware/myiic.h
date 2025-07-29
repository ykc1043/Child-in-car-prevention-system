#ifndef __MYIIC_H
#define __MYIIC_H

#include "stm32h7xx_hal.h"
#include "delay.h"

	 
     
     
     

/************以下接口由用户接入 ***************/
// I2C 引脚定义
#define IIC_SDA_Pin 		GPIO_PIN_14       //#define IIC_SDA_Pin GPIO_PIN_7
#define IIC_SDA_GPIO_Port 	GPIOD             //#define IIC_SDA_GPIO_Port GPIOD
#define IIC_SCL_Pin 		GPIO_PIN_8        //#define IIC_SCL_Pin GPIO_PIN_7
#define IIC_SCL_GPIO_Port 	GPIOA             //#define IIC_SCL_GPIO_Port GPIOB

// 模拟 I2C 操作宏
#define IIC_SCL_HIGH() HAL_GPIO_WritePin(IIC_SCL_GPIO_Port, IIC_SCL_Pin, GPIO_PIN_SET)
#define IIC_SCL_LOW()  HAL_GPIO_WritePin(IIC_SCL_GPIO_Port, IIC_SCL_Pin, GPIO_PIN_RESET)
#define IIC_SDA_HIGH() HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_SET)
#define IIC_SDA_LOW()  HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_RESET)
#define IIC_SDA_READ() HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin)
void SDA_IN(void);
void SDA_OUT(void);
/************以上接口由用户接入 ***************/


// I2C 地址定义
#define SHT_IIC_ADDR 0x44

// SHT40 命令定义
#define SHT40_MEASURE_TEMPERATURE_HUMIDITY 0xFD
#define SHT40_READ_SERIAL_NUMBER 0x89
#define SHT40_HEATER_200mW_1s 0x39

// 函数声明
void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(unsigned char ack);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Scan_Devices(void);
uint8_t SHT_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t SHT_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
uint8_t SHT_Write_Byte(uint8_t reg, uint8_t data);
uint8_t SHT_Read_Byte(uint8_t reg);

uint32_t SHT40_Read_Serial_Number(void);
void SHT40_Read_RHData(float *temperature, float *humidity);

#endif
