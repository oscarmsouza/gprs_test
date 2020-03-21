/*
 * gprs.c
 *
 *  Created on: 9 de mar de 2020
 *      Author: oscar
 */

#include "gprs.h"

UART_HandleTypeDef huart5;
UART_HandleTypeDef hlpuart1;

//gsm varibles

uint8_t byteGSM[BYTE_GSM_SIZE];
uint32_t bytesToReceive = sizeof(byteGSM);
uint8_t byte, attempts;
char nn_com[BYTE_GSM_SIZE];

int fn_check_ok() {
	uint8_t ok = 0;
	for (int var = 0; var < strlen(nn_com); ++var) {
		if (nn_com[var] == 79 && nn_com[var + 1] == 75) {
			ok = 1;
			break;
		}
	}

	return ok;
}

void fn_init_gprs() {
	fn_fprint("GPRS INIT\r\n");
	fn_fprint("GPRS PWR ON\r\n");
	HAL_GPIO_WritePin(EN_GPRS_GPIO_Port, EN_GPRS_Pin, 1);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPRS_RST_GPIO_Port, GPRS_RST_Pin, 1);
	HAL_Delay(3000);
	HAL_GPIO_WritePin(GPRS_PWR_ON_GPIO_Port, GPRS_PWR_ON_Pin, 1);
	fn_fprint("GPRS ON ENABLE\r\n");
	HAL_Delay(10000);
	int count = 100;
	while (fn_check_ok() == 0 && count != 0) {
		HAL_Delay(100);
		fn_fprint_gprs("AT&K0");
		fn_fprint(nn_com);
		fn_fprint(".");
		fn_fprint("\r\n");
		count--;
	}
	fn_fprint_gprs("AT&K0");
	fn_fprint(nn_com);
	fn_fprint_gprs("ATE0");
	HAL_Delay(10);
	fn_fprint_gprs("AT+CMEE=2"); //no
	fn_fprint(nn_com);
	HAL_Delay(10);
	fn_fprint_gprs("AT+CPIN?");
	fn_fprint(nn_com);
	fn_fprint("Manufacturer identification");
	fn_fprint("\r\n");
	fn_fprint_gprs("AT+CGMI");
	fn_fprint(nn_com);
	HAL_Delay(10);
	fn_fprint("Model identification");
	fn_fprint("\r\n");
	fn_fprint_gprs("AT+CGMM");
	fn_fprint(nn_com);
	HAL_Delay(10);
	fn_fprint("IMEI identification");
	fn_fprint("\r\n");
	fn_fprint_gprs("AT+CGSN");
	fn_fprint(nn_com);
	HAL_Delay(10);
	fn_fprint("SIM CARD identification");
	fn_fprint("\r\n");
	fn_fprint_gprs("AT+CCID");
	fn_fprint(nn_com);
	HAL_Delay(10);
	fn_fprint("CLOCK STATUS");
	fn_fprint("\r\n");
	fn_fprint_gprs("AT+CCLK?");
	fn_fprint(nn_com);
	HAL_Delay(10);
	fn_fprint("VIVO NETWORK REGISTER\r\n");
	fn_fprint_gprs("AT+UPSD=1,1,\"zap.vivo.com.br\"");
	HAL_Delay(10);
	fn_fprint(nn_com);
	fn_fprint_gprs("AT+UPSD=1,2,\"vivo\"");
	HAL_Delay(10);
	fn_fprint(nn_com);
	fn_fprint_gprs("AT+UPSD=1,3,\"vivo\"");
	HAL_Delay(10);
	fn_fprint(nn_com);
	fn_fprint_gprs("AT+CGATT=1");
	HAL_Delay(10);
	fn_fprint(nn_com);
	fn_fprint_gprs("AT+CREG=1");
	HAL_Delay(10);
	fn_fprint(nn_com);
	fn_fprint_gprs("AT+UPSDA=1,3");
	HAL_Delay(10);
	fn_fprint(nn_com);
	fn_fprint_gprs("AT+UPSD=1,8");
	HAL_Delay(10);
	fn_fprint(nn_com);
	fn_fprint_gprs("AT+UPSD=1,0");
	HAL_Delay(10);
	fn_fprint(nn_com);

	fn_fprint_gprs("AT+UPSND=1,0");  //ip adress
	fn_fprint(nn_com);
	HAL_Delay(10);

	fn_fprint_gprs("AT+UDELFILE=\"uplink.fss\"");
	fn_fprint(nn_com);
	HAL_Delay(10);
	fn_fprint_gprs("AT+UDWNFILE=\"uplink.fss\",10,\"OK\"");
	fn_fprint(nn_com);
	HAL_Delay(10);
	fn_fprint_gprs("AT+URDFILE=\"uplink.fss\"");
	fn_fprint(nn_com);
	HAL_Delay(10);
	fn_fprint_gprs("AT+UHTTP=1");
	fn_fprint(nn_com);
	HAL_Delay(1000);
	fn_fprint_gprs("AT+UHTTP=1,1,\"wizebox-api-dev.herokuapp.com\"");
	fn_fprint(nn_com);
	HAL_Delay(1000);
	//wizebox-api-dev.herokuapp.com/sigfox/uplink?device=45C288&data=02241D3DE1417AFCA12345
	fn_fprint_gprs("AT+UHTTPC=1,0,\"/sigfox/uplink?device=45C288&data=02241D3DE1417AFCA12345\",\"teste1.txt\"");
	fn_fprint(nn_com);
	HAL_Delay(1000);

}

void fn_fprint_gprs(char* command) {
	//HAL_UART_Transmit(&huart2,(uint8_t*)"AT&K0/r/n/0",11,100);
	memset(nn_com, 0, 500);
	int delay_gsm = 1000;	//3000;
	int tamanho = strlen(command);
	tamanho += 3;
	char new_command[tamanho];
	strcpy(new_command, command);
	strcat(new_command, "\r\n\0");
	uint8_t* new_com = (uint8_t*) new_command;

	uint8_t estado = 0;
	uint8_t posFimString = 0;

	if (HAL_UART_Transmit_IT(&huart5, (uint8_t*) new_com, tamanho) == HAL_OK)

	{
		HAL_UART_Transmit_IT(&hlpuart1, (uint8_t*) new_com, tamanho);
		estado = 1;
		HAL_Delay(300);
	}

	//HAL_UART_Receive(&huart1, &byte, 1,200);
	HAL_UART_Receive_IT(&huart5, (uint8_t*) &byteGSM, bytesToReceive);

	if (strcmp(command, "AT+UPSDA=1,3") == 0) //strcmp(command,"AT+URDFILE=\"resposta.txt\"")==0 ||
			{
		delay_gsm = 2000;

	}
	if (estado == 1) {
		HAL_Delay(delay_gsm);
		huart5.RxXferCount = 0;
		estado = 0;
		byte = 0;
		huart5.pRxBuffPtr = (uint8_t*) &byteGSM;
		for (uint16_t ii = 0; ii < bytesToReceive; ii++) {
			if (byteGSM[ii] == 0)
				break;

			//(uint8_t*) nn_com[ii] = byteGSM[ii];
			posFimString++;
		}
		strncpy(nn_com, (char*) byteGSM, 500);
		//HAL_UART_Transmit(&huart1,nn_com[ii], posFimString,1);
		//if(strcmp (nn_com,"at+cpin?\r\r\n+CPIN: READY\r\n\r\nOK\r\n")==0)
		// {
		//st_flags.simcard = true;
		// }
		HAL_Delay(delay_gsm);
		clearBuffer(byteGSM, posFimString);
	}

	//
}

void clearBuffer(uint8_t *pbuffer, uint8_t bufferSize) {

	for (uint8_t ii = 0; ii < bufferSize; ii++) {
		pbuffer[ii] = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	UartReady = SET;
	if (huart->Instance == LPUART1) {
		UartReady = SET;
		/* Transmit one byte with 100 ms timeout */
		HAL_UART_Transmit_IT(&huart5, &byte, 1);

		/* Receive one byte in interrupt mode */
		HAL_UART_Receive_IT(&hlpuart1, &byte, 1);
	} else if (huart->Instance == USART5) {

		/* Transmit one byte with 100 ms timeout */
		HAL_UART_Transmit_IT(&hlpuart1, (uint8_t*) &byteGSM, bytesToReceive);

		/* Receive one byte in interrupt mode */
		HAL_UART_Receive_IT(&huart5, (uint8_t*) &byteGSM, bytesToReceive);

	}

}
