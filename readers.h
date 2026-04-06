/* 
 * File:   readers.h
 * Author: famisa
 *
 * Created on October 19, 2022, 10:24 AM
 */

#ifndef READERS_H
#define READERS_H

#define	RDR_CRT285		1
#define RDR_CN670		2
#define RDR_CIM6903		3
#define RDR_ACR1222L	4
#define RDR_CRT603		5
#define RDR_ARCONTIA	6
#define DIS_KYT2664		7
#define RDR_ACR1252L	8
#define RDR_CHM2901RF 	9


#define CARD_PRESENT	1

#define LOCK_ON			1	// se bloquea
#define LOCK_OFF		0	// se libera

#define MAGAZINE1					0x00
#define MAGAZINE2					0x01



class readers
{
public:
	char modelo;		// modelo del Reader
	char modelo_exp;	// modelo del reader del dispensador

	char TipoSAM;

	readers();

	libMIA		MIA;	// clase para todos los movimientos de bits
	CLASS_BOX	BOX;	// Desglose de Monedas y Billetes

	// READERS
	CLASS_CRT285	reader_crt285;		// Reader CRT_285	China Creator
	CLASS_CIVINTEC	reader_cn670;		// Reader CN670		Civintec
	//CLASS_CIM6903	reader_cim6903;		// Reader CIM6903	KYT
	CLASS_CHM29XX	reader_chm2901;		// Reader CHM29XX	KYT
	CLASS_ACR1222L	reader_acr1222;		// Reader CR1222L	ACS
	CLASS_CRT603vx	reader_crt603;		// Reader CRT603	China Creator
	CLASS_KYT26xx	reader_kyt2664;		// Reader KYT2664	KYT
	

	int initComm( char modelo, char tport, char namecomm[], int baud );
	int initUSB( char modelo, libusb_device_handle    *nusb );
	int initPorts( char marca, char tport, char namecomm[], int baud );

	int AutentificaSAM(char slot );

	int Open();		// Open USB
	int Open(int iComPort, long lBaudRate);		// Open RS232
	int Close();
	int Identifica();

	int CardPresent ();
	int Read_MFUID (unsigned char UID[]);
	int Read_Sector (int TypeKey, int isec, int fsec, BYTE pPASS[16][6], BYTE readblock[64][16]);

	int Read_Sector1A(BYTE MFUID[], BYTE pPASS[16][6], BYTE readblock[64][16]);
	int READ_ALL_WITH_KEY(int tKey, BYTE pPASS[16][6], BYTE ReadBlocks[64][16]);


	int GetCardKey ( BYTE TypeKey, BYTE mfuid[], BYTE MFKEY[16][6], BYTE readblock[64][16] );
	int ReadValue ( int TypeKey, int PurseBlock, BYTE pPASS[16][6]);
	int ReadMonedero(int block,int *vPurse1, int *vPurse2);
	int ReadAllBlocks();
	int ReadAllData (int TypeKey);

//	int READ_ALL_WITH_KEY( int TypeKey,  unsigned char UID[], unsigned char pPASS[16][6], unsigned char ReadBlocks[64][16] );

	int WriteBlock(int block);
	int WriteBlocks(int TypeKey, char iblock, char fblock);
	int WriteAllCard(int ctrl);
	int WriteAllBlock(void);
	int	WriteInitCard(void);

	int SetMode(void);
	
	int HistMovements(void);
	
	int IncrementaMonedero(int TypeKey, int block, int increment);
	int DecrementaMonedero(int sector, int block, int increment);

	int UNLOCK(char lock);

	void ClearReadBlocks();

	int LedOnOff(int NoLed, int Opcion);

	int Buzzer(BYTE opcion);

	// Dispensador
	int GetCard(BYTE stk);

	int StackerStatus(void);
	int GetStacker(unsigned char nstk);

	int EjectCard(void);
	int CaptureCard(void);


private:

//	int Unlock(char lock);

};
#endif /* READERS_H */

