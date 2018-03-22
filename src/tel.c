#include "tel.h"
#include "stm32f10x_conf.h"
#include "delay.h"

void PWR_InitOut() {
	/* Bit configuration structure for GPIOA PIN8 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void PWR_InitIn() {
	/* Bit configuration structure for GPIOA PIN8 */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void USART1_Init(void) {
	/* USART configuration structure for USART1 */
	USART_InitTypeDef usart1_init_struct;
	/* Bit configuration structure for GPIOA PIN9 and PIN10 */
	GPIO_InitTypeDef gpioa_init_struct;

	/* GPIOA PIN9 alternative function Tx */
	gpioa_init_struct.GPIO_Pin = GPIO_Pin_9;
	gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	gpioa_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpioa_init_struct);
	/* GPIOA PIN9 alternative function Rx */
	gpioa_init_struct.GPIO_Pin = GPIO_Pin_10;
	gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	gpioa_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpioa_init_struct);

	/* Enable USART1 */
	USART_Cmd(USART1, ENABLE);
	/* Baud rate 9600, 8-bit data, One stop bit
	 * No parity, Do both Rx and Tx, No HW flow control
	 */
	usart1_init_struct.USART_BaudRate = 9600;
	usart1_init_struct.USART_WordLength = USART_WordLength_8b;
	usart1_init_struct.USART_StopBits = USART_StopBits_1;
	usart1_init_struct.USART_Parity = USART_Parity_No;
	usart1_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart1_init_struct.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	/* Configure USART1 */
	USART_Init(USART1, &usart1_init_struct);
	/* Enable RXNE interrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	/* Enable USART1 global interrupt */
	NVIC_EnableIRQ(USART1_IRQn);
}

void UU_PutChar(USART_TypeDef* USARTx, char ch)
{
  while(!(USARTx->SR & USART_SR_TXE));
  USARTx->DR = ch;
}

void UU_PutString(USART_TypeDef* USARTx, char * str)
{
  while(*str != 0)
  {
    UU_PutChar(USARTx, *str);
    str++;
  }
}

void Tel_Init() {
	USART1_Init();
	Tel_On();

	UU_PutString(USART1, "AT+CPIN =9255\n\n");
}

void Tel_On() {
	PWR_InitOut();
	GPIO_SetBits(GPIOA, GPIO_Pin_11);
	Delay_Ms(2500);
	PWR_InitIn();
	Delay_Ms(500);
}

void Tel_Off() {
	PWR_InitOut();
	GPIO_ResetBits(GPIOA, GPIO_Pin_11);
	Delay_Ms(2000);
	PWR_InitIn();
	Delay_Ms(500);
}

void TEL_IRQHandler(void) {
	/* RXNE handler */
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		/* If received 't', toggle LED and transmit 'T' */
		if ((char) USART_ReceiveData(USART1) == 't') {
			//led_toggle();
			//USART_SendData(USART1, 'T');
			/* Wait until Tx data register is empty, not really
			 * required for this example but put in here anyway.
			 */
			/*
			 while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			 {
			 }*/
		}
	}

	/* ------------------------------------------------------------ */
	/* Other USART1 interrupts handler can go here ...             */
}
