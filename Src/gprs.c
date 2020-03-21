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

	fn_send_gprs_command("AT&K0");

	fn_send_gprs_command("AT&K0");

	fn_send_gprs_command("AT+CPIN?");

	/*fn_fprint("Manufacturer identification");
	fn_fprint("\r\n");
	fn_send_gprs_command("AT+CGMI");

	fn_fprint("Model identification");
	fn_fprint("\r\n");
	fn_send_gprs_command("AT+CGMM");

	fn_fprint("IMEI identification");
	fn_fprint("\r\n");
	fn_send_gprs_command("AT+CGSN");

	fn_fprint("SIM CARD identification");
	fn_fprint("\r\n");
	fn_send_gprs_command("AT+CCID");*/

	fn_fprint("VIVO NETWORK REGISTER\r\n");
	fn_send_gprs_command("AT+UPSD=1,1,\"zap.vivo.com.br\"");

	fn_send_gprs_command("AT+UPSD=1,2,\"vivo\"");

	fn_send_gprs_command("AT+UPSD=1,3,\"vivo\"");

	fn_fprint("NETWORK STATUS\r\n");

	fn_send_gprs_command("AT+COPS?");

	fn_send_gprs_command("AT+CREG?");

	fn_send_gprs_command("AT+CGATT?");

	fn_send_gprs_command("AT+CGATT=1");

	fn_send_gprs_command("AT+CREG=1");

	fn_send_gprs_command("AT+UPSDA=1,3");

	fn_send_gprs_command("AT+UPSD=1,8");

	fn_send_gprs_command("AT+UPSD=1,0");

	fn_send_gprs_command("AT+UPSND=1,0");  //ip adress

	fn_send_gprs_command("AT+UHTTP=1");

	fn_send_gprs_command("AT+UHTTP=1,1,\"wizebox-api-dev.herokuapp.com\"");

	//wizebox-api-dev.herokuapp.com/sigfox/uplink?device=45C288&data=02241D3DE1417AFCA12345
	fn_send_gprs_command(
			"AT+UHTTPC=1,0,\"/sigfox/uplink?device=45C288&data=02241D3DE1417AFCA12345\",\"teste1.txt\"");
}

void fn_send_gprs_command(char* at) {
	memset(nn_com, 0, BYTE_GSM_SIZE);
	memset(byteGSM, 0, BYTE_GSM_SIZE);
	uint16_t tamanho = strlen(at);

	tamanho += 3;
	char new_command[tamanho];
	strcpy(new_command, at);
	strcat(new_command, "\r\n\0");
	uint8_t* new_com = (uint8_t*) new_command;

	for (int var = 0; var < tamanho; var++) {
		byte = new_com[var];
		if (HAL_UART_Transmit_IT(&huart5, &byte, 1) == HAL_OK) {
			while (UartReady != SET) {

			}
			//Reset transmission flag
			UartReady = RESET;
		}
	}
	if (strcmp (at, "AT+CGATT=1") == 0)
		HAL_Delay(50000);
	HAL_Delay(1000);
	if (HAL_UART_Receive_IT(&huart5, (uint8_t*) byteGSM, BYTE_GSM_SIZE)
			== HAL_OK) {

		huart5.pRxBuffPtr = (uint8_t*) &byteGSM;

	}
	//Reset transmission flag

	for (uint16_t ii = 0; ii < BYTE_GSM_SIZE; ii++) {
		if (byteGSM[ii] != 0) {
			uint8_t pass[1] = { 0 };
			pass[0] = byteGSM[ii];
			HAL_UART_Transmit(&hlpuart1, (uint8_t*) pass, 1, 100);
			HAL_Delay(10);
		}
	}

	//clearBuffer(byteGSM, posFimString);
}

void fn_fprint_gprs(char* command) {
	memset(nn_com, 0, 500);

	int tamanho = strlen(command);
	tamanho += 3;
	char new_command[tamanho];
	strcpy(new_command, command);
	strcat(new_command, "\r\n\0");
	uint8_t* new_com = (uint8_t*) new_command;
	uint8_t posFimString = 0;

	if (HAL_UART_Transmit_IT(&huart5, (uint8_t*) new_com, tamanho) == HAL_OK) {
		while (UartReady != SET) {

		}
		//Reset transmission flag
		UartReady = RESET;
		HAL_UART_Transmit_IT(&hlpuart1, (uint8_t*) new_com, tamanho);
		while (UartReady != SET) {

		}
		//Reset transmission flag
		UartReady = RESET;
	}
	HAL_UART_Receive_IT(&huart5, (uint8_t*) &byteGSM, bytesToReceive);

	huart5.RxXferCount = 0;
	//byte = 0;
	huart5.pRxBuffPtr = (uint8_t*) &byteGSM;
	for (uint16_t ii = 0; ii < bytesToReceive; ii++) {
		if (byteGSM[ii] == 0)
			break;

		posFimString++;
	}
	strncpy(nn_com, (char*) byteGSM, 500);
	clearBuffer(byteGSM, posFimString);

}

void clearBuffer(uint8_t *pbuffer, uint8_t bufferSize) {

	for (uint8_t ii = 0; ii < bufferSize; ii++) {
		pbuffer[ii] = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	UartReady = SET;

}
