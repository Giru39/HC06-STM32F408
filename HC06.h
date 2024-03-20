

#ifndef HC06_H_
#define HC06_H_

#include "stm32f4xx.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


void inicializarBluetooth(void);
void activar_blue_it(void);
void desactivar_blue_it(void);
uint8_t blue_getc(void);
void blue_Putc(char c);
void blue_Puts(char *s);
void blue_gets(char* s, uint8_t* nm);
#endif /* HC05_H_ */
