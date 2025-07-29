#include "sgp30.h"
#include "main.h"
#include "delay.h"


//#define SPG30_IIC_USE_HARDWARE ///< ����ú���ʹ��Ӳ��IIC�����θú�ʹ�����ģ��IIC


//////////////////////////////////////////////////////////////////////////////////////////
///
///       ��ֲ�޸���
///
//////////////////////////////////////////////////////////////////////////////////////////


#ifndef SPG30_IIC_USE_HARDWARE

#define SGP30_GPIO_CLOCK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()

#define SGP30_SDA_Pin                      GPIO_PIN_14
#define SGP30_SDA_GPIO_Port                GPIOD

#define SGP30_SCK_Pin                      GPIO_PIN_8
#define SGP30_SCK_GPIO_Port                GPIOA


#define SGP30_SCK_L()    HAL_GPIO_WritePin(IIC_SCL_GPIO_Port, IIC_SCL_Pin, GPIO_PIN_RESET)
#define SGP30_SCK_H()    HAL_GPIO_WritePin(IIC_SCL_GPIO_Port, IIC_SCL_Pin, GPIO_PIN_SET)
#define SGP30_SDA_L()    HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_RESET)
#define SGP30_SDA_H()    HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_SET)
#define SGP30_READ_SDA() (HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin) == GPIO_PIN_SET ? 1 : 0)

/// ����SDA����Ϊ���ģʽ
static void SDA_SET_OUT()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = IIC_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SDA_GPIO_Port, &GPIO_InitStruct);
}

/// ����SDA����Ϊ����ģʽ
static void SDA_SET_IN()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = IIC_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(IIC_SDA_GPIO_Port, &GPIO_InitStruct);
}

/// ΢����ʱ����
static void sgp30_delay_us(uint32_t us)
{
    delay_us(us);
}

#endif


/// ������ʱ����
static void sgp30_delay_ms(uint32_t nms)
{
	HAL_Delay(nms);
}

/// ��ʼ��SGP30��IIC����
static int sgp30_iic_init(void)
{
#ifndef SPG30_IIC_USE_HARDWARE
//	GPIO_InitTypeDef GPIO_InitStruct = {0};

//	/* GPIO Ports Clock Enable */
//	SGP30_GPIO_CLOCK_ENABLE();

//	/*Configure GPIO pin Output Level */
//	HAL_GPIO_WritePin(GPIOB, SGP30_SCK_Pin|SGP30_SDA_Pin, GPIO_PIN_SET);

//	/*Configure GPIO pins : PBPin PBPin */
//	GPIO_InitStruct.Pin = SGP30_SCK_Pin|SGP30_SDA_Pin;
//	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//	GPIO_InitStruct.Pull = GPIO_NOPULL;
//	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//	
	SGP30_SCK_H();  // ����ʱ������
	SGP30_SDA_H();  // ������������
#else
	MX_I2C2_Init();
#endif
	return 0;
}






#ifndef SPG30_IIC_USE_HARDWARE

/////////////////////////////////////////////////////////////////////////////////////
///
///                               ģ��IICЭ��ʵ��
///
/////////////////////////////////////////////////////////////////////////////////////

static void IIC_Start(void)
{
    SDA_SET_OUT();
    SGP30_SDA_H();
    SGP30_SCK_H();
    sgp30_delay_us(5);
    SGP30_SDA_L(); //START:when CLK is high,DATA change form high to low
    sgp30_delay_us(6);
    SGP30_SCK_L();
}

static void IIC_Stop(void)
{
    SDA_SET_OUT();
    SGP30_SCK_L();
    SGP30_SDA_L(); //STOP:when CLK is high DATA change form low to high
    SGP30_SCK_H();
    sgp30_delay_us(6);
    SGP30_SDA_H();
    sgp30_delay_us(6);
}

static uint8_t IIC_Wait_Ack(void)
{
    uint8_t tempTime = 0;
    SGP30_SDA_H();
    sgp30_delay_us(1);
    SDA_SET_IN();
    SGP30_SCK_H();
    sgp30_delay_us(1);
    while (SGP30_READ_SDA())
    {
        tempTime++;
        if (tempTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    SGP30_SCK_L();
    return 0;
}

static void IIC_Ack(void)
{
    SGP30_SCK_L();
    SDA_SET_OUT();
    SGP30_SDA_L();
    sgp30_delay_us(2);
    SGP30_SCK_H();
    sgp30_delay_us(5);
    SGP30_SCK_L();
}

static void IIC_NAck(void)
{
    SGP30_SCK_L();
    SDA_SET_OUT();
    SGP30_SDA_H();
    sgp30_delay_us(2);
    SGP30_SCK_H();
    sgp30_delay_us(5);
    SGP30_SCK_L();
}

static void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    SDA_SET_OUT();
    SGP30_SCK_L();
    for (t = 0; t < 8; t++)
    {
        if ((txd & 0x80) > 0) //0x80  1000 0000
            SGP30_SDA_H();
        else
            SGP30_SDA_L();
        txd <<= 1;
        sgp30_delay_us(2);
        SGP30_SCK_H();
        sgp30_delay_us(2);
        SGP30_SCK_L();
        sgp30_delay_us(2);
    }
}

static uint8_t IIC_Read_Byte(uint8_t ack)
{
    uint8_t i, receive = 0;
    SDA_SET_IN();
    for (i = 0; i < 8; i++)
    {
        SGP30_SCK_L();
        sgp30_delay_us(2);
        SGP30_SCK_H();
        receive <<= 1;
        if (SGP30_READ_SDA())
            receive++;
        sgp30_delay_us(1);
    }
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return receive;
}

static int sgp30_iic_write(uint8_t addr, const uint8_t* buf, uint32_t len)
{
	int i;
    IIC_Start();
    IIC_Send_Byte(addr);
    IIC_Wait_Ack();
	for(i = 0; i < len; i++)
	{
		IIC_Send_Byte(buf[i]);
		IIC_Wait_Ack();
	}
    IIC_Stop();
	return 0;
}

static int sgp30_iic_read(uint8_t addr, uint8_t* buf, uint32_t len)
{
	int i;
    IIC_Start();
    IIC_Send_Byte(addr);
	IIC_Wait_Ack();
	for(i = 0; i < len - 1; i++)
	{
		buf[i] = IIC_Read_Byte(1);
	}
	buf[i] = IIC_Read_Byte(0);  // SGP30��������ʱ������һ���ֽڲ���Ҫ�ȴ�ACK
	IIC_Stop();
	return 0;
}

#else

/* SGP30�������ӵ�Ӳ��I2C�ӿ� */
#define SGP30_IIC_Handle (&hi2c2)


static int sgp30_iic_write(uint8_t addr, const uint8_t* buf, uint32_t len)
{
    if (HAL_I2C_Master_Transmit(SGP30_IIC_Handle, addr, (uint8_t *)buf, len, 100) == HAL_OK)
        return 0;
    return -1;
}

static int sgp30_iic_read(uint8_t addr, uint8_t* buf, uint32_t len)
{
    if (HAL_I2C_Master_Receive(SGP30_IIC_Handle, addr, (uint8_t *)buf, len, 100) == HAL_OK)
        return 0;
    return -1;
}

#endif








//////////////////////////////////////////////////////////////////////////////////////////
///
///       SGP30��������
///
//////////////////////////////////////////////////////////////////////////////////////////

static uint8_t sgp30_checksum(const uint8_t* buf, uint32_t len);
int sgp30_get_serial_id(uint8_t id[6])
{
    uint8_t buf[32];
    uint8_t crc[3];

    buf[0] = (SGP30_CMD_GET_SERIAL_ID & 0XFF00) >> 8;
    buf[1] = (SGP30_CMD_GET_SERIAL_ID & 0X00FF);

    if (sgp30_iic_write(SGP30_ADDR_WRITE, buf, 2) < 0)
        return -1;

    if (sgp30_iic_read(SGP30_ADDR_READ, buf, 9) < 0)
        return -2;

    crc[0] = buf[2];
    crc[1] = buf[5];
    crc[2] = buf[8];

    id[0]  = buf[0];
    id[1]  = buf[1];
    id[2]  = buf[3];
    id[3]  = buf[4];
    id[4]  = buf[6];
    id[5]  = buf[7];

    if(
        sgp30_checksum(&id[0], 2) != crc[0] ||
        sgp30_checksum(&id[2], 2) != crc[1] ||
        sgp30_checksum(&id[4], 2) != crc[2]
    )
        return -3;

    return 0;
}

static uint8_t sgp30_checksum(const uint8_t* buf, uint32_t len)
{
	const uint8_t Polynomial = 0x31;
	uint8_t Initialization = 0XFF;
    uint8_t i = 0, k = 0;
	while(i < len)
	{
		Initialization ^= buf[i++];
		for(k = 0; k < 8; k++)
		{
			if(Initialization & 0X80)
				Initialization = (Initialization << 1) ^ Polynomial;
			else
				Initialization = (Initialization << 1);
		}
	}
	return Initialization;
}

int sgp30_soft_reset(void)
{
    uint8_t cmd = 0X06;
    return sgp30_iic_write(0X00, &cmd, 1);
}

int sgp30_init(void)
{
    uint8_t buf[2];

	// ��ʼ��IIC
	if(sgp30_iic_init() < 0)
		return -1;

    // �����λ
    if (sgp30_soft_reset() < 0)
        return -2;

    // �ȴ���λ���
    sgp30_delay_ms(50);

    buf[0] = (SGP30_CMD_INIT_AIR_QUALITY & 0XFF00) >> 8;
    buf[1] = (SGP30_CMD_INIT_AIR_QUALITY & 0X00FF);

	// ��ʼ�����Ʋ�������
    if (sgp30_iic_write(SGP30_ADDR_WRITE, buf, 2) < 0)
        return -3;

    return 0;
}

int sgp30_read(uint16_t* CO2, uint16_t* TVOC)
{
    uint8_t buf[8] = {0};

    buf[0] = (SGP30_CMD_MEASURE_AIR_QUALITY & 0XFF00) >> 8;
    buf[1] = (SGP30_CMD_MEASURE_AIR_QUALITY & 0X00FF);

    // ����������������
    if (sgp30_iic_write(SGP30_ADDR_WRITE, buf, 2) < 0)
        return -1;

    // �ȴ��������
    sgp30_delay_ms(10);

    // ��ȡ�յ�������
    if (sgp30_iic_read(SGP30_ADDR_READ, buf, 6) < 0)
        return -2;

    // У��CRC
    if (sgp30_checksum(&buf[3], 2) != buf[5])
        return -3;

    if (CO2 != NULL)    *CO2  = (buf[0] << 8) | buf[1];
    if (TVOC != NULL)   *TVOC = (buf[3] << 8) | buf[4];

    return 0;
}

