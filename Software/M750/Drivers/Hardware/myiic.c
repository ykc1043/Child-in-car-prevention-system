#include "myiic.h"
#include "stdio.h"


/************以下接口由用户接入 ***************/
// 设置 SDA 为输入模式
void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = IIC_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(IIC_SDA_GPIO_Port, &GPIO_InitStruct);
}

// 设置 SDA 为输出模式
void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = IIC_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SDA_GPIO_Port, &GPIO_InitStruct);
}

// 初始化 IIC
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 初始化 SCL
    GPIO_InitStruct.Pin = IIC_SCL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(IIC_SCL_GPIO_Port, &GPIO_InitStruct);

    // 初始化 SDA
    SDA_OUT();

    // 初始状态
    IIC_SCL_HIGH();
    IIC_SDA_HIGH();
	
}
/************以上接口由用户接入 ***************/


// 产生 IIC 起始信号
void IIC_Start(void)
{
    SDA_OUT();
    IIC_SDA_HIGH();
    IIC_SCL_HIGH();
    delay_us(4);
    IIC_SDA_LOW();
    delay_us(4);
    IIC_SCL_LOW();
}

// 产生 IIC 停止信号
void IIC_Stop(void)
{
    SDA_OUT();
    IIC_SCL_LOW();
    IIC_SDA_LOW();
    delay_us(4);
    IIC_SCL_HIGH();
    IIC_SDA_HIGH();
    delay_us(4);
}

// 等待应答信号
uint8_t IIC_Wait_Ack(void)
{
    uint8_t errTime = 0;
    SDA_IN();
    IIC_SDA_HIGH();
    delay_us(1);
    IIC_SCL_HIGH();
    delay_us(1);

    while (IIC_SDA_READ())
    {
        errTime++;
        if (errTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL_LOW();
    return 0;
}

// 发送 ACK 信号
void IIC_Ack(void)
{
    IIC_SCL_LOW();
    SDA_OUT();
    IIC_SDA_LOW();
    delay_us(2);
    IIC_SCL_HIGH();
    delay_us(2);
    IIC_SCL_LOW();
}

// 发送 NACK 信号
void IIC_NAck(void)
{
    IIC_SCL_LOW();
    SDA_OUT();
    IIC_SDA_HIGH();
    delay_us(2);
    IIC_SCL_HIGH();
    delay_us(2);
    IIC_SCL_LOW();
}

// 发送一个字节
void IIC_Send_Byte(uint8_t txd)
{
    uint8_t t;
    SDA_OUT();
    IIC_SCL_LOW();
    for (t = 0; t < 8; t++)
    {
        if (txd & 0x80)
            IIC_SDA_HIGH();
        else
            IIC_SDA_LOW();
        txd <<= 1;
        delay_us(2);
        IIC_SCL_HIGH();
        delay_us(2);
        IIC_SCL_LOW();
        delay_us(2);
    }
}

// 读取一个字节
uint8_t IIC_Read_Byte(unsigned char ack)
{
    uint8_t i, receive = 0;
    SDA_IN();
    for (i = 0; i < 8; i++)
    {
        IIC_SCL_LOW();
        delay_us(2);
        IIC_SCL_HIGH();
        receive <<= 1;
        if (IIC_SDA_READ())
            receive++;
        delay_us(1);
    }
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return receive;
}






void IIC_Scan_Devices(void)
{
	uint8_t addr = 0;
	uint8_t ack;

	printf("\r\n\r\n");
	for (addr= 0x00; addr < 0xff; ) {
		IIC_Start();
		IIC_Send_Byte(addr);
		ack = IIC_Wait_Ack();
		IIC_Stop();
		if (ack==0) {
			// We got an ack
			printf("%02x ", addr);
		} else {
			printf("-- ");	
		}

		if (addr > 0 && (addr + 1) % 16 == 0) printf("\r\n");//%02x ", addr);

		addr++;
		// Delay_ms(1);
	}
	printf("\r\n\r\n");
	
}


// IIC连续写
// addr:器件地址
// reg:寄存器地址
// len:写入长度
// buf:数据区
// 返回值:0,正常
//     其他,错误代码
uint8_t SHT_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t i;
	IIC_Start();
	IIC_Send_Byte((addr << 1) | 0); // 发送器件地址+写命令
	if (IIC_Wait_Ack())				// 等待应答
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg); // 写寄存器地址
	IIC_Wait_Ack();		// 等待应答
	for (i = 0; i < len; i++)
	{
		IIC_Send_Byte(buf[i]); // 发送数据
		if (IIC_Wait_Ack())	   // 等待ACK
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_Stop();
	return 0;
}

// IIC连续读
// addr:器件地址
// reg:要读取的寄存器地址
// len:要读取的长度
// buf:读取到的数据存储区
// 返回值:0,正常
//     其他,错误代码
uint8_t SHT_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	IIC_Start();
	IIC_Send_Byte((addr << 1) | 0); // 发送器件地址+写命令
	if (IIC_Wait_Ack())				// 等待应答
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg); // 写寄存器地址
	IIC_Wait_Ack();		// 等待应答
	IIC_Start();
	delay_ms(10);//！！！！SHT40这里要10ms延迟，其他不要
	IIC_Send_Byte((addr << 1) | 1); // 发送器件地址+读命令
	IIC_Wait_Ack();					// 等待应答
	while (len)
	{
		if (len == 1)
			*buf = IIC_Read_Byte(0); // 读数据,发送nACK
		else
			*buf = IIC_Read_Byte(1); // 读数据,发送ACK
		len--;
		buf++;
	}
	IIC_Stop(); // 产生一个停止条件
	return 0;
}

// IIC写一个字节
// reg:寄存器地址
// data:数据
// 返回值:0,正常
// 其他,错误代码
uint8_t SHT_Write_Byte(uint8_t reg, uint8_t data)
{
	IIC_Start();
	IIC_Send_Byte((SHT_IIC_ADDR << 1) | 0); // 发送器件地址+写命令
	if (IIC_Wait_Ack())						// 等待应答 若无回答 SHT_IIC_Wait_Ack()==1
	{
		IIC_Stop();
		return 1; // ERROR
	}
	IIC_Send_Byte(reg);	 // 写寄存器地址
	IIC_Wait_Ack();		 // 等待应答
	IIC_Send_Byte(data); // 发送数据
	if (IIC_Wait_Ack())	 // 等待ACK
	{
		IIC_Stop();
		return 1;
	}
	IIC_Stop();
	return 0; // NORMAL
}

// IIC读一个字节
// reg:寄存器地址
// 返回值:读到的数据
uint8_t SHT_Read_Byte(uint8_t reg)
{
	uint8_t res;
	IIC_Start();
	IIC_Send_Byte((SHT_IIC_ADDR << 1) | 0); // 发送器件地址+写命令
	IIC_Wait_Ack();							// 等待应答
	IIC_Send_Byte(reg);						// 写寄存器地址
	IIC_Wait_Ack();							// 等待应答
	IIC_Start();
	IIC_Send_Byte((SHT_IIC_ADDR << 1) | 1); // 发送器件地址+读命令
	IIC_Wait_Ack();							// 等待应答
	res = IIC_Read_Byte(0);					// 读取数据,发送nACK
	IIC_Stop();								// 产生一个停止条件
	return res;
}

/*************************************************************************************************
*	函 数 名: SHT40_Read_Serial_Number
*	入口参数: 无
*	返回值:32bit的序列号
*	函数功能: 读取SHT40的出场唯一序列号
*	说    明:无
*************************************************************************************************/
uint32_t SHT40_Read_Serial_Number(void)
{
	uint32_t Serial_Number;
	uint8_t I2C_Transmit_Data[1];
	I2C_Transmit_Data[0]=SHT40_READ_SERIAL_NUMBER;
	uint8_t I2C_Receive_Data[6]={0};
//	HAL_I2C_Master_Transmit(&hi2c1, SHT30_Write, I2C_Transmit_Data,1,HAL_MAX_DELAY);
	SHT_Write_Len(SHT_IIC_ADDR,I2C_Transmit_Data[0],1,NULL);//IIC连续写
	
//	HAL_I2C_Master_Receive(&hi2c1, SHT30_Read, I2C_Receive_Data,6,HAL_MAX_DELAY);
	SHT_Read_Len(SHT_IIC_ADDR,NULL,6,I2C_Receive_Data); //IIC连续读 	
	for(int i=0;i<6;i++)
	{
		printf("0x%x ",I2C_Receive_Data[i]);
	}
	printf("\r\nSHT40_READ_SERIAL_NUMBER:0x%x\r\n",I2C_Transmit_Data[0]);
	Serial_Number=(I2C_Receive_Data[0] << 24)|
			(I2C_Receive_Data[1] << 16)|
			(I2C_Receive_Data[3] << 8)|
			(I2C_Receive_Data[4] << 0);
	return Serial_Number;
}


void SHT40_Read_RHData(float *temperature, float *humidity)
{
  uint8_t writeData[1] = {SHT40_MEASURE_TEMPERATURE_HUMIDITY};
  uint8_t readData[6] = {0};
  uint32_t tempData = 0;
//  HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)SHT40_Write, (uint8_t *)writeData, 1, HAL_MAX_DELAY);
//  SHT_Write_Len(SHT_IIC_ADDR,0xfd,1,NULL);//IIC连续写
//  
//  delay_ms(10);
//  
  SHT_Read_Len(SHT_IIC_ADDR,writeData[0],6,readData);
//  HAL_I2C_Master_Receive(&hi2c1, (uint16_t)SHT40_Read, (uint8_t *)readData, 6, HAL_MAX_DELAY);
//	for(int i=0;i<6;i++)
//	{
//		printf("0x%x ",readData[i]);
//	}
//	printf("\r\n");

  tempData = readData[0] << 8 | readData[1];
  *temperature = (tempData * 175.0f) / 65535.0f - 45;

  tempData = readData[3] << 8 | readData[4];
  *humidity = (tempData * 125.0f) / 65535.0f - 6;
}

