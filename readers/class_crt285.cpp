
#include "usbdev.h"

/* 
 * File:   class_crt285.cpp
 * Author: FAMISA
 * 
 * Created on October 3, 2022, 4:16 PM
 */
//
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
CLASS_CRT285::CLASS_CRT285()
{
	iRecvDataLen = 0;	
	iSendDataLen = 0;
}

/******************************************************************************/
/*  Abre el Puerto USB                                                        */
/******************************************************************************/
int CLASS_CRT285::Open(void)
{
	if( rd285.CRT288x_OpenConnection(CRT_OPEN_TYPEUSB, 0 ,0) != 0)
	{
		Close();
		return 0x01;	// ERROR USB
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*  Abre el Puerto RS232                                                        */
/******************************************************************************/
int CLASS_CRT285::Open(int iComPort, long lBaudRate)
{	int re;

	Close();
	if( rd285.CRT288x_OpenConnection(1,iComPort, lBaudRate) != 0)
	{
		Close();
		return 0x02;    // ERROR RS232
	}
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::LockedProcess(int iLockType)
{
	return rd285.CRT288x_LockedProcess(iLockType);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::LockedStatus(void)
{
	return rd285.CRT288x_GetLockStatus();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::Led(int iLightType, int iFlag)
{
	rd285.CRT288x_LEDProcess(CRT_LED_ALL, CRT_LED_OFF);

	rd285.CRT288x_LEDProcess(iLightType, iFlag);
	
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::InitComm(char tport, char namecomm[], int baud)
{	int k;

	if(tport==0)
	{	// USB
		printf("RECARGA USB\n");
		for(k=0; k<4; k++)
		{	//printf("k=%d\n",k);
			if(Open() == 0)
			{	if( rd285.CRT288x_InitDev( 1 )==0)
					return	0x00;	// OK
				else
					Close();
			}
		}
		printf("Error Init Reader\n");
//		Close();
		return 0x101;
	}
	else
	{	// RS232
		for(k=0; k<2; k++)
//			if(Open(namecomm, baud) == 0)
			if(Open(1, baud) == 0)		// corregir libreria para que acepte el namecomm
				if( rd285.CRT288x_InitDev( 1 )==0)
				{
					
					return	0x00;	// OK
				}
				else
					Close();
		printf("Error Init Reader\n");
		Close();
		return 0x101;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::InitUSB(libusb_device_handle    *nusb )
{	int k;

	printf("RECARGA USB\n");

	rd285.nUSB = nusb;

	if( rd285.CRT288x_InitDev( 1 )==0)
		return	0x00;	// OK

	printf("Error Init Reader\n");
	return 0x101;
}

/******************************************************************************/
/*   Cierra la conexion activa                                                */
/******************************************************************************/
int CLASS_CRT285::Close()
{
	return rd285.CRT288x_CloseConnection();
}

/******************************************************************************/
/* 1 no card                                                                  */
/* 3 the card inside the reader                                               */
/* 9 the machine offline, the card status unknown                             */
/******************************************************************************/
int CLASS_CRT285::CardPresent(void)
{	int re;

	re = rd285.CRT288x_GetCardStatus();
	if(re==CRT_CARD_PRESENT)
		re = 0x00;
	return re;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::AutentificaSAM(int nSlot, int pKey)
{	unsigned char atrasc[100];
	unsigned char Challenge[8], RespChall[8],ExternAuth[20];
	int i;

	// SAM SLOT CHANGE
	if( rd285.CRT288x_SAMSlotChange(nSlot) != 0)
	{
		printf("Error SELECT SLOT\n");
		return 0x10;	// Select Slot Error
	}

	/**************************************************************************/
	if( rd285.CRT288x_SAMActivation(0, &iRecvDataLen, byRecvData) != 0)
	{
		printf("Error Activation\n");
		return 0x11;	// Error en Activacion del SAM
	}

	memcpy(atrasc, byRecvData+5, iRecvDataLen-8);
	atrasc[iRecvDataLen-8] = '\0';

	/**************************************************************************/
	iSendDataLen = sizeof(APDU_SELECT_PIN_PSAM) - 1;
	if(pKey == PSAM)
		printf("SelPin=[%d][%s]\n", iSendDataLen, APDU_SELECT_PIN_PSAM);
	else
		printf("SelPin=[%d][%s]\n", iSendDataLen, APDU_SELECT_PIN_ISAM);

	if(pKey == PSAM)
		i = rd285.CRT288x_SAMApdu(0, iSendDataLen , APDU_SELECT_PIN_PSAM, &iRecvDataLen , byRecvData);
	else
		i = rd285.CRT288x_SAMApdu(0, iSendDataLen , APDU_SELECT_PIN_ISAM, &iRecvDataLen , byRecvData);
	if( i != 0)
	{
		printf("ERROR SELECT PIN\n");
		return 0x12;
	}
	else
	{
//		printf("OK SELECT PIN\n");

//		printf("DATOS [%d]= ", iRecvDataLen);
//		for(i=0; i<iRecvDataLen; i++)
//			printf("%02x ", byRecvData[i]);
//		printf("\n");
		Sam_Response = (unsigned short)byRecvData[0]<<8 + (unsigned short)byRecvData[1];
//		printf("Sam_Response=[%04X]\n",Sam_Response);
	}

	/**************************************************************************/
	printf("\nVERIFY PIN\n");
	iSendDataLen = sizeof(APDU_VERIFY_PIN) - 1;
	if( rd285.CRT288x_SAMApdu(0, iSendDataLen , APDU_VERIFY_PIN, &iRecvDataLen , byRecvData) != 0)
	{
		printf("ERROR VERIFY PIN\n");
		return 0x13;	// ERROR VERIFY PIN
	}
	else
	{
//		printf("OK VERIFY PIN\n");
//		printf("DATOS [%d]= ", iRecvDataLen);
//		for(i=0; i<iRecvDataLen; i++)
//			printf("%02x ", byRecvData[i]);
//		printf("\n");		
		Sam_Response = (unsigned short)byRecvData[0]<<8 + (unsigned short)byRecvData[1];
//		printf("Sam_Response=[%04X]\n",Sam_Response);
	}

	/**************************************************************************/
//	printf("\nCHALLENGE\n");
	iSendDataLen = sizeof(APDU_GET_CHALLENGE) - 1;
	if( rd285.CRT288x_SAMApdu(0, iSendDataLen, APDU_GET_CHALLENGE, &iRecvDataLen , byRecvData) != 0)
	{
		printf("ERROR GET CHALLENGE\n");
		return 0x14;	// ERROR GET CHALLENGE
	}

//	printf("OK CHALLENGE\n");
//	printf("DATOS [%d]= ", iRecvDataLen);
//	for(i=0; i<iRecvDataLen; i++)
//		printf("%02x ", byRecvData[i]);
//	printf("\n");		
	if(iRecvDataLen!=13)
	{
		printf("ERROR LONGITUD RESPUESTA\n");
		return 0x14;	// ERROR GET CHALLENGE
	}
	for(i=0; i<8; i++)
		Challenge[i] = byRecvData[i];
	Sam_Response = (unsigned short)byRecvData[8]<<8 + (unsigned short)byRecvData[9];

//	printf("Challenge=[%04x]->", Sam_Response);
//	for(i=0; i<8; i++)
//		printf("[%02x]",Challenge[i]);
//	printf("\n");

	Three_DES_Encrypt(Challenge,RespChall,pKey);

//	printf("OUT Challenge=[%04x]->", Sam_Response);
//	for(i = 0 ; i < 8; i++)
//		printf("%02hhX ",RespChall[i]);
//	printf("\n\n\n");

	ExternAuth[0]	=	0x00;
	ExternAuth[1]	=	0x82;
	ExternAuth[2]	=	0x00;
	ExternAuth[3]	=	0x83;
	ExternAuth[4]	=	0x08;
	ExternAuth[5]	=	RespChall[0];
	ExternAuth[6]	=	RespChall[1];
	ExternAuth[7]	=	RespChall[2];
	ExternAuth[8]	=	RespChall[3];
	ExternAuth[9]	=	RespChall[4];
	ExternAuth[10]	=	RespChall[5];
	ExternAuth[11]	=	RespChall[6];
	ExternAuth[12]	=	RespChall[7];

	rd285.Bcd2Asc(ExternAuth, 13, bySendData);
	bySendData[13*2] = '\0';

//	printf("OUT ASCII=[%d][%s]->", 26, bySendData);
//	printf("\n\n\n");

	memset(byRecvData, '\0', 100);
//	printf("\n\n RESPUESTA AL CHALLENGE\n");
	if( rd285.CRT288x_SAMApdu(0, 26 , bySendData, &iRecvDataLen , byRecvData) != 0 )
	{
		printf("ERROR GET CHALLENGE\n");
		return 0x15;	// error en encriptamiento
	}

//	printf("KEY \n");
//	printf("KEY DATOS [%d]= ", iRecvDataLen);
//	for(i=0; i<iRecvDataLen; i++)
//		printf("%02x ", byRecvData[i]);
//	printf("\n");
	Sam_Response = (unsigned short)byRecvData[0]<<8 + (unsigned short)byRecvData[1];
//	printf("Sam_Response=[%04X]\n",Sam_Response);

//	printf("SAM PASS \n");
//	printf("KEY PASS [%d]= ", iRecvDataLen);
//	for(i=0; i<8; i++)
//		printf("%02x ", byRecvData[i]);
//	printf("\n");

//	printf("\nUID SAM\n");
	iSendDataLen = sizeof(APDU_GET_UID_SAM) - 1;
	if( rd285.CRT288x_SAMApdu(0, iSendDataLen, APDU_GET_UID_SAM, &iRecvDataLen , byRecvData) != 0)
	{
		printf("ERROR UID\n");
		return 0x15;	// error en encriptamiento
	}
		
	Sam_Response = (unsigned short)byRecvData[8]<<8 + (unsigned short)byRecvData[9];
	if( (Sam_Response!=0x9000) || (iRecvDataLen!= 13) )
	{
		printf("ERROR UID SAM\n");
		return 0x14;	// ERROR GET CHALLENGE
	}
	for(i=0; i<8; i++)
	{	SamUID[i] = byRecvData[i];
//		printf("%02x ", byRecvData[i]);
	}
//	printf("\n");

	return 0x00;		// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::GetCardKey( unsigned char mfuid[], unsigned char MFKEY[16][6], unsigned char readblock[64][16] )
{	unsigned char ExternAuth[20];
	int i,k;

	// Get all keys A
	ExternAuth[0] = 0x80;
	ExternAuth[1] = 0xF8;
	ExternAuth[2] = 0x00;
	ExternAuth[3] = 0x8E;
	ExternAuth[4] = 0x08;
	ExternAuth[5] = mfuid[0];
	ExternAuth[6] = mfuid[1];
	ExternAuth[7] = mfuid[2];
	ExternAuth[8] = mfuid[3];
	ExternAuth[9] =  readblock[4][13];
	ExternAuth[10] = readblock[4][14];
	ExternAuth[11] = 0x00;
	ExternAuth[12] = 0x00;


	rd285.Bcd2Asc(ExternAuth, 13, bySendData);
	bySendData[13*2] = '\0';


	if( rd285.CRT288x_SAMApdu(0, 26 , bySendData, &iRecvDataLen , byRecvData) != 0)
	{
		printf("ERROR Keys A\n");
		return 0x20;	// Error al obtener card key
	}

	if( rd285.CRT288x_SAMApdu(0, 10 , APDU_READ_LAST_KEY, &iRecvDataLen , byRecvData) != 0)
	{
		printf("ERROR Get Last Key \n");
		return 0x0100;
	}

	for(k=0; k<16; k++)
	{	for(i=0; i<6; i++)
//			if( k!=1 )
				MFKEY[k][i] = byRecvData[i];
	}

	return 0x00;	//OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::Read_MFUID( unsigned char MFUID[])
{	unsigned char ExternAuth[20];
	unsigned char cmdData[1024] = {0};
	unsigned char RecvData[1024] = {0};
	unsigned char stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen,i;

	cmdDataLen = 5;
	ExternAuth[0] = 0x43;
	ExternAuth[1] = 0x60;
	ExternAuth[2] = 0x30;
	ExternAuth[3] = 0x41;
	ExternAuth[4] = 0x42;

	rd285.Bcd2Asc(ExternAuth, 5, bySendData);
	bySendData[5*2] = '\0';

	if( rd285.CRT288x_ExeCommand( 10, bySendData, &RecvDataLen, RecvData, stCode) != 0)
	{
		printf("Error al obtener UID\n");
		return 0x30;		// error al obtener el UID
	}

	for(i=0; i<4; i++)
		MFUID[i] = RecvData[4+i];
	MFUID[4] = MFUID[0];
	MFUID[5] = MFUID[1];

	return 0;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::Read_Sector(int TypeKey, int isec, int fsec, unsigned char pPASS[16][6], unsigned char readblock[64][16])
{	int ns, i;

	for(ns=isec; ns<fsec+1; ns++)
	{
		// (int iMode, int iKs, int iSn, int uDataLength, unsigned char lpData[])
		// Key Verification, KeyA, Sector 1, lenpass=6, password
		if( rd285.CRT288x_MifareKeyProcess(CRT_MIFARE_KEY_VERIFICATION, TypeKey, ns, 6, pPASS[ns] ) != 0)
		{
			printf("\nError Pass Verification\n\n");
			return 0x40;	// Error al verificar PASS del Sector
		}

		// Leer Sector Indicado
		//							  (int iMode, int iSn, int iBn, int iLc, int* uDataLength, unsigned char lpData[])
		if( rd285.CRT288x_MifareCardProcess(1, ns, 0, 4, &iRecvDataLen , byRecvData) != 0)
		{
			printf("\nError Read Sector [%d]\n\n", isec);
			return 0x41;	// Error Read Sector
		}
		else
		{
			Sam_Response = (unsigned short)byRecvData[iRecvDataLen-3]<<8 + (unsigned short)byRecvData[iRecvDataLen-2];

			if(Sam_Response != 0x9000)
			{
				printf("\nError Read Sector [%04x]\n", Sam_Response);
				return 0x41;	// Error SAM RESPONSE
			}
			for(i=0; i<iRecvDataLen-3; i++)
				readblock[ns*4][i] = byRecvData[i];
		}
	}
	return 0x00;
}


/******************************************************************************/
/* Lee el Sector Monedero                                                     */
/******************************************************************************/
int CLASS_CRT285::ReadValue(int TypeKey, int PurseBlock, unsigned char pPASS[16][6] )
{	int ns,i, re;
	int np;
	int valor;

	ns = PurseBlock /4;		// Numero de Sector a Leer

	if( rd285.CRT288x_MifareKeyProcess(CRT_MIFARE_KEY_VERIFICATION, TypeKey, ns, 6, pPASS[ns]) != 0)
	{
		printf("\n0=Error Pass Verification re:\n\n");
		return 0x50;	// Error al verificar PASS Read Value
	}

	// Leer Monededero
	for(np=0; np<2; np++)
	{
		if( (re=rd285.CRT288x_MifareCardProcess(CRT_MIFARE_READ_BALANCE, ns, np+1, 1, &iRecvDataLen , byRecvData)) != 0)
		{
			printf("\nError Read Value %d\n\n", re);
			return 0x51;	// Error Read Sector
		}
		else
		{	//("\nOK Read Value len=[%d]\n", iRecvDataLen);

			Sam_Response = (unsigned short)byRecvData[iRecvDataLen-3]<<8 + (unsigned short)byRecvData[iRecvDataLen-2];

			if(Sam_Response != 0x9000)
			{
				printf("\nError Read Sector [%04x]\n", Sam_Response);
				return 0x41;	// Error SAM RESPONSE
			}
			valor = *(int *)&byRecvData[0];

			sprintf(message, "Monedero%d = %d\n",np, valor);
		}
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::DecrementaBalance(int TypeKey, int PurseSector, int Purse1o2, unsigned char pPASS[16][6], int valor )
{	int i, re;
	int np;

	if( rd285.CRT288x_MifareKeyProcess(CRT_MIFARE_KEY_VERIFICATION, TypeKey, PurseSector, 6, pPASS[PurseSector]) != 0)
	{
		printf("\n0=Error Pass Verification re:\n\n");
		return 0x50;	// Error al verificar PASS Read Value
	}

	iRecvDataLen = valor;
	// Decremenra Monededero
	if( (re=rd285.CRT288x_MifareCardProcess(CRT_MIFARE_DECREMENT, PurseSector, Purse1o2, 1, &iRecvDataLen , byRecvData)) != 0)
	{
		printf("\nError Decrementa Balance %d\n\n", re);
		return 0x51;	// Error Read Sector
	}
	else
	{
		Sam_Response = (unsigned short)byRecvData[iRecvDataLen-3]<<8 + (unsigned short)byRecvData[iRecvDataLen-2];

		if(Sam_Response != 0x9000)
		{
			printf("\nError Decrementa Balance [%04x]\n", Sam_Response);
			return 0x41;	// Error SAM RESPONSE
		}

		valor = *(int *)&byRecvData[0];

		sprintf(message, "Decrementa Balance%d = %d\n",np, valor);
	}

	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::IncrementaBalance(int TypeKey, int PurseBlock, unsigned char pPASS[16][6], int valor )
{	int i, re;
	int np;
	int nb,ns;

	ns = PurseBlock / 4;	// Numero de Sector a Leer
	nb = PurseBlock % 4;	// offset

	if( rd285.CRT288x_MifareKeyProcess(CRT_MIFARE_KEY_VERIFICATION, TypeKey, ns, 6, pPASS[ns]) != 0)
	{
		printf("\n0=Error Pass Verification re:\n\n");
		return 0x50;	// Error al verificar PASS Read Value
	}

	iRecvDataLen = valor;
	// Incrementa Monededero
	if( (re=rd285.CRT288x_MifareCardProcess(CRT_MIFARE_INCREMENT, ns, nb, 1, &iRecvDataLen , byRecvData)) != 0)
	{
		printf("\nError Incrementa Balance %d\n\n", re);
		return 0x51;	// Error Read Sector
	}
	else
	{
		Sam_Response = (unsigned short)byRecvData[iRecvDataLen-3]<<8 + (unsigned short)byRecvData[iRecvDataLen-2];

		if(Sam_Response != 0x9000)
		{
			printf("\nError Incrementa Balance [%04x]\n", Sam_Response);
			return 0x41;	// Error SAM RESPONSE
		}

		valor = *(int *)&byRecvData[0];

		sprintf(message, "Incrementa Balance%d = %d\n",np, valor);
	}

	return 0x00;	// OK
}

/******************************************************************************/
/*                   Lee toda la informacion de la MIA                        */
/******************************************************************************/
int CLASS_CRT285::READ_ALL( int TypeKey,  unsigned char UID[], unsigned char pPASS[16][6], unsigned char ReadBlocks[64][16] )
{	int i;

	if( Read_MFUID( UID) !=0 )
	{
		printf("Error en Read UID\n");
		return 0x0400;		// ERROR EN UID
	}

	// Lllave del primer sector
	for(i=0; i<6; i++)
		pPASS[1][i] = UID[i];

	// Read Sector 1
	if( Read_Sector(0, 1, 1, pPASS, ReadBlocks) != 0)
	{
		printf("Error en Read Sector 1\n");
		return 0x0401;		// ERROR READ SECTOR 1
	}

	if( GetCardKey ( UID, pPASS, ReadBlocks) != 0)
	{
		printf("Error en Get Keys\n");
		return 0x0402;	// Error al obtener las llaves de la MIA
	}


	if( Read_Sector( TypeKey, 0, 15, pPASS, ReadBlocks) != 0)
	{
		printf("Error en ReadAll\n");
		return 0x0403;	// Error al leer todos los sectores de la MIA
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT285::WriteBlock(int TypeKey, int block, unsigned char pPASS[16][6], unsigned char wrblock[64][16] )
{	int i, re;
	int nb,ns;

	ns = block / 4;	// Numero de Sector a Leer
	nb = block % 4;	// offset

	if(block==0x00)
		return 0x00;	//Block 0, NO ESCRIBE

	if(nb==0x03)
		return 0x00;	// no escribe, cada 4 block


	if( rd285.CRT288x_MifareKeyProcess(CRT_MIFARE_KEY_VERIFICATION, TypeKey, ns, 6, pPASS[ns]) != 0)
	{
		printf("\n0=Error Pass Verification re:\n\n");
		return 0x50;	// Error al verificar PASS Read Value
	}

	for(i=0; i<16; i++)
		byRecvData[i] = wrblock[block][i];
	iRecvDataLen = 16;

	// Write Block
	if( (re=rd285.CRT288x_MifareCardProcess(CRT_MIFARE_WRITE_SECTOR, ns, nb, 1, &iRecvDataLen , byRecvData)) != 0)
	{
		printf("\nError Write Block %d\n\n", re);
		return 0x51;	// Error Read Sector
	}
	else
	{
		Sam_Response = (unsigned short)byRecvData[iRecvDataLen-3]<<8 + (unsigned short)byRecvData[iRecvDataLen-2];

		if(Sam_Response != 0x9000)
		{
			printf("\nError Write Block [%04x]\n", Sam_Response);
			return 0x41;	// Error SAM RESPONSE
		}
	}

	return 0x00;	// OK
}

int CLASS_CRT285::InicializaValue()
{
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_CRT285::ImpDatosRec(char msg[], int datalen, unsigned char datarec[])
{	int  i;

	printf("%s", msg);
	printf("Datos = [");
	for(i=0; i<datalen; i++)
		printf("%02x ", datarec[i]);
	printf("]\n");
}
/******************************************************************************/