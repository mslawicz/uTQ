/*
 * stdio_redirection.c
 *
 *  Created on: 15 maj 2022
 *      Author: marci
 */

#include "stdio_redirection.h"
#include <stdio.h>

static UART_HandleTypeDef* pConsole = NULL;

void redirectStdio(UART_HandleTypeDef* pUart)
{
    pConsole = pUart;
    /* Disable I/O buffering for STDOUT stream, so that
     * chars are sent out as soon as they are printed. */
    setvbuf(stdout, NULL, _IONBF, 0);
}

int _read(int file, char *ptr, int len) /* @suppress("Name convention for function") */
{
    UNUSED(file);
    UNUSED(len);
    if(NULL == pConsole)
    {
        return 0;
    }
    /* read one byte only, according to _fstat returning character device type */
    if(HAL_OK == HAL_UART_Receive(pConsole, (uint8_t*)ptr, 1, HAL_MAX_DELAY))
    {
        return 1;
    }
    return 0;
}

int _write(int file, char *ptr, int len) /* @suppress("Name convention for function") */
{
    UNUSED(file);
    UNUSED(len);    
    if(NULL == pConsole)
    {
        return 0;
    }

    static const uint32_t Timeout = 10;
    if(HAL_OK == HAL_UART_Transmit(pConsole, (uint8_t*)ptr, len, Timeout))
    {
        return len;
    }
    return 0;
}


