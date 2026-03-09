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
CLASS_ACR1222L::CLASS_ACR1222L()
{
	memset(MIA_DATA, 0x00, sizeof(MIA_DATA));
	
	nbReaders = 4;
	rdracs = (char **)calloc(nbReaders, sizeof(char *));
	if (NULL == rdracs)
	{
		printf("Not enough memory for readers[]\n");
	}	
	
	
	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::ReaderContext(SCARDCONTEXT hContxt, char **rdr, BYTE nRDR)
{	BYTE k;

	printf("CONTEXT*************************************************************\n");
	nReader = nRDR;
	pContext = hContxt;
	for(k=0; k<4; k++)
	{
//		printf("RDR=%d\n",k );
		rdracs[k] = rdr[nRDR + k];
		// Obtiene el handler para c/u de los 4 lectores virtuales
		rv = SCardConnect(pContext, rdracs[k], SCARD_SHARE_DIRECT,
					SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard[k], &dwActiveProtocol);
		if(rv != SCARD_S_SUCCESS)
			printf("*******************ERROR CONNECT = %08lx\n", rv);

	}
	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::ReaderhCard(void)
{
	hCard[0] = 0;

	return SCARD_S_SUCCESS;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::ReaderBeginTransaction(BYTE nRDR)
{
	rv = SCardBeginTransaction(hCard[nRDR]);

	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::ReaderEndTransaction(BYTE nRDR)
{
	
	rv = SCardEndTransaction(hCard[nRDR], SCARD_EJECT_CARD);

	return rv;
	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::PCSC_ERROR(LONG rv, char *text)
{
	if (rv != SCARD_S_SUCCESS)
	{
		printf("%s: %s (0x%lX)\n", text, pcsc_stringify_error(rv), rv);
	}
	else
	{ 
		printf("%s: OK\n\n",text);
	}
	
	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_ACR1222L::printRec(BYTE *recB, DWORD sizB, const char *text)
{	int i;
	return;
	printf("%s [%lu]: ", text, sizB);
	for (i=0; i<sizB; i++)
		printf("%02X ", recB[i]);
	printf("\n");
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_ACR1222L::printTx(BYTE *recB, DWORD sizB)
{	int i;

return;
	printf("Trasmited [%lu]: ", sizB);
	for (i=0; i<sizB; i++)
		printf("%02X ", recB[i]);
	printf("\n");
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_ACR1222L::printAllMIA(void)
{	byte k,j;

return;
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
void CLASS_ACR1222L::printAllKey(BYTE key[16][6])
{	byte k,j;

return;
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

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::CardConnect(BYTE nRDR, DWORD sharemode)	// Conexion al Reader
{

	printf("Card Connect\n");

	dwActiveProtocol = -1;
	rv = SCardConnect(pContext, rdracs[nRDR], sharemode,
				SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard[nRDR], &dwActiveProtocol);
	if(rv != SCARD_S_SUCCESS)
	{	printf("ERROR CONNECT\n");
		PCSC_ERROR(rv, (char *)"ERROR CONNECT");
		return rv;
//		exit(0);
	}
	else
		printf("SUCESS CONNECT\n");

	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::CardReConnect(BYTE nRDR, DWORD sharemode)
{

	rv = SCardReconnect(hCard[nRDR], sharemode, 
				SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, SCARD_LEAVE_CARD, &dwActiveProtocol);

	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::CardDisConnect(BYTE nRDR)
{

	rv = SCardDisconnect(hCard[nRDR], SCARD_LEAVE_CARD);
	
	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::SendAPDU(BYTE nRDR, BYTE *data, DWORD len)
{	char i;

	dwRecvLength = sizeof(pbRecvBuffer);

//	printf("Sending: ");
//	for (i=0; i<len; i++)
//		printf("%02X ", data[i]);
//	printf("\n");

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], data, len,
		&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"SendAPDU");
		return rv;
	}

//	printf("Received: ");
//	for (i=0; i<dwRecvLength; i++)
//		printf("%02X ", pbRecvBuffer[i]);
//	printf("\n");
//	PCSC_ERROR(rv, (char *)"SCardTransmit");

	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
//LONG CLASS_ACR1222L::GetUID(WORD nReader, char *data)
LONG CLASS_ACR1222L::GetUID(unsigned char *UID)
{	char i;

//	printf("Read UID\n");
	memset(MFUID,0x00, sizeof(MFUID));
	rv = CardPresent2(ACS_RDR0);
	if(rv != SCARD_S_SUCCESS)
	{
//		PCSC_ERROR(rv, (char *)"UID");
		return rv;
	}
//	printf("Reconected OK\n");

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[ACS_RDR0], pioSendPci[ACS_RDR0], ACS_GET_UID, sizeof(ACS_GET_UID),
												&pioRecvPci[ACS_RDR0], pbRecvBuffer, &dwRecvLength);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"UID");
		memset(MFUID,0x00, sizeof(MFUID));
		return rv;
	}

//	printf("Received: ");
//	for (i=0; i<dwRecvLength; i++)
//		printf("%02X ", pbRecvBuffer[i]);
//	printf("\n");

	memset(MFUID,0x00, sizeof(MFUID));
	for (i=0; i<dwRecvLength-2; i++)
	{
		MFUID[i] = pbRecvBuffer[i];
		UID[i] = MFUID[i];
	}
	MFUID[i+0] = pbRecvBuffer[0];
	MFUID[i+1] = pbRecvBuffer[1];

	UID[i+0] = pbRecvBuffer[0];
	UID[i+1] = pbRecvBuffer[1];

//	printf("MFUID: ");
//	for (i=0; i<dwRecvLength; i++)
//		printf("%02X ", MFUID[i]);
//	printf("\n");

	sprintf(UIDSTR, "%02X%02X%02X%02X%02X%02X", 
			MFUID[0], MFUID[1], MFUID[2], MFUID[3], MFUID[4], MFUID[5] );

//	printf("*******UID =%s\n", UIDSTR);
	return rv;
}	

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::GetLastKey(BYTE nRDR)
{

	printf("Get LastKey\n");
	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], APDU_READ_LAST_KEY_HEX, sizeof(APDU_READ_LAST_KEY_HEX),
							&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"Last Key");
		return rv;
	}

//	printRec(pbRecvBuffer, dwRecvLength, "Received: ");
//	printf("Received: ");

	return rv;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::GetCardKey(BYTE nRDR, unsigned char mfuid[], unsigned char MFKEY[16][6], unsigned char readblock[64][16] )
{	BYTE k,i;

	printf("GetCardKey\n");

	MFGET_KEY[ 5] = mfuid[0];
	MFGET_KEY[ 6] = mfuid[1];
	MFGET_KEY[ 7] = mfuid[2];
	MFGET_KEY[ 8] = mfuid[3];
	MFGET_KEY[ 9] = readblock[4][13];
	MFGET_KEY[10] = readblock[4][14];
//	printRec(MFGET_KEY, sizeof(MFGET_KEY), "Get Key SAM");

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], MFGET_KEY, sizeof(MFGET_KEY),
		&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	if(rv != SCARD_S_SUCCESS)
	{
		printf("************Error en GetCardKey %08lx\n",rv);
		return rv;
	}
	
	printRec(pbRecvBuffer, dwRecvLength, "Received");
	GetResponse(pbRecvBuffer, dwRecvLength);

	rv = GetLastKey(nRDR);
	if(rv != SCARD_S_SUCCESS)
	{
		printf("Error en Last Card Key\n");
		return rv;
	}
//	printRec(pbRecvBuffer, dwRecvLength, "Received");
	GetResponse(pbRecvBuffer, dwRecvLength);
	
	for(k=0; k<16; k++)
	{	for(i=0; i<6; i++)
//			if( k!=1 )
				MFKEY[k][i] = RDR_DATA[i];
	}
//	printAllKey(MFKEY);
	

	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::GetUserInfo(BYTE nRDR, unsigned char area, DWORD len, char *data)
{	unsigned char k;

	printf("Get Data\n");
	printf("GET DATA RDR = %d -> AREA=%d\n",nRDR, area);

	memset(data, 0x00, sizeof(data));

	ACS_GET_DATA[2] = (area==1)?0x4C:0x4D;
	ACS_GET_DATA[5] = (len>>8) & 0xFF;
	ACS_GET_DATA[6] = len & 0xFF;

	rv = SCardControl(hCard[nRDR], IOCTL_CCID_ESCAPE, ACS_GET_DATA, sizeof(ACS_GET_DATA),
								pbRecvBuffer, sizeof(pbRecvBuffer), &dwRecvLength);
	if (rv != SCARD_S_SUCCESS) 
	{
		printf("Error en GetData = %8lx\n", rv);
		return rv;
	}
//	printf("OK control\n");
//	printRec(pbRecvBuffer, dwRecvLength, "Received");

	for(k=0; k<dwRecvLength; k++)
		data[k] = pbRecvBuffer[k];
	
	return rv;
}

/******************************************************************************/
/* SET DATA REGION 1                             */
/******************************************************************************/
LONG CLASS_ACR1222L::SetUserInfo(BYTE nRDR, BYTE area, char *data)
{	BYTE datos[300];
	WORD k, len,j;

	printf("SET DATA AREA=%d\n",area);
	memset(datos, 0x00, sizeof(datos));

	len = sizeof(ACS_SET_DATA); // + sizeof(data) + 2;

	for(k=0; k<len; k++)
		datos[k] = ACS_SET_DATA[k];
	datos[2] = (area==1)?0x4A:0x4B;

	len = sizeof(data)+1; 
	if(len>256) len=256;

	j=sizeof(ACS_SET_DATA);
	datos[j++] = len>>8;
	datos[j++] = len&0xFF;

	len -=1;
	for(k=0; k<len; k++)
		datos[j++] = data[k];
	j++;
	printRec(datos, j,"Received");

	rv = CardReConnect(nRDR, SCARD_SHARE_DIRECT);

	rv = SCardControl(hCard[nRDR], IOCTL_CCID_ESCAPE, datos, j,
								pbRecvBuffer, sizeof(pbRecvBuffer), &dwRecvLength);

	if (rv != SCARD_S_SUCCESS) 
	{
		printf("Error en Leds = %8lx\n", rv);
		return rv;
	}
//	printRec(pbRecvBuffer, dwRecvLength, "Received");

	return rv;
}

/******************************************************************************/
/* buzzer                                                                     */
/******************************************************************************/
LONG CLASS_ACR1222L::ActiveBuzzer(BYTE cseg)
{
	printf("buzzer\n");
	ACS_BUZZ[5] = cseg;

	CardReConnect(ACS_RDR0, SCARD_SHARE_DIRECT);

	rv = SCardControl(hCard[ACS_RDR0], IOCTL_CCID_ESCAPE, ACS_BUZZ, sizeof(ACS_BUZZ),
								pbRecvBuffer, sizeof(pbRecvBuffer), &dwRecvLength);
	if (rv != SCARD_S_SUCCESS) 
	{ 
		printf("Error en Active Buzzer = %8lx\n", rv);
		return rv;
	}
	usleep(100000);
//	printRec(pbRecvBuffer, dwRecvLength, "Received");
	return SCARD_S_SUCCESS;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::CardPresent( BYTE nRDR )
{	char i;

//	printf("Card Status\n");
	rv = CardConnect(nRDR, SCARD_SHARE_SHARED);
	if(rv == SCARD_E_NO_SMARTCARD)
	{
		PCSC_ERROR(rv, (char *)"Error CardConnect");
		return rv;
	}

	/* get card status */
	dwAtrLen = sizeof(pbAtr);
	dwReaderLen = sizeof(pbReader);
	rv = SCardStatus(hCard[nRDR], pbReader, &dwReaderLen, &dwState, &dwProt,
		pbAtr, &dwAtrLen);
	if(rv != SCARD_S_SUCCESS)
	{
		printf("Error en CardStatus = %8lx\n",rv);
		return rv;
	}

    switch(dwActiveProtocol)
    {
        case SCARD_PROTOCOL_T0:
            pioSendPci[nRDR] = SCARD_PCI_T0;
            break;
        case SCARD_PROTOCOL_T1:
            pioSendPci[nRDR] = SCARD_PCI_T1;
            break;
        default:
            printf("Unknown protocol\n");
            break;
    }
	return rv;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::CardPresent2( BYTE nRDR )
{	char i;

//	printf("Card Status\n");
	rv = CardReConnect(nRDR, SCARD_SHARE_SHARED);
	if(rv == SCARD_E_NO_SMARTCARD)
	{
//		PCSC_ERROR(rv, (char *)"Error CardConnect");
		return rv;
	}
//	PCSC_ERROR(rv, (char *)"CardConnect");

	/* get card status */
	dwAtrLen = sizeof(pbAtr);
	dwReaderLen = sizeof(pbReader);
	rv = SCardStatus(hCard[nRDR], pbReader, &dwReaderLen, &dwState, &dwProt,
		pbAtr, &dwAtrLen);
	if(rv != SCARD_S_SUCCESS)
	{
		printf("Error en CardStatus = %8lx\n",rv);
		return rv;
	}

    switch(dwActiveProtocol)
    {
        case SCARD_PROTOCOL_T0:
            pioSendPci[nRDR] = SCARD_PCI_T0;
            break;
        case SCARD_PROTOCOL_T1:
            pioSendPci[nRDR] = SCARD_PCI_T1;
            break;
        default:
            printf("Unknown protocol\n");
            break;
    }
	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::GetCardAttribute(BYTE nRDR)
{	byte i;

	/* get card status */
	dwAtrLen = sizeof(pbAtr);
	dwReaderLen = sizeof(pbReader);
	rv = SCardStatus(hCard[nRDR], pbReader, &dwReaderLen, &dwState, &dwProt,
		pbAtr, &dwAtrLen);
	if(rv != SCARD_S_SUCCESS)
	{
		printf("Error en CardStatus = %8lx\n",rv);
		return rv;
	}

	printf(" Reader: %s (length %ld bytes)\n", pbReader, dwReaderLen);
	printf(" State: 0x%lX\n", dwState);
	printf(" Prot: %ld\n", dwProt);
	printf(" ATR (length %ld bytes):", dwAtrLen);
	for (i=0; i<dwAtrLen; i++)
		printf(" %02X", pbAtr[i]);
	printf("\n");

    switch(dwActiveProtocol)
    {
        case SCARD_PROTOCOL_T0:
            pioSendPci[nRDR] = SCARD_PCI_T0;
            break;
        case SCARD_PROTOCOL_T1:
            pioSendPci[nRDR] = SCARD_PCI_T1;
            break;
        default:
            printf("Unknown protocol\n");
            break;
    }
	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::GetCardStatus(BYTE nRDR)
{	SCARD_READERSTATE rdrState = {};

	rdrState.szReader = rdracs[nRDR];
	rdrState.dwCurrentState = SCARD_STATE_UNAWARE;
	rdrState.dwCurrentState = SCARD_STATE_UNKNOWN;

	rv = SCardGetStatusChange(pContext, 100, &rdrState, 1);
	if(rv == SCARD_S_SUCCESS)
	{
//		printf("Event = %08lx-%08lx\n",rdrState.dwCurrentState, rdrState.dwEventState );

		if( (rdrState.dwCurrentState & SCARD_STATE_CHANGED) == 0)
		{
			
	//		return 0x1002;		// misma tarejeta en lwctor
		}

		if (rdrState.dwEventState & SCARD_STATE_PRESENT)
		{
			printf("CARD PRESENTE =%08lx\n",rdrState.dwEventState );

			return 0x00;			//SCARD_STATE_PRESENT;
		}
		else if (rdrState.dwEventState & SCARD_STATE_EMPTY)
		{
			printf("NO HAY TARJETA =%08lx\n",rdrState.dwEventState );
			return SCARD_STATE_EMPTY;			
		}
    }

	return rv;
}

/******************************************************************************/
/*  Result E1h 00h 00h 00h 10h Serial Number (16 bytes)                       */
/******************************************************************************/
LONG CLASS_ACR1222L::GetSerialNum(SCARDCONTEXT hContxt, char **rdr, BYTE nRDR, char *datos)
{
	rv = CardReConnect(nRDR, SCARD_SHARE_DIRECT);
	if (rv != SCARD_S_SUCCESS) 
	{
		printf("Error en Connect = %8lx\n", rv);
		return rv;
	}

	rv = SCardControl(hCard[nRDR], IOCTL_CCID_ESCAPE, ACS_GET_SN, sizeof(ACS_GET_SN),
								pbRecvBuffer, sizeof(pbRecvBuffer), &dwRecvLength);

	if (rv != SCARD_S_SUCCESS) 
	{
		printf("Error en SerialNumber = %8lx\n", rv);
		return rv;
	}
//	printRec(pbRecvBuffer, dwRecvLength, "Received");

	unsigned char k,len;
	len = pbRecvBuffer[4];
	for(k=0; k<len; k++)
		datos[k] = pbRecvBuffer[5+k];
	datos[k] = 0x00;
	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::GetUserId(SCARDCONTEXT hContxt, char **rdr, BYTE nRDR, BYTE area, DWORD len, char *datos)
{
//	printf("RDR=%d\n", nRDR);

	pContext = hContxt;
	rdracs[nRDR] = rdr[nRDR];

	// Obtiene el handler para el acs
	dwActiveProtocol = -1;
	rv = SCardConnect(pContext, rdracs[nRDR], SCARD_SHARE_DIRECT,
				SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard[nRDR], &dwActiveProtocol);
	if(rv != SCARD_S_SUCCESS)
		printf("*******************ERROR CONNECT = %08lx\n", rv);

	rv = GetUserInfo(nRDR,AREA1,len, ID);
	
	strcpy(datos,ID);
	return rv;
}


/******************************************************************************/
/* leds -> b0= green b1=blue b2=orange b3=red                                 */
/******************************************************************************/
LONG CLASS_ACR1222L::SetLed(unsigned char leds)
{	

	ACS_LEDS[3] = leds;

	rv = CardReConnect(ACS_RDR0, SCARD_SHARE_DIRECT);
	if (rv != SCARD_S_SUCCESS) 
	{ 
		printf("Error en Connect = %8lx\n", rv);
		return rv;
	}

	rv = SCardControl(hCard[ACS_RDR0], IOCTL_CCID_ESCAPE, ACS_LEDS, sizeof(ACS_LEDS),
								pbRecvBuffer, sizeof(pbRecvBuffer), &dwRecvLength);

	if (rv != SCARD_S_SUCCESS) 
	{ 
		printf("Error en Leds = %8lx\n", rv);
		return rv;
	}
//	printRec(pbRecvBuffer, dwRecvLength, "Received");
	
	return rv;
}

/******************************************************************************/
/*LD_AUTHKEY 0xFF, 0x82, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 */
/*                                         UID ...                            */
/******************************************************************************/
LONG CLASS_ACR1222L::MF_LoadKey(BYTE nRDR, BYTE nkey, BYTE *Key)
{	byte k;

	LOAD_KEY[3] = nkey;		// ?? Key Number

	for(k=0; k<6;k++)
		LOAD_KEY[5+k] = Key[k];

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], LOAD_KEY, sizeof(LOAD_KEY),
						&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	return rv;
}


/******************************************************************************/
/*  Authenticate Data Bytes: 5 Bytes  B1=0x01    B2=0x00 B3=Block Number      */
/*                                    B4=TypeKey b5=Key Number                */
/******************************************************************************/
LONG CLASS_ACR1222L::MF_AuthBlk(BYTE nRDR, BYTE nblk, BYTE KeyType, BYTE KeyNumber)
{
	AUTH_BLK[7] = nblk;		//Block
	if(KeyType==KEYA)		// KeyType
		AUTH_BLK[8] = 0x60;
	else
		AUTH_BLK[8] = 0x61;
	AUTH_BLK[9] = 0x00;		//KeyNumber;

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], AUTH_BLK, sizeof(AUTH_BLK),
						&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	return rv;
}


/******************************************************************************/
/*  Authenticate Data Bytes: 5 Bytes  B1=0x01    B2=0x00 B3=Block Number      */
/*                                    B4=TypeKey b5=Key Number                */
/******************************************************************************/
LONG CLASS_ACR1222L::MF_AuthBlk2(BYTE nRDR, BYTE nblk, BYTE KeyType, BYTE KeyNumber)
{
	AUTH_BLK2[3] = nblk;
	if(KeyType==KEYA)
		AUTH_BLK2[4] = 0x60;
	else
		AUTH_BLK2[4] = 0x61;

	AUTH_BLK2[5] = 0x00;		//KeyNumber;

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], AUTH_BLK2, sizeof(AUTH_BLK2),
						&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	GetResponse(pbRecvBuffer, dwRecvLength);
	if(RDR_RESPONSE != 0x9000)
	{
		printf("************* Error Response = %04x -> %08lx\n", RDR_RESPONSE, rv);
		return RDR_RESPONSE+1;
	}
	return rv;
}

/******************************************************************************/
/* Read Binary Blocks FFh B0h 00h No,Block No.Bytes.to.Read                   */
/******************************************************************************/
LONG CLASS_ACR1222L::MF_RdBlock (BYTE nRDR, BYTE nblk, BYTE nBytes)
{
	READ_BLK[3] = nblk;			// numero de block
	READ_BLK[4] = nBytes;		// total de bytes a leer

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], READ_BLK, sizeof(READ_BLK),
						&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	GetResponse(pbRecvBuffer, dwRecvLength);
	if(RDR_RESPONSE != 0x9000)
	{
		return RDR_RESPONSE+1;
	}
	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::MF_RdSector (BYTE nRDR, BYTE nsec, BYTE KeyType, BYTE *pass, BYTE ReadBlocks[64][16])
{	byte k;

	// load Key
	rv = MF_LoadKey(nRDR, 0,pass);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"Load Key");
		return rv;
	}

	rv = MF_AuthBlk2(nRDR, nsec*4, KeyType, 0x00);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"Auth Block");
		return rv;
	}

	GetResponse(pbRecvBuffer, dwRecvLength);
	if(RDR_RESPONSE != 0x9000)
	{
		printf("Error en Auth %04X\n", RDR_RESPONSE);
		return RDR_RESPONSE;
	}

	rv = MF_RdBlock (nRDR,  nsec*4, 48);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"Error Read Block");
		return rv;
	}

	for(k=0; k<48; k++)
		ReadBlocks[nsec*4][k] = RDR_DATA[k];
	
	return rv;
}

/******************************************************************************/
/* Read Binary Blocks FFh B0h 00h No,Block No.Bytes.to.Read                   */
/******************************************************************************/
LONG CLASS_ACR1222L::MF_WrBlock (BYTE nRDR, BYTE nblk, BYTE nBytes, BYTE *buffer)
{	BYTE k;

	if( (nblk % 4) == 3)
	{
		printf("BLOCK NO SE DEBE ESCRIBIR\n");
		return 0x00;
	}

	wrBuffer[0] = 0xFF;
	wrBuffer[1] = 0xD6;
	wrBuffer[2] = 0x00;
	wrBuffer[3] = nblk;
	wrBuffer[4] = nBytes;
	for(k=0; k<nBytes; k++)
		wrBuffer[5+k] = buffer[k];
	lenwr = 5 + nBytes;

//	printf("WRITW BLK=%d TB=%d\n", nblk, nBytes);

//	printTx(wrBuffer, lenwr);

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], wrBuffer, lenwr,
						&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	GetResponse(pbRecvBuffer, dwRecvLength);
	if(RDR_RESPONSE != 0x9000)
	{
		printf("*************  RDBLOCK Response = %04x -> %08lx\n", RDR_RESPONSE, rv);
		return RDR_RESPONSE+1;
	}
	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::WriteBlock(BYTE typekey, BYTE nblock, BYTE pPASS[16][6], unsigned char writelocks[64][16])
{	byte k;

//printf("key= [");
//for(k=0;k<6;k++)
//	printf("%02X,", pPASS[nblock/4][k]);
//printf("]\n");

//	printf("write block = %d\n", nblock);
	// load Key
	rv = MF_LoadKey(ACS_RDR0, 0,pPASS[nblock/4]);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"Load Key");
		return rv;
	}

	rv = MF_AuthBlk2(ACS_RDR0, nblock, typekey, 0x00);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"Auth Block");
		return rv;
	}

	GetResponse(pbRecvBuffer, dwRecvLength);
	if(RDR_RESPONSE != 0x9000)
	{
		printf("Error en Auth %04X\n", RDR_RESPONSE);
		return RDR_RESPONSE;
	}

//	printf("Read Block = %d\n\n", nblock);

	rv = MF_WrBlock (ACS_RDR0, nblock, 16, writelocks[nblock]);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"Error write Block");
		return rv;
	}
	GetResponse(pbRecvBuffer, dwRecvLength);
	if(RDR_RESPONSE != 0x9000)
	{
		printf("Error en Auth %04X\n", RDR_RESPONSE);
		return RDR_RESPONSE+1;
	}	

	for(k=0; k<16; k++)
		MIA_DATA[nblock][k] = RDR_DATA[k];
	
	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::MF_WrSector  (BYTE nRDR, BYTE nsec, BYTE KeyType, BYTE *pass, BYTE *buffer)
{	byte k;

//	printf("Read Sector = %d\n", nsec);
	// load Key
	rv = MF_LoadKey(nRDR, 0,pass);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"Load Key");
		return rv;
	}

	rv = MF_AuthBlk2(nRDR, nsec*4, KeyType, 0x00);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"Auth Block");
		return rv;
	}

	GetResponse(pbRecvBuffer, dwRecvLength);
	if(RDR_RESPONSE != 0x9000)
	{
		printf("Error en Auth %04X\n", RDR_RESPONSE);
		return RDR_RESPONSE;
	}

//	printf("Read Block = %d\n\n", nsec*4);

	rv = MF_WrBlock (nRDR, nsec*4, 64, buffer);
	if(rv != SCARD_S_SUCCESS)
	{
		PCSC_ERROR(rv, (char *)"Error Read Block");
		return rv;
	}
	GetResponse(pbRecvBuffer, dwRecvLength);
	if(RDR_RESPONSE != 0x9000)
	{
		printf("Error en Auth %04X\n", RDR_RESPONSE);
		return RDR_RESPONSE+1;
	}	

	for(k=0; k<48; k++)
		MIA_DATA[nsec*4][k] = RDR_DATA[k];
	
	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::READ_ALL_WITH_KEY( BYTE nRDR, int TypeKey, BYTE pPASS[16][6], BYTE ReadBlocks[64][16] )
{	int k;

	for(k=0; k<16; k++)
	{
		rv = MF_RdSector (ACS_RDR0, k, TypeKey, pPASS[0], ReadBlocks);
		if(rv != SCARD_S_SUCCESS)
		{
			printf("Error Read Sector=%d\n",k);
			return 0x01;	// Error
		}
		if( RDR_RESPONSE!= 0x9000)
		{
			printf("Error Read Sector=%d\n",k);
			RDR_RESPONSE;
			return 0x01;	// Error
		}
	}	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::MF_ReadAll(BYTE nRDR, BYTE TypeKey)
{	BYTE k;
	unsigned char uid[6];
	
//	printf("Read All\n");
	rv = GetUID(uid);
	if (rv != SCARD_S_SUCCESS) 
	{
		printf("Error en ReadUID = %8lx\n", rv);
		return rv;
	}

	for(k=0; k<64; k++)
	{
		// Read Block x Block
		
		
	}

	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_ACR1222L::GetResponse(BYTE *recB, DWORD sizB)
{	BYTE k;

	memset(RDR_DATA, 0x00, sizeof(RDR_DATA));

	if(sizB < 2)
	{
		printf("\n\n ERROR EN sisB= %ld   *********************************\n\n", sizB);
		RDR_RESPONSE = 0xFFFF;
		return;
	}
	
	RDR_RESPONSE = (recB[sizB-2]*0x100) + (recB[sizB-1]);
	
	for(k=0; k< (sizB-2); k++)
		RDR_DATA[k] = recB[k];
}
	
/******************************************************************************/
/*                                                                            */
/******************************************************************************/

LONG CLASS_ACR1222L::AutentificaSAM(BYTE nRDR, BYTE pKey)
{	char k;

//	printf("\n********  AUTENTIFICA SAM =%d******\n\n", pKey);

	if(CardPresent(nRDR)!=SCARD_S_SUCCESS)
	{
		printf("SAM NO PRESENTE\n");
		return rv;
	}

	dwRecvLength = sizeof(pbRecvBuffer);
	if(pKey == PSAM)
		rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], APDU_SELECT_PIN_PSAM_HEX, sizeof(APDU_SELECT_PIN_PSAM_HEX),
							&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	else
		rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], APDU_SELECT_PIN_ISAM_HEX, sizeof(APDU_SELECT_PIN_ISAM_HEX),
							&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	if(rv!= SCARD_S_SUCCESS)
	{
		printf("Error en SELECT PIN\n");
		return rv;
	}

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], APDU_VERIFY_PIN_HEX, sizeof(APDU_VERIFY_PIN_HEX),
							&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	if(rv!= SCARD_S_SUCCESS)
	{
		printf("Error en VERIFY PIN\n");
		return rv;
	}

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], APDU_GET_CHALLENGE_HEX, sizeof(APDU_GET_CHALLENGE_HEX),
							&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	if(rv!= SCARD_S_SUCCESS)
	{
		printf("Error en GET CHALLENGUE\n");
		return rv;
	}

	GetResponse(pbRecvBuffer, dwRecvLength);

	if(RDR_RESPONSE!= 0x9000)
	{
		printf("Error en Get Challenge\n");
		return RDR_RESPONSE;
	}
			
	Three_DES_Encrypt(RDR_DATA, pbRecvBuffer, (int)pKey);

	memcpy(pbRecChallenge, pbRecvBuffer, 8);

	for(k=0; k<8;k++)
		AUTH_SAM[5+k] = pbRecChallenge[k];

	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], AUTH_SAM, sizeof(AUTH_SAM),
							&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	if( rv!= SCARD_S_SUCCESS)
	{
		printf("Error en GET CHALLENGUE\n");
		return rv;
	}
	GetResponse(pbRecvBuffer, dwRecvLength);
	
	dwRecvLength = sizeof(pbRecvBuffer);
	rv = SCardTransmit(hCard[nRDR], pioSendPci[nRDR], APDU_GET_UID_SAM_HEX, sizeof(APDU_GET_UID_SAM_HEX),
							&pioRecvPci[nRDR], pbRecvBuffer, &dwRecvLength);
	if( rv!= SCARD_S_SUCCESS)
	{
		printf("Error en GET UID SAM\n");
		return rv;
	}
	GetResponse(pbRecvBuffer, dwRecvLength);

	if(RDR_RESPONSE!= 0x9000)
	{
		printf("Error en Get Challenge\n");
		return RDR_RESPONSE;
	}
	memcpy(SamUID, RDR_DATA,8);
	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::MF_Decrement(BYTE addr, BYTE PurseBlock, int valor)
{
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::MF_Increment(BYTE addr, BYTE PurseBlock, int valor)
{
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG CLASS_ACR1222L::MF_Transfer(BYTE addr, BYTE PurseBlock)
{
	
	return 0x00;
}
////////////////////////////////////////////////////////////////////////////////