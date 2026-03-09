/* 
 * File:   CRT603vx.cpp
 * Author: Elias Gonzalez
 * 
 * Created on February 13, 2023, 7:29 PM
 */

//#include "CRT603vx.hpp"

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
CLASS_CRT603vx::CLASS_CRT603vx() 
{

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::InitComm(char *SerialComName, speed_t speed)
{
	ReaderClose(argc,argv);

	int i= 0;
	int iMode = CRT_OPEN_TYPERS232;
	int oldMode = g_iMode;
	g_iMode = iMode;

	iRet = RS232_OpenWithBaudRate(SerialComName, speed);
	if(iRet != 0)
	{
		printf("Error al abrir puerto serial %s\n",SerialComName);
		return iRet;
	}
	g_bOpen = true;

	iRet = SetLed(0x01);
	
//	return iRet;

	iRet = ReaderBuzzer(argc,argv,1,0,0);
	if(iRet != 0)
	{
		printf("Error Buzzer\n");
	//     ReaderClose(argc,argv);
		g_iMode = oldMode;
	}
	usleep(200000);	// espera fin del bbuzzer
	SetLed(0x00);

	return iRet;

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::ReaderConnect()
{
	iSendLen = 0;
	bySendData[iSendLen++] = 0x02;
	bySendData[iSendLen++] = 0x03;

	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return -2;

	memcpy(ReaderInfo.ReaderVersion, byRecvData+1, iRecvLen-2);
	printf("Reader Version:%s",ReaderInfo.ReaderVersion);

    return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void  CLASS_CRT603vx::GetResponse(BYTE *recB, int sizB)
{	BYTE k;

	memset(MF_Data, 0x00, sizeof(MF_Data));

	if(sizB < 4)
	{
		printf("\n\n ERROR EN sisB= %d   *********************************\n\n", sizB);
		MF_Response = 0xFFFF;
		return;
	}
	
	MF_Response = (recB[sizB-3]*0x100) + (recB[sizB-2]);

	for(k=0; k< (sizB-4); k++)
		MF_Data[k] = recB[k+1];

	printf("RDR_RESPONSE = %04x\n", MF_Response);
	printRec(MF_Data, sizB-4, "RESPONSE");
	
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::AutentificaSAM(BYTE slot, BYTE keytype)
{	char k;

	printf("\n********  AUTENTIFICA SAM %d Key=%d******\n\n", slot, keytype);

	iRet = GetSamInterface(slot);
	if(iRet!=0)
	{
		printf("SAM NO PRESENTE\n");
		return iRet;
	}

	printf("====================================================================\n");
	printf("SELECT PIN\n");
	if(keytype == PSAM)
		iRet = SamTransmit(APDU_SELECT_PIN_PSAM_HEX, sizeof(APDU_SELECT_PIN_PSAM_HEX), slot);
	else
		iRet = SamTransmit(APDU_SELECT_PIN_ISAM_HEX, sizeof(APDU_SELECT_PIN_ISAM_HEX), slot);

	if(iRet != 0x00)
	{
		printf("Error en SELECT PIN\n");
		return iRet;
	}
	printRec(byRecvData, iRecvLen, "Received");
	printf("SELECT PIN OK\n");
	printf("====================================================================\n");

	printf("\nVERIFY PIN\n");
	iRet = SamTransmit(APDU_VERIFY_PIN_HEX, sizeof(APDU_VERIFY_PIN_HEX), slot);

	if(iRet != 0x00)
	{
		printf("Error en SELECT PIN\n");
		return iRet;
	}
	printRec(byRecvData, iRecvLen, "Received");
	printf("VERIFY PIN OK\n");
	printf("====================================================================\n");

	printf("\nGET CHALLENGE\n");
	iRet = SamTransmit(APDU_GET_CHALLENGE_HEX, sizeof(APDU_GET_CHALLENGE_HEX), slot);

	if(iRet != 0x00)
	{
		printf("Error en GET CHALLENGUE\n");
		return iRet;
	}
	printRec(byRecvData, iRecvLen, "Received");
	printf("GET CHALLENGUE OK\n");

	GetResponse(byRecvData, iRecvLen);

	printf("====================================================================\n");

	Three_DES_Encrypt(MF_Data, byRecvData, (int)keytype);

	memcpy(Challenge, byRecvData, 8);

	printf("DES ENCRIPT OK\n");
	printRec(byRecvData, 8, "Received");

	for(k=0; k<8;k++)
		APDU_AUTH_SAM_HEX[5+k] = Challenge[k];
	printRec(APDU_AUTH_SAM_HEX, sizeof(APDU_AUTH_SAM_HEX), "AUTH SAM");

	printf("\nAUTENTIFICAION DEL SAM SLot=%d\n",slot);
	iRet = SamTransmit(APDU_AUTH_SAM_HEX, sizeof(APDU_AUTH_SAM_HEX), slot);
	if(iRet != 0x00)
	{
		printf("Error en Autentificacion\n");
		return iRet;
	}
	printRec(byRecvData, iRecvLen, "Received");
	printf("Autentificacion OK\n");

	GetResponse(byRecvData, iRecvLen);

	printf("====================================================================\n");
	printf("\nGET UID SAM\n");
	
	iRet = SamTransmit(APDU_GET_UID_SAM_HEX, sizeof(APDU_GET_UID_SAM_HEX), slot);
	if(iRet != 0x00)
	{
		printf("Error GET UID SAM\n");
		return iRet;
	}

	printRec(byRecvData, iRecvLen, "Received");
	printf("GET UID SAM OK\n");
	GetResponse(byRecvData, iRecvLen);

	printf("====================================================================\n");
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::GetSamInterface(int pSlot)
{

//    unsigned char byRecvData[1024] = {0};
//    int iRecvLen = 0;

	printf("GetSamInterface Slot=%d\n", pSlot);

	bySendData[ 0] = 0x02;							// total de bytes
	bySendData[ 1] = (pSlot==Slot_1)?0x51: 0x52;	// SAM 1/2 card 1reset
													// parity byte
	iSendLen = 2;

	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);

	if(iRet != 0)
		return iRet;

	for(int i = 0; i < iRecvLen; i++)
		printf("%02X ",byRecvData[i]);
	printf("\n");

    return 0;
}

/******************************************************************************/
/* NumBytes - 0x53 - APDU - Parity byte                                       */
/******************************************************************************/
int CLASS_CRT603vx::SamTransmit(unsigned char *pAPDU,int pAPDULen,int pSlot)
{
	unsigned char bufCmd[100];
	unsigned char buflen, k;

    printf("Sam Transmit APDU Slot=%d\n ", pSlot);

	memset(bufCmd,0x00, sizeof(bufCmd));

	buflen = pAPDULen + 3;

	bufCmd[ 0] = buflen;							// total de bytes
	bufCmd[ 1] = (pSlot==Slot_1)?0x53:0x54;;		// SAM 1/2
	for(k=0; k<pAPDULen; k++)
		bufCmd[2+k] = pAPDU[k];						// APDU
													// Parity Byte
	iSendLen = buflen;
	
    for(int i = 0; i < buflen; i++)
        printf("%02X ",bufCmd[i]);
    printf("\n");

	
	iSendLen = buflen;
	memcpy(bySendData, bufCmd, buflen);

	iRet = CRT_ExeCommand(iSendLen, bySendData, &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;

        for(int i = 0; i < iRecvLen; i++)
            printf("%02X ",byRecvData[i]);
            
        printf("\n");

  
    return 0x00;
}	

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::GetLastKey(BYTE slot)
{
	printf("********************************************************************\n");
	printf("Get LastKey\n");

	iRet = SamTransmit(APDU_READ_LAST_KEY_HEX, sizeof(APDU_READ_LAST_KEY_HEX), slot);
	if(iRet != 0x00)
	{
		printf("Error en GetLastKey=%d\n", iRecvLen);
		return iRet;
	}
	printRec(byRecvData, iRecvLen, "Received");
	printf("GetLastKey OK\n");

	GetResponse(byRecvData, iRecvLen);

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::GetCardKey(BYTE slot, unsigned char mfuid[], unsigned char MFKEY[16][6], unsigned char readblock[64][16] )
{	BYTE k,i;

	printf("====================================================================\n");
	printf("GetCardKey\n");

	APDU_MFGET_KEY_HEX[ 5] = mfuid[0];
	APDU_MFGET_KEY_HEX[ 6] = mfuid[1];
	APDU_MFGET_KEY_HEX[ 7] = mfuid[2];
	APDU_MFGET_KEY_HEX[ 8] = mfuid[3];
	APDU_MFGET_KEY_HEX[ 9] = readblock[4][13];
	APDU_MFGET_KEY_HEX[10] = readblock[4][14];

	iRet = SamTransmit(APDU_MFGET_KEY_HEX, sizeof(APDU_MFGET_KEY_HEX), slot);
	if(iRet != 0x00)
	{
		printf("Error en GetCardKey=%d\n", iRecvLen);
		return iRet;
	}
	printRec(byRecvData, iRecvLen, "Received");
	printf("GetCardKey OK\n");

	GetResponse(byRecvData, iRecvLen);

	iRet= GetLastKey(slot);
	if(iRet != 0x00)
	{
		printf("Error en GetLastKey=%d\n", iRecvLen);
		return iRet;
	}

	for(k=0; k<16; k++)
	{	for(i=0; i<6; i++)
//			if( k!=1 )
				MFKEY[k][i] = MF_Data[i];
	}
	printAllKey(MFKEY);
	printf("====================================================================\n");

	return iRet;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::MifareGetUID(unsigned char *UID)
{
	iSendLen = 0;
	bySendData[iSendLen++] = 0x03;		// total de bytes del cmd
	bySendData[iSendLen++] = 0x20;		// Seek Card
	bySendData[iSendLen++] = 0x00;		// Mode =0-> seek all cards
										// Parity Byte
	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;

	ReaderInfo.CSNLen = 4;
	memset(ReaderInfo.CSN,0x00,8);
	for(int i = 0; i < ReaderInfo.CSNLen; i++)
		MF_UID[i] = ReaderInfo.CSN[i] = byRecvData[i + 1];

	// LLave Sector 1 PSAM
	MF_UID[4] = MF_UID[0];
	MF_UID[5] = MF_UID[1];

	for(int i = 0; i < 6; i++)
		UID[i] = MF_UID[i];

	printf("UID ok\n");
	
    return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::MF_RdBlock (byte iblk, byte tkey, byte *pass)
{	int i;

	printf("Read block\n");

	unsigned char byRecvData[1024] = {0};

	if( (iblk % 4)==3)
		return 0x00;

	int iRecvLen = 0;

	bySendData[ 0] = 0x0A;		// total de bytes
	bySendData[ 1] = 0x21;		// Read block
	bySendData[ 2] = (tkey==KEYA)?0:1;
	bySendData[ 3] = iblk;
	bySendData[ 4] = 0x99;		//pass[0];	// Key
	bySendData[ 5] = 0x88;		//pass[1];
	bySendData[ 6] = 0x85;		//pass[2];
	bySendData[ 7] = 0xC9;		//pass[3];
	bySendData[ 8] = 0x99;		//pass[4];
	bySendData[ 9] = 0x88;		//pass[5];
	iSendLen = 10;

	for(int i = 0; i < iSendLen; i++)
            printf("%02X ",bySendData[i]);
	printf("\n");
 
	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
//	if(iRet != 0)
//		return iRet;

	
	for(i = 0; i < iRecvLen; i++)
            printf("%02X ",byRecvData[i]);
	printf("\n");

	ReaderInfo.DataLen = 16;
	memset(ReaderInfo.Data,0x00,100);
	for(int i = 0; i < ReaderInfo.DataLen; i++)
            ReaderInfo.Data[i] = byRecvData[i+1];
	ReaderInfo.DataLen = ReaderInfo.DataLen;

	for(i=0; i<16; i++)
		MIA_DATA[iblk][i] = ReaderInfo.Data[i];
	
	
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::MF_RdSector (byte isec, byte tkey, byte *pass, byte readblock[64][16])
{	int i;

	printf("Read Sector = %d-%d\n", isec, tkey );

	unsigned char byRecvData[1024] = {0};

	int iRecvLen = 0;

	bySendData[ 0] = 0x0A;		// total de bytes
	bySendData[ 1] = 0x29;		// Read Sector
	bySendData[ 2] = (tkey==KEYA)?0:1;
	bySendData[ 3] = isec;
	bySendData[ 4] = pass[0];	// Key
	bySendData[ 5] = pass[1];
	bySendData[ 6] = pass[2];
	bySendData[ 7] = pass[3];
	bySendData[ 8] = pass[4];
	bySendData[ 9] = pass[5];
	iSendLen = 10;

	for(int i = 0; i < iSendLen; i++)
            printf("%02X ",bySendData[i]);
	printf("\n");
 
	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;

	for(i = 0; i < iRecvLen; i++)
            printf("%02X ",byRecvData[i]);
	printf("\n");

	ReaderInfo.DataLen = 64;
	memset(ReaderInfo.Data,0x00,100);
	for(int i = 0; i < ReaderInfo.DataLen; i++)
            ReaderInfo.Data[i] = byRecvData[i+1];
	ReaderInfo.DataLen = ReaderInfo.DataLen;

	for(i=0; i<64; i++)
	{	readblock[isec*4][i] = ReaderInfo.Data[i];
		printf("-%02x", readblock[isec][i] );
	}
	printf("\n");
	
	return 0;
}


/******************************************************************************/
/*  0x1A
0x22
Secret key ID
Block NO.
Secret key
dada
Parity                                                                        */
/******************************************************************************/
int CLASS_CRT603vx::MF_WrBlock (byte iblk, byte tkey, byte *pass, byte *buf)
{	int i;

	unsigned char byRecvData[1024] = {0};

	if( (iblk % 4)==3)
		return 0x00;

	int iRecvLen = 0;

	bySendData[ 0] = 0x1A;					// total de bytes
	bySendData[ 1] = CRT_WriteBlock;		// Write Block block
	bySendData[ 2] = (tkey==PSAM)?0:1;
	bySendData[ 3] = iblk;
	for(i=0; i<6; i++)
	bySendData[ 4+i] = pass[i];	// Key
	for(i=0; i<16; i++)
	bySendData[ 10+i] = buf[i];	// Data
								// Parity Byte
	iSendLen = bySendData[ 0];

	for(int i = 0; i < iSendLen; i++)
            printf("%02X ",bySendData[i]);
	printf("\n");
 
	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;
	
	return 0;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::MifareLoadKey(int argc, char** argv,int iKeyId, int iKeyLen, unsigned char byKeyData[])
{
	unsigned char bySendData[1024] = {0};
	unsigned char byRecvData[1024] = {0};
	int iSendLen = 0;
	int iRecvLen = 0;

	bySendData[iSendLen++] = 0x09;
	bySendData[iSendLen++] = 0x2D;
	bySendData[iSendLen++] = iKeyId;
	
	unsigned char byTempData[1024] = {0};
	int iTempLen = 0;
	//if(IsAscData(byKeyData, iKeyLen))
	//{
	//	iTempLen = iKeyLen/2;
	//	MyAsc2Bcd(byKeyData, iKeyLen, byTempData);
	//}
	//else
	//{
		iTempLen = iKeyLen;
		memcpy(byTempData, byKeyData, iKeyLen);
	//}
	memcpy(bySendData+iSendLen, byTempData, iTempLen);
	iSendLen += iTempLen;

	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;

        ReaderInfo.DataLen = 3;
	memset(ReaderInfo.Data,0x00,100);
	for(int i = 0; i < ReaderInfo.DataLen; i++)
		ReaderInfo.Data[i] = byRecvData[i];
	ReaderInfo.DataLen = ReaderInfo.DataLen;
	
        
	return 0;
}


//int CLASS_CRT603vx::MF_RdSector (byte nsec, byte tkey, byte *pass)
//{	
//	return 0x00;
//}



int CLASS_CRT603vx::MifareRead(int pSector, int pBlock,int iKeyType, unsigned char * pMifareKey)
{
    int iMode = 0;
    int iKeyLen = 6;
	unsigned char bySendData[1024] = {0};
	unsigned char byRecvData[1024] = {0};
	int iSendLen = 0;
	int iRecvLen = 0;

	bySendData[iSendLen++] = 0x0A;
	bySendData[iSendLen++] = 0x21;
	bySendData[iSendLen++] = ((iKeyType==PSAM))?0:1;

//		bySendData[iSendLen++] = (iKeyType&0x01) | (iMode&0x01)<<1;
	bySendData[iSendLen++] = pSector*4 + pBlock;

	unsigned char byTempData[1024] = {0};
	int iTempLen = 0;
	//if(IsAscData(byKeyData, iKeyLen))
	//{
	//	iTempLen = iKeyLen/2;
	//	MyAsc2Bcd(byKeyData, iKeyLen, byTempData);
	//}
	//else
	//{
		iTempLen = iKeyLen;
		memcpy(byTempData, pMifareKey, iKeyLen);
	//}
	memcpy(bySendData+iSendLen, byTempData, iTempLen);
	iSendLen += iTempLen;
        
	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;
		
	ReaderInfo.DataLen = 16;
	memset(ReaderInfo.Data,0x00,100);
	for(int i = 0; i < ReaderInfo.DataLen; i++)
            ReaderInfo.Data[i] = byRecvData[i+1];
	ReaderInfo.DataLen = ReaderInfo.DataLen;
	
	//if(memcmp(Response_Fail,Response,ResLen) == 0){
        //    cout << "Read block " << pBlock << " fail" << endl;
        //    return -2; // Error de authentificacion
        //}
        
	return 0;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::READ_ALL_WITH_KEY( int TypeKey,  unsigned char UID[], unsigned char pPASS[16][6], unsigned char ReadBlocks[64][16] )
{	int i;

	for(i=0; i<16; i++)	
		if( MF_RdSector (i, KEYB, pPASS[i], ReadBlocks) != 0)
		{
			printf("Error en ReadAll\n");
			return 0x0403;	// Error al leer todos los sectores de la MIA
		}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int ccMF_RdBlock (byte iblk, byte fblk, byte tkey, byte *pass)
{
	
	
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::ReaderClose(int argc, char** argv)
{
	g_bOpen = false;

	if(g_iMode == 0)
	{
		USB_Close();
	}
	else if(g_iMode == 1)
	{
		RS232_Close();
	}

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::CRT_ExeCommand(int iSendDataLen, unsigned char bySendData[], int* iRecvDataLen, unsigned char byRecvData[])
{	int iRet = 0;

	unsigned char TempRecvBuf[1024] = {0};
	int iTempRecvLen = 0;

	unsigned char TempSendBuf[1024] = {0};
	int iTempSendLen = 0;

	if(bySendData == NULL )
		return -1;

	bySendData[iSendLen++] = CalXOR(bySendData,iSendLen);

//	printBuf(bySendData, iSendLen, "Tx=");

	if(!g_bOpen)
		return -1;

	iRecvLen = 0;
	if(RS232_Write(iSendLen, bySendData) != 0)
		return -1;

	if(byRecvData == NULL)
	    return CRT603_ERR_CMDPARAM ;

	if(Recv_Data_RS232(iRecvDataLen, byRecvData) != 0)
		return -1;

	if(iRet != 0)
	   return iRet;


	if (iTempRecvLen>0 )
	{
	    *iRecvDataLen = iTempRecvLen;
	    for (int i=0; i<iTempRecvLen; i++)
		byRecvData[i]=TempRecvBuf[i];
	}
 
    if((*iRecvDataLen==0xFF)&&(byRecvData[(*iRecvDataLen-2)]!=0x90))
    {
        memset(bySendData,0,sizeof((byte *)bySendData));
        memset(TempSendBuf,0,sizeof(TempSendBuf));
        bySendData[0]=0x02;
        bySendData[1]=0xF0;
        OrganizePacket(2,bySendData, &iTempSendLen, TempSendBuf);

		if(!g_bOpen)
			return -1;

		if(RS232_Write(iTempSendLen, TempSendBuf) != 0)
			return -1;

		if(byRecvData == NULL)
			return CRT603_ERR_CMDPARAM ;

		if(Recv_Data_RS232(iRecvDataLen, byRecvData) != 0)
			return -1;

		if(iRet != 0)
			return iRet;

		if (iTempRecvLen>0 )
		{
			*iRecvDataLen = iTempRecvLen;
			for (int i=0; i<iTempRecvLen; i++)
			byRecvData[i]=TempRecvBuf[i];
		}

    }
	if(bySendData[1] != byRecvData[0])
		return -2;

    return iRet;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::ReaderBuzzer(int argc, char** argv,int pTime,int pFrecuency,int pDutyCycle)
{
	iSendLen = 0;
	bySendData[iSendLen++] = 0x03;		// total de bytes
	bySendData[iSendLen++] = 0x14;		// Buzzer setting
	bySendData[iSendLen++] = pTime*10;	// parametros para el buzzer
										// parity byte
	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;
	
	usleep(100000);
    return 0;	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::SetLed(bool bStatus)
{
	iSendLen = 0;
	bySendData[iSendLen++] = 0x03;		// total de bytes del cmd
	bySendData[iSendLen++] = 0x13;		// LED Indicator setting
	bySendData[iSendLen++] = bStatus;

	// parity byte
	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_CRT603vx::PrintUID(void)
{	char k;

	printf("***********************MIFARE UID*********************************\n");
	for(k=0; k<6; k++)
		printf("%02X ", MF_UID[k] );
	printf("\n***************************************************************\n");
	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_CRT603vx::printRec(BYTE *recB, int sizB, const char *text)
{	int i;

	printf("%s [%d: ", text, sizB);
	for (i=0; i<sizB; i++)
		printf("%02X ", recB[i]);
	printf("]\n");
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_CRT603vx::printBuf(BYTE *recB, int sizB, const char *text)
{	int i;

	printf("%s [%d -> ", text, sizB);
	for (i=0; i<sizB; i++)
		printf("%02X ", recB[i]);
	printf("]\n");
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_CRT603vx::printAllMIA(void)
{	byte k,j;

	printf("\n");
	printf("************************************************\n");
	for(k=0; k<63; k++)
	{	printf("B%02d = ",k);
		for(j=0; j<16; j++)
			printf("%02X ", MIA_DATA[k][j]);
		printf("\n");
	
	}	
	printf("************************************************\n");
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_CRT603vx::printAllKey(BYTE key[16][6])
{	byte k,j;


	printf("\n");
	printf("************************************************\n");
	for(k=0; k<16; k++)
	{	printf("B%02d = ",k);
		for(j=0; j<6; j++)
			printf("%02X ", key[k][j]);
		printf("\n");
	
	}	
	printf("************************************************\n");
}

////////////////////////////////////////////////////////////////////////////////




int CLASS_CRT603vx::CRT_Write(unsigned int length,unsigned char buffer[])
{
	if(!g_bOpen)
	{
	    printf("dev is not open!\n");
	    return -1;
	}

	if(g_iMode == 0)
	{
	    return USB_Write(length, buffer);
	}
	else if(g_iMode == 1)
	{
	    return RS232_Write(length, buffer);
	}
	else
	{
	    printf("CRT_Write err mode is unknow\n");
	    return -1;
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::CRT_Read(unsigned int length,unsigned char buffer[])
{
	if(!g_bOpen)	// 	dev is not open!
	     return -1;

	if(g_iMode == 0)
	     return USB_Read(length, buffer);
	else if(g_iMode == 1)
	     return RS232_Read(length, buffer);		//length, buffer);
	else
	     return -1;		//CRT_Read err mode is unknow
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned char CLASS_CRT603vx::CalXOR(unsigned char strOrder[],unsigned int strLen)
{
	unsigned char XORValue=0;
	int ii = 0;
	for(ii=0;ii<strLen;ii++)
	{
		XORValue^=strOrder[ii];
	}
	return XORValue;
}


int CLASS_CRT603vx::Recv_Data(int *iRecvDataLen, unsigned char byRecvData[])
{
	if(g_iMode == 0)
	{
	    return Recv_Data_USB(iRecvDataLen, byRecvData);
	}
	else if(g_iMode == 1)
	{
	    return Recv_Data_RS232(iRecvDataLen, byRecvData);
	}
	else
	{
	    printf("Unknow Mode :%d",g_iMode);
	    return -99;
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CRT603vx::Recv_Data_RS232(int *iRecvDataLen, unsigned char byRecvData[])
{	int ret;

	unsigned char TempRecvBuf[1024] = {0};
	int iTempRecvLen = 0;

	iTempRecvLen = 1;
	ret = CRT_Read(iTempRecvLen, TempRecvBuf);
	if(ret < 0)
	{
		printf("Recv_Data_RS232 Read head err  \n");
		return -2;
	}

	iTempRecvLen = TempRecvBuf[0];
	memset(TempRecvBuf, 0x00, sizeof(TempRecvBuf));

	ret = CRT_Read(iTempRecvLen, TempRecvBuf);  //recv packet data
	if(ret < 0)
	{
		printf("Recv_Data_RS232 recv packet data err  \n");
		return -2;
	}

	*iRecvDataLen = iTempRecvLen;
	memcpy(byRecvData, TempRecvBuf, iTempRecvLen);

	return 0;
}


int CLASS_CRT603vx::Recv_Data_USB(int *iRecvDataLen, unsigned char byRecvData[])
{
	unsigned char TempRecvBuf[1024] = {0};
	int iTempRecvLen = 64;

	iRet = CRT_Read(iTempRecvLen, TempRecvBuf);
	if(iRet < 0)
	{
		printf("Recv_Data_USB Read head err  \n");
		return iRet;
	}
	
	int iGetLen = TempRecvBuf[0];
	int ii = 1;
	while(1)
	{
		if(iGetLen < iTempRecvLen*ii)
		{
			break;
		}

		iRet = CRT_Read(iTempRecvLen, TempRecvBuf+iTempRecvLen*ii);
		if(iRet < 0)
		{
			printf("Recv_Data_USB Read head err  \n");
			return iRet;
		}
		
		ii++;
		
	}

	*iRecvDataLen = iGetLen;
	memcpy(byRecvData, TempRecvBuf+1, iGetLen);
	return 0;
}



int CLASS_CRT603vx::OrganizePacket(int iSendDataLen, unsigned char bySendData[], int* iOutPacketLen, unsigned char byOutPacketData[])
{
	if(bySendData == NULL )
	{
		printf("bySendData is NULL \n");
		return -1;
	}

	int iLen = 0;
	for (iLen=0;iLen<iSendDataLen;iLen++)
	{
		byOutPacketData[iLen]=bySendData[iLen];
	}
	byOutPacketData[iLen++] = CalXOR(byOutPacketData,iLen);
	*iOutPacketLen = iLen;
	
	return 0;	
}


//解析包
int CLASS_CRT603vx::AnalysisPacket(int *iRecvDataLen, unsigned char byRecvData[])
{
	if(byRecvData == NULL)
	{
	    printf("byRecvData or  bystCode is NULL \n");
	    return CRT603_ERR_CMDPARAM ;
	}
	unsigned char TempRecvBuf[1024] = {0};
	int iTempRecvLen = 0;

	int i = 0;

	iRet = Recv_Data(&iTempRecvLen, TempRecvBuf);
	if(iRet != 0)
	{
	   return iRet;
	}

	if (iTempRecvLen>0 )
	{
	    *iRecvDataLen = iTempRecvLen;
	    for (i=0;i<iTempRecvLen;i++)
		byRecvData[i]=TempRecvBuf[i];
	}

	return 0;
}

int CLASS_CRT603vx::ReaderInitialization(int argc, char** argv, speed_t speed)
{
    ReaderClose(argc,argv);
    int i= 0;
    int iMode = 1;
    int oldMode = g_iMode;
    g_iMode = iMode;

    if(iMode == CRT_OPEN_TYPEUSB)
    {
        //printf("CRT_OpenDev USB \n");
        //iRet = USB_Open();
    }
    else if(iMode == CRT_OPEN_TYPERS232)
    {	
        //printf("CRT_OpenDev ittysid: %s  ibaudrate: %d！\n",nttys, iBaudRate);
        iRet = RS232_OpenWithBaudRate(SerialComName, speed);
    }
    else
    {
         printf("Error al abrir puerto serial %s\n",SerialComName);
         return -1;
    }


    if(iRet != 0)
    {
        return iRet;
    }

    g_bOpen = true;
    iRet = ReaderBuzzer(argc,argv,1,0,0);

    if(iRet != 0)
    {
        ReaderClose(argc,argv);
        g_iMode = oldMode;
    }

    return iRet;
}


int CLASS_CRT603vx::MifareWrite(int argc, char** argv,int pSector,int pBlock, unsigned char * pDataToWrite,int iKeyType,unsigned char byKeyData[])
{
    
    int iMode = 0;
    int iKeyLen = 6;
	unsigned char bySendData[1024] = {0};
	unsigned char byRecvData[1024] = {0};
	int iSendLen = 0;
	int iRecvLen = 0;
    int iDataLen = 16;
        
	bySendData[iSendLen++] = 0x1A;
	bySendData[iSendLen++] = 0x22;
	bySendData[iSendLen++] = (iKeyType&0x01) | (iMode&0x01)<<1;
	bySendData[iSendLen++] = pSector*4 + pBlock;
	
	unsigned char byTempData[1024] = {0};
	int iTempLen = 0;
	//if(IsAscData(byKeyData, iKeyLen))
	//{
	//	iTempLen = iKeyLen/2;
	//	MyAsc2Bcd(byKeyData, iKeyLen, byTempData);
	//}
	//else
	//{
		iTempLen = iKeyLen;
		memcpy(byTempData, byKeyData, iKeyLen);
	//}
	memcpy(bySendData+iSendLen, byTempData, iTempLen);
	iSendLen += iTempLen;

	memset(byTempData, 0, sizeof(byTempData));
	iTempLen = 0;
	//if(IsAscData(byInData, iDataLen))
	//{
        //    iTempLen = iDataLen/2;
        //    MyAsc2Bcd(byInData, iDataLen, byTempData);
	//}
	//else
	//{
            iTempLen = iDataLen;
            memcpy(byTempData, pDataToWrite, iDataLen);
	//}
	memcpy(bySendData+iSendLen, byTempData, iTempLen);
	iSendLen += iTempLen;

	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;
		
	ReaderInfo.DataLen = 4;
	memset(ReaderInfo.Data,0x00,100);
	for(int i = 0; i < ReaderInfo.DataLen; i++)
            ReaderInfo.Data[i] = byRecvData[i];
	ReaderInfo.DataLen = ReaderInfo.DataLen;

	return 0;
}

int CLASS_CRT603vx::MifareCopyValue(int argc, char** argv,int pSector, int pSourceValueBlock, int pTargetValueBlock,int iKeyType, unsigned char byKeyData[])
{
    int iMode = 0;
    int iKeyLen = 6;
	unsigned char bySendData[1024] = {0};
	unsigned char byRecvData[1024] = {0};
	int iSendLen = 0;
	int iRecvLen = 0;

	bySendData[iSendLen++] = 0x0B;
	bySendData[iSendLen++] = 0x27;
	bySendData[iSendLen++] = (iKeyType&0x01) | (iMode&0x01)<<1;
	bySendData[iSendLen++] = pSector*4 + pSourceValueBlock;
	bySendData[iSendLen++] = pSector*4 + pTargetValueBlock;	

	unsigned char byTempData[1024] = {0};
	int iTempLen = 0;
	//if(IsAscData(byKeyData, iKeyLen))
	//{
	//	iTempLen = iKeyLen/2;
	//	MyAsc2Bcd(byKeyData, iKeyLen, byTempData);
	//}
	//else
	//{
		iTempLen = iKeyLen;
		memcpy(byTempData, byKeyData, iKeyLen);
	//}
	memcpy(bySendData+iSendLen, byTempData, iTempLen);
	iSendLen += iTempLen;

	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;

	ReaderInfo.DataLen = iRecvLen;
	memset(ReaderInfo.Data,0x00,100);
	for(int i = 0; i < ReaderInfo.DataLen; i++)
		ReaderInfo.Data[i] = byRecvData[i];
	ReaderInfo.DataLen = ReaderInfo.DataLen;

	return 0;
}

int CLASS_CRT603vx::MifareDecrementValue(int argc, char** argv,int pSector, int pValueBlock, unsigned long pValue,int iKeyType,  unsigned char byKeyData[])
{
    int iKeyLen = 6;
    int iMode = 0;
	unsigned char bySendData[1024] = {0};
	unsigned char byRecvData[1024] = {0};
	int iSendLen = 0;
	int iRecvLen = 0;

	bySendData[iSendLen++] = 0x0E;
	bySendData[iSendLen++] = 0x26;
	bySendData[iSendLen++] = (iKeyType&0x01) | (iMode&0x01)<<1;
	bySendData[iSendLen++] = pSector*4 + pValueBlock;
	
	unsigned char byTempData[1024] = {0};
	int iTempLen = 0;
	//if(IsAscData(byKeyData, iKeyLen))
	//{
	//	iTempLen = iKeyLen/2;
	//	MyAsc2Bcd(byKeyData, iKeyLen, byTempData);
	//}
	//else
	//{
		iTempLen = iKeyLen;
		memcpy(byTempData, byKeyData, iKeyLen);
	//}
	memcpy(bySendData+iSendLen, byTempData, iTempLen);
	iSendLen += iTempLen;

	memcpy(bySendData+iSendLen, &pValue, sizeof(pValue));
	iSendLen += sizeof(pValue);

	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;
		
	ReaderInfo.DataLen = iRecvLen;
	memset(ReaderInfo.Data,0x00,100);
	for(int i = 0; i < ReaderInfo.DataLen; i++)
		ReaderInfo.Data[i] = byRecvData[i];
	ReaderInfo.DataLen = ReaderInfo.DataLen;

	return 0;
}


int CLASS_CRT603vx::MifareIncrementValue(int argc, char** argv,int pSector, int pValueBlock, unsigned long pValue,int iKeyType,  unsigned char byKeyData[])
{
    int iKeyLen = 6;
    int iMode = 0;
    unsigned char bySendData[1024] = {0};
    unsigned char byRecvData[1024] = {0};
    int iSendLen = 0;
    int iRecvLen = 0;

	bySendData[iSendLen++] = 0x0E;
	bySendData[iSendLen++] = 0x25;
	bySendData[iSendLen++] = (iKeyType&0x01) | (iMode&0x01)<<1;
	bySendData[iSendLen++] = pSector*4 + pValueBlock;
	
	unsigned char byTempData[1024] = {0};
	int iTempLen = 0;
	//if(IsAscData(byKeyData, iKeyLen))
	//{
	//	iTempLen = iKeyLen/2;
	//	MyAsc2Bcd(byKeyData, iKeyLen, byTempData);
	//}
	//else
	//{
		iTempLen = iKeyLen;
		memcpy(byTempData, byKeyData, iKeyLen);
	//}
	memcpy(bySendData+iSendLen, byTempData, iTempLen);
	iSendLen += iTempLen;

	memcpy(bySendData+iSendLen, &pValue, sizeof(pValue));
	iSendLen += sizeof(pValue);

	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;
		
	ReaderInfo.DataLen = iRecvLen;
	memset(ReaderInfo.Data,0x00,100);
	for(int i = 0; i < ReaderInfo.DataLen; i++)
		ReaderInfo.Data[i] = byRecvData[i];
	ReaderInfo.DataLen = ReaderInfo.DataLen;

	return 0;
}

/**/
int CLASS_CRT603vx::MifareReadSector(int iKeyType, int iMode, int iSector, int iKeyLen, unsigned char byKeyData[])
{
	unsigned char bySendData[1024] = {0};
	unsigned char byRecvData[1024] = {0};
	int iSendLen = 0;
	int iRecvLen = 0;

	bySendData[iSendLen++] = 0x0A;
	bySendData[iSendLen++] = 0x29;
	bySendData[iSendLen++] = (iKeyType&0x01) | (iMode&0x01)<<1;
	bySendData[iSendLen++] = iSector;
	
	unsigned char byTempData[1024] = {0};
	int iTempLen = 0;
	//if(IsAscData(byKeyData, iKeyLen))
	//{
	//	iTempLen = iKeyLen/2;
	//	MyAsc2Bcd(byKeyData, iKeyLen, byTempData);
	//}
	//else
	//{
		iTempLen = iKeyLen;
		memcpy(byTempData, byKeyData, iKeyLen);
	//}
	memcpy(bySendData+iSendLen, byTempData, iTempLen);
	iSendLen += iTempLen;

	//int i=0;
	//for(i=0;i<iSendLen;i++)
	//{
	//	printf("%02X", bySendData[i]);
	//}
	//printf("\n");

	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;

	ReaderInfo.DataLen = iRecvLen;
	memset(ReaderInfo.Data,0x00,100);
	for(int i = 0; i < ReaderInfo.DataLen; i++)
		ReaderInfo.Data[i] = byRecvData[i+1];
	ReaderInfo.DataLen = ReaderInfo.DataLen;

	return 0;
}


 
int CLASS_CRT603vx::MifareLoginA(int argc, char** argv,int pSector,int pBlock, unsigned char * pMifareKey)
{	
    for(int i = 0; i < 6; i++) Key[i] = pMifareKey[i];
    //MifareLoadKey(argc,argv,0,6,Key);
    return 0;
}


int CLASS_CRT603vx::MifareLoginB(int argc, char** argv,int pSector,int pBlock, unsigned char * pMifareKey)
{	
    for(int i = 0; i < 6; i++) Key[i] = pMifareKey[i];
    return 0;
}


int CLASS_CRT603vx::SetWorkMode(bool bWire, bool bAutoFindCard)
{
//	unsigned char bySendData[1024] = {0};
	unsigned char byRecvData[1024] = {0};
//	int iSendLen = 0;
	int iRecvLen = 0;

	bySendData[iSendLen++] = 0x03;
	bySendData[iSendLen++] = 0x11;
	bySendData[iSendLen++] = (0x01&bWire) | bAutoFindCard?0x02:0x00;

	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;

	return 0;
}

/******************************************************************************/
/*  Mode 1=9600, 2=19200, 3=38400 4=115200 };
                                                                          */
/******************************************************************************/
int CLASS_CRT603vx::SetBaudRate(speed_t baud)
{
//	unsigned char bySendData[1024] = {0};
	unsigned char byRecvData[1024] = {0};
//	int iSendLen = 0;
	int iRecvLen = 0;
	
	BYTE iMode;

	iMode = 1;

	switch(baud)
	{
		case 9600:		iMode = 1;	break;
		case 19200:		iMode = 2;	break;
		case 38400:		iMode = 3;	break;
		case 115200:	iMode = 4;	break;
	}

	bySendData[ 0] = 0x03;		// total de bytes
	bySendData[ 1] = 0x10;		// set baud
	bySendData[ 2] = iMode;		// baud selectred
								// parity byte

	iRet = CRT_ExeCommand(iSendLen, bySendData,  &iRecvLen, byRecvData);
	if(iRet != 0)
		return iRet;

	return 0;
}
////////////////////////////////////////////////////////////////////////////////