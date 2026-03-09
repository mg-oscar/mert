
#include "class_CV_CN670.h"

/* 
 * File:   class_civintec.cpp
 * Author: FAMISA
 * 
 * Created on October 3, 2022, 4:16 PM
 */
//
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
CLASS_CIVINTEC::CLASS_CIVINTEC()
{
	devAddress	 = 0;
	iRecvDataLen = 0;
	iSendDataLen = 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC:: Init( BYTE addr, unsigned char nusb)
{
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::Buzzer(BYTE op)
{	unsigned char buff[5];

	switch(op)
	{	case 0:	// OFF
//				cn670.ActiveBuzzer(0,0,buff);
				break;
		case 1:	// ON
//				cn670.ActiveBuzzer(0,1,buff);
				break;
		case 2:
				buff[0]=0x01;
				buff[1]=0x01;
				buff[2]=0x00;
				buff[3]=0x00;
				buff[4]=0x01;
				op = 4;				
				break;
		case 3:
				buff[0]=0x01;
				buff[1]=0x01;
				buff[2]=0x01;
				buff[3]=0x01;
				buff[4]=0x03;
				op = 4;				
				break;
		case 4:
				buff[0]=0x0A;
				buff[1]=0x00;
				buff[2]=0x00;
				buff[3]=0x00;
				buff[4]=0x01;
				op = 4;				
				break;
		case 5:
				buff[0]=0x01;
				buff[1]=0x01;
				buff[2]=0x00;
				buff[3]=0x00;
				buff[4]=0x01;
				op = 4;				
				break;
		case 6:
				buff[0]=0x02;
				buff[1]=0x01;
				buff[2]=0x01;
				buff[3]=0x01;
				buff[4]=0x08;
				op = 4;				
				break;
		default:
				buff[0]=0x00;
				buff[1]=0x00;
				buff[2]=0x00;
				buff[3]=0x00;
				buff[4]=0x01;
				op = 4;		
				break;
	}
	cn670.ActiveBuzzer(0,op,buff);
	return 0;
}

/******************************************************************************/
/*  Abre el Puerto RS232                                                        */
/******************************************************************************/
int CLASS_CIVINTEC::Open(int iComPort, long lBaudRate)
{
    return 0x00;
}

/******************************************************************************/
/*   Cierra la conexion activa                                                */
/******************************************************************************/
int CLASS_CIVINTEC::Close()
{
	cn670.CV_CloseUSB();
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_CIVINTEC::PrintRx(char msg[])
{	int i;

	printf("%s  = %d\n", msg, iRecvDataLen);
	for(i=0; i<iRecvDataLen; i++)
		printf("%02x ", byRecvData[i]);
	printf("\n");
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::CardPresent( unsigned char MFUID[] )
{	int status;

	if( cn670.CardPresent(MFUID) == 0)
	{			
		return 0x00;
	}

	return 0x01;	// error o no hay tarjeta presente
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
short int CLASS_CIVINTEC::SamResponse()
{
	return (unsigned short)byRecvData[iRecvDataLen-2]<<8 + (unsigned short)byRecvData[iRecvDataLen-1];
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::AutentificaSAM(int nSlot, char tSAM)
{	int i,re;
	BYTE Challenge[8], RespChall[8],ExternAuth[20];


	byRecvData[0] = 0x00;
	iRecvDataLen = 1;
	if( (re=cn670.ISO7816_ATR( 0, 1, byRecvData , iRecvDataLen, 200)) != 0)
	{
		printf("ERROR SAM ATR\n");
		return 0x200;
	}
//	PrintRx((char *)"ATR");

	memcpy(ATR, &byRecvData[4], iRecvDataLen-4);
	ATR[iRecvDataLen-4] = '\0';
//	printf("ATR=[%s]\n", ATR);
	
	if( (re=cn670.ISO7816_APDU_Exchange(0, 1, APDU_SELECT_PIN_PSAM_HEX, 7, byRecvData, iRecvDataLen, 200)) != 0)
	{
		printf("ERROR SAM SELECT PIN %d\n", re);
		return 0x200;
	}
	Sam_Response = SamResponse();

	PrintRx((char *)"Select");
//	printf("SAM RESPONSE = %04X\n", Sam_Response);


	if( (re=cn670.ISO7816_APDU_Exchange(0, 1, APDU_VERIFY_PIN_HEX, 13, byRecvData, iRecvDataLen, 200)) != 0)
	{
		printf("ERROR SAM VERIFY PIN %d\n", re);
		return 0x200;
	}	
	Sam_Response = SamResponse();

	PrintRx((char *)"Verify");
//	printf("SAM RESPONSE = %04X\n", Sam_Response);


	if( (re=cn670.ISO7816_APDU_Exchange(0, 1, APDU_GET_CHALLENGE_HEX, 5, byRecvData, iRecvDataLen, 200)) != 0)
	{
		printf("ERROR SAM CHALLENGE %d\n", re);
		return 0x200;
	}
	Sam_Response = SamResponse();

	PrintRx((char *)"Challenge");
//	printf("SAM RESPONSE = %04X\n", Sam_Response);

	memcpy(Challenge, byRecvData, 8);

	
//	printf(" IN TRIPLE DEA -> ");
//	for(i=0; i<8; i++)
//		printf("[%02x]",Challenge[i]);
//	printf("\n");

	Three_DES_Encrypt(Challenge,RespChall,PSAM);

//	printf("OUT TRIPLE DEA ->");
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

	if( (re=cn670.ISO7816_APDU_Exchange(0, 1, ExternAuth, 13, byRecvData, iRecvDataLen, 200)) != 0)
	{
		printf("ERROR SAM DESafio %d\n", re);
		return 0x200;
	}
	Sam_Response = SamResponse();

	PrintRx((char *)"Response Challenge");
//	printf("Sam_Response=[%04X]\n",Sam_Response);


	if( (re=cn670.ISO7816_APDU_Exchange(0, 1, APDU_GET_UID_SAM_HEX, 5, byRecvData, iRecvDataLen, 200)) != 0)
	{
		printf("ERROR GET SAM UID %d\n", re);
		return 0x200;
	}
	Sam_Response = SamResponse();

	PrintRx((char *)"SAM UID");
//	printf("Sam_Response=[%04X]\n",Sam_Response);


	memcpy(SAMUID, byRecvData, 8);
//	printf("SAM UID=[");
//	for(i=0; i<8; i++)
//		printf("%02x ", SAMUID[i] );
//	printf("]\n");

	return 0x00;
	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::GetCardKey( BYTE TypeKey, BYTE mfuid[], BYTE MFKEY[16][6], BYTE readblock[64][16] )
{	BYTE ExternAuth[20];
	int i,k, re;
	int nmaxlen, maxlen = 50;
	int datalen;
	BYTE RecData[100];
	BYTE ATR[50];

	if(TypeKey != KEYDEF)
	{	//printf("1\n");
		ATR[0] = 0x01;
		maxlen = 1;
		nmaxlen = 100;

		// Get all keys A
		ExternAuth[0]  = 0x80;
		ExternAuth[1]  = 0xF8;
		ExternAuth[2]  = 0x00;
		ExternAuth[3]  = 0x8E;
		ExternAuth[4]  = 0x08;
		ExternAuth[5]  = mfuid[0];
		ExternAuth[6]  = mfuid[1];
		ExternAuth[7]  = mfuid[2];
		ExternAuth[8]  = mfuid[3];
		ExternAuth[9]  = readblock[4][13];
		ExternAuth[10] = readblock[4][14];
		ExternAuth[11] = 0x00;
		ExternAuth[12] = 0x00;

		RecData[0] = 1;
		datalen = 1;
		datalen = 13;
		re = 13;
		maxlen = 20;
 
		if( cn670.ISO7816_APDU_Exchange(0, 1, ExternAuth, 13, RecData, datalen, maxlen) != 0)
		{	printf("ERROR Get Key \n");
			return 0x200;
		}

		Sam_Response = ((unsigned short)RecData[datalen-2]<<8) + (unsigned short)RecData[datalen-1];
//		printf("Sam_Response->=[%04X]\n", Sam_Response);

		if(Sam_Response == 0x6110)
		{
//			printf("\n\n Get Last Key \n");

			if( cn670.ISO7816_APDU_Exchange(0, 1, APDU_READ_LAST_KEY_HEX, 5, RecData, datalen, maxlen) != 0)
			{
				printf("ERROR Get Last Key \n");
				return 0x0100;
			}
		}
	}
	else
	{	// carga key default
		for(i=0; i<6; i++)
			RecData[i] = 0xFF;
	}

	for(k=0; k<16; k++)
		for(i=0; i<6; i++)
			if(k!=1)
				MFKEY[k][i] = RecData[i];

	return 0x00;	//OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::Read_MFUID( BYTE MFUID[])
{	int status = -1;
	unsigned char NbTg, drec;
	unsigned char InitData[8];

//	cn670.SetUSBNum(nUSB);

	byRecvData[0] = 0;		// Baud Rate-> 0. 106 kpbs, ISO14443 Type A .
	drec = 1;

	status = cn670.PN_InListPassiveTarget(0, 0, 2, 0, 0, InitData, &NbTg, &drec, byRecvData);

	if(status!= 0)
	{
		printf("Error en Read UID = %02X\n", status);
		return 0x200;
	}
	if(NbTg>1)
	{
		printf("Error Mas de 1 tarjeta = %d \n", NbTg);
		return 0x201;	// Mas de 1 tarjeta
	}

	for(int i=0; i<byRecvData[4];i++)
		MFUID[i] = byRecvData[4+i+1];
	MFUID[4] = MFUID[0];
	MFUID[5] = MFUID[1];

    return 0x00;
}

/******************************************************************************/
/* Lee n Sectores completos, sus 4 blocks 0..3                                */
/******************************************************************************/
int CLASS_CIVINTEC::Read_Sector(int TypeKey, int isec, int fsec, BYTE MFUID[], BYTE pPASS[16][6], BYTE readblock[64][16])
{	unsigned char Buffer[80];
	int i, k, inten;
	BYTE nblock;
	int nsec,re;

	nsec = isec;
	nblock = isec * 4;

	for(nsec=isec; nsec<fsec+1; nsec++)
	{
		nblock = nsec * 4;
		if( Autentifica(TypeKey, nblock, MFUID, pPASS[nsec]) != 0)
		{
			printf("Error al autentificar el sector=%d\n", nsec);
			return 0x200;
		}
		//Lectura de los 3 primeros bloques de cada sector
		memset(Buffer,0x00,64); //Pone en Cero el bufer
		if( cn670.MF_Read(0, nblock , 3, Buffer) != 0)
		{
			printf("Error Read Block=%d\n", nblock);
			return 0x200;
		}
		for(i=0;i<48;i++)
				readblock[nblock][i] = Buffer[i];

		if( Autentifica(TypeKey, nblock+3, MFUID, pPASS[nsec]) != 0)
		{
			printf("Error al autentificar el sector=%d\n", nsec);
			return 0x200;
		}
		nblock += 3;
		//Lectura del 4to block del  sector
		memset(Buffer,0x00,64); //Pone en Cero el bufer
		if( cn670.MF_Read(0, nblock , 1, Buffer) != 0)
		{
			printf("Error Read Block=%d\n", nblock);
			return 0x200;
		}
		for(i=0;i<16;i++)
				readblock[nblock][i] = Buffer[i];		
	}
	return 0x00;
}

/******************************************************************************/
/* Lee el Sector 1 completo  llave UID                                        */
/******************************************************************************/
int CLASS_CIVINTEC::Read_Sector1A(BYTE MFUID[], BYTE pPASS[16][6], BYTE readblock[64][16])
{	unsigned char Buffer[80];
	int i;

	for(i=0; i<6; i++)
		pPASS[1][i] = MFUID[i];

	if( Autentifica(KEYA, 4, MFUID, pPASS[1]) != 0)
	{
		printf("Error al autentificar el sector 1-4\n");
		return 0x200;
	}

	memset(Buffer,0x00,sizeof(Buffer)); //Pone en Cero el bufer
	if( cn670.MF_Read(0, 4 , 3, Buffer) != 0)
	{
		printf("Error Read Block=%d\n", 4);
		return 0x200;
	}

	for(i=0;i<48;i++)
		readblock[4][i] = Buffer[i];

	if( Autentifica(KEYA, 7, MFUID, pPASS[1]) != 0)
	{
		printf("Error al autentificar el sector 1-7\n");
		return 0x200;
	}

	//Lectura del 4to block del  sector
	memset(Buffer, 0x00, sizeof(Buffer)); //Pone en Cero el bufer
	if( cn670.MF_Read(0, 7 , 1, Buffer) != 0)
	{
		printf("Error Read Block=%d\n", 7);
		return 0x200;
	}
	for(i=0;i<16;i++)
		readblock[7][i] = Buffer[i];

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::READ_ALL( int TypeKey,  unsigned char UID[], 
								unsigned char pPASS[16][6], unsigned char ReadBlocks[64][16] )
{	int i;

//	printf("MFUID = [");
//	for(i=0; i<6; i++)
//		printf("%02X ", UID[i]);
//	printf("]\n");

	if(TypeKey!=KEYDEF)
	{
		// Lllave del primer sector
		for(i=0; i<6; i++)
			pPASS[1][i] = UID[i];
		// Read Sector 1
		if( Read_Sector(0, 1, 1, UID, pPASS, ReadBlocks) != 0)
		{
			printf("Error en Read Sector 1\n");
			return 0x0401;		// ERROR READ SECTOR 1
		}
//		printf("Read Sector 1 OK\n");	
	}

	if( GetCardKey ( TypeKey,  UID, pPASS, ReadBlocks) != 0)
	{
		printf("Error en Get Keys\n");
		return 0x0402;	// Error al obtener las llaves de la MIA
	}

	if( Read_Sector( TypeKey, 0, 15, UID, pPASS, ReadBlocks) != 0)
	{
		printf("Error en ReadAll\n");
		return 0x0403;	// Error al leer todos los sectores de la MIA
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::READ_ALL_WITH_KEY( int TypeKey,  unsigned char UID[], unsigned char pPASS[16][6], unsigned char ReadBlocks[64][16] )
{	int i;

	if( Read_Sector( TypeKey, 0, 15, UID, pPASS, ReadBlocks) != 0)
	{
		printf("Error en ReadAll\n");
		return 0x0403;	// Error al leer todos los sectores de la MIA
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::ReadMonederos(int addr, int PurseBlock, int *value1,  int *value2 )
{	unsigned char  *p;



//	if( Read_Sector(KEYA, 3, 3, MIA.MifareUID, MIA.KeyA, MIA.ReadBlocks) != 0)
	{
		printf("Error en Read Sector de Monederos \n");
		return 0x200;
	}	

	p = (unsigned char *)value1;
//	*p++ = MIA.ReadBlocks[13][0]; *p++ = MIA.ReadBlocks[13][1]; *p++ = MIA.ReadBlocks[13][2]; *p++ = MIA.ReadBlocks[13][3];

    p = (unsigned char *)value2;
//	*p++ = MIA.ReadBlocks[14][0]; *p++ = MIA.ReadBlocks[14][1]; *p++ = MIA.ReadBlocks[14][2]; *p++ = MIA.ReadBlocks[14][3];
	
	printf("Monederos 1=%d 2=%d\n\n", *value1, *value2);

	return 0x00;
	
}

/******************************************************************************/
/* Lee el Sector Monedero                                                     */
/******************************************************************************/
int CLASS_CIVINTEC::ReadValue(int TypeKey, int PurseBlock, BYTE MFUID[], BYTE pPASS[16][6] )
{	int i, k;
	int nsec;
	int re;
	int value1, value2;

	printf("\n\n **********************   Read Value  *********************\n");

	
	nsec = PurseBlock / 4;
	printf("S=%d b=%d\n", nsec, PurseBlock);

	if( Autentifica(TypeKey, PurseBlock, MFUID, pPASS[nsec]) != 0)
	{	printf("Error al autentificar el block=%d\n", PurseBlock);
		return 0x200;
	}

//	if( (re=MF_ReadValue(0, PurseBlock, &value)) != 0)
//	{	printf("Error ReadValue=%d\n", re);
//		return 0x200;
//	}

	if(ReadMonederos(0, Purse_1, &value1,  &value2 ) != 0)
	{	printf("Error Read MOnedero\n");
		return 0x200;
	}

	printf("Value1=%d  value2=%d \n ", value1, value2);

    return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::DecrementaBalance(int TypeKey, BYTE MFUID[], int PurseBlock, BYTE pPASS[16][6], int valor )
{	int nsector;


    printf("Inicia Proceso de Decrementar = %d\n", valor);   
	nsector = PurseBlock/4;
	if( Autentifica(TypeKey, PurseBlock, MFUID, pPASS[nsector]) != 0)
	{	printf("Error al autentificar\n");
		return 0x200;
	}

	if( cn670.MF_Decrement(0, PurseBlock, valor) != 0 )
	{	printf("Error al Decrementar \n");
		return 0x200;
	}
	if( cn670.MF_Transfer(0, PurseBlock) != 0)
	{	printf("Error al Transferir \n");
		return 0x200;
	}

	return 0x00;	// OK
}

/******************************************************************************/
/*  Escribe un block en la MIA                                                */
/******************************************************************************/
int CLASS_CIVINTEC::WriteBlock(int TypeKey, int blk, BYTE MFUID[], BYTE pPASS[16][6], BYTE writeblock[64][16])
{	unsigned char Buffer[80];
	int i, k, inten;
	BYTE nblock;
	int nsec,re;


	if(blk==0)				// no escribe en el block 0
	{	printf("blk0\n");
		return 0x00;	
	}
	if(	(blk % 4) == 0x03 )	// no escribe el 3er block de cada sector
	{	printf("blk3\n");
		return 0x00;	// no escribe en el block 0
	}

	nsec = blk / 4;

	printf("wr 2\n");
	if( Autentifica(TypeKey, blk, MFUID, pPASS[nsec]) != 0)
	{
		printf("Error al autentificar el sector=%d\n", nsec);
		return 0x200;
	}

	printf("wr 3\n");
	printf("[block = %d  ]\n", blk);

	if( cn670.MF_Write(0, blk , 1,  writeblock[blk]) != 0)
	{
		printf("Error write Block=%d\n", blk);
		return 0x200;
	}

	printf("wr ok\n");
	return 0x00;	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::Write_Sector(int TypeKey, int isec, int fsec)
{

	return 0;
}
	
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::Incrementa()
{

	return 0;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::InicializaValue()
{

	return 0;
}

/******************************************************************************/
/*                      Autentifica un Sector                                 */
/******************************************************************************/
// CRT_MIFARE_KEY_VERIFICATION, TypeKey, ns, 6, pPASS[ns])
int CLASS_CIVINTEC::Autentifica(int TypeKey, int nblock, BYTE MFUID[], BYTE pPASS[])
{	int i,re;
	unsigned char MFKeyType;

	if( cn670.PN_SelectTarget(0, 1) != 0)
    {   printf("Error en Select\n");
        return 0x202;  //success
    }

//	printf("MFLoadKey\n");
	if( cn670.MF_LoadKey(0, pPASS) !=0 )
	{	printf(" Error LoadKey \n");
		return 0x202;
	}

//	printf("TypeKey=%d\n", TypeKey);
	if((TypeKey == KEYA) || (TypeKey==KEYDEF)) MFKeyType = 0x60; else MFKeyType = 0x61;

	if( (re=cn670.MF_Auth(0, MFKeyType, MFUID, nblock)) != 0)
	{
		printf("Error Auth Block %d %d\n", re, nblock);
		return 0x204;
	}
    return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::ConfigAutoRead(void)
{	int i;

	for(i=0; i<5; i++)
	{	usleep(10000);
		// Habilitar Modo de AutoRead
		if( cn670.SendSetWiegandMode(0) != 0)
		{
			printf("Error SetWiegandMode\n");
			continue;
		}
		usleep(10000);
		if( cn670.SendGetWiegandMode(0) != 0)
		{
			printf("Error GetWiegandMode\n");
			continue;
		}
		usleep(10000);
		if( cn670.SendSetWiegandModeLB(0, 0x10) != 0)
		{
			printf("Error Set AutoRead Enable\n");
			continue;
		}
		return 0;
	}

	return 1;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::ConfigAutoRead1(void)
{	int i;

	usleep(10000);
	// Habilitar Modo de AutoRead
	if( cn670.SendSetWiegandMode(0) != 0)
	{
		printf("Error SetWiegandMode\n");
		return 0x0101;
	}
	usleep(10000);
	if( cn670.SendGetWiegandMode(0) != 0)
	{
		printf("Error GetWiegandMode\n");
		return 0x0102;
	}
	usleep(10000);
	if( cn670.SendSetWiegandModeLB(0, 0x10) != 0)
	{
		printf("Error Set AutoRead Enable\n");
		return 0x0103;
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIVINTEC::ReadUID(unsigned char UID[])
{	int re,k;
	unsigned char brx[100];

	re = cn670.USB_Read2(100, brx, 40);
	if(re>0)
	{	// recibio datos, verificar sequencia
		if(re==11)
		{	if(brx[4]==0)
			{
				UID[0] = brx[5];
				UID[1] = brx[6];
				UID[2] = brx[7];
				UID[3] = brx[8];
				UID[4] = brx[5];
				UID[5] = brx[6];
				return 0x00;
			}
		}
	}
	return 1;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_CIVINTEC::ImpDatosRec(char msg[], int datalen, BYTE datarec[])
{	int  i;

	printf("%s", msg);
	printf("Datos = [");
	for(i=0; i<datalen; i++)
		printf("%02x ", datarec[i]);
	printf("]\n");
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_CIVINTEC::imprime_RdBlocks(BYTE sn, BYTE datos[64][16])
{	int i,k;

	printf("Sector = %d\n", sn);
	for(i=0; i<64; i++)
	{	printf("%02d=", i);
		for(k=0; k<16; k++)
			printf("%02x", datos[i][k]);
		printf("\n");
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_CIVINTEC::imprime_Key(BYTE typeKey, BYTE datos[16][6])
{	int i,k;

	printf("Key = %d\n", typeKey);
	for(i=0; i<16; i++)
	{	printf("%02d = ", i);
		for(k=0; k<6; k++)
			printf("%02X ", datos[i][k]);
		printf("\n");
	}
}
/******************************************************************************/