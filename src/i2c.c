#include "i2c.h"

#define Timed(x) Timeout = 0xFFFF; while (x) \
{ if (Timeout -- == 0) goto errReturn;}

void I2C_LowLevel_Init(int ClockSpeed, int OwnAddress) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIOB clocks
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// Configure I2C clock and GPIO
	GPIO_StructInit(&GPIO_InitStructure);
	/* I2C1 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* I2C1 SDA and SCL configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);

	/* I2C1 Reset */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

	I2C_Struct_Init(ClockSpeed, OwnAddress);
}

void I2C_Struct_Init(int ClockSpeed, int OwnAddress) {
	I2C_InitTypeDef I2C_InitStructure;
	I2C_DeInit(I2C1);
	/* Configure I2Cx */
	I2C_StructInit(&I2C_InitStructure);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = OwnAddress;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress =
	I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
}

Status I2C_Write(const uint8_t* buf, uint32_t nbyte, uint8_t SlaveAddress) {
	__IO uint32_t Timeout = 0;

	if (nbyte) {
		Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

		if (I2C_StartTransmission(SlaveAddress) == Error)
			goto errReturn;

		// EV6 Write first byte EV8_1
		I2C_SendData(I2C1, *buf++);

		while (--nbyte) {
			if (I2C_Send(*buf++) == Error)
				goto errReturn;
		}

		if (I2C_EndTransmission() == Error)
			goto errReturn;
	}
	return Success;

	errReturn: return Error;
}

Status I2C_StartTransmission(uint8_t SlaveAddress) {
	__IO uint32_t Timeout = 0;
	Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	// Intiate Start Sequence
	I2C_GenerateSTART(I2C1, ENABLE);
	Timed(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	// Send Address EV5
	I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Transmitter);
	Timed(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	return Success;

	errReturn: return Error;
}

Status I2C_Send(uint8_t Data) {
	__IO uint32_t Timeout = 0;
	// wait on BTF
	Timed(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));
	I2C_SendData(I2C1, Data);

	return Success;

	errReturn: return Error;
}

Status I2C_EndTransmission() {
	__IO uint32_t Timeout = 0;
	Timed(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));
	I2C_GenerateSTOP(I2C1, ENABLE);
	Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));

	return Success;

	errReturn: return Error;
}

Status I2C_WriteTwoBytes(uint8_t SlaveAddress, uint8_t byte1, uint8_t byte2) {
	uint8_t buf[] = { byte1, byte2 };
	return I2C_Write(buf, 2, SlaveAddress);
}

Status I2C_Read(uint8_t *buf, uint32_t nbyte, uint8_t SlaveAddress) {
	__IO uint32_t Timeout = 0;
	if (!nbyte)
		return Success;

	// Wait for idle I2C interface
	Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

	// Enable Acknowledgment , clear POS flag
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);

	// Intiate Start Sequence (wait for EV5)
	I2C_GenerateSTART(I2C1, ENABLE);
	Timed(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

	// Send Address
	I2C_Send7bitAddress(I2C1, SlaveAddress, I2C_Direction_Receiver);

	// EV6
	Timed(!I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR));
	if (nbyte == 1) { /* read 1 byte */
		// Clear Ack bit
		I2C_AcknowledgeConfig(I2C1, DISABLE);

		// EV6_1 -- must be atomic -- Clear ADDR, generate STOP
		__disable_irq();
		(void) I2C1->SR2;
		I2C_GenerateSTOP(I2C1, ENABLE);
		__enable_irq();

		// Receive data EV7
		Timed(!I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE));
		*buf++ = I2C_ReceiveData(I2C1);
	} else if (nbyte == 2) { /* read 2 bytes */
		// Set POS flag
		I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Next);

		// EV6_1 -- must be atomic and in this order
		__disable_irq();
		(void) I2C1->SR2; // Clear ADDR flag
		I2C_AcknowledgeConfig(I2C1, DISABLE); // Clear Ack bit
		__enable_irq();

		// EV7_3 -- Wait for BTF, program stop, read data twice
		Timed(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));
		__disable_irq();
		I2C_GenerateSTOP(I2C1, ENABLE);
		*buf++ = I2C1->DR;
		__enable_irq();
		*buf++ = I2C1->DR;
	} else { /* read 3 or more bytes */
		(void) I2C1->SR2; // Clear ADDR flag

		while (nbyte-- != 3) {
			// EV7 -- cannot guarantee 1 transfer completion time,
			// wait for BTF instead of RXNE
			Timed(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));
			*buf++ = I2C_ReceiveData(I2C1);
		}
		Timed(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF));

		// EV7_2 -- Figure 1 has an error , doesn't read N-2 !
		I2C_AcknowledgeConfig(I2C1, DISABLE); // clear ack bit
		__disable_irq();
		*buf++ = I2C_ReceiveData(I2C1); // receive byte N-2
		I2C_GenerateSTOP(I2C1, ENABLE); // program stop
		__enable_irq();
		*buf++ = I2C_ReceiveData(I2C1); // receive byte N-1

		// wait for byte N
		Timed(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		*buf++ = I2C_ReceiveData(I2C1);
		nbyte = 0;
	}
	// Wait for stop
	Timed(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
	return Success;

	errReturn: return Error;
}
