/* 
 * File:   mensajes.h
 * Author: sisamo
 *
 * Created on November 8, 2022, 3:42 PM
 */

#ifndef MENSAJES_H
#define MENSAJES_H

// Listado de Mensajes en Español e Ingles

typedef struct
{
	char msg[60];
	char font[50];
	int  weight;
	int  size;
} msg;


#define MSG_INICIA			0		// Inicializando
#define MSG_PERIFERICOS		1		// Perifericos
#define MSG_RETIRA_MIA		2		// Retira MIA
#define MSG_ESPERA			3		// Espera un Momento
#define MSG_MANTTO			4		// Equipo en Mantenimiento
#define MSG_LIMITE_SEMI		5		// Casi Lleno
#define MSG_RECARGA			6		// Mensaje de Recarga
#define MSG_VENTA_RECARGA	7		// Mensaje de Venta/Recarga
#define MSG_FALTANTE        8		// mensaje de $ faltante
#define MSG_ERROR_VENTA		9		// mensaje de venta de tarjeta
#define MSG_IDIOMA			10		// Cambiar Idioma
#define MSG_VENTA			11		// VENTA
#define MSG_ACEPTAR			12		// FINALIZAR
#define MSG_DESATORA		13		// DESATORAR
#define MSG_MOVIMIENTOS		14		// TRANSACCIONES
#define MSG_INICIAL			15		// saldo inicial
#define MSG_SUPAGO			16		// cantidad depositada
#define MSG_TOTAL			17		// Cantidad Total
#define MSG_COSTO			18		// Costo Tarjeta
#define MSG_BONO			19		// Bono	
#define MSG_VALIDA			20		// Validando
#define MSG_REGRESAR		21		// Regresar al menu anterior
#define MSG_IMPRIMIR		22		// Imprime Movimiento
#define MSG_CONFIRMA		23		// Confirma Impresion
#define MSG_LIMITE_EXC		24		// Limite Excedido
#define MSG_INSERTA_MIA		25		// Recarga MIA
#define MSG_NO				26		// NO
#define MSG_SALDO			27		// Saldo
#define MSG_CAJAS			28		// Error en Caja de Valores
#define MSG_CERRANDO		29		// Cerrando Sesion
#define MSG_INI_RECARGA		30		// Iniciando Sesion
#define MSG_INI_VENTA		31		// Iniciando Sesion
#define MSG_INCOMPLETA		32		// Venta Incompleta
#define MSG_FUERA_SERVICIO	33		// Fuera de Servicio
#define MSG_BILL_RESET		34		// Billetero Reset

#define TOTAL_MENSAJES		35
msg mensajes_ep[TOTAL_MENSAJES] =
{
	{"Inicializando\nEspere\nun\nMomento", "Sans", 700, 65},				// 0
	{"Estableciendo\nComunicacion\ncon\nPerifericos",  "Sans", 700, 65},	// 1
	{"RETIRE\nSU\nTARJETA", "Sans", 700, 100},								// 2
	{"Espere\nun\nMomento", "Sans", 700, 100},								// 3
	{"EQUIPO\nEN\nMANTENIMIENTO", "Sans", 700, 75},							// 4
	{"CAJAS CASI LLENAS", "Sans", 700 ,55},									// 5
	{"DEPOSITE", "Sans", 700, 70},											// 6
	{"DEPOSITE", "Sans", 700, 70},											// 7
	{"NO SE COMPLETA EL COSTO DE LA TARJETA\n\nFALTAN: ",  "Sans", 700, 30},// 8
	{"ERROR EN VENTA DE TARJETA\n\nTome su comprobante",  "Sans", 700, 40},	// 9
	{"ENGLISH",  "Sans", 700, 18},											// 10
	{"VENTA",  "Sans", 700, 50},											// 11
	{"FINALIZAR", "Sans", 700, 50},											// 12
	{"DESTRABAR", "Sans", 700, 18},											// 13
	{"MOVIMIENTOS",  "Sans", 700, 40},										// 14
	{"SALDO INICIAL", "Sans", 700, 40},										// 15
	{"SU PAGO",  "Sans", 700, 40},											// 16
	{"SALDO ACTUAL",  "Sans", 700, 40},										// 17
	{"COSTO TARJETA",  "Sans", 700, 40},									// 18
	{"BONO", "Sans", 700, 20},												// 19
	{"VALIDANDO", "Sans", 700, 60},											// 20
	{"REGRESAR", "Sans", 700, 55},											// 21
	{"IMPRIMIR\nMOVIMIENTO", "Sans", 700, 35},								// 22
	{"CONFIRMAR\nIMPRESION", "Sans", 700, 45},								// 23
	{"LIMITE\nEXCEDIDO", "Sans", 700, 70},									// 24
	{"INSERTA\nTARJETA", "Sans", 700, 60},									// 25
	{"REGRESA", "Sans", 700, 40},											// 26
	{"Cargando\nsu\nSaldo", "Sans", 700, 100},								// 27
	{"NO SE DETECTA\n\nCAJA DE VALORES", "Sans", 700, 70},					// 28
	{"FINALIZANDO\n\nPROCESO", "Sans", 700, 100},							// 29
	{"INICIANDO\n\n**RECARGA**", "Sans", 700, 100},							// 30
	{"INICIANDO\n\n** VENTA **", "Sans", 700, 100},							// 31
	{"No se completo el costo\n\nTome su comprobante",  "Sans", 700, 40},	// 32
	{"FUERA DE SERVICIO\n\nERROR LECTOR RECARGA", "Sans", 700, 50},			// 33
	{"REVISANDO BILLETERO\n\nESPERE UN MOMENTO", "Sans", 700, 50},	// 34
};

msg mensajes_in[TOTAL_MENSAJES] =
{
	{"Initializing\nPlease wait a\nmoment",  "Sans", 700, 65},				// 0
	{"Establishing\nCommunication\nwith\nPeripherals",  "Sans", 700, 65},	// 1
	{"REMOVE\nYOUR\n CARD",  "Sans", 700, 100},								// 2
	{"Wait\na\nMoment",  "Sans", 700, 100},									// 3
	{"Equipment\nin\nMaintenance",  "Sans", 700, 70},						// 4
	{"ALMOST FULL BOXES",  "Sans", 700, 60},								// 5
	{"DEPOSIT", "Sans", 700, 70},											// 6
	{"DEPOSIT", "Sans", 700, 70},											// 7
	{"THE COST OF THE CARD IS NOT COMPLETED",  "Sans", 700, 30},			// 8
	{"ERROR IN CARD SALE\n\ntake your voucher",  "Sans", 700, 40},			// 9
	{"ESPAÑOL",  "Sans", 700, 20},											// 10
	{"SALE",  "Sans", 700, 50},												// 11
	{"FINALIZE",  "Sans", 700, 55},											// 12
	{"UNJAM",  "Sans", 700, 22},											// 13
	{"TRANSACTIONS",  "Sans", 700, 35},										// 14
	{"INITIAL BALANCE",  "Sans", 700, 35},									// 15
	{"YOUR PAYMENT",  "Sans", 700, 40},										// 16
	{"CURRENT BALANCE",  "Sans", 700, 35},									// 17
	{"CARD COST",  "Sans", 700, 40},										// 18
	{"BONUS",  "Sans", 700, 20},											// 19
	{"VALIDATING",  "Sans", 700, 60},										// 20
	{"RETURN", "Sans", 700, 55},											// 21
	{"PRINT\nTRANSACTION", "Sans", 700, 35},								// 22
	{"CONFIRM\nPRINT", "Sans", 700, 45},									// 23
	{"LIMIT\nEXCEEDED", "Sans", 700, 70},									// 24
	{"INSERT\nCARD", "Sans", 700, 60},										// 25
	{"CANCEL", "Sans", 700, 30},											// 26
	{"Loading\nyour\nBalance", "Sans", 700, 100},							// 27
	{"NO VALUE\n\nBOX DETECTED", "Sans", 700, 70},							// 28
	{"FINISHING\n\nPROCESS", "Sans", 700, 100},								// 29
	{"STARTING\n\nRECHARGE", "Sans", 700, 100},								// 30
	{"STARTING\n\nSELL", "Sans", 700, 100},									// 31
	{"The cost is not complete\n\ntake your voucher",  "Sans", 700, 40},	// 32
	{"OUT OF SERVICE\n\nRECHARGE READER ERROR", "Sans", 700, 50},			// 33
	{"CHECKING BILL VALIDATOR\nPLEASE WAIT A MOMENT", "Sans", 700, 40},		// 34
};

#endif /* MENSAJES_H */