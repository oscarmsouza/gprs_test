/*
 * gprs.h
 *
 *  Created on: 9 de mar de 2020
 *      Author: oscar
 */

#ifndef GPRS_H_
#define GPRS_H_

#include "main.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "sigfox.h"
#include "encoder.h"


#define BYTE_GSM_SIZE       1000


void fn_init_gprs();
void clearBuffer(uint8_t *pbuffer, uint8_t bufferSize);
void fn_fprint_gprs(char* command);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void fn_send_gprs_command(char* at);
#endif /* GPRS_H_ */
