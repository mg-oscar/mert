/*
 * File:   class_civintec.h
 * Author: FAMISA
 *
 * Created on October 3, 2022, 4:16 PM
 */

#ifndef CLASS_CIVINTEC_H
#define CLASS_CIVINTEC_H

// Tipo de Comunicacion
#define CIVINTEC_RS232		0	
#define CIVINTEC_USB		1
#define CIVINTEC_UDP		2

class CLASS_CIVINTEC: public tripleDEA
{
public:
	CLASS_CIVINTEC();

	unsigned char devAddress;
//	unsigned char nUSB;
	unsigned char SAMUID[10];	// UID SAM

	CLASS_CV_CN670 cn670;


	int Init( unsigned char addr, unsigned char nusb);
	int CardPresent(unsigned char MFUID[]);

	int Open(int iComPort, long lBaudRate);		// Open RS232
	int Close();

	int Read_Sector(int TypeKey, int isec, int fsec, BYTE MFUID[], BYTE pPASS[16][6], BYTE readblock[64][16]);	
	int Read_Sector1A(BYTE MFUID[], BYTE pPASS[16][6], BYTE readblock[64][16]);	
	int READ_ALL( int TypeKey,  unsigned char UID[], unsigned char pPASS[16][6], unsigned char ReadBlocks[64][16] );
	int READ_ALL_WITH_KEY( int TypeKey,  unsigned char UID[], unsigned char pPASS[16][6], unsigned char ReadBlocks[64][16] );

	int Read_MFUID(BYTE MFUID[]);
	int ReadValue(int TypeKey, int PurseBlock, BYTE MFUID[], BYTE pPASS[16][6] );	
	int DecrementaBalance(int TypeKey, BYTE MFUID[], int PurseBlock, BYTE pPASS[16][6], int valor);
	int Write_Sector(int TypeKey, int isec, int fsec);

	int WriteBlock(int TypeKey, int blk, BYTE MFUID[], BYTE pPASS[16][6], BYTE readblock[64][16]);	

	int Incrementa();
	int InicializaValue();

	int AutentificaSAM(int nSlot, char tSAM);
	int GetCardKey( BYTE TypeKey, BYTE mfuid[], BYTE MFKEY[16][6], BYTE readblock[64][16] );

	int Buzzer(BYTE op);

	int ConfigAutoRead (void);
	int ConfigAutoRead1(void);
	int ReadUID(unsigned char UID[]);

	void imprime_RdBlocks(BYTE sn, BYTE datos[64][16]);
	void imprime_Key(BYTE typeKey, BYTE datos[16][6]);

private:
	int iRecvDataLen;
	BYTE byRecvData[200];
	BYTE iSendDataLen;
	BYTE bySendData[200];
	BYTE stCode[100];
	char message[100];
	char ATR[100];

	unsigned short Sam_Response;

	int Autentifica(int TypeKey, int nblock, BYTE MFUID[], BYTE pPASS[]);
	int ReadMonederos(int addr, int PurseBlock, int *value1,  int *value2);
	short int SamResponse();

	void ImpDatosRec(char msg[], int datalen, BYTE datarec[]);
	void PrintRx(char msg[]);

};

#endif /* CLASS_CIVINTEC_H */