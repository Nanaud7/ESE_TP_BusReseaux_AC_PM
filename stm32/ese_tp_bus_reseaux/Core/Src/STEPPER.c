/*	@Binôme	Arnaud CHOBERT & Pierre-Loïc MIRIO
 * 	@TP		Capteurs et réseaux
 * 	@Année	2021-2022
 */

#include <STEPPER.h>

// Variables
Stepper_Struct Stepper;
uint32_t TxMailbox[8];	// pointeur vers la boite au lettre de transmission

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

/*	@brief	Modifier le coefficient K de la commande du moteur pas à pas
 *  @param	Structure Stepper_Struct
 *  @param	Nouveau coefficient K
 *	@retval	0
 */
uint8_t Stepper_SetK(Stepper_Struct* Stepper, uint16_t K) {
	Stepper->K = K;
	return 0;
}

/*	@brief	Lire le coefficient K utilisé pour la commande du moteur pas à pas
 *  @param	Structure Stepper_Struct
 *	@retval	Coefficient K en float
 */
float Stepper_GetK(Stepper_Struct* Stepper) {
	return Stepper->K;
}

/*	@brief	Lire l'angle du moteur pas à pas
 *  @param	Structure Stepper_Struct
 *	@retval	Angle du moteur pas à pas
 */
float Stepper_GetA(Stepper_Struct* Stepper) {
	return Stepper->A;
}

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
