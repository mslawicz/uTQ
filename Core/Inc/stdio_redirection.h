/*
 * stdio_redirection.h
 *
 *  Created on: 15 maj 2022
 *      Author: marci
 */

#ifndef INC_STDIO_REDIRECTION_H_
#define INC_STDIO_REDIRECTION_H_

#include "stm32f4xx_hal.h"

void redirectStdio(UART_HandleTypeDef* uart);
int _read(int file, char *ptr, int len);
int _write(int file, char *ptr, int len);

#endif /* INC_STDIO_REDIRECTION_H_ */
