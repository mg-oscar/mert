/* 
 * File:   xmls.h
 * Author: famisa
 *
 * Created on January 10, 2023, 11:58 AM
 */

#ifndef XMLS_H
#define XMLS_H

#include <libxml/parser.h>
#include <dirent.h>
#include <stdio.h>

//
// ********************* Estructuras de Datos de los XML ***********************
//
// ************************* Encabezado General ********************************
typedef struct
{	char tipo[10];
	int  version;
	time_t time_creacion;
}XML_HDR_TDE;

// *******************Estructura para los puertos ******************************
typedef struct
{	
	char Marca;
	char SAM_SLOT0;			// 0=NO  1=PSAM  2=ISAM  3=0xFFFFFFFFFFFFFFFF
	char SAM_SLOT1;			// 0=NO  1=PSAM  2=ISAM  3=0xFFFFFFFFFFFFFFFF
	char SAM_SLOT2;			// 0=NO  1=PSAM  2=ISAM  3=0xFFFFFFFFFFFFFFFF
	char SAM_SLOT3;			// 0=NO  1=PSAM  2=ISAM  3=0xFFFFFFFFFFFFFFFF
	char TipoPuerto;		// 0 = USB  1= RS232
	char SerialName[50];	// Nombre del puerto serial
	int  SerialBaud;		// Baud 
}PORTRDR;

typedef struct
{
	int  COIN_PROTOCOL;		// 0=CCTALK   1=MDB
	char COIN_COMM[50];		// Comm Monedero
	int  COIN_BAUD;			// Baud Monedero
	int  COIN_VERIFICACION;	// cheksum o crc
	int  COIN_ECHO;			// comm con Eco
} PORTMOD;

typedef struct
{
	PORTRDR PortRdr[5];
	PORTMOD PortMod[3];
}PORTS;

typedef struct
{
	// EXPENDEDORA
	char EXP_Marca;						// 0 = NO  1 = CHINA-CREATOR  2 = CIVINTEC
										// 3 = ARCONTIA .....
	char EXP_SAM_SLOT0;					// 0=NO  1=PSAM  2=ISAM  3=0xFFFFFFFFFFFFFFFF
	char EXP_SAM_SLOT1;					// 0=NO  1=PSAM  2=ISAM  3=0xFFFFFFFFFFFFFFFF
	char EXP_SAM_SLOT2;					// 0=NO  1=PSAM  2=ISAM  3=0xFFFFFFFFFFFFFFFF
	char EXP_SAM_SLOT3;					// 0=NO  1=PSAM  2=ISAM  3=0xFFFFFFFFFFFFFFFF
	char EXP_TipoPuerto;				// 0 = USB  1= RS232
	char EXP_SerialName[50];			// Nombre del puerto serial
	int  EXP_SerialBaud;				// Baud del Puerto Serie

	char EXP_USBName[50];				// Nombre del puerto USB
	char EXP_USBMarca;					// Reader en Dispensador

	// RECARGADORA
	char REC_Marca;						//
	char REC_SAM_SLOT0;					//
	char REC_SAM_SLOT1;					//
	char REC_SAM_SLOT2;					//
	char REC_SAM_SLOT3;					//
	char REC_TipoPuerto;				//
	char REC_SerialName[50];			//
	int  REC_SerialBaud;				//

	// CAJA MONEDAS
	char COINBOX_Marca;					//
	char COINBOX_SAM_SLOT0;				//
	char COINBOX_SAM_SLOT1;				//
	char COINBOX_SAM_SLOT2;				//
	char COINBOX_SAM_SLOT3;				//
	char COINBOX_TipoPuerto;			//
	char COINBOX_SerialName[50];		//
	int  COINBOX_SerialBaud;			//

	// CAJA BILLETES
	char BILLBOX_Marca;					//
	char BILLBOX_SAM_SLOT0;				//
	char BILLBOX_SAM_SLOT1;				//
	char BILLBOX_SAM_SLOT2;				//
	char BILLBOX_SAM_SLOT3;				//
	char BILLBOX_TipoPuerto;			//
	char BILLBOX_SerialName[50];		//
	int  BILLBOX_SerialBaud;			//

	// PUERTA
	char PUERTA_Marca;					//
	char PUERTA_SAM_SLOT0;				//
	char PUERTA_SAM_SLOT1;				//
	char PUERTA_SAM_SLOT2;				//
	char PUERTA_SAM_SLOT3;				//
	char PUERTA_TipoPuerto;				//
	char PUERTA_SerialName[50];			//
	int  PUERTA_SerialBaud;				//

	char MODIO_COMM[50];				// Comm Modulo I/O
	int  MODIO_BAUD;					// Baud Modulo I/O
	int  MODIO_TP;						// Total de Modulos

	int  COIN_PROTOCOL;					// 0=CCTALK   1=MDB
	char COIN_COMM[50];					// Comm Monedero
	int  COIN_BAUD;						// Baud Monedero
	int  COIN_VERIFICACION;				// cheksum o crc
	int  COIN_ECHO;						// comm con Eco
	char COIN_MARCA[4];						//

	int  BILL_PROTOCOL;					// 0=CCTALK   1=MDB
	char BILL_COMM[50];					// Comm Billetero
	int  BILL_BAUD;						// Baud Billetero
	int  BILL_VERIFICACION;				// cheksum o crc
	int	 BILL_ECHO;						// comm con ECO
	char BILL_MARCA[4];						//
} PUERTOS;

// ************************ Billetes / Monedas *********************************
typedef struct
{
	int tog_reg;
	int	id[10];
	int denominacion[10];
	int	enable[10];
	char name[10][30];
}BILL_COIN_NOTES;
	
// ************************ BALANCE SCHEME ** **********************************
typedef struct
{	int tot_reg;
	int	profile[50];
	int max_balance[50];
	int	expiration[50];
}BALANCE;

// ****************** XML DE BONOS *********************************************
typedef struct
{	int	 tot_reg;
	char name[20][20];
	int  code[20];
	int  value[20];
	int	 bono[20];
}BONOS_RANGO;

typedef struct
{
	int	tot_reg;
	int code[10];
	char name[10][30];
}BONOS_NAME;

typedef struct
{	int tot_reg;
	int code[30];
	int profile[30];
}BONOS_PROFILE;

// ****************** XML DE MENU's *********************************************
typedef struct
{	int	 tot_reg;
	int  opcion[10];
}MENU_TYPE;

typedef struct
{
	int	tot_reg;
	int code[10];
	char name[10][30];
}MENU_NAME;

typedef struct
{	int tot_reg;
	int profile[30];
	int menu[30];
	char name[30][50];
}MENU_PROFILE;

// ***************************   XML DE USUARIOS   *****************************
typedef struct
{	int tot_reg;
	unsigned int UID[500];
	char NumLog[500][30];
    char profile[500];
	char name[500][40];
	char pwd[500][6]; 
}USERS;

// ********* XML DEL NOMBRE DE LINEAS Y ESTACIONES   ***************************
typedef struct
{	int		tot_reg;
	int		code[40];
	int		codeTDE[40];
	char	name[40][50];
}S_N;

typedef struct
{	int		tot_reg;
	int		code[10];
	int		codeTDE[10];
	char	name[10][50];
	S_N		estaciones[10];
}LINE_STATION;

// ********* XML DE IDENTIFICACION DE LA MERT   ********************************
typedef struct
{
	char code[15];
	char name[50];
	int  state;
	int  lineaTDE;		// TDE Linea
	int  estacionTDE;	// TDE Estacion
	int  lineaTV;		// Telvent Linea
	int  estacionTV;	// Telvent Estacion
	int  NumDisp;
}ID_MERT;

// *********** XML DE LIMITES DE MONEDERO/BILLETERO ****************************
typedef struct
{
	int type;
	int cost_travel;
	int cost_card;
	int max_balance;
	int coin_capacity_stored;
	int coin_threshold_stored;
	int threshold_stored;
	int capacity_validator;
	int capacity_dispenser;
	int threshold_dispenser;
	int bill_capacity_stored;
	int bill_threshold_stored;
	int time_ope_refill;
	int time_ope_purchase;
	char url[100];
} MERT_LIMIT;

// ********************* XML READERS *******************************************
typedef struct
{	int  tot_reg;
	char posicion[10][20];	//c1
	int	 slot[10];			//c2
	long UID[10];			//c3
} READERS;

// ********************* XML LISTA NEGRA MIA ***********************************
typedef struct
{
	int tog_reg;
	long long bl_mia[10000];
}BL_MIA;

// ********************* XML LISTA NEGRA SAM ***********************************
typedef struct
{
	int tog_reg;
	long long bl_sam[100];
}BL_SAM;

//********************* XML CONFIG MERT ****************************************
typedef struct 
{	int Dispensador;
	int Monedero;	
	int Billetero;
	int Stacker;	// Stacker Activo
	int NoMov;		// No de Mov en Entrada/Retiro de Tarjetas
	int Captura;	// tarjetas en Bandeja de captura del dispensador
	int BinBoxMax;	// Numero Maximo de Tarjetas en Bandeja de Captura
	int NoMovEvent;	// Numero de Eventos en la MERT
}CONFIG_MERT;

// ****************************************************************************

// Puertos
XML_HDR_TDE		MertPortXmlHdr;
PUERTOS         MertPort;	

// Lista de Lineas y Estaciones
XML_HDR_TDE		LineasXmlHdr;
LINE_STATION	Lineas;

// Lista de Usuarios
XML_HDR_TDE		UsuariosXmlHdr;
USERS			Usuarios;

// BALANCE
XML_HDR_TDE		BalanceXmlHdr;
BALANCE			BalanceDatos;

// BONOS
XML_HDR_TDE		BonosXmlHdr;
BONOS_NAME		BonosName;
BONOS_RANGO		BonosRango[5];
BONOS_PROFILE	BonosProfile;

// MENU's
XML_HDR_TDE		MenuXmlHdr;
MENU_NAME		MenuName;
MENU_TYPE		MenuType[10];
MENU_PROFILE	MenuProfile;


// Identificacion de la MERT
XML_HDR_TDE		MertIDXmlHdr;
ID_MERT			MertID;

// Varios Datos
XML_HDR_TDE		MertLmtXmlHdr;
MERT_LIMIT		MertLmt;

// Readers
XML_HDR_TDE		ReadersXmlHdr;
READERS			Readers;

// Blacklist MIA
XML_HDR_TDE		BlackListMIAXmlHdr;
BL_MIA			BlackListMIA;

// Blacklist SAM
XML_HDR_TDE		BlackListSAMXmlHdr;
BL_SAM			BlackListSAM;

// Config MERT
XML_HDR_TDE		ConfigXmlHdr;
CONFIG_MERT		Config;

// Billetes/Monedas Aceptadas
XML_HDR_TDE		BankXmlHdr;
BILL_COIN_NOTES BankNotes[2];

xmlNode *GetXmlHeaderTDE(xmlDoc *docXml, char* path, XML_HDR_TDE *xmlhdr);

#endif /* XMLS_H */

