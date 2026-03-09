/*
 * File:   class_civintec.h
 * Author: FAMISA
 *
 * Created on October 3, 2022, 4:16 PM
 */

#include <PCSC/wintypes.h>
#include <PCSC/winscard.h>
#include <reader.h>

#ifndef CLASS_ACR1222L_H
#define CLASS_ACR1222L_H

char **grdracs = NULL;
word tReaders;
SCARDCONTEXT ghContext;
LPSTR mszReaders = NULL;

#define IOCTL_CCID_ESCAPE SCARD_CTL_CODE(1)



// On/Off Leds
BYTE ACS_LEDS[]		= { 0xFF, 0x00, 0x44, 0x0F, 0x00};
// Active Buzzer
BYTE ACS_BUZZ[]		= { 0xE0, 0x00, 0x00, 0x28, 0x01, 0x10};

// Write Data Storage 1 o 2
BYTE ACS_SET_DATA[] = { 0xFF, 0x00, 0x4A, 0x00, 0x00, 0x00, 0x14 }; // MSB LEN, LSB LEN, DATA
// Read Data Storage 1 o 2
BYTE ACS_GET_DATA[] = { 0xFF, 0x00, 0x4C, 0x00, 0x00, 0x00, 0x14 }; // MSB LEN, LSB LEN
// Read Serial Number
BYTE ACS_GET_SN[]	= { 0xE0, 0x00, 0x00, 0x33, 0x00 };
// Read UID
BYTE ACS_GET_UID[]	= { 0xFF, 0xCA, 0x00, 0x00, 0x00};
//Load Keys
BYTE LOAD_KEY[]		= { 0xFF, 0x82, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
// Authentication Block -> 10 bytes
BYTE AUTH_BLK[]		= { 0xFF, 0x86, 0x00, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00};
// Authentication Block -> 6 bytes OBSOLETE
BYTE AUTH_BLK2[]	= { 0xFF, 0x88, 0x00, 0x00, 0x00, 0x00};
//Read Binary Blocks 
BYTE READ_BLK[]		= { 0xFF, 0xB0, 0x00, 0x00, 0x00};
// Get Card Key
BYTE MFGET_KEY[]	= { 0x80, 0xF8, 0x00, 0x8E, 0x08, 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00};

BYTE AUTH_SAM[]		= {	0x00, 0x82, 0x00, 0x83, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 

BYTE pbSendBuffer[] = { 0x00, 0xA4, 0x00, 0x00, 0x02, 0x3F, 0x00 };


#define ACS_RDR0	0	// CardLess
#define ACS_SAM1	1
#define ACS_SAM2	2
#define ACS_SAM3	3

#define AREA1		1
#define AREA2		2




class CLASS_ACR1222L: public tripleDEA
{
public:
	CLASS_ACR1222L();

	BYTE SamUID[10];		// UID en HEX
	BYTE MFUID[8];		// UID en HEX
	char UIDSTR[12];		// UID en txt
	char ID[100];		// ID del Reader

	BYTE MF_KEYA[16][6];
	BYTE MF_KEYB[16][6];

	BYTE MIA_DATA[64][16];
	
	WORD RDR_RESPONSE;
	BYTE RDR_DATA[100];

	BYTE block[16];		//???

	//**************************************************************************

	LONG ReaderContext(SCARDCONTEXT hContext, char **rdracs, BYTE nRDR);	

	LONG ReaderhCard(void);

	LONG ReaderBeginTransaction(BYTE nRDR);
	LONG ReaderEndTransaction(BYTE nRDR);

	LONG CardConnect(BYTE rNDR, DWORD sharemode);
	LONG CardReConnect(BYTE nRDR, DWORD sharemode);
	LONG CardDisConnect(BYTE nRDR );

	LONG CardPresent(BYTE nRDR);
	LONG CardPresent2(BYTE nRDR);
	LONG GetCardStatus(BYTE nRDR);
	LONG GetCardAttribute(BYTE nRDR);


	LONG ActiveBuzzer(BYTE cseg);
	LONG SetLed(unsigned char leds);

	LONG GetUserInfo(BYTE nRDR, BYTE area, DWORD len, char *datos);
	LONG SetUserInfo(BYTE nRDR, BYTE area, char *data);
	LONG GetUserId(SCARDCONTEXT hContxt, char **rdr, BYTE nRDR, BYTE area, DWORD len, char *datos);

	LONG GetSerialNum(SCARDCONTEXT hContxt, char **rdr, BYTE nRDR, char *datos);


	LONG SendAPDU(BYTE nRDR, BYTE *data, DWORD len);

	LONG GetUID(unsigned char *uid);

	LONG WriteBlock(BYTE typekey, BYTE nblock, BYTE pPASS[16][6], unsigned char writelocks[64][16]);

	LONG GetLastKey(BYTE nRDR);
	LONG GetCardKey(BYTE nRDR,  unsigned char mfuid[], unsigned char MFKEY[16][6], unsigned char readblock[64][16] );

	LONG MF_LoadKey(BYTE nRDR, BYTE nkey, BYTE *Key);
	LONG MF_AuthBlk(BYTE nRDR, BYTE nblk, BYTE KeyType, BYTE KeyNumber);
	LONG MF_AuthBlk2(BYTE nRDR, BYTE nblk, BYTE KeyType, BYTE KeyNumber);
	LONG MF_RdBlock (BYTE nRDR, BYTE nblk, BYTE nBytes);

	LONG MF_RdSector (BYTE nRDR, BYTE nsec, BYTE KeyType, BYTE *pass, BYTE ReadBlocks[64][16]);	
	LONG READ_ALL_WITH_KEY( BYTE nRDR, int TypeKey, BYTE pPASS[16][6], BYTE ReadBlocks[64][16] );

	LONG MF_ReadAll(BYTE nRDR, BYTE TypeKey);
	
	LONG MF_WrSector (BYTE nRDR, BYTE nsec, BYTE KeyType, BYTE *pass, BYTE *buffer);
	LONG MF_WrBlock (BYTE nRDR, BYTE nblk, BYTE nBytes, BYTE *buffer);

	// SAM
	LONG AutentificaSAM(BYTE nRDR, BYTE pKey);

	LONG MF_Decrement(BYTE addr, BYTE PurseBlock, int valor);
	LONG MF_Increment(BYTE addr, BYTE PurseBlock, int valor);
	LONG MF_Transfer(BYTE addr, BYTE PurseBlock);
	//**************************************************************************/

	void printAllMIA(void);
	void printAllKey( BYTE key[16][6] );

private:
	
	SCARDCONTEXT pContext;
	char **rdracs = NULL;
	SCARDHANDLE hCard[4];	// para pruebas
	BYTE nReader;
	
	BYTE CARD_PRESENT[4];
	LONG rv;

	
	char *ptr;

	BYTE pPASS[6];

	
	int nbReaders;

	DWORD dwActiveProtocol, dwReaderLen, dwState, dwProt, dwAtrLen;
	BYTE pbAtr[MAX_ATR_SIZE] = "";
	char pbReader[MAX_READERNAME] = "";
	const SCARD_IO_REQUEST *pioSendPci[4];
	SCARD_IO_REQUEST pioRecvPci[4];

	BYTE pbRecvBuffer[1024];
	BYTE pbRecChallenge[8];

	BYTE wrBuffer[128];
	BYTE lenwr;

	DWORD dwSendLength, dwRecvLength;

	LONG PCSC_ERROR(LONG rv, char *text);
	void printRec(BYTE *recB, DWORD sizB, const char *text);
	void printTx(BYTE *recB, DWORD sizB);
//	void printAllMIA(void);
	void GetResponse(BYTE *recB, DWORD sizB);

};

#endif /* CLASS_ACR1222L_H */
