/*	@Binôme	Arnaud CHOBERT & Pierre-Loïc MIRIO
 * 	@TP		Capteurs et réseaux
 * 	@Année	2021-2022
 */

#ifndef INC_SHELL_H_
#define INC_SHELL_H_

#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "BMP280.h"
#include "STEPPER.h"

// Structure
typedef struct Shell_Struct{
	UART_HandleTypeDef* huart;
} Shell_Struct;

// Variable
extern Shell_Struct Shell;
extern char charReceived;

// Prototypes
uint8_t Shell_Init(Shell_Struct* Shell, UART_HandleTypeDef* huart);
uint8_t Shell_CharReceived(Shell_Struct* Shell, char charReceived);
uint8_t Shell_FetchExecute(Shell_Struct* Shell, char* cmd);

#endif /* INC_SHELL_H_ */
