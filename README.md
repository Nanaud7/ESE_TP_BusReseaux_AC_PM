# ESE_TP_BusReseaux_AC_PM

Pierre-Loïc MIRIO et Arnaud CHOBERT<br/>
ESE 2021-2022

## Introduction

Le TP de bus et réseaux nous a permis de mettre en pratique ce que l'on a vu dans le cours. Nous avons notamment réalisé :
- une liaison I2C entre un STM32 et un capteur de pression/température,
- une liaison UART entre un STM32 et un Raspberry Pi,
- une interface REST sur le Raspberry,
- une liaison CAN entre un STM32 et une carte pilotant un moteur pas à pas.

<img src="https://zupimages.net/up/21/46/owm7.png" width="90%" height="90%">

## TP1 - Bus I2C

Cette première partie est consacrée à l'interfaçage d'un STM32 avec un capteur de pression/température et un accéléromètre. Ces deux composants partagent le même bus I2C et le STM32 joue le rôle de Master du bus. Pour cette partie le code a été développé en langage C en utilisant la bibliothèque HAL.<br/>
Lors du TP, nous avons uniquement développé une bibliothèque pour le capteur de pression/température BMP280 par manque de temps.

### Bibliothèque pour le BMP280 :

Cette bibliothèque est composée de deux fichiers : BMP280.c et BMP280.h
Le premier est le code source contenant toutes les fonctions nécessaires à la communication I2C et au traitement des données. Le second est le fichier d'en-tête contenant l'adresse I2C du capteur, les adresses des registres et les prototypes des fonctions.

__1. Identification du BMP280__

  ```c
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
  ```

Les adresses des registres ont été écrites avec des macros (*#define*) dans le fichier d'en-tête. Pour les passer aux fonctions HAL, nous avons d'abord initialisé des variables avec leur valeur (avec la variable *reg* dans l'exemple ci-dessus).

__2. Configuration du BMP280__

 ```c
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
  ```

Pour configurer le capteur, il faut transmettre deux octets. Le premier octet contient l'adresse du registre à modifier et le second ce que l'on souhaite écrire à cette adresse. Pour cela, nous avons créé un tableau avec le type *uint8_t* contenant ces deux informations et nous l'avons passé en paramètre de la fonction HAL.<br/>

Nous avons aussi ajouté une requête de la configuration du capteur après la reconfiguration pour que l'utilisateur de la bibliothèque puisse vérifier si la fonction a bien fonctionné. La configuration reçue est en retour de cette fonction.

__3. Récupération de l'étalonnage__

 ```c
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
  ```
  
Cette fonction permet de récupérer les données d'étalonnage du capteur qui sont spécifiques à chaque capteur et réalisées par le fabricant. Ces données sont utiles pour le calcul de la température et de la pression.<br/>
Cette fonction permet de récupérer les 26 octets de l'étalonnage. Elle n'est utile que pour l'exercice, nous verrons en dessous que nous feront plutôt des requêtes sur les quelques octets qui nous intéressent.
  
__4. Récupération des données et calcul de la température__

 ```c
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
  ```

Cette fonction est découpée en quatre parties. La première est la récupération des données brutes du capteur sur trois octets. La seconde partie consiste à remettre en ordre les données en accord avec les indications de la datasheet du composant. La troisième partie est la récupération des 6 octets d'étalonnage nécessaires au calcul de la température compensée. L'ultime partie, au niveau du retour de la fonction, est l'appel de la formule permettant d'obtenir la température à partir des données brutes et de l'étalonnage du capteur. Vous pouvez retrouver ci-dessous la formule :

 ```c
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
  ```

La fonction permettant d'obtenir la pression fonctionne de la même manière, il n'est donc pas nécessaire de la décrire ici. Vous pouvez la retrouver dans les fichiers de cette bibliothèque.

## TP2 - Interfaçage STM32 - Raspberry

=> Arnaud
=> PL

## TP3 - Interface REST

=> PL

## TP4 - Bus CAN

=> Arnaud

## Conclusion
