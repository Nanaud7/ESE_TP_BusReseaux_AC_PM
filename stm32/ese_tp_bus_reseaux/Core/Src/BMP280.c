/*	@Binôme	Arnaud CHOBERT & Pierre-Loïc MIRIO
 * 	@TP		Capteurs et réseaux
 * 	@Année	2021-2022
 */

#include "BMP280.h"

/*	@brief	Récupération de l'identifiant du capteur
 *	@retval Identifiant du capteur
 */
uint8_t BMP280_GetID() {
	uint8_t reg = (uint8_t)REG_ID;	// Registre du BMP280 contenant l'ID du capteur
	uint8_t id;

	if(HAL_OK == HAL_I2C_Master_Transmit(&hi2c3, BMP280_ADDR, &reg, I2C_MEMADD_SIZE_8BIT, HAL_MAX_DELAY)){
		if(HAL_OK == HAL_I2C_Master_Receive(&hi2c3, BMP280_ADDR, &id, I2C_MEMADD_SIZE_8BIT, HAL_MAX_DELAY));
	}

	return id;
}

/*	@brief	Configuration du capteur
 *	@retval Nouvelle configuration dU le capteur pour vérification
 */
uint8_t BMP280_SetConfig() {
	uint8_t reg = (uint8_t)REG_CTRL_MEAS;
	uint8_t ctrl_meas_config = 0b01010111;
	uint8_t frame[2] = {reg, ctrl_meas_config};
	uint8_t ctrl_meas_read = 0;

	if(HAL_OK == HAL_I2C_Master_Transmit(&hi2c3, BMP280_ADDR, frame, I2C_MEMADD_SIZE_16BIT, HAL_MAX_DELAY)){
		if(HAL_OK == HAL_I2C_Master_Transmit(&hi2c3, BMP280_ADDR, &reg, I2C_MEMADD_SIZE_8BIT, HAL_MAX_DELAY))
			if(HAL_OK == HAL_I2C_Master_Receive(&hi2c3, BMP280_ADDR, &ctrl_meas_read, I2C_MEMADD_SIZE_8BIT, HAL_MAX_DELAY));
	}

	return ctrl_meas_read;
}

/*	@brief	Récupération de l'étalonnage du capteur
 *	@retval 0
 */
uint8_t BMP280_GetCalibration(uint8_t* calib){
	uint8_t reg = (uint8_t)REG_CALIBT1;
	for(int i=0; i<26; i++)
		//printf("calib_array[%d]\t= %d\r\n",i,calib_array[i]);

		if(HAL_OK == HAL_I2C_Master_Transmit(&hi2c3, BMP280_ADDR, &reg, I2C_MEMADD_SIZE_8BIT, HAL_MAX_DELAY)){
			if(HAL_OK == HAL_I2C_Master_Receive(&hi2c3, BMP280_ADDR, calib, 26, HAL_MAX_DELAY)){
				//for(int i=0; i<26; i++) printf("calib_array[%d]\t= %d\r\n",i,calib_array[i]);
			}
		}

	return 0;
}

/*	@brief	Récupération de la température compensée
 *	@retval Température compensée
 */
float BMP280_GetTemperature(){
	// Récupération de la temperature brute dans les registres du BMP280
	uint8_t reg = (uint8_t)REG_TEMP_MSB;
	uint8_t tempArray[3] = {0};
	if(HAL_OK == HAL_I2C_Master_Transmit(&hi2c3, BMP280_ADDR, &reg, 1, HAL_MAX_DELAY)){
		if(HAL_OK == HAL_I2C_Master_Receive(&hi2c3, BMP280_ADDR, tempArray, 3, HAL_MAX_DELAY));
	}

	// Mise en ordre des registres
	uint32_t rawTemp = tempArray[0] << 12 | tempArray[1] << 4 | tempArray[2] >> 4;

	// Récupération de la calibration dans les registres du BMP280
	reg = (uint8_t)REG_CALIBT1;
	uint8_t calibArray[6] = {0};
	if(HAL_OK == HAL_I2C_Master_Transmit(&hi2c3, BMP280_ADDR, &reg, 1, HAL_MAX_DELAY)){
		if(HAL_OK == HAL_I2C_Master_Receive(&hi2c3, BMP280_ADDR, calibArray, 6, HAL_MAX_DELAY));
	}

	return (float)BMP280_CompensateT(calibArray, rawTemp)/100;
}

/*	@brief	Calcul de la température compensée
 * 	@param	Calibration de la mesure de température
 * 	@param	Température brute récupérée
 *	@retval Température compensée
 */
uint32_t BMP280_CompensateT(uint8_t *calib, uint32_t rawTemp) {
	uint32_t dig_T1 = calib[0] | calib[1]<<8;
	uint32_t dig_T2 = calib[2] | calib[3]<<8;
	uint32_t dig_T3	= calib[4] | calib[5]<<8;

	uint32_t var1 = ((((rawTemp >> 3) - (dig_T1 << 1)))
			* dig_T2) >> 11;
	uint32_t var2 = (((((rawTemp >> 4) - dig_T1)
			* ((rawTemp >> 4) - dig_T1)) >> 12)
			* dig_T3) >> 14;

	uint32_t t_fine = var1 + var2;
	return (t_fine * 5 + 128) >> 8;
}

/*	@brief	Récupération de la pression compensée
 *	@retval Pression compensée
 */
float BMP280_GetPressure(){
	// Récupération de la pression brute dans les registres du BMP280
	uint8_t reg = (uint8_t)REG_PRESS_MSB;
	uint8_t pressArray[3] = {0};
	if(HAL_OK == HAL_I2C_Master_Transmit(&hi2c3, BMP280_ADDR, &reg, 1, HAL_MAX_DELAY)){
		if(HAL_OK == HAL_I2C_Master_Receive(&hi2c3, BMP280_ADDR, pressArray, 3, HAL_MAX_DELAY));
	}

	// Mise en ordre des registres
	uint32_t rawPress = pressArray[0] << 12 | pressArray[1] << 4 | pressArray[2] >> 4;

	// Récupération de la calibration dans les registres du BMP280
	reg = (uint8_t)REG_CALIBP1;
	uint8_t calibArray[18] = {0};
	if(HAL_OK == HAL_I2C_Master_Transmit(&hi2c3, BMP280_ADDR, &reg, 1, HAL_MAX_DELAY)){
		if(HAL_OK == HAL_I2C_Master_Receive(&hi2c3, BMP280_ADDR, calibArray, 18, HAL_MAX_DELAY));
	}

	return BMP280_CompensateP(calibArray, rawPress)/256;
}

/*	@brief	Calcul de la pression compensée
 * 	@param	Calibration de la mesure de pression
 * 	@param	Pression brute récupérée
 *	@retval Pression compensée
 */
uint32_t BMP280_CompensateP(uint8_t *calib, uint32_t rawPress) {
	uint32_t dig_P1 = calib[0] | calib[1]<<8;
	uint32_t dig_P2 = calib[2] | calib[3]<<8;
	uint32_t dig_P3	= calib[4] | calib[5]<<8;
	uint32_t dig_P4	= calib[6] | calib[7]<<8;
	uint32_t dig_P5	= calib[8] | calib[9]<<8;
	uint32_t dig_P6	= calib[10] | calib[11]<<8;
	uint32_t dig_P7	= calib[12] | calib[13]<<8;
	uint32_t dig_P8	= calib[14] | calib[15]<<8;
	uint32_t dig_P9	= calib[16] | calib[17]<<8;

	uint32_t t_fine = 0;
	uint64_t var1, var2, p;

	var1 = ((uint64_t)t_fine) - 128000;
	var2 = var1 * var1 * (uint64_t)dig_P6;
	var2 = var2 + ((var1*(uint64_t)dig_P5)<<17);
	var2 = var2 + (((uint64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (uint64_t)dig_P3)>>8) + ((var1 * (uint64_t)dig_P2)<<12);
	var1 = (((((uint64_t)1)<<47)+var1))*((uint64_t)dig_P1)>>33;
	if(var1 == 0) {
		return 0;	// avoid exception caused by division by zero
	}
	p = 1048576-rawPress;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((uint64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((uint64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((uint64_t)dig_P7)<<4);
	return (uint32_t)p;
}
