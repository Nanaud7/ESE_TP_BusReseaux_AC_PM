/*	@Binôme	Arnaud CHOBERT & Pierre-Loïc MIRIO
 * 	@TP		Capteurs et réseaux
 * 	@Année	2021-2022
 */

#ifndef INC_BMP280_H_
#define INC_BMP280_H_

#include "stdint.h"
#include "i2c.h"

// Macros
#define BMP280_ADDR 0x77<<1 // 0x77<<1
// Adresses des registres du BMP280
#define REG_CALIBT1		0x88
#define REG_CALIBP1		0x8E
#define REG_ID 			0xD0
#define REG_CTRL_MEAS 	0xF4
#define REG_CONFIG		0xF5
#define REG_PRESS_MSB	0xF7
#define REG_PRESS_LSB	0xF8
#define REG_PRESS_XLSB	0xF9
#define REG_TEMP_MSB 	0xFA
#define REG_TEMP_LSB 	0xFB
#define REG_TEMP_XLSB 	0xFC

// Prototypes
uint8_t BMP280_GetID();
uint8_t BMP280_SetConfig();
uint8_t BMP280_GetCalibration(uint8_t* calib);
float BMP280_GetTemperature();
uint32_t BMP280_CompensateT(uint8_t *calib, uint32_t rawTemp);
float BMP280_GetPressure();
uint32_t BMP280_CompensateP(uint8_t *calib, uint32_t rawPress);


#endif /* INC_BMP280_H_ */
