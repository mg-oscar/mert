/* 
 * File:   CRT603vx.hpp
 * Author: Elias Gonzalez / FAMISA
 *
 * Created on February 13, 2023, 7:29 PM
 * Modified on Octubre 6, 2024 10:53 AM
 */

#ifndef CRT603VX_HPP
#define CRT603VX_HPP

/*#include <stdbool.h>
#include "usb.h"
#include "rs232.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdarg.h>

#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <ctype.h>
*/

const speed_t CRT_BAUD[] = { 9600, 19200, 38400,115200 };

// Comandos CRT603
#define CRT_CheckCardStatus					0x00	// Check Card Status
#define CRT_RESET							0x01	// reset
#define CRT_AutoTesting_RF					0x02	// Auto‐testing RF card type
#define CRT_GetFirmware						0x03	// Getting firmware version
#define CRT_SetBaud							0x10	// Baud rate setting
#define CRT_WorkStatus						0x11	// Module working status
#define CRT_SetLed							0x13	// LED Indicator setting
#define CRT_Buzzer							0x14	// Buzzer setting
#define CRT_SeekCard						0x20	// Seek card
#define CRT_ReadBlock						0x21	// Read block
#define CRT_ReadSector						0x29	// Read sector ( 4 blocks )
#define CRT_WriteBlock						0x22	// Write block
#define CRT_ValueInit						0x23	// Value initialization
#define CRT_ValueRead						0x24	// Read value
#define CRT_ValueIncrement					0x25	// Increment operation
#define CRT_ValueDecrement					0x26	// Decrement operation
#define CRT_ValueBackup						0x27	// Value backup
#define CRT_ValueTransmitting				0x47	//Value transmitting
#define CRT_MifareHalt						0x28	// Mifare card halt
#define CRT_LoadKey							0x2D	// Download secret key from module
#define CRT_RdEEPROM						0x15	// EEPROM reading
#define CRT_WrEEPROM						0x16	// EEPROM writing
#define CRT_RESET_ISOA						0x30	// ISO14443‐4 TYPE A card reset
#define CRT_SendAPDU_ISOA					0x31	// Send APDU to ISO14443‐4 card
#define CRT_ObtAPDU							0xF0	// Obtain APDU 0x31 Command remains
#define CRT_DetectCardMoving				0xF1	// Detecting of RF card moving in and moving out
#define CRT_RdUltraLight					0x41	// UltraLight reading
#define CRT_WrUltraLight					0x42	// UltraLight writing
#define CRT_RdModeSetting					0x70	// Card reading mode setting
#define CRT_RESET_ISOB						0x60	// ISO14443‐4 TYPE B card reset
#define CRT_CardHaltISOB					0x62	// ISO14443‐4 TYPEA,TYPE B card halt
#define CRT_ResetSAM1						0x51	// SAM card 1 reset
#define CRT_ResetSAM2						0x52	// SAM card 2 reset
#define CRT_SendAPDU_SAM1					0x53	// Send APDU to SAM card 1
#define CRT_SendAPDU_SAM2					0x54	// Send APDU to SAM card 2
#define CRT_SeekReset_ISO_AB				0x68	// Seek and reset ISO14443‐4 card，both type A and B


/*
1 No card
2 CRC parity error
3 Data overflow error
4 validate negative
5 Card CRC parity error
6 Card BCC parity error
8 Card serial number error
9 Card data length byte error
10 Card doesn’t pass the checking
11 Receive error digit from the card
12 Receive error byte from the card
13 Recover value error
14 Transmitting value error
15 Writing data error
16 Increment error
17 Decrement error
18 Reading data error
19 Downloading key error
20 Frame error
21 Polling card error
22 Card selected error
23 Gain card UID error
24 Value initialization error
25 Reading & writing value error
26 Polling RF CPU card failure
27 RF CPU card deactivating error
28 Command error
*/



//#define BYTE unsigned char
//#define DWORD unsigned long

#define ENQ  0x05
#define ACK  0x06
#define NAK  0x15
#define EOT  0x04
#define CAN  0x18
#define STX  0x02
#define ETX  0x03
#define US   0x1F


#define CRT603_ERRCOUNT					-200
#define CRT603_ERR_CMD					(CRT_ERRCOUNT-1) 
#define CRT603_ERR_CMDPARAM				(CRT_ERRCOUNT-2) 
#define CRT603_ERR_CMDDENIAL			(CRT_ERRCOUNT-3) 
#define CRT603_ERR_DEVNOTSUP			(CRT_ERRCOUNT-4) 
#define CRT603_ERR_CMDDATA				(CRT_ERRCOUNT-5) 
#define CRT603_ERR_LOCKCARD				(CRT_ERRCOUNT-11) 
#define CRT603_ERR_EEPROM				(CRT_ERRCOUNT-15) 
#define CRT603_ERR_TRACKCHECK			(CRT_ERRCOUNT-20) 
#define CRT603_ERR_READTRACK			(CRT_ERRCOUNT-21) 
#define CRT603_ERR_POWERDOWN			(CRT_ERRCOUNT-30) 
#define CRT603_ERR_ICPROCESS			(CRT_ERRCOUNT-41) 
#define CRT603_ERR_ICPOWERSHORT			(CRT_ERRCOUNT-60) 
#define CRT603_ERR_ICACTIVATIONFAIL		(CRT_ERRCOUNT-61) 
#define CRT603_ERR_ICDENIAL				(CRT_ERRCOUNT-62) 
#define CRT603_ERR_ICNOTRESPOND			(CRT_ERRCOUNT-63) 
#define CRT603_ERR_ICNOTRESPOND_OTHER	(CRT_ERRCOUNT-64) 
#define CRT603_ERR_ICNOTACTIVATION		(CRT_ERRCOUNT-65) 
#define CRT603_ERR_ICNOTSUP				(CRT_ERRCOUNT-66) 
#define CRT603_ERR_ICEMV				(CRT_ERRCOUNT-66) 
#define CRT603_ERR_WRITEDATA			(CRT_ERRCOUNT-70) 
#define CRT603_ERR_RECVACK				(CRT_ERRCOUNT-71) 
#define CRT603_ERR_SENDACK				(CRT_ERRCOUNT-72) 
#define CRT603_ERR_READDATA				(CRT_ERRCOUNT-73) 
#define CRT603_ERR_COMMTIMEOUT			(CRT_ERRCOUNT-80) 
#define CRT603_ERR_CANCEL				(CRT_ERRCOUNT-90) 
#define CRT603_ERR_NOCARDPROCESS		(CRT_ERRCOUNT-97) 
#define CRT603_ERR_PROCESSFAILD			(CRT_ERRCOUNT-98) 
#define CRT603_ERR_UNKNOWN				(CRT_ERRCOUNT-99) 

#define CRT_OPEN_TYPEUSB				0
#define CRT_OPEN_TYPERS232				1 


//#define CRT_CARDST_NOCARD             0 
#define CRT_CARDST_INSIDE             3 
#define CRT_CARDST_UNKNOW             9 

#define LOG_PATH "/home/"
#define MAX_LOG_SIZE 1024

static int g_iMode = 0; 
static bool g_bOpen = false;

#define Slot_1 				1
#define Slot_2 				2
#define Slot_3 				3
#define Slot_4 				4

#define Key_Type_A			1
#define Key_Type_B			2

//using std::cout;
//using std::endl;

typedef struct
{
	char			FirmwareVersion[10];
	int				Error;
	unsigned char	Data[100];
	int				DataLen;
	unsigned char	CSN[8];
	int				CSNLen;
	char			ReaderVersion[100];

}CRT_STRUCT;


class CLASS_CRT603vx: public usbdev, rs232_603, tripleDEA
{

public:
	int argc;
	char** argv;

	unsigned char MF_UID[6];

	BYTE MIA_DATA[64][16];
	unsigned short int MF_Response;
	BYTE MF_Data[100];


	
	
	char mensaje[300]; 
	char SerialComName[30];
	int serial_fp;
	int j;
	unsigned char Key[6];

	CRT_STRUCT ReaderInfo;

	unsigned char bySendData[1024] = {0};
	unsigned char byRecvData[1024] = {0};
	int iSendLen;
	int iRecvLen = 0;
	int iRet = 0;

	CLASS_CRT603vx();

	int InitComm(char *ComName, speed_t speed);
	int MF_RdBlock (byte iblk, byte tkey, byte *pass);
	
	int MF_RdSector (byte isec, byte tkey, byte *pass, byte readblock[64][16] );

	int READ_ALL_WITH_KEY( int tkey,  unsigned char UID[], unsigned char pPASS[16][6], unsigned char ReadBlocks[64][16] );

	int MF_WrBlock (byte iblk, byte tkey, byte *pass, byte *buf);
	void GetResponse(BYTE *recB, int sizB);
	int GetSamInterface(int pSlot);
	int SamTransmit(unsigned char *pAPDU, int pAPDULen, int pSlot);
	int SamTransmitE(unsigned char *pAPDU, int pAPDULen, int pSlot);
	int AutentificaSAM(BYTE slot, BYTE keytype);

	int GetLastKey(BYTE slot);
	int GetCardKey(BYTE slot,  unsigned char mfuid[], unsigned char MFKEY[16][6], unsigned char readblock[64][16] );
	

	int ReaderClose(int argc, char** argv);    
	int ReaderBuzzer(int argc, char** argv,int pTime,int pFrecuency,int pDutyCycle);
	int ReaderConnect(void);

	
	
//	int MF_RdSector (byte nsec, byte tkey, byte *pass);


	void printRec(BYTE *recB, int sizB, const char *text);
	void printAllMIA(void);
	void printAllKey(BYTE key[16][6]);

	void printBuf(BYTE *recB, int sizB, const char *text);
	

	
	
	unsigned char Challenge[8];
	int ReaderInitialization(int argc, char** argv, speed_t speed);    


	int MifareGetUID(unsigned char *UID);
	int MifareLoginA(int argc, char** argv,int pSector,int pBlock, unsigned char * pMifareKey);
	int MifareLoginB(int argc, char** argv,int pSector,int pBlock, unsigned char * pMifareKey);    
	int MifareWrite(int argc, char** argv,int pSector,int pBlock, unsigned char * pDataToWrite,int iKeyType,unsigned char byKeyData[]);   
	int MifareRead(int pSector, int pBlock,int iKeyType, unsigned char * pMifareKey);;    

	int MifareLoadKey(int argc, char** argv,int iKeyId, int iKeyLen, unsigned char byKeyData[]);    

	int MifareCopyValue(int argc, char** argv,int pSector, int pSourceValueBlock, int pTargetValueBlock,int iKeyType, unsigned char byKeyData[]);
	int MifareDecrementValue(int argc, char** argv,int pSector, int pValueBlock, unsigned long pValue,int iKeyType,  unsigned char byKeyData[]);
	int MifareIncrementValue(int argc, char** argv,int pSector, int pValueBlock, unsigned long pValue,int iKeyType,  unsigned char byKeyData[]);
	int MifareReadSector(int iKeyType, int iMode, int iSector, int iKeyLen, unsigned char byKeyData[]);

	int SetBaudRate(speed_t baud);
	int SetWorkMode(bool bWire, bool bAutoFindCard);
	int SetLed(bool bStatus);


    int CRT_Write(unsigned int length,unsigned char buffer[]);
    int CRT_Read(unsigned int length,unsigned char buffer[]);

    int Recv_Data(int *iRecvDataLen, unsigned char byRecvData[]);
    int Recv_Data_RS232(int *iRecvDataLen, unsigned char byRecvData[]);
    int Recv_Data_USB(int *iRecvDataLen, unsigned char byRecvData[]);
    int OrganizePacket(int iSendDataLen, unsigned char bySendData[], int* iOutPacketLen, unsigned char byOutPacketData[]);
    int AnalysisPacket(int *iRecvDataLen, unsigned char byRecvData[]);
    int CRT_ExeCommand(int iSendDataLen, unsigned char bySendData[], int* iRecvDataLen, unsigned char byRecvData[]);
    void MyAsc2Bcd(const unsigned char ucInData[], int iInLen, unsigned char ucOutBcd[]);
    void bcd2asc(int iInLen, const unsigned char ucInData[], unsigned char ucOutAsc[]);
    bool IsAscData(const unsigned char ucInData[], int iInLen);
    unsigned char CalXOR(unsigned char strOrder[],unsigned int strLen);

	void PrintUID(void);
	
    private:
};
#endif /* CRT603VX_HPP */

