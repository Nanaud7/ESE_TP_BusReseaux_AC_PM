/*	@Binôme	Arnaud CHOBERT & Pierre-Loïc MIRIO
 * 	@TP		Capteurs et réseaux
 * 	@Année	2021-2022
 */

#include <SHELL.h>

// Constantes
const char shellPrompt[] = "\r\n<CHOBERT-MIRIO> ";
const char cmdNotRecognized[] = "\r\nCommande non reconnue";

// Variables
Shell_Struct Shell;
char charReceived = 0;		// Dernier caractère reçu
char charArray[32] = "";	// Chaîne contenant tous les caractères reçues
uint8_t indexSh = 0;		// Indice de progressino dans la chaîne

// Commandes configurées
const char *ShellCmd[]= {
		"GET_T", // 0
		"GET_P", // 1
		"SET_K", // 2
		"GET_K", // 3
		"GET_A", // 4
		0
};

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

	case 1:
		// GET_P
		sprintf(buffer, "\r\nP=%ldPa\r\n",(uint32_t)BMP280_GetPressure());
		HAL_UART_Transmit(Shell->huart,(uint8_t*)buffer, sizeof(buffer), HAL_MAX_DELAY);
		break;

	case 2:
		// SET_K
		sprintf(buffer, "\r\nSET_K=OK\r\n"); // Work in progress
		HAL_UART_Transmit(Shell->huart,(uint8_t*)buffer, sizeof(buffer), HAL_MAX_DELAY);
		break;

	case 3:
		// GET_K
		sprintf(buffer, "\r\nK=%.5f\r\n",Stepper.K);
		HAL_UART_Transmit(Shell->huart,(uint8_t*)buffer, sizeof(buffer), HAL_MAX_DELAY);
		break;

	case 4:
		// GET_A
		sprintf(buffer, "\r\nA=%.4f\r\n",Stepper.K);
		HAL_UART_Transmit(Shell->huart,(uint8_t*)buffer, sizeof(buffer), HAL_MAX_DELAY);
		break;

	default:
		HAL_UART_Transmit(Shell->huart,(uint8_t*)cmdNotRecognized, sizeof(cmdNotRecognized), HAL_MAX_DELAY);
		break;
	}

	return 0;
}
