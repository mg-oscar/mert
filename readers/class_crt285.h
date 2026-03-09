/* 
 * File:   class_crt285.h
 * Author: FAMISA
 *
 * Created on October 3, 2022, 4:16 PM
 */

#ifndef CLASS_CRT285_H
#define CLASS_CRT285_H


#define CRT_CARD_PRESENT		0x03

class CLASS_CRT285: public tripleDEA
{
public:
	CLASS_CRT285();
	int	TypePort;		// 0 = USB   1=RS232

	
	crt285 rd285;
	
	unsigned char SamUID[8];

	int Open(void);								// Open USB
	int Open(int iComPort, long lBaudRate);		// Open RS232
	int InitComm(char tport, char namecomm[], int baud);
	int InitUSB(libusb_device_handle    *nusb );
	int Close();
	int Read_MFUID(BYTE MFUID[]);
	int Read_Sector(int TypeKey, int isec, int fsec, BYTE pPASS[16][6], BYTE readblock[64][16]);
	int ReadValue  (int TypeKey, int PurseBlock,     BYTE pPASS[16][6]);
	int DecrementaBalance(int TypeKey, int PurseSector, int Purse1o2, BYTE pPASS[16][6], int valor);
	int IncrementaBalance(int TypeKey, int PurseBlock, BYTE pPASS[16][6], int valor );
	int WriteBlock(int TypeKey, int PurseBlock, BYTE pPASS[16][6], BYTE wrblock[64][16]);

	int InicializaValue();

	int AutentificaSAM(int nSlot, int pKey);
	int GetCardKey( BYTE mfuid[], BYTE MFKEY[16][6], BYTE readblock[64][16]);

	int CardPresent(void);
	int READ_ALL(int TypeKey,  BYTE UID[] , BYTE pPASS[16][6], BYTE ReadBlocks[64][16] );

	int Led(int iLightType, int iFlag);
	
	int LockedProcess(int iLockType);
	int LockedStatus(void);
	
private:

	int iRecvDataLen;
	BYTE byRecvData[1024];
	int iSendDataLen;
	BYTE bySendData[1024];

	char message[100];

	unsigned short Sam_Response;

	void ImpDatosRec(char msg[], int datalen, BYTE datarec[]);
};

#endif /* CLASS_CRT285_H */