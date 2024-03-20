/*
 * HC06.c
 *
 *  Created on: 23 oct. 2019
 *      Author: Marcos
 *
 * IMPORTANTE: forzar el HSE_VALUE a 8MHz. Para lograr esto buscar en el archivo stm32f4xx.h
 * este valor y dejar en:
 *			 #if !defined  (HSE_VALUE)
 *			 #define HSE_VALUE    ((uint32_t)8000000) //!< Value of the External oscillator in Hz
 *		 	#endif // HSE_VALUE
 *
 * La otra opción es definir el símbolo:
 *
 * Click con boton derecho sobre el nombre del proyecto e ir a PROPIEDADES
 * C/C++ General
 * Code Analysis
 * Paths and Symbols
 * Y en la pestañan  --> #Symbols
 * Agregar un simbolo con -->  Add...
 * Definir el simbolo:  Nombre --> HSE_VALUE
 * 					 Valor -->  8000000
 *
 ******************************************************************************
 */
#include "HC06.h"
void delay_bt(int c)
{
	while (c--);
}
void inicializarBluetooth(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE); //clock USART6 y GPIOC, el usart6 usa el clock del apb2... en stm32f4xx_rcc.h te aparecen los define que tenes que usar, y s es apb1 o 2
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //usa los pines PC6(TX) Y PC7(RX).. el clock del puerto que usan los pines
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6 | GPIO_Pin_7;//aca cambias los pines de la uart que utilzas
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;

	GPIO_Init(GPIOC,&GPIO_InitStructure);//el puerto de los pines

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6);//Conf los pines para usarlos como uart6
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7, GPIO_AF_USART6);

	USART_InitStructure.USART_BaudRate=9600; //mi modulo usa este baudrate pero depende del modulo.. en general es este pero se puede cambiar
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx | USART_Mode_Tx;;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART6,&USART_InitStructure);//inicia el uart que elegis

	/*configuracion de la interrupcion
	 * en stm32f4xx.h aparecen los irqn de cada uart */
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //aca le cambias la prioridad a la interrupcion
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
	//configura la interrupcion para que cuando halla data en el registro que recibe se active
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	/*se activa la interrupcion cuando el registro receptor no esta vacio
	 * en stm32f4xx_usart.h aparecen los registros que pueden activar una interrupcion
	 */
	USART_Cmd(USART6,ENABLE);//activas la uart
}

void desactivar_blue_it(void)
{
	USART_ITConfig(USART6, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART6,DISABLE);
}
void activar_blue_it(void)
{
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART6,ENABLE);
}
uint8_t blue_getc(void) //lee un dato

{
	uint8_t temp=254;
	uint16_t mask=0x00FF;// es para eliminar el bit de STOP
	uint16_t aux;
	if(USART_GetFlagStatus(USART6, USART_FLAG_RXNE) != RESET)//si hay informacion lee
	{
		aux=USART_ReceiveData(USART6);//recibe
		temp=(uint8_t)( aux & mask);
	}
	return temp;
}

void blue_Putc(char c)
{
	while(USART_GetFlagStatus(USART6,USART_FLAG_TXE) == RESET);
	USART_SendData(USART6, c); //envia..
}
void blue_Puts(char *s) // envia un string
{
	while(*s)
	{
		while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET){}
		USART_SendData(USART6,*s);
		s++;
	}
}

void blue_gets(char* s,uint8_t* nm)//lee varios datos, en nm guardo la cantidad de datos que lee
{
	uint8_t i=0;
	uint8_t aux=0;
	do
	{
		while(USART_GetFlagStatus(USART6, USART_FLAG_RXNE) != RESET){}
		s[i]=blue_getc();

		if(!s[i]) aux++;
		i++;
	}while(!aux);
	*nm=i;//guarda la cantidad que leyo

}
