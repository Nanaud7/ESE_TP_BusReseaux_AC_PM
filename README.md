# TP Bus et réseaux - Pierre-Loïc MIRIO et Arnaud CHOBERT

## Table des matières

- [Introduction](#introduction)
- [Broches utilisées pour ce TP](#broches-utilisées-pour-ce-tp)
- [TP1 - Bus I2C](#tp1---bus-i2c)
- [TP2 - Interfaçage STM32 - Raspberry](#tp2---interfaçage-stm32---raspberry)
- [TP3 - Interface REST](#tp3---interface-rest)
- [TP4 - Bus CAN](#tp4---bus-can)
- [TP5 - Intégration](#tp5---intégration)
- [Conclusion](#conclusion)

## Introduction

Le TP de bus et réseaux nous a permis de mettre en pratique ce que l'on a vu dans le cours. Nous avons notamment réalisé :
- une liaison I2C entre un STM32 et un capteur de pression/température,
- une liaison UART entre un STM32 et un Raspberry Pi,
- une interface REST sur le Raspberry,
- une liaison CAN entre un STM32 et une carte pilotant un moteur pas à pas.

<img src="https://zupimages.net/up/21/46/owm7.png" width="90%" height="90%">

## Broches utilisées pour ce TP

### Broches utilisées par la NUCLEO-L476RG:<br/>
<img src="https://zupimages.net/up/21/46/7cju.png" width="75%" height="75%">
Il ne faut pas oublier d'ajouter un fil de masse entre la carte NUCLEO et la Raspberry Pi.

### Broches utilisées par le Raspberry Pi:<br/>

>>> Pierre-Loïc

## TP1 - Bus I2C

Cette première partie est consacrée à l'interfaçage d'un STM32 avec un capteur de pression/température et un accéléromètre. Ces deux composants partagent le même bus I2C et le STM32 joue le rôle de Master du bus. Pour cette partie le code a été développé en langage C en utilisant la bibliothèque HAL.<br/>
Lors du TP, nous avons uniquement développé une bibliothèque pour le capteur de pression/température BMP280.

Notre projet STM32CubeIDE utilise pour le TP1 la configuration suivante :
- Fréquence d'horloge de 80MHz qui est la fréquence maximale de notre microcontrôleur,
- Génération des fichiers d'initialisation des périphériques active,
- Périphérique I2C3 activé avec la configuration par défaut à 100kHz (les capteurs permettent aussi le 400kHz) __(SCL sur PC0 et SDA sur PC1)__,
- Périphérique UART2 activé à 115200 Bits/s avec interruption (pour avoir un retour d'informations).

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

Cette seconde partie du TP consiste à mettre en place une liaison série entre le SMT32 et le Raspberry Pi. Nous avons développé un shell sur le STM32 fonctionnant avec cette liaison série. Ci-dessous nous présenterons les codes développés pour ces deux émetteurs/récepteurs.

### Shell sur STM32

Nous avons ajouté à la configuration de notre projet STM32CubeIDE le périphérique UART3 à 115200 Bit/s avec interruption __(RX sur PC11 et TX sur PC10)__.</br>
Cette bibliothèque est composée de deux fichiers : SHELL.c et SHELL.h

__1. Initialisation de la structure de configuration__

La structure de configuration de notre shell prend la forme suivante :

 ```c
 // Structure
typedef struct Shell_Struct{
	UART_HandleTypeDef* huart;
} Shell_Struct;
  ```

Cette structure est très simple puisqu'elle prend uniquement la référence du périphérique UART l'utilisant. Une première structure de configuration a été créé dans le fichier source SHELL.c et mise en *extern* dans le fichier d'en-tête.

 ```c
/*	@brief	Initialisation de notre shell
 * 	@param	Shell_Struct Structure contenant les paramètres de notre shell à initialiser
 * 	@param	UART_Handle UART utilisé par le shell
 *	@retval 0
 */
uint8_t Shell_Init(Shell_Struct* Shell, UART_HandleTypeDef* huart){
	Shell->huart = huart;

	// Démarrage de l'interruption sur l'UART
	HAL_UART_Receive_IT(Shell->huart, (uint8_t*)&charReceived, 1);

	// Transmission du prompt de notre shell
	HAL_UART_Transmit(Shell->huart, (uint8_t*)shellPrompt, sizeof(shellPrompt), HAL_MAX_DELAY);

	return 0;
}
  ```

Cette fonction Shell_Init(), appelée dans le *main* , permet d'initialiser la structure avec la référence à l'UART, d'initialiser l'interruption de cet UART et d'afficher le prompt de notre shell.

 ```
 <CHOBERT-MIRIO>
  ```

__2. Réception d'un caractère et traitement de la chaîne__

Lorsqu'un caractère arrive sur le port série une interruption se déclenche et dirige l'exécution du programme vers l'adresse pointée par le vecteur d'interruption. On utilise une fonction *Callback* pour ajouter le traitement de cette interruption à notre fichier main.c.

 ```c
 void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart){

	if(huart->Instance == USART3){
		Shell_CharReceived(&Shell, charReceived);
		HAL_UART_Receive_IT(&huart3, (uint8_t*)&charReceived, 1);

		// Visualisation de la trame provenant de la Raspberry
		HAL_UART_Transmit(&huart2, (uint8_t*)&charReceived, 1, HAL_MAX_DELAY);
	}
}
  ```

Le caractère reçu est ajouté à la chaîne de caractère en cours et l'interruption est relancée. On transmet aussi ce qui a été reçu sur la liaison série du PC (*UART2*) pour vérifier la transmission.

 ```c
/*	@brief	Traitement du dernier caractère reçu
 * 	@param	Shell_Struct Structure contenant les paramètres de notre shell
 * 	@param	charReceived Caractère à traiter
 *	@retval 0
 */
uint8_t Shell_CharReceived(Shell_Struct* Shell, char charReceived){
	// Vérification fin de chaîne
	if(charReceived !='\r' && indexSh<32){
		// Transmission du caractère reçu
		HAL_UART_Transmit(Shell->huart, (uint8_t*)&charReceived, 1, HAL_MAX_DELAY);
		// Ajout du caractère dans la chaîne
		charArray[indexSh] = charReceived;
		indexSh++;
	}
	else{
		// Recherche et exécution de la commabde reçue
		Shell_FetchExecute(Shell, charArray);
		// Réinitialisation de la chaîne de caractères
		memset(charArray, 0, sizeof (charArray));
		indexSh=0;
		// Transmission du prompt de notre shell
		HAL_UART_Transmit(Shell->huart, (uint8_t*)shellPrompt, sizeof(shellPrompt), HAL_MAX_DELAY);
	}

	return 0;
}
 ```
  
Chaque caractère reçu est ajouté à la chaîne de caractères et retransmis pour l'afficher sur la console de l'utilisateur. Lorsque le caractère est un retour chariot ou que la chaîne de caractères est pleine, la commande est recherchée parmi celles configurées et la fonction correspondante est exécutée.

 ```c
/*	@brief	Recherche et exécution de la commande
 * 	@param	Shell_Struct Structure de configuration du shell
 * 	@param	cmd Chaîne de caractères contenant la commande de l'utilisateur
 *	@retval 0
 */
uint8_t Shell_FetchExecute(Shell_Struct* Shell, char* cmd){

	// Recherche de la commande parmi celles configurées
	uint8_t cmdSelected = -1;
	for (uint8_t j = 0; ShellCmd[j]; j++){
		if(!strcmp(cmd, ShellCmd[j])) cmdSelected = j;
	}

	char buffer[32] = ""; // Buffer utilisé par toutes les commandes ci-dessous
	switch(cmdSelected){
	case 0:
		// GET_T
		sprintf(buffer, "\r\nT=+%.2f_C\r\n",(float)BMP280_GetTemperature());
		HAL_UART_Transmit(Shell->huart,(uint8_t*)buffer, sizeof(buffer), HAL_MAX_DELAY);
		break;
		
	...

	default:
		HAL_UART_Transmit(Shell->huart,(uint8_t*)cmdNotRecognized, sizeof(cmdNotRecognized), HAL_MAX_DELAY);
		break;
	}

	return 0;
}
 ```

La fonction ci-dessus compare la chaîne reçue avec celles ajoutées dans un tableau de chaînes de caractères. Elle obtient un indice de ce tableau si une commande a été reconnu. Grâce à un *switch()* on peut sélectionner la fonction à exécuter ou transmettre un message d'erreur dans le cas ou la commande n'est pas reconnue.</br></br>
La bibliothèque pourrait être améliorée en ajoutant les variables du shell (la chaîne de caractères par exemple) dans la structure de configuration. Cela permettrait de rendre la bibliothèque totalement indépendante et d'être utilisable par plusieurs liaisons séries en simultané.
 
### UART avec Python sur Raspberry Pi

>>> Pierre-Loïc

## TP3 - Interface REST

>>> Pierre-Loïc

## TP4 - Bus CAN

Le STM32L476 utilisé pour ces TP, intègre un contrôleur CAN. Pour transmettre sur un bus CAN il faut aussi utiliser un Tranceiver CAN qui est ici un TJA1050 monté sur un shield au format Arduino. L'objectif est ici de piloter un module moteur pas à pas par le bus CAN.<br/>

Nous avons ajouté à la configuration de notre projet STM32CubeIDE le périphérique CAN1 avec une vitesse de 500kbit/s comme conseillé dans le sujet  __(RX sur PB8 et TX sur PB9)__.<br/>

Nous avons créé une nouvelle bibliothèque pour le contrôle du moteur pas à pas. Cette dernière est composée de deux fichiers : STEPPER.c et STEPPER.h

__1. Initialisation de la structure de configuration__

Comme pour le shell présenté plus haut, nous avons défini notre propre structure de configuration qui prend la forme suivante :

 ```c
 // Structure
typedef struct Stepper_Struct{
	CAN_HandleTypeDef* hcan;
	float K; // Coefficient K
	float A; // Angle
} Stepper_Struct;
  ```

Cette structure contient la référence du contrôleur CAN du SMT32, le coefficient K de la commande du moteur et l'angle A étant l'angle courant.

 ```c
/*	@brief	Initialisation de la structure
 *  @param	Stepper_Struct Structure de configuration du stepper à initialiser
 *	@retval	0
 */
uint8_t Stepper_Init(Stepper_Struct* Stepper, CAN_HandleTypeDef* hcan) {
	Stepper->hcan = hcan;
	Stepper->K = 100;
	Stepper->A = 0;

	// Activation du contrôleur CAN
	if(HAL_OK != HAL_CAN_Start(Stepper->hcan)){
		while(1);
	}
	return 0;
}
  ```
  
La fonction Stepper_Init() initialise la structure et active le contrôleur CAN.
  
__2. Transmettre un ordre de rotation en angle__

Afin de transmettre un ordre de rotation, nous avons créé la fonction Stepper_SetAngle() que voici :

 ```c
/*	@brief	Envoyer un ordre de rotation en angle au moteur pas à pas
 *  @param	Structure Stepper_Struct
 *  @param	angle Angle à réaliser
 *  @param	sign Sens de rotation
 *	@retval 0
 */
uint8_t Stepper_SetAngle(Stepper_Struct* Stepper, uint8_t angle, uint8_t sign) {
	// Initialisation de la structure du header
	CAN_TxHeaderTypeDef CanHeader;
	CanHeader.StdId = 0x61;						// Angle (0x61)
	CanHeader.ExtId = 0;						// Pas utilisé ici
	CanHeader.IDE = CAN_ID_STD;					// Trame standard
	CanHeader.RTR = CAN_RTR_DATA;				// Trame contenant des données
	CanHeader.DLC = 2;							// Trame contenant 2 octets
	CanHeader.TransmitGlobalTime = DISABLE;		// Pas de mesure du temps de réponse

	// Données à transmettre
	uint8_t trameCAN[2] = {angle,sign};

	// Envoi de le trame
	if(HAL_OK != HAL_CAN_AddTxMessage(&hcan1, &CanHeader, trameCAN, TxMailbox)){
		return 1;
	}

	return 0;
}
  ```
  
Cette fonction prend en paramètres la structure de configuration, l'angle de rotation et le sens de rotation. La fonction commence par créer l'en-tête de la trame avec les éléments suivants :
- StdId : Message ID dans le mode standard. Ici 0x61 pour la commande en angle (cf. documentation du module moteur pas à pas).
- ExtId : Message ID dans le mode étendu. Ici à 0 puisque nous utilisons le mode standard.
- IDE : Mode utilisé. Ici à CAN_ID_STD pour indiquer que nous utilisons le mode standard.
- RTR : Type utilisé. Ici à CAN_RTR_DATA puisque nous souhaitons transmettre des données en plus du message ID.
- DLC : Taille des données à transmettre. Ici à 2 pour transmettre deux octets.
- TransmitGlobal : Mesure de temps de réponse du bus CAN. Ici à DISABLE puisque nous ne l'utilisons pas.

La fonction construit un tableau avec les données à transmettre comprenant l'angle et le sens de rotation en suivant les indications de la documentation du module. La trame peut ensuite être envoyée avec une des fonctions HAL.
  
## TP5 - Intégration

### Commande du moteur pas à pas en fonction du capteur de température avec le STM32

Dans la boucle infinie du programme nous avons écrit un petit bout de programme permettant de piloter le moteur en fonction des variations de la température mesurée. 

```c
uint8_t angle = 0, sign = 0;
float previousTemp = 0;

while (1)
{
	// Pilotage du moteur en fonction de la température
	float temp = BMP280_GetTemperature();
	angle = (temp - previousTemp) * Stepper.K; 	// Multiplication par le coefficient K
	previousTemp = temp;

	if(angle >= 0) 	sign = 0;
	else{
		sign = 1;
		angle*=-1;
	}
	Stepper_SetAngle(&Stepper, angle, sign);
	
	HAL_Delay(1000);
}
```

La commande en angle est calculée à partir de la multiplication du coefficient K avec la différence entre la nouvelle température mesurée et la température de la précédente exécution. Le sens de rotation est ensuite défini en fonction du résultat précédent et l'ordre est envoyé avec la fonction Stepper_SetAngle() vu au-dessus.

## Conclusion

L'objectif principal du TP a été rempli puisque nous avons mis en pratique les enseignements vus en cours.<br/>

Lors de ce TP nous avons réussi à:
- récupérer les données de température et pression sur le Raspberry Pi par l'intermédiaire de notre shell,
- piloter le module moteur pas à pas avec le bus CAN.

>>> Pierre-Loïc
