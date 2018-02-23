#include "stm32f10x_conf.h"


typedef uint8_t Status;
#define Success 1
#define Error 0

Status I2C_Write(const uint8_t* buf, uint32_t nbyte, uint8_t SlaveAddress);
Status I2C_Read(uint8_t *buf, uint32_t nbyte, uint8_t SlaveAddress);
void I2C_LowLevel_Init(int ClockSpeed , int OwnAddress);
Status I2C_WriteTwoBytes(uint8_t SlaveAddress, uint8_t byte1, uint8_t byte2);
Status I2C_Send(uint8_t Data);
Status I2C_EndTransmission();
Status I2C_StartTransmission(uint8_t SlaveAddress);
void I2C_Struct_Init(int ClockSpeed, int OwnAddress);
