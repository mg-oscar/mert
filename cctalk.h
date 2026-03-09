/* 
 * File:   cctalk.h
 * Author: FAMISA
 *
 * Created on September 12, 2022, 6:13 PM
*/

#ifndef CCTALK_H
#define CCTALK_H


// Peso de las monedas en grms
#define M5C						1.58		// 5 ctvos
#define M10C					1.755		// 10 ctvos
#define M20CN					2.258		// 20 ctvos nueva
#define M20CD					3.04		// 20 ctvos (dorada)
#define M50CN					3.103		// 50 ctvos nueva
#define M50CD					4.39		// 50 ctvos (dorada)
#define M1PESO					3.95		// 1 PESO
#define M2PESOS					5.19		// 2 PESOS
#define M5PESOS					7.07		// 5 PESOS
#define M10PESOS				10.329		// 10 PESOS
#define M20PESOS				12.67		// 20 PESOS


#define COIN_FACTOR				1
#define COIN_MAYOR				2000		// Moneda de mayor denominacion aceptada


#define MASTER_ADDRESS			0x01
#define COIN_ADDRESS			0x02
#define BILL_ADDRESS			0x28

#define CCTALK_CHK				0x00		// Se usa CHECKSUM
#define CCTALK_CRC				0x01		// Se usa CRC

#define CCTALK_SIN_ECO			0x00		// Comunicacion sin eco en la tx
#define CCTALK_CON_ECO			0x01		// Comunicacion con eco en la tx

#define CCTALK_ON				0x01
#define CCTALK_OFF				0x00

#define BILL_ENABLE_STACKER		0x01
#define BILL_ENABLE_ESCROW		0x02

// Ordenes para el billetero
#define BILL_RETURN				001		// Regresa el billete
#define BILL_ACCEPT				0x01		// Acepta el billete
#define BILL_SEND_TO_CASH_BOX	0x01		// Envia el billete al stacker
#define BILL_EXTENDED_TIMEOUT	0xFF		// Extend Escrow Timeout

// Respuestas
#define BILL_ERROR_ESCROW		254		// ERROR -> escrow is empty
#define BILL_FAILED_ROUTE		255		// failed to route bill 

#define CCTALK_BOX				0
#define CCTALK_ESCROW			1

#define BILLETE_EN_CAJA			0
#define BILLETE_EN_ESCROW		1
#define BILLETE_RECHAZADO		2
#define BILLETE_NADA			3

#define MONEDA_EN_CAJA			0


#define BILLETE_POS_DUMMY		0x7F	//0x0F  // 20250129

typedef struct
{
	unsigned char events;		// numero de evento
	unsigned char datos[10];	// buffer 5 ultimos eventos
}CCTALK_EVENTS;

#define TOTAL_DENOMINACION_MAX		32

typedef struct 
{
	char cctalk_id [50];		// Equipment ID
	char cctalk_mid[50];		// Manufacturer ID
	char cctalk_pc [50];		// Product Code
	char cctalk_bc [50];		// Build Code
	char cctalk_sr [50];		// Build Code
	long cctalk_sn;				// numero de serie
	long cctalk_pp;				// polling priority
}CC_TALK_ID;

typedef struct
{
	long cctalk_ic;			// Insertion Counter
	long cctalk_ac;			// Accept Counter
	long cctalk_rc;			// Reject Counter
	long cctalk_fc;			// Fraud Counter
}COUNTERS;


// Contador de Monedas/Billetes
typedef struct
{
	char cvf[TOTAL_DENOMINACION_MAX][6];
	int valor[TOTAL_DENOMINACION_MAX];
	int counter[TOTAL_DENOMINACION_MAX];
	int cantidad;				// Total en Monedas/Billetes
}CNT_BILL_COIN;

// timeout milisegundos
#define TIMEOUT_CCTALK			200
#define TIMEOUT_CCTALK_EVENTS	300

#define MONEDAS_ENABLE			0xFF
#define BILLETES_ENABLE			0xFF

#define MONEDAS_DISABLE			0x00
#define BILLETES_DISABLE		0x00


#define CMD_BILL_POLL				0x00
#define CMD_BILL_ENABLE				0x01
#define CMD_BILL_DISABLE			0x02
#define CMD_BILL_ACCEPT				0x03
#define CMD_BILL_RETURN				0x04
#define CMD_BILL_RESET				0x05
#define CMD_BILL_SENSOR				0x06

#define CMD_COIN_POLL				0x00
#define CMD_COIN_ENABLE				0x01
#define CMD_COIN_DISABLE			0x02
#define CMD_COIN_ACCEPT				0x03
#define CMD_COIN_RETURN				0x04
#define CMD_COIN_WAIT				0x05

#define CMD_TEST					232


class cctalk: public serialport
{
public:

	unsigned char status;			// 1= Conexion establecida
	unsigned char inhibit;			// Status Inhibit

	unsigned char cctalk_slave_address;
	unsigned char cctalk_event_old=-1;

	unsigned char TotalDenominaciones;
	unsigned char EnableChannel[4];

	unsigned char Posicion;
	unsigned char Path;
	unsigned char error;

	int	cctalk_scala;		// billetero
	int	cctalk_divisor;		// billetero
	int cctalk_factor;		// monedero
	
	unsigned char fg_master_inhibit;	//	char ;		//

	CC_TALK_ID		cctalkID;		// Identificacion general del dispositivo
	CCTALK_EVENTS	cctalkevents;
	CNT_BILL_COIN	depositado;		// dinero depositado 

	COUNTERS		counters;

	// exclusivo billetero
//	unsigned char bill_reset;		// indica si se debe enviar un reset al billetero
	unsigned char num_intentos;		// contador de intentos de Reset

	
	cctalk();

	
	// En Comun
	int	Inicializa(unsigned char addr, int verif, int eco);



	int ReadEvents(char first);
	unsigned int RdEvent(unsigned char ne);

	int Value(unsigned char position);
	int Value_orgx(unsigned char position);
	
	int cctalk_OnOff(char status);


	int RouteBill(unsigned char op);	//	Comando para aceptar o regresar el billete

	void EraseEvents(void);

	int SendDisable(void);
	int SendEnable(void);

	int SendRequestSelfTest(void);
	int SendResetDevice(void);

	
	int SendWaitMasterInhibit(unsigned char status);
	int	SendMasterInhibitStatus(unsigned char status);


	int SendModifyInhibitBillStatus(unsigned char *dat, unsigned char tdat );
	int SendModifyInhibitStatus( unsigned char *dat, unsigned char tdat );
	
	void ClearDepositado(void);

	int	ReadAllCounters(void);
	

private:
	int  cc_talk_mseg;			// polling priority
	char cc_talk_cvf[6];		// Denominacion de Billete/Moneda

	unsigned char cctalk_buffer[100];
	unsigned char cctalk_apuntador=0;
	
	char cctalk_datos[50];

	unsigned char cctalk_verifica;				// 0=CHECKSUM   1=CRC
	unsigned char cctalk_echo;					// 0=SIN ECO	1=CON ECO

	
	void tx_cctalk(unsigned char tx[], int len);

	char analizaRx(unsigned char *tx, char txlen);			//checksum
	char analizaRxCrc(unsigned char *tx, char txlen);		// crc

	int	RequestBillID(void);			// Lee la denominacion de los billetes
	int RequestCoinID(void);			// Lee la denominacion de las monedas
	int	RequestScaling(void);			// Lee Scala/Factor
	int	Divisor(unsigned char d);

	int calc_cvf( char cvf[], int fac);

	int CountersToEeprom(void);			// Trasfiere los contadores al EEPROM
	
	int obtiene_pp(int re);			// calcula polling priority 
	int obtiene_is(int re);	// inhibit status

	int obtiene_flag(int re);		// obtiene option flag
	void obtiene_rbce(int re);		// lee buffer de eventos

	void obtiene_coinid(int re);	// identifica la denominacion de la moneda
	void obtiene_billid(int re);	// identifica la denominacion del billete

	void ObtieneDatosStr(int re);		// obtiene la informacion de cualquier respuesta
	long ObtieneDatosNum(int re);		// ontiene datos numericos
	unsigned char ObtieneDato1B(int re);	// obtiene un byte
	unsigned int  ObtieneDato2B(int re);	// numero de 2 bytes

	int SendCmdCCTALK(unsigned char tx[], int len, int timeout);

	int	SendCmdGral(unsigned char cmd);
	int	SendCmd1B(unsigned char cmd, unsigned char dato);

	unsigned short crc_CCITT_A( unsigned short l, unsigned char *p, unsigned short seed );
	unsigned char checksum( unsigned char l, unsigned char *p, unsigned char seed );
};

#define CCTALK_RESET_DEVICE						0x01	// Reset Device
#define CCTALK_BILL_OPERATING_MODE				0x99	// Modify bill operating mode
#define CCTALK_ROUTE_BILL						0x9A	// Route bill
#define CCTALK_REQUEST_SCALING					0x9C	// Request Country Scaling Factor
#define CCTALK_BILL_ID							0x9D	// Request Bill ID
#define CCTALK_BILL_READ_BUFFER_EVENTOS			0x9F	// Read buffered bill events
#define CCTALK_REQUEST_COIN_ID					0xB8	// Request Coin ID
#define CCTALK_REQUEST_BUILD_CODE				0xC0	// Request Build Code
#define CCTALK_REQUEST_FRAUD_COUNTER			0xC1	// Request Fraud counter
#define CCTALK_REQUEST_REJECT_COUNTER			0xC2	// Request Reject counter
#define CCTALK_COUNTERS_TO_EEPROM				0xC6	// Counters to EEPROM
#define CCTALK_REQUEST_OPTION_FLAG				0xD5	// REQUEST OPTION FLAG
#define CCTALK_REQUEST_ACCEPT_COUNTER			0xE1	// Request Accept counter
#define CCTALK_REQUEST_INSERTION_COUNTER		0xE2	// Request Insertion Code
#define CCTALK_REQUEST_MASTER_INHIBIT_STATUS	0xE3	// Request Master inhibit status
#define CCTALK_MODIFY_MASTER_INHIBIT_STATUS		0xE4	// Modify Master inhibit status
#define CCTALK_COIN_READ_BUFFER_EVENTOS			0xE5	// Read Buffer Credits & Errors
#define CCTALK_REQUEST_INHIBIT_STATUS			0xE6	// Request Note Inhibit Channels
#define CCTALK_MODIFY_INHIBIT_STATUS			0xE7	// Set Note Inhibit Channels
#define CCTALK_REQUEST_SOFTWARE_REVISION		0xF1	// Request Software Revision
#define CCTALK_REQUEST_SERIAL_NUMBER			0xF2	// Request Serial Number
#define CCTALK_REQUEST_PRODUCT_CODE				0xF4	// Request Product Code
#define CCTALK_REQUEST_EQUIPMENT_ID				0xF5	// Request Equipment ID
#define CCTALK_REQUEST_MANUFACTURER_ID          0xF6	// Request Manufacturer ID
#define CCTALK_REQUEST_VARIABLE_SET				0xF7	// Request variable set
#define CCTALK_REQUEST_POLLING_PRIORITY			0xF9	// Request Polling Priority
#define CCTALK_SIMPLE_POLL						0xFE	// SIMPLE POLL


#define CCTALK_ERROR_BUSY				0x00		// Busy
#define CCTALK_ERROR_REJECTED			0x01		// Rejected
#define CCTALK_ERROR_RESET				0x04		// Reset
#define CCTALK_ERROR_JAMMED				0x06		// Atorado
#define CCTALK_ERROR_FRAUD				0x06		// Intento de Fraude
#define CCTALK_ERROR_BOX_OUT			0x0B		// Caja Fuera de Posicion
#define CCTALK_ERROR_INTENT_REMOVE		0x0D		// Intento de Remover Billetee
#define CCTALK_ERROR_SENSOR				0x11		// Falla de Sensor\n
#define CCTALK_ERROR_MOTOR				0x13		// Falla de Algun Motor;


#define CCTALK_MASTER_INHIBIT				0		// Master inhibit active Status
#define CCTALK_ERROR_BILL_RETURNED			1		// Bill returned from escrow Status
#define CCTALK_ERROR_INVALID_BILL_1			2		// Invalid bill ( due to validation fail ) Reject
#define CCTALK_ERROR_INVALID_BILL_2			3		// Invalid bill ( due to transport problem ) Reject
#define CCTALK_ERROR_INVALID_BILL_3			4		// Inhibited bill ( on serial ) Status
#define CCTALK_ERROR_INVALID_BILL_4			5		// Inhibited bill ( on DIP switches ) Status
#define CCTALK_ERROR_BILL_JAMMED_TRANS1		6		// Bill jammed in transport ( unsafe mode ) Fatal Error
#define CCTALK_ERROR_BILL_JAMMED_STK		7		// Bill jammed in stacker Fatal Error
#define CCTALK_ERROR_BILL_PULLED			8		// Bill pulled backwards Fraud Attempt
#define CCTALK_ERROR_BILL_TAMPER			9		// Bill tamper Fraud Attempt
#define CCTALK_STACKER_OK					10		// Stacker OK Status
#define CCTALK_ERROR_STACKER_REMOVED		11		// Stacker removed Status
#define CCTALK_ERROR_STACKER_INSERTED		12		// Stacker inserted Status
#define CCTALK_ERROR_STACKER_FAULT			13		// Stacker faulty Fatal Error
#define CCTALK_ERROR_STACKER_FULL			14		// Stacker full Status
#define CCTALK_ERROR_STACKER_JAMMED			15		// Stacker jammed Fatal Error
#define CCTALK_ERROR_BILL_JAMMED_TRANS2		16		// Bill jammed in transport ( safe mode ) Fatal Error
#define CCTALK_ERROR_OPTO_FRAUD				17		// Opto fraud detected Fraud Attempt
#define CCTALK_ERROR_STRING_FRAUD			18		// String fraud detected Fraud Attempt
#define CCTALK_ERROR_ANTI_STRING			19		// Anti-string mechanism faulty Fatal Error
#define CCTALK_ERROR_BARCODE_DETECTED		20		// Barcode detected Status
#define CCTALK_ERROR_UNKNOW_BILL			21		// Unknown bill type stacked Status
#define CCTALK_ERROR_UNKNOW_BILL2			22		// Unknown bill error status


const  char *CCTAK_BILL_ERROR[] =
{
	"Master inhibit active Status",
	"Bill returned from escrow Status",
	"Invalid bill ( due to validation fail ) Reject",
	"Invalid bill ( due to transport problem ) Reject",
	"Inhibited bill ( on serial ) Status",
	"Inhibited bill ( on DIP switches ) Status",
	"Bill jammed in transport ( unsafe mode ) Fatal Error",
	"Bill jammed in stacker Fatal Error",
	"Bill pulled backwards Fraud Attempt",
	"Bill tamper Fraud Attempt",
	"Stacker OK Status",
	"Stacker removed Status",
	"Stacker inserted Status",
	"Stacker faulty Fatal Error",
	"Stacker full Status",
	"Stacker jammed Fatal Error",
	"Bill jammed in transport ( safe mode ) Fatal Error",
	"Opto fraud detected Fraud Attempt",
	"String fraud detected Fraud Attempt",
	"Anti-string mechanism faulty Fatal Error",
	"Barcode detected Status",
	"Unknown bill type stacked Status",
	"Unknown Error Status",	
};




/******************************************************************************/
/* tabla de CRC's para el calculo del CRC-CCITT                               */
/* Polynomial = x^16 + x^12 + x^5 + 1                                         */
/******************************************************************************/
const unsigned short crc_ccitt_lookup[] =
{
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};


#endif /* CCTALK_H */