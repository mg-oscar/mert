/* 
 * File:   kyt_chm29xx.h
 * Author: user
 *
 * Created on March 2, 2023, 2:17 PM
*/

#define	PURSE1		0x0D
#define	PURSE2		0x0E

#ifndef CLASS_CHM29XX_H
#define CLASS_CHM29XX_H

class CLASS_CHM29XX: private tripleDEA, rs232, public libMIA
{
public:
	CLASS_CHM29XX();

	unsigned char dataBuf[100];	// Buffer Datos de la respuesta
	int	sData;					// total de datos en el buffer			

	int totDatResponse;		// Total de Datos en la respuesta
	unsigned int ECode;				// E-Code del Dispensador
	unsigned int Dis_Response;		// Response Positive/Negative del Dispensador
	unsigned int Sam_Response;		// Status/Error Response SAM

	unsigned char ATR[40];	// ATR del SAM
	int sATR;				// longitud del ATR

	
	char Model[40];
	char Version[40];
	unsigned char Stacker;			// Status del Stacker
	unsigned char kty_error[31];	// Banderas de Error
	
	unsigned char SAMUID[8];		// UID SAM

	int CardPresent(BYTE UID[]);	// Read UID de la tarjeta


	
	
	int Identifica(void);			// obtiene datos generales del Dispensador
	int GetCard(void);				// Toma unatarjeta del stacker y la mueve al reader
//	int CardPresent(BYTE UID[]);	// Read UID de la tarjeta
	int EjectCard(void);			// Expulsa completamente la tarjeta	
	int CapturaCard(void);				// captura la tarjeta

	int Read_Sector1A(	unsigned char MFUID[], 
						unsigned char pPASS[16][6], 
						unsigned char readblock[64][16]);
	int ResetSAM1(void);			// Reset SAM1
	int AutentificaSAM(int pKey);	// Autentifica SAM
	int ReadAllMIA(	unsigned char TypeKey,
					unsigned char mfuid[],
					unsigned char LlavesMIA[16][6],
					unsigned char DatosMIA[64][16]);
	int READ_ALL_WITH_KEY(int tKey, BYTE pPASS[16][6], BYTE ReadBlocks[64][16] );
	int ReadAllSector(	int TypeKey, 
						unsigned char pPASS[16][6],
						unsigned char ReadBlocks[64][16] );
	
	
	int GetCardKey( unsigned char mfuid[],
					unsigned char MFKEY[16][6], 
					unsigned char readblock[64][16] );	// Read la llave de ma MIA
	int WriteSectorMIA(	unsigned char TypeKey,
						unsigned char nsector,
						unsigned char mfuid[],
						unsigned char LlavesMIA[16][6],
						unsigned char DatosMIA[64][16]);
	int WriteBlock(int tKey, int block, BYTE pPASS[16][6], BYTE wrblock[64][16] );

	// Dispensador
	int ReadStatusStacker(void);	// Lee el Status del Stacker
	int EjectHold(void);			// Expulsa la tarjeta y al sostiene con el rodillo
	// EX-RF MODULE
	int RF_EnableAntenna(unsigned char nAntenna);
	// Generales
	int InitComm(char tport, char namecomm[], int baud);
	int Close();


	// Dispensador
	int ReadListError(void);		// Obtiene la lista de errores/status

private:
	unsigned char rxBuf[100];	// Trama de la respuesta
	int	rbuf;					// total de datos en el buffer

	unsigned char cmdBuf[100];	// Trama del comando
	int	sbuf;					// total de datos en el buffer

	unsigned char auxBuf[100];	// datos complementarios p/c comando
	int sAux;					// size buffer auxiliar

	unsigned char ndec;			// aux en la decodificacion de la respuesta
	
	unsigned char lastCmd;		// ultimo comando enviado

//	int CardPresent(BYTE UID[]);	// Read UID de la tarjeta

	int ReadModel(void);			// Pide el Modelo del CIM 
	int ReadFirmware(void);			// Read Version Firmware

	int creaTrama(char NoCmd, unsigned char *datos, int tdat);
	int CreaTramaAPDU(unsigned char trama[], unsigned char inst[], int siz);
	int SendCmd(char NoCmd, unsigned char *datos, int tdat, int timeout);
	int send1B(unsigned char by);
	int waitResponse(unsigned char resp, int timeout);
	int waitACK(void);
	int AnalizaRespuesta(unsigned char rx);
	int ObtieneDatosAPDU(void);

	// EX-RF MODULE
	int RF_CardPresent(unsigned char MFUID[]);

	int searchErr(int err);
	int ObtieneErrores(void);
	int Sincroniza(void);

	// DISPENSADOR
	int ReadPosicionMIA(void);

	int RF_WriteBlock(unsigned char nblock, unsigned char datos[]);
	int RF_LoadKey(unsigned char tkey, unsigned char nblock, unsigned char key[]);
	int RF_Autentifica(void);

	int RF_Halt(void);
	int RF_ReadBlock (unsigned char  nblock, unsigned char datos[]);
	int RF_ReadSector(unsigned char nblock, unsigned char datos[]);

	void PrintError(void);
};

// Respuestas
#define KYT_ERROR			-1
#define KYT_POSITIVE		0
#define KYT_OK				0
#define KYT_NEGATIVE		1

// STATUS STACKER
#define STACKER_UNKNOW				0x00		// Stacker sin valor
#define STACKER_GOOD				0x01		// Good		-> Too many cards loading
#define STACKER_WARNING				0x02		// Warning	-> Too few cards loading
#define STACKER_EMPTY				0x03		// Empty	-> No cards in stacker


// TimeOut para c/u de los comandos, en milisegundos
#define TO_GENERAL		100
#define TO_RF_GENERAL	200
#define TO_RESET_SAM	800
#define TO_CAPTURA		3000
#define TM_GETCARD		2000
#define TO_EJECT_DROP	4000
#define TO_EJECT_HOLD	2000

//		Status RF MODULE	Result
#define RF_OK				0x00		// Command operating success
#define RF_ERROR			0x01		// Command operating error
#define RF_CARD_ERROR		0x02		// Card Operating Error
#define RF_TIMEOUT			0x04		// Command time-out
#define RF_CMD_UNDEF		0x10		// Command Undefine


// Antenna
#define	RF_Antenna1			0x01
#define	RF_Antenna2			0x02
#define	RF_Antenna12		0x03

// SYSTEM COMMAND
//		Command						Hex
#define RF_CmdGetVersion			0x10
#define RF_CmdLED1On				0x11
#define RF_CmdLED1Off				0x12
#define RF_CmdLED2On				0x13
#define RF_CmdLED2Off				0x14
#define RF_CmdBuzzer				0x15
#define RF_CmdComSpeed				0x16
#define RF_CmdRFReset				0x20

// ISO-14443A Command
//		Command						Hex
#define RF_CmdModeISO14443A			0x61
#define RF_CmdTypeA_ReqStdToSelect	0x30
#define RF_CmdTypeA_ReqAllToSelect	0x33
#define RF_CmdTypeAAPDU				0x31
#define RF_CmdTypeA_Halt			0x39


// Mifare Command
//		Command						Hex
#define RF_CmdModeISO14443A			0x61
#define RF_CmdTypeA_ReqStdToSelect	0x30
#define RF_CmdTypeA_ReqAllToSelect	0x33
#define RF_CmdTypeA_Halt			0x39
#define RF_CmdAuthenticationA		0x62
#define RF_CmdAuthenticationB		0x63
#define RF_CmdAuthentication2		0x64
#define RF_CmdReadBlock				0x65
#define RF_CmdWriteBlock			0x66
#define RF_CmdIncrementTransfer		0x69
#define RF_CmdDecrementTransfer		0x6a
#define RF_CmdRestore				0x6b
#define RF_CmdReadSector			0x6c
#define RF_CmdTransfer				0x6d
#define RF_CmdRestoreTransfer		0x6e

// Mifare Ultra Light Command
//		Command						Hex
#define RF_CmdModeISO14443A			0x61
#define RF_CmdTypeA_ReqStdToSelect	0x30
#define RF_CmdTypeA_ReqAllToSelect	0x33
#define RF_CmdTypeA_Halt			0x39
#define RF_CmdUltraLightTransive	0x40

// ISO14443 TYPE B Command
//		Command						Hex
#define RF_CmdModeISO14443B			0x22
#define RF_CmdTypeBAPDU				0x41

// ISO15693 Command
//		Command						Hex
#define RF_CmdConfigISO15693ICode2	0x80
#define RF_CmdISO15693Inventory		0x30
#define RF_CmdISO15693Command1		0x82
#define RF_CmdISO15693Command2		0x83


//
// ********************** SOLO PARA DISPENSADOR ********************************
//
#define BC_MODULE					0x01		// BARCODE
#define IC_MODULE					0x02		// IC
#define RF_MODULE					0x03		// RF

// Card Position
#define SEN1						0x01
#define SEN2						0x02
#define SEN3						0x04
#define SEN4						0x08
#define SEN5						0x10
#define SEN6						0x20
#define SEN7						0x40
#define SEN8						0x80


// COMMANDS
// COMMON = STATUS
#define CMD_GetModel						0	// ok
#define CMD_GetFirmware						1	// ok
#define CMD_GetStackerStatus				2	// ok
#define CMD_GetCardPosition					3	//
#define CMD_GetAllSensorStatus				4	//
// COMMON MOVE
#define CMD_MoveCardFromStacker				5	//
#define CMD_CardTakeMoveToEachModule		6	//
#define CMD_CardEjectDrop					7	//
#define CMD_CardEjectHold					8
#define CMD_CardCapture						9
// BARCODE
#define CMD_ReadBarcode						10
#define CMD_MoveAndReadBarcode				11
#define CMD_RetryToReadBarcode				12
// RF CONTROL
#define CMD_ExternalRfControl				13
// IC CARD
#define CMD_ICCardReset						14
#define CMD_ICCardControl					15
// SAM1
#define CMD_SAM1CardReset					16
#define CMD_SAM1CardDirectControl			17
// SAM2
#define CMD_SAM2CardReset					18
#define CMD_SAM2CardDirectControl			19
// SAM3
#define CMD_SAM3CardReset					20
#define CMD_SAM3CardDirectControl			21
// SAM4
#define CMD_SAM4CardReset					22
#define CMD_SAM4CardDirectControl			23

struct KYT_CMD
{
	unsigned char command[3];
} kyt_cmd[25] = 
{
	// COMMON
	// STATUS
	{'C','1','1'},		//  0 Get Model
	{'C','1','2'},		//  1  Get Firmware Version
	{'C','1','3'},		//  2  Get Stacker Status
	{'C','1','6'},		//  3 Get Card Position
	{'C','1','5'},		//  4 Get All Sensor Status
	// MOVE
	{'C','3','1'},		// 5 Move Card From Stacker
	{'C','3','2'},		// 6 Card Move To Each Module
	{'C','3','6'},		// 7 Eject a Card (Drop)
	{'C','3','7'},		// 8 Eject a Card (Hold)
	{'C','3','4'},		// 9 Capture an Error Card
	// BARCODE
	// READ
	{'B','0','1'},		// 10 Read Barcode
	{'B','3','4'},		// 11 Move a Card from Stacker and Read Barcode
	// RETRY
	{'B','0','2'},		// 12 Retry to Read Barcode
	// RF CONTROL
	{'Y','1','0'},		// 13 External RF Module Control
	// IC CARD
	//CONTROL
	{'I','2','1'},		// 14 IC Card Reset
	{'I','2','2'},		// 15 IC Card Direct Control
	// SAM CARD
	// SAM1 CARD CONTROL
	{'I','2','5'},		// 16 SAM1 Reset (Option)
	{'I','2','6'},		// 17 SAM1 Direct Control (Option)
	// SAM2 CARD CONTROL
	{'I','2','8'},		// 18 SAM2 Reset (Option)
	{'I','2','9'},		// 19 SAM2 Direct Control (Option)
	// SAM3 CARD CONTROL
	{'I','3','1'},		// 20 SAM3 Reset (Option)
	{'I','3','2'},		// 21 SAM3 Direct Control (Option)
	// SAM4 CARD CONTROL
	{'I','3','4'},		// 22 SAM4 Reset (Option)
	{'I','5','5'},		// 23 SAM4 Direct Control (Option)
};

struct KYT_STATUS_STACKER
{
	unsigned char status[40];
} kyt_stk_status[4] = 
{
	{"UNKNOW ERROR"},							//  0 Error desconocido
	{"Stacker Good"},							//  1 Con suficientes tarjetas
	{"Warning = few Card in the stacker"},		//  2 Con pocas tarjetas
	{"Stacker Empty = No cards in the stacker"}	//  3 Sin Tarjetas
};


#define KYT_TOTAL_ERRORES		31
struct KYT_ERRORES
{	int	codigo;
	char error[30];
} kyt_errores[KYT_TOTAL_ERRORES] = 
{
	{ 0x2001, "NOT_DEFINE_COMMAND"},			// 0
	{ 0x2003, "COMM_FRAME_ERROR"},				// 1
	{ 0x2004, "CARD_JAM"},						// 2
	{ 0x2005, "NO_CARD"},						// 3
	{ 0x2006, "CARD_PRESENT"},					// 4
	{ 0x200E, "INVALID_CARD_POSITION_ERROR"},	// 5
	{ 0x2105, "STACKER_EMPTY"},					// 6
	{ 0x3500, "BARCODE_READ_ERROR"},			// 7
	{ 0x2301, "RF_COMMUNICATION_ERROR"},		// 8
	{ 0x2204, "IC_CONTACT_ERROR"},				// 9
	{ 0x2205, "IC_CONTROL_ERROR"},				// 10
	{ 0x2214, "IC_MODULE_COMM_ERROR"},			// 11
};

#define KYT_GOOD					0x0000		// Normal Execution

// E-CODE
#define	KYT_NOT_DEFINE_COMMAND				0x00
#define KYT_COMM_FRAME_ERROR				0x01
#define KYT_CARD_JAM						0x02
#define KYT_NO_CARD							0x03
#define KYT_CARD_PRESENT					0x04
#define KYT_INVALID_CARD_POSITION_ERROR		0x05
#define KYT_STACKER_EMPTY					0x06
#define KYT_BARCODE_READ_ERROR				0x07
#define KYT_RF_COMM_ERROR					0x08
#define KYT_IC_CONTACT_ERROR				0x09
#define KYT_IC_CONTROL_ERROR				0x0A
#define KYT_IC_MODULE_COMM_ERROR			0x0B





#define SOH		0x01		// Start of Header
#define STX		0x02		// Start of Text
#define ETX		0x03		// End of Text
#define ENQ		0x05		// Enquiry
#define ACK		0x06		// Positive Acknowledge
#define NAK		0x15		// Negative Acknowledge
#define CAN		0x18		// Cancel

// ************************Command *********************************************
//SOH Null Length STX CMD DATA ETX BCC
//1B   1B   2B	  1B  3B  nB   1B  1B

//Positive Response Frame Format
// SOH Null Length STX CMD GOOD 0x01 DATA ETX BCC
// 1B  1B   2B     1B  3B  2B   1B   NB   1B  1B

//Negative Response Frame Format
//SOH Null Length STX CMD E-Code 0x00 ETX BCC
//1 byte 1 byte 2 byte 1 byte 3 byte 2 byte 1 byte 1 byte 1 byt

// BCC -> todos los datos entre NULL y ETX inclusive

//Null Reserved:	Always 0x00.
//Length:			Data Length from the CMD to DATA.
//CMD				Instruction Code (3 Bytes)
//GOOD				Normal Execution : 0x0000 (2 Bytes)
//E-Code			Command Failed: Refer to “Error Code” (2 Bytes)
//BCC				Check Sum.

#endif /* KYT_CHM-29XX_H */

