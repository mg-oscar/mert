/* 
 * File:   kyt_cim6903.h
 * Author: user
 *
 * Created on March 2, 2023, 2:17 PM
*/

#define	PURSE1		0x0D
#define	PURSE2		0x0E


#ifndef CLASS_CIM6903_H
#define CLASS_CIM6903_H

class CLASS_CIM6903: private tripleDEA, rs232
{
public:
	CLASS_CIM6903();

	unsigned char cmdBuf[100];	// Trama del comando
	int	sbuf;					// total de datos en el buffer			

	unsigned char rxBuf[100];	// Trama de la respuesta
	int	rbuf;					// total de datos en el buffer

	unsigned char auxBuf[100];	// datos complementarios p/c comando

	unsigned char ndec;		// aux en la decodificacion de la respuesta

	char Model[40];
	char Version[40];
	unsigned char Stacker[2];		// Status del Stacker
	unsigned char kty_error[31];	// Banderas de Error

	int READ_ALL_WITH_KEY( int TypeKey, BYTE pPASS[16][6], BYTE ReadBlocks[64][16] );
	int READ_ALL( int TypeKey,  BYTE UID[], BYTE pPASS[16][6], BYTE ReadBlocks[64][16] );
	int Read_Sector1A(BYTE MFUID[], BYTE pPASS[16][6], BYTE readblock[64][16]);
	int WriteBlock(int tKey, int block, BYTE pPASS[16][6], BYTE wrblock[64][16] );

	int ReadSector(int TypeKey, BYTE nSector, BYTE pPASS[], BYTE readblock[]);
	int Autentifica(int TypeKey, int nblock, BYTE MFUID[], BYTE pPASS[]);

	// Instrucciones Dispensador
	int MoveMiaToReader(BYTE stk);
	int CardPresent(BYTE UID[]);


	// Dispensador
	int Identifica(void);			// obtiene datos generales del Dispensador
	int CardMoveFromStack(char stk);// Mueve del stacker 1 o 2 al Reader
	int CardTakeMoveToRF(void);		// Toma la tarjeta del frente y al mueve al RF
	int EjectHold(void);			// Expulsa la tarjeta y al sostiene con el rodillo
	int EjectDrop(void);			// Expulsa completamente la tarjeta
	int Captura(void);				// captura la tarjeta
	int ReadModel(void);			// Pide el Modelo del CIM 
	int ReadFirmware(void);			// Read Version Firmware
	int ReadStatusStacker(void);	// Lee el Status de los 2 Stackers
	int ReadListError(void);		// Obtiene la lista de errores/status


	// EX-RF MODULE
	int RF_EnableAntenna(unsigned char nAntenna);
	int RF_WriteBlock(unsigned char nblock, unsigned char datos[]);


	// Generales
	int InitComm(char tport, char namecomm[], int baud);
	int Close();

private:
	int creaTrama(char NoCmd, unsigned char *datos, int tdat);
	int SendCmd(char NoCmd, unsigned char *datos, int tdat);
	int send1B(unsigned char by);
	int waitResponse(unsigned char resp);
	int waitACK(void);
	int AnalizaRespuesta(unsigned char rx);
	int searchErr(int err);
	int ObtieneErrores(void);

	int Sincroniza(void);

	
	// DISPENSADOR
	int ReadPosicionMIA(void);

	
	// EX-RF MODULE
	int RF_LoadKey(unsigned char tkey, unsigned char nblock, unsigned char key[]);
	int RF_Autentifica(void);
	int RF_CardPresent(unsigned char MFUID[]);

	int RF_Halt(void);
	int RF_ReadBlock (unsigned char  nblock, unsigned char datos[]);
	int RF_ReadSector(unsigned char nblock, unsigned char datos[]);
	int RF_Buzzer(unsigned char tm);

	// Monedero
	int RF_PurseIncrementTransfer(int TypeKey, BYTE MFUID[], int PurseBlock, BYTE pPASS[16][6], int valor);
	int RF_PurseDecrementTransfer(int TypeKey, BYTE MFUID[], int PurseBlock, BYTE pPASS[16][6], int valor);
	int RF_PurseRestore(int TypeKey, int PurseBlock, BYTE pPASS[6], int valor);
	int RF_PurseTransfer(int TypeKey, BYTE MFUID[], int PurseBlock, BYTE pPASS[16][6], int valor);
	int RF_PurseRestoreTransfer(int TypeKey, BYTE MFUID[], int PurseBlock, BYTE pPASS[16][6], int valor);
	
	
	
	void PrintError(void);
};

// Respuestas
#define KYT_ERROR			-1
#define KYT_POSITIVE		0
#define KYT_OK				0
#define KYT_NEGATIVE		1

//
// **********************  SOLO PARA EL MODULO DE RF ***************************
//

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

#define STACKER1					0x01
#define STACKER2					0x02

//#define MAGAZINE1					0x00
//#define MAGAZINE2					0x01


#define MG_MODULE					0x01		// Magnetic
#define IC_MODULE					0x02		// IC
#define RF_MODULE					0x03		// RF


// STATUS STACKER
#define STACKER_ERROR				0x00		// Stacker sin valor
#define STACKER_GOOD				0x01		// Good		-> Too many cards loading
#define STACKER_WARNING				0x02		// Warning	-> Too few cards loading
#define STACKER_EMPTY				0x03		// Empty	-> No cards in stacker

// Card Position
#define SEN1						0x01
#define SEN2						0x02
#define SEN3						0x04
#define SEN4						0x08
#define SEN5						0x10
#define SEN6						0x20
#define SEN7						0x40
#define SEN8						0x80


#define CMD_GetModel						0
#define CMD_GetFirmware						1
#define CMD_GetStacker						2
#define CMD_GetStatus						3
#define CMD_GetError						4
#define CMD_GetCardPosition					5
#define CMD_SetRTC							6
#define CMD_SetRetryCount					7
#define CMD_BuzzControl						8
#define CMD_SetBaud							9
#define CMD_CardMoveFromStack				10
#define CMD_CardTakeMoveTo					11
#define CMD_CardCapture						12
#define CMD_CardEjectDrop					13
#define CMD_CardEjectHold					14
#define CMD_SoftwareReset					15

// Comandos NO IMPLEMENTADOS EN VERSION EX-RF
#define CMD_MC_Read							16
#define CMD_MC_WriteVerify					17
#define CMD_MC_WriteStacker					18
#define CMD_MC_ReadAll						19
#define CMD_MC_HeaderCleaning				20
#define CMD_ICCardReset						21
#define CMD_ICCardDirectControl				22
#define CMD_SelectsICorSam					23
#define CMD_SAM1Reset						24
#define CMD_SAM1DirectControl				25
#define CMD_SAM1Deactivation				26
#define CMD_SAM2Reset						27
#define CMD_SAM2DirectControl				28
#define CMD_SAM2Deactivation				29
#define CMD_SAM3Reset						30
#define CMD_SAM3DirectControl				31
#define CMD_SAM3Deactivation				32
#define CMD_SAM4Reset						33
#define CMD_SAM4DirectControl				34
#define CMD_SAM4Deactivation				35
#define CMD_RFCardReadinBlockRange			36
#define CMD_RFCardWriteinBlockRange			37
#define CMD_ReadRFCardDataSector			38
#define CMD_WriteRFCardDataSector			39
#define CMD_RFCardReadinBlockDirectKey		40
#define CMD_RFCardWriteinBlockDirectKey		41
#define CMD_ReadRFCardDataDirectKey			42
#define CMD_WriteRFCardDataDirectKey		43
#define CMD_IncreasesBalance				44
#define CMD_DecreasesBalance				45
#define CMD_IncreasesBalanceDirectKey		46
#define CMD_DecreasesBalanceDirectKeySECRET	47
#define CMD_ChangeSecretKey					48
#define CMD_ChangeSecretKeyAllSameKey		49
#define CMD_SelectsSecretKeyIndex			50
#define CMD_ChangeRFCardSecretKey			51
#define CMD_ChangeRFCardSecretKeyDirectKey	52
#define CMD_KeySetChangeSecretKey			53
#define CMD_KeySetChangeSecretKeyAllSameKey	54
#define CMD_CheckRFCard						55
#define CMD_ReadsDataMifareUltraLight		56
#define CMD_WritesDataMifareUltraLight		57
#define CMD_ReadsUIDMifareUltra				58
#define CMD_ExternalRFModuleControl			59


struct KYT_CMD
{
	unsigned char command[3];
} kyt_cmd[60] = 
{
	{'C','1','1'},		//  0 Get Model
	{'C','1','2'},		//  1  Get Firmware Version
	{'C','1','3'},		//  2  Get Stacker
	{'C','1','4'},		//  3 Get Status List
	{'C','1','5'},		//  4 Get Error List
	{'C','1','6'},		//  5 Get Card Position
	{'C','2','1'},		//  6 Set RTC IC Check
	{'C','2','4'},		//  7 Set Retry Count Check
	{'C','2','5'},		//  8 Set Buzz On/Off Cont. Check
	{'C','2','6'},		//  9 Set Baud Rate Check
	{'C','3','1'},		// 10 Card Move From Stacker
	{'C','3','2'},		// 11 Card Move To ...
	{'C','3','4'},		// 12 Card Capture Backward
	{'C','3','6'},		// 13 Card Eject (Drop Mode) Forward
	{'C','3','7'},		// 14 Card Eject (Hold Mode) Forward
	{'C','4','2'},		// 15 Software Reset
	{'M','3','1'},		// 16 Magnetic Card Read
	{'M','3','3'},		// 17 Magnetic Card Write Verify**
	{'M','3','4'},		// 18 Magnetic Card Write From Stacker Verify**
	{'M','3','5'},		// 19 Magnetic Card All Track Read
	{'M','5','1'},		// 20 MSRW Header Cleaning
	{'I','2','1'},		// 21 IC Card Reset
	{'I','2','2'},		// 22 IC Card Direct Control
	{'I','2','3'},		// 23 Selects IC card or Sam card
	{'I','2','5'},		// 24 SAM1 Reset (Option)
	{'I','2','6'},		// 25 SAM1 Direct Control (Option)
	{'I','2','7'},		// 26 SAM1 Deactivation (Option)
	{'I','2','8'},		// 27 SAM2 Reset (Option)
	{'I','2','9'},		// 28 SAM2 Direct Control (Option)
	{'I','3','0'},		// 29 SAM2 Deactivation (Option)
	{'I','3','1'},		// 30 SAM3 Reset (Option)
	{'I','3','2'},		// 31 SAM3 Direct Control (Option)
	{'I','3','3'},		// 32 SAM3 Deactivation (Option)
	{'I','3','4'},		// 33 SAM4 Reset (Option)
	{'I','3','5'},		// 34 SAM4 Direct Control (Option)
	{'I','3','6'},		// 35 SAM4 Deactivation (Option)
	{'R','3','1'},		// 36 RF Card Read in Block Range
	{'R','3','2'},		// 37 RF Card Write in Block Range
	{'R','3','6'},		// 38 Read RF card data in sector range
	{'R','3','7'},		// 39 Write RF card data in sector range
	{'R','2','3'},		// 40 RF Card Read in Block Range with Direct Key
	{'R','2','4'},		// 41 RF Card Write in Block Range with Direct Key
	{'R','2','6'},		// 42 Read RF card data in sector range with Direct Key
	{'R','2','7'},		// 43 Write RF card data in sector range with Direct Key
	{'R','4','1'},		// 44 Increases balance in RF card
	{'R','4','2'},		// 45 Decreases balance in RF card
	{'R','2','8'},		// 46 Increases balance in RF card with Direct Key
	{'R','2','9'},		// 47 Decreases balance in RF card with Direct Key
	{'R','5','1'},		// 48 Change ‘Secret Key'} to other Key
	{'R','5','2'},		// 49 Change ‘Secret Key'} to all the same Key value
	{'R','5','3'},		// 50 Selects ‘Secret Key Index'}
	{'R','5','4'},		// 51 Change RF Card ‘Secret Key'} to other key
	{'R','2','5'},		// 52 Change RF Card ‘Secret Key'} to other key with Direct Key
	{'R','5','5'},		// 53 Key Set and Change ‘Secret Key'} to other Key
	{'R','5','6'},		// 54 Key Set and Change ‘Secret Key'} to all the same Key value
	{'R','6','1'},		// 55 Check RF card in antenna area
	{'U','3','1'},		// 56 Reads data on Mifare Ultra Light card.
	{'U','3','2'},		// 57 Writes data on Mifare Ultra Light card.
	{'U','4','1'},		// 58 Reads UID (Serial Number) on Mifare Ultra Light card.
	{'Y','1','0'}		// 59 External RF Module
};


#define KYT_TOTAL_ERRORES		31
struct KYT_ERRORES
{	int	codigo;
	char error[30];
} kyt_errores[KYT_TOTAL_ERRORES] = 
{	{ 0x2001, "NOT_DEFINE_COMMAND"},			// 0
	{ 0x2002, "NOT_USE_COMMAND"},				// 1
	{ 0x2003, "COMM_FRAME_ERROR"},				// 2
	{ 0x2004, "CARD_JAM"},						// 3
	{ 0x2005, "NO_CARD"},						// 4
	{ 0x2006, "CARD_PRESENT"},					// 5
	{ 0x2007, "BUSY"},							// 6
	{ 0x2100, "DISPENSER_ERROR"},				// 7
	{ 0x2101, "DISPENSER_COMM_ERROR"},			// 8
	{ 0x2102, "STACKER1_ERROR"},				// 9
	{ 0x2103, "STACKER2_ERROR"},				// 10
	{ 0x2104, "ALL_EMPTY"},						// 11
	{ 0x2105, "STACKER1_EMPTY"},				// 12
	{ 0x2106, "STACKER2_EMPTY"},				// 13
	{ 0x2107, "STACKER1_WARNING"},				// 14
	{ 0x2108, "STACKER2_WARNING"},				// 15
	{ 0x2109, "ERROR_BIN_FULL"},				// 16
	{ 0x2200, "MSRW_ERROR"},					// 17
	{ 0x2201, "MSRW_COMM_ERROR"},				// 18
	{ 0x2202, "MSRW_WRITE_ERROR"},				// 19
	{ 0x2203, "MSRW_READ_ERROR"},				// 20
	{ 0x2204, "IC_CONTACT_ERROR"},				// 21
	{ 0x2205, "IC_CONTROL_ERROR"},				// 22
	{ 0x2300, "RF_ERROR"},						// 23
	{ 0x2301, "RF_COMM_ERROR"},					// 24
	{ 0x2302, "RF_AUTHEN_ERROR"},				// 25
	{ 0x2303, "RF_WRITE_ERROR"},				// 26
	{ 0x2304, "RF_READ_ERROR"},					// 27
	{ 0x2305, "RF_DETECT_ERROR"},				// 28
	{ 0x2306, "RF_AMOUNT_ERROR"},				// 29
	{ 0x2703, "SAM_COMM_ERROR"}					// 30
};

#define SOH		0x01		// Start of Header
#define STX		0x02		// Start of Text
#define ETX		0x03		// End of Text
#define ENQ		0x05		// Enquiry
#define ACK		0x06		// Positive Acknowledge
#define NAK		0x15		// Negative Acknowledge
#define CAN		0x18		// Cancel



#define KYT_GOOD					0x0000		// Normal Execution

#define	KYT_NOT_DEFINE_COMMAND		0x00
#define KYT_NOT_USE_COMMAND			0x01
#define KYT_COMM_FRAME_ERROR		0x02
#define KYT_CARD_JAM				0x03
#define KYT_NO_CARD					0x04
#define KYT_CARD_PRESENT			0x05
#define KYT_BUSY					0x06
#define KYT_DISPENSER_ERROR			0x07
#define KYT_DISPENSER_COMM_ERROR	0x08
#define KYT_STACKER1_ERROR			0x09
#define KYT_STACKER2_ERROR			0x0A
#define KYT_ALL_EMPTY				0x0B
#define KYT_STACKER1_EMPTY			0x0C
#define KYT_STACKER2_EMPTY			0x0D
#define KYT_STACKER1_WARNING		0x0E
#define KYT_STACKER2_WARNING		0x0F
#define KYT_ERROR_BIN_FULL			0x10
#define KYT_MSRW_ERROR				0x11
#define KYT_MSRW_COMM_ERROR			0x12
#define KYT_MSRW_WRITE_ERROR		0x13
#define KYT_MSRW_READ_ERROR			0x14
#define KYT_IC_CONTACT_ERROR		0x15
#define KYT_IC_CONTROL_ERROR		0x16
#define KYT_RF_ERROR				0x17
#define KYT_RF_COMM_ERROR			0x18
#define KYT_RF_AUTHEN_ERROR			0x19
#define KYT_RF_WRITE_ERROR			0x1A
#define KYT_RF_READ_ERROR			0x1B
#define KYT_RF_DETECT_ERROR			0x1C
#define KYT_RF_AMOUNT_ERROR			0x1D
#define KYT_SAM_COMM_ERROR			0x1F



//****************************** BUZZER ****************************************
// <Mode>
// Code		Mode				Detail
// 0x01		‘Setting Mode’		Set ‘Buzz Control’
// 0x02		‘Check Mode’		Check ‘Buzz Control’
//
// <Buzz Status>
// Code		Setting		Detail Note
// 0x01		Buzz Off	Buzz Off
// 0x02		Buzz On		Buzz On Default

// **********************Retry Count *******************************************
//<Mode>
//Code	Mode Detail
//0x01	‘Setting Mode’	Set ‘Retry Count’
//0x02	‘Check Mode’	Check ‘Retry Count’

//<Retry Count>
// Code Setting									Detail Note
// 0x00 NON Do not retry
// 0x01 Once Execute the instruction again.
// 0x02 Twice Retry it twice
// 0x03 Three times Retry it three times		Default

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


/****************************** ERRORES ****************************************
#define KYT_GOOD					0x0000		// Normal Execution

#define	KYT_NOT_DEFINE_COMMAND		0x2001
//Description : Using the command that does not defined in this model.
//Action : Use the valid command in this model.

#define KYT_NOT_USE_COMMAND			0x2002
//Description : Not available command in this model.
//Action : Use the valid command in this model.

#define KYT_COMM_FRAME_ERROR		0x2003
//Description : Sending the command that has the invalid communication frame.
//Action : Check the data format and the corresponding module specification.

#define KYT_CARD_JAM				0x2004
//Description : When the card is jammed.
//Action : Remove the jammed card.

#define KYT_NO_CARD					0x2005
//Description : No cards.
//Action : Insert the card.

#define KYT_CARD_PRESENT			0x2006
//Description : When the card exists already in the terminal.
//Action : Eject the card.

#define KYT_BUSY					0x2007
//Description : When the terminal is running or busy.
//Action : Wait until the previous operation is completed.

#define KYT_DISPENSER_ERROR			0x2100
//Desctiption : Not Applicable Dispenser.
//Action : Reset the terminal and exchange the dispenser..

#define KYT_DISPENSER_COMM_ERROR	0x2101
//Description : Dispenser communication error
//Action : Check the communication line and reset the terminal.

#define KYT_STACKER1_ERROR			0x2102
// Description : The first STACKER ERROR
// Action : Be sure that the card is loaded at the first stacker.

#define KYT_STACKER2_ERROR			0x2103
// Description: The second STACKER ERROR
// Action : Be sure that the card is loaded at the second stacker.

#define KYT_ALL_EMPTY				0x2104
// Description : No cards at both the first and second stacker.
// Action : Load the card in the stacker.

#define KYT_STACKER1_EMPTY			0x2105
// Description : No card at the first stacker.
// Action : Load the card at the first stacker.

#define KYT_STACKER2_EMPTY			0x2106
// Description : No card at the second stacker.
// Action : Load the card at the second stacker.
#define KYT_STACKER1_WARNING		0x2107
// Description : Too few cards in the first stacker.
// Action : Load the card at the first stacker.

#define KYT_STACKER2_WARNING		0x2108
//Description : Too few cards in the second stacker.
//Action : Load the card at the second stacker.

#define KYT_ERROR_BIN_FULL			0x2109
//Description : Too many cards in the ‘CAPTURE BOX’.
//Action : Keep the capture box empty.

#define KYT_MSRW_ERROR				0x2200
// Description : The MS Reader/Writer that cannot use in this model.
// Action : Change the MS Reader/Writer.

#define KYT_MSRW_COMM_ERROR			0x2201
// Description : The MS Reader/Writer communication error.
// Action : Check the communication line and reset the terminal.

#define KYT_MSRW_WRITE_ERROR		0x2202
// Description : Error when the MS Reader/Writer is writing on the card.
// Action : Clean the header and check the card.

#define KYT_MSRW_READ_ERROR			0x2203
// Description : Error when the MS Reader/Writer is reading on the card.
// Action : Clean the header and check the card.

#define KYT_IC_CONTACT_ERROR		0x2204
// Description : Error while the terminal contacts the IC card.
// Action : Be sure that the current card is an IC card.

#define KYT_IC_CONTROL_ERROR		0x2205
//Description : Error while the terminal executes the IC card command.
//Action : Check if the command is able to use in the contacted card.

#define KYT_RF_ERROR				0x2300
// Description : Unavailable RF module.
// Action : Change the RF MODULE

#define KYT_RF_COMM_ERROR			0x2301
// Description : Communication error at the RF Module.
// Action : Check the connection socket

#define KYT_RF_AUTHEN_ERROR			0x2302
//Description : Authentication Error at the RF Module.
//Action : Change the ‘SECRET KEY’

#define KYT_RF_WRITE_ERROR			0x2303
//Description : Error while the terminal writes at the RF Card.
//Action : Be sure that the card exists in the detection range.

#define KYT_RF_READ_ERROR			0x2304
// Description : Error while the terminal reads at the RF Card.
// Action: Be sure that the card exists in the detection range.

#define KYT_RF_DETECT_ERROR			0x2305
// Description : No RF Card.
// Action : Insert the RF Card into the terminal.

#define KYT_RF_AMOUNT_ERROR			0x2306
// Description : Error while the terminal increases(or decreases) the balance at the RF card.
//Action : Tune the RF module.

#define KYT_SAM_COMM_ERROR			0x2703
//Description : Communication error at the SAM Module.
//Action : Check the communication line and reset the terminal.
*/
#endif /* KYT_CIM6903_H */

