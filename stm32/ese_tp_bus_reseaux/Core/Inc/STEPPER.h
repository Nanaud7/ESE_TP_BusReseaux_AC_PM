/*	@Binôme	Arnaud CHOBERT & Pierre-Loïc MIRIO
 * 	@TP		Capteurs et réseaux
 * 	@Année	2021-2022
 */

#ifndef INC_STEPPER_H_
#define INC_STEPPER_H_

#include "stdint.h"
#include "can.h"

// Structure
typedef struct Stepper_Struct{
	CAN_HandleTypeDef* hcan;
	float K; // Coefficient K
	float A; // Angle
} Stepper_Struct;

// Variable
extern Stepper_Struct Stepper;

// Prototypes
uint8_t Stepper_Init(Stepper_Struct* Stepper, CAN_HandleTypeDef* hcan);
uint8_t Stepper_SetK(Stepper_Struct* Stepper, uint16_t K);
float Stepper_GetK(Stepper_Struct* Stepper);
float Stepper_GetA(Stepper_Struct* Stepper);
uint8_t Stepper_SetAngle(Stepper_Struct* Stepper, uint8_t angle, uint8_t sign);

#endif /* INC_STEPPER_H_ */
