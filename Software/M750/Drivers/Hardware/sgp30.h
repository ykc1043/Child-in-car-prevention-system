#ifndef _SGP30_H_
#define _SGP30_H_

#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <stdint.h>



#define SGP30_ADDR          0x58
#define	SGP30_ADDR_WRITE	(SGP30_ADDR << 1)         //0xb0
#define	SGP30_ADDR_READ		((SGP30_ADDR << 1) + 1)   //0xb1


/* ��ʼ�������������� */
#define SGP30_CMD_INIT_AIR_QUALITY 0x2003

/* ��ʼ������������ */
#define SGP30_CMD_MEASURE_AIR_QUALITY 0x2008

/* ��ȡ���� */
#define SGP30_CMD_GET_SERIAL_ID  0X3682


int sgp30_init(void);
int sgp30_read(uint16_t* CO2, uint16_t* TVOC);
int sgp30_get_serial_id(uint8_t id[6]);
int sgp30_soft_reset(void);

#endif /* _SGP30_H_ */
