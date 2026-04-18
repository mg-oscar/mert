/* File:   kyt_chm29xx.cpp
 * Author: famisa
 * 
 * Created on March 2, 2023, 2:17 PM
 */

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
CLASS_CHM29XX::CLASS_CHM29XX()
{
	memset( kty_error, 0, sizeof(kty_error) );		// borra la lista de errores/status
	ECode = 0x0000;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::AnalizaRespuesta(unsigned char rx)
{	static int tdat, trec;
	unsigned char bcc;

//	printf("%02X,", rx);
	switch(ndec)
	{	case 0:		// SOH=0x01
			if(rx== SOH)
			{	//printf("1\n");
				rbuf = 0;
				rxBuf[rbuf] = rx;
				ndec++;
			}
			break;
		case 1:		// NULL siempre 0
			if(rx== 0x00)
			{	//printf("2\n");
				rbuf++;
				rxBuf[rbuf] = rx;
				ndec++;
			}
			break;
		case 2:		// MSB LENGTH
			tdat = int(rx<<8);
			//printf("3=%d\n", tdat);
			rbuf++;
			rxBuf[rbuf] = rx;
			ndec++;
			break;
		case 3:		// LSB LENGTH
			tdat += rx;
//			printf("4=%d\n", tdat);
			tdat += 3;	// 4 bytes mas, 			
			rbuf++;
			rxBuf[rbuf] = rx;
			ndec++;
			trec = 0;
			break;

		case 4:		// LSB LENGTH
			rbuf++;
			rxBuf[rbuf] = rx;
			trec++;
			if(trec==tdat)
			{
				ndec = 0;
				rbuf++;	// total de datos recibidos
//				printf("\nTotal de Datos recibidos\n");
//				for(trec=0; trec<rbuf; trec++)
//					printf("%02X-", rxBuf[trec]);
//				printf("\n");

				bcc = 0;
				for(trec=1; trec<(rbuf-1); trec++)
				{	bcc = bcc ^ rxBuf[trec];
//					printf("%d= %02X-%02X\n", trec, rxBuf[trec], bcc);
				}
//				printf("BCC= %02X\n", bcc);
				if(bcc == rxBuf[rbuf-1])
				{
//					printf("BCC OK\n");
					// hacemos varias validaciones, como el comando que se envio
					// sea el mismo que viene en la respuesta
					if(rbuf>= 13)
					{	if( (rxBuf[8]==0x00) && (rxBuf[9]==0x00) && (rxBuf[10]==0x01))
						{	// Response GOOD
//							printf("POSITIVE\n");
						}
						else
						{	// Response Error
//							printf("NEGATIVE\n");
						}
						return 0x00;
					}
					return 0x01;
				}
				return 0x01;
			}
			break;
	}
	return 0x01;	// Respuesta incompleta o erronea
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::ObtieneDatosAPDU(void)
{	int k;

//	printf("\nultimo comando =%d***************************************\n",lastCmd );

	ECode = (rxBuf[8]*0x100) + rxBuf[9];	// ECode
	Dis_Response = rxBuf[10];

	if(lastCmd==CMD_SAM1CardReset)
	{
		// obtiene ATR del SAM
		sATR = rbuf-17;
		for(k=0; k<sATR; k++)
			ATR[k] = rxBuf[k+15];

		Sam_Response = (rxBuf[12]*0x100) + rxBuf[13];

		printf("********************************************\n");
		printf("ECode        = %04X\n", ECode);
		printf("Dis_Response = %02X\n", Dis_Response);
		printf("Sam_Response = %04X\n", Sam_Response);
		printf("ATR          = ");
		for(k=0;k< sATR ;k++)
			printf("%c", ATR[k]);
		printf("\n");
		printf("********************************************\n");
		return 0x00;
	}

	if(lastCmd==CMD_SAM1CardDirectControl)
	{
		// obtiene datos del comando de APDU
		printf("Datos RX APDU\n");
		printf("Len = %d\n", rbuf);
		for(k=0;k<rbuf;k++)
			printf("%02X ", rxBuf[k]);
		printf("\n");

		printf("%d - %02X\n",rbuf-4, rxBuf[rbuf-4]);
		printf("%d - %02X\n",rbuf-3, rxBuf[rbuf-3]);
		
		Sam_Response = (rxBuf[rbuf-4]*0x100) + rxBuf[rbuf-3];

		sData = rbuf-15;
		printf("SData = %d\n", sData);

		for(k=0; k<sData; k++)
			dataBuf[k] = rxBuf[k+11];

		printf("********************************************\n");
		printf("ECode        = %04X\n", ECode);
		printf("Dis_Response = %02X\n", Dis_Response);
		printf("Sam_Response = %04X\n", Sam_Response);
		printf("DATOS SAM    = ");
		if(sData !=0)
		{	for(k=0;k< sData ;k++)
				printf("%02X.", dataBuf[k]);
		}
		else
			printf("SIN DATOS");
//		printf("\n");
//		printf("********************************************\n");
		return 0x00;
	}

//	printf("-------------------------------\n");

return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::waitResponse(unsigned char resp, int timeout)
{	unsigned int tm= milisegundos();

	while( (milisegundos()-tm) < timeout)
	{
		RS232_Read();
		while( existDat() )
		{
			if( AnalizaRespuesta(getBufferRx()) == 0)
			{	// Respuesta completa OK
				// comprobar si es POSITIVE o NEGATIVE

//				printf("Respuesta Completa\n");
				send1B(ACK);
				ECode = (rxBuf[8]*0x100) + rxBuf[9];
				if( (rxBuf[8]==0x00) && (rxBuf[9]==0x00) && (rxBuf[10]==0x01))
					return KYT_POSITIVE;		// Response GOOD
				else
					return KYT_NEGATIVE;	// Negative Response Format
			}
		}
	}
//	printf("TIMEOUT\n");
	send1B(NAK);

	return KYT_ERROR;	// TimeOut
}

/******************************************************************************/
/* Espera un ACK o un NAK                                                     */
/******************************************************************************/
int CLASS_CHM29XX::waitACK(void)
{	unsigned char rx;
	unsigned int tm= milisegundos();
//	printf("\n[");
	while( (milisegundos()-tm) < 500)
	{
		RS232_Read();
//		if( apIniBufRx != apFinBufRx)
		if( existDat() )
		{	rx = getBufferRx();
//			printf("%02X]", rx);
			if(rx == ACK)
			{	//printf("ACK\n");
				usleep(20000);
				send1B(ENQ);
				ndec = 0;
				return ACK;
			}
			if(rx == NAK)
			{	// Reenvia mismo Comando
				//printf("NAK\n");
				ndec = 0;
				usleep(20000);
				RS232_Write( sbuf, cmdBuf);
				return NAK;
			}
			if(rx == CAN)
			{	// CANCEL
				printf("CANCEL\n");
				ndec = 0;
//				usleep(20000);
//				RS232_Write( sbuf, cmdBuf);
				return CAN;
			}
		
			return 0;
		}
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::creaTrama(char NoCmd, unsigned char *datos, int tdat)
{	int i;
	unsigned char bcc;

	lastCmd = NoCmd;

	sbuf = 3 + tdat;

	cmdBuf[ 0] = SOH;
	cmdBuf[ 1] = 0x00;			// NULL
	cmdBuf[ 2] = (sbuf>>8);		// MSB LENGTH	
	cmdBuf[ 3] = sbuf & 0xFF;	// LSB LENGTH
	cmdBuf[ 4] = STX;

	cmdBuf[ 5] = kyt_cmd[NoCmd].command[0];
	cmdBuf[ 6] = kyt_cmd[NoCmd].command[1];
	cmdBuf[ 7] = kyt_cmd[NoCmd].command[2];

	for(i=0; i<tdat; i++)
		cmdBuf[ 8+ i] = datos[i];
	cmdBuf[ 8 + i] = ETX;
	sbuf = 8 + i;
	sbuf++;

	bcc = 0;
	for(i=1; i< sbuf; i++)
		bcc = bcc ^ cmdBuf[i];
	cmdBuf[sbuf] = bcc;
	sbuf++;	// total de datos del frame

	return 0;
}

/******************************************************************************/
/* Envia comaando de 1B, sin datos                                            */
/******************************************************************************/
int CLASS_CHM29XX::send1B(unsigned char by)
{	unsigned char dat[2];
	dat[0] = by;
	RS232_Write( 1, dat);
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::Sincroniza(void)
{	int i, k;
	unsigned int tm;
	unsigned char rx;

//	send1B(NAK);

	k=0;
	for(i=0; i<10; i++)
	{
		send1B(ENQ);
		tm= milisegundos();
		while( (milisegundos()-tm) < 200)
		{
			RS232_Read();
			while( existDat() )
			{
				if( getBufferRx() == ENQ)
				{
					k++;
					tm -= 1000;
				}
			}
		}
	}
//	printf("ENQ=%d\n",k);
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::SendCmd(char NoCmd, unsigned char *datos, int tdat, int timeout)
{	char seq;
	int trec;

	seq = 0;
	while(1)
	{
		switch(seq)
		{	case 0:
				creaTrama(NoCmd, datos, tdat);
				ndec = 0;
				RS232_Write( sbuf, cmdBuf);
				seq++;
				break;
			case 1:
				if( (trec=waitACK()) == 0 )	// ACK o NAK
				{
					// error TIMEOUT
					send1B(NAK);
//					printf("TIMEOUT\n");
					return -1;
				}
				seq++;
				break;
			case 2:
				if( (trec=waitResponse(0, timeout)) >= 0)
				{
					ObtieneDatosAPDU();
					return 0;
				}
				send1B(NAK);
				return -1;
				break;
		}
	}
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::searchErr(int nerr)
{	int k;

	for(k=0; k < KYT_TOTAL_ERRORES; k++)
	{
		if( nerr == kyt_errores[k].codigo)
		{
			return k+1;
		}
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::ObtieneErrores(void)
{	int tot, i, err, ne;

	tot = rbuf - 13;	// tot/2 = cantidad de errores reportados
//	printf("Tot=%d\n", tot);

	for(i=0; i<tot; i+=2)
	{	err = (int(rxBuf[11+i])<<8) + rxBuf[12+i];
		if( (ne=searchErr(err)) !=0)
		{	ne = ne -1;
			kty_error[ne] = 0x01;	// Pone bandera de Error
//			printf("Error%d=%04x  %d-%s\n", i/2, err, ne, kyt_errores[ne].error  );
		}
		else
			printf("Error NO ENCONTRADO = %04x\n", (unsigned int)err );			
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::CapturaCard(void)
{
	int k;

	for(k=0; k<5; k++)
	{
		usleep(200000);
		// captura la Tarjeta
		if( SendCmd(CMD_CardCapture, auxBuf, 0, TO_CAPTURA ) != 0)
		{
			printf("Error en Captura\n");
			continue;

		}
		else
		{
			ECode = (rxBuf[8]*0x100) + rxBuf[9];
		}

		usleep(200000);
		if(ReadPosicionMIA()==0)
		{
			if(rxBuf[0x0B]== 0x00)
			{	
				// NO HAY TARJETA EN READER
				return 0x00;	// OK
			}
		}
	}
	return 0x01;	// error no se capturo la tarjeta
}

/******************************************************************************/
/* Dispensa la tarjeta y la Sostiene con los rodillos                         */
/******************************************************************************/
int CLASS_CHM29XX::EjectHold(void)
{
	if( SendCmd(CMD_CardEjectHold, auxBuf, 0, TO_EJECT_HOLD ) != 0)
	{
		printf("Error en Dispensa & Suelta\n");
		return 0x01;
	}
	return 0;
}

/******************************************************************************/
/* Dispensa la tarjeta y la suelta fuera del dispensador                      */
/******************************************************************************/
int CLASS_CHM29XX::EjectCard(void)
{
	// Dispensa la tarjeta y la suelta 
	if( SendCmd(CMD_CardEjectDrop, auxBuf, 0, TO_EJECT_DROP ) != 0)
	{
		printf("Error en Dispensa & Suelta\n");
		ECode = (rxBuf[8]*0x100) + rxBuf[9]; 

		return 0x01;
	}
	ECode = (rxBuf[8]*0x100) + rxBuf[9]; 
	printf("OK Dispensa y Suelta\n");

	return 0;
}

/******************************************************************************/
/* Mueve una tarjeta del Stacker al Modulo de RF                              */
/******************************************************************************/
int CLASS_CHM29XX::GetCard(void)
{	int re;
	// Mueve una tarjeta del Stacker al Reader
	auxBuf[0] = 0x03;		//
	auxBuf[1] = RF_MODULE;	// Move to RF Module
	if( (re=SendCmd(CMD_MoveCardFromStacker, auxBuf, 2,TM_GETCARD )) != 0)
	{
		printf("Error en Card move\n");
		return 0x01;
	}
	ECode = (rxBuf[8]*0x100) + rxBuf[9]; 
	printf("RE Card move = %04x\n", ECode);

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::ReadModel(void)
{	int re;

	memset(Model, 0, sizeof(Model));

	if( (re=SendCmd(CMD_GetModel, auxBuf, 0, TO_GENERAL )) < 0)
	{
		printf("--Error en GetModel\n");
		return KYT_ERROR;
	}
	if(re == KYT_POSITIVE)
	{
		memcpy(Model, &rxBuf[0x0B], 30);	// Modelo
	}
	else
	{	
		printf("Negative=%02X%02X\n", rxBuf[0x08], rxBuf[0x09]);
		return KYT_NEGATIVE;
	}
	return KYT_OK;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::ReadFirmware(void)
{	int re;

	memset(Version, 0, sizeof(Version));

	//**************************************************************************
	if( (re=SendCmd(CMD_GetFirmware, auxBuf, 0, TO_GENERAL )) < 0)
	{
		printf("Error en GetFirmware\n");
		return KYT_ERROR;
	}
	if(re == KYT_POSITIVE)
	{
		memcpy(Version, &rxBuf[0x0B], 30);	// Version
	}
	else
	{	
		printf("Negative=%02X%02X\n", rxBuf[0x08], rxBuf[0x09]);
		return KYT_NEGATIVE;
	}
	return KYT_OK;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::ReadStatusStacker(void)
{	int re;

	Stacker = STACKER_UNKNOW;

	if( (re=SendCmd(CMD_GetStackerStatus, auxBuf, 0, TO_GENERAL )) < 0)
	{
		printf("Error en GetStatusStacker\n");
		return KYT_ERROR;
	}

	if(re == KYT_POSITIVE)
	{
		Stacker = rxBuf[0x0B];
	}
	else
	{
		printf("Negative->=%02X%02X\n", rxBuf[0x08], rxBuf[0x09]);
		return KYT_NEGATIVE;
	}
	return KYT_OK;
}




//
// RUTINAS PARA EL ACCESO AL SAM1
//
/******************************************************************************/
/* Reset SAM1                                                                 */
/******************************************************************************/
int CLASS_CHM29XX::ResetSAM1(void)
{
	// Dispensa la tarjeta y la suelta
	if( SendCmd(CMD_SAM1CardReset, auxBuf, 0, TO_RESET_SAM ) != 0)
	{
		printf("Error en Reset SAM1\n");
		ECode = (rxBuf[8]*0x100) + rxBuf[9];

		return 0x01;
	}

	ECode = (rxBuf[8]*0x100) + rxBuf[9]; 
	printf("Error Code   = [%04X]\n", ECode);
	printf("OK Reset SAM1\n");

	// *********** obtiene ATR DEL SAM1

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::CreaTramaAPDU(unsigned char dinst[], unsigned char apdu[], int len)
{
	dinst[0] = 0x00;
	dinst[1] = len;
	memcpy(&dinst[2], apdu, len);
	len +=2;
	
	printf("APDU=%02X\n[",len);
	for(int k=0; k<len;k++)
		printf(" %02X", dinst[k]);
	printf("]\n");

	return len;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::AutentificaSAM(int pKey)
{	unsigned char tmpBuf[100];
	unsigned char Challenge[8], RespChall[8],ExternAuth[20];
	int i,re;

	//**************************************************************************
	if(pKey == PSAM)
	{
		sAux = CreaTramaAPDU(tmpBuf,APDU_SELECT_PIN_PSAM_HEX, sizeof(APDU_SELECT_PIN_PSAM_HEX));
		re = SendCmd(CMD_SAM1CardDirectControl, tmpBuf, sAux, TO_RESET_SAM );
	}
	else
	{
		sAux = CreaTramaAPDU(tmpBuf,APDU_SELECT_PIN_ISAM_HEX, sizeof(APDU_SELECT_PIN_ISAM_HEX));
		re = SendCmd(CMD_SAM1CardDirectControl, tmpBuf, sAux, TO_EJECT_HOLD );
	}
	if( re != 0)
	{
		printf("\nERROR SELECT PIN\n");
		return 0x12;
	}
	else
	{
		printf("\n");
		printf("OK SELECT PIN\n");
	}
	
	//**************************************************************************
	printf("\nVERIFY PIN\n");
	sAux = CreaTramaAPDU(tmpBuf,APDU_VERIFY_PIN_HEX, sizeof(APDU_VERIFY_PIN_HEX));
	re = SendCmd(CMD_SAM1CardDirectControl, tmpBuf, sAux, TO_EJECT_HOLD );
	if( re != 0)
	{
		printf("\nERROR VERIFY PIN\n");
		return 0x13;	// ERROR VERIFY PIN
	}
	else
	{
		printf("\nOK VERIFY PIN\n");
	}

	//**************************************************************************
	printf("\nCHALLENGE\n");
	sAux = CreaTramaAPDU(tmpBuf,APDU_GET_CHALLENGE_HEX, sizeof(APDU_GET_CHALLENGE_HEX));
	re = SendCmd(CMD_SAM1CardDirectControl, tmpBuf, sAux, TO_EJECT_HOLD );
	if( re != 0)
	{
		printf("ERROR GET CHALLENGE\n");
		return 0x14;	// ERROR GET CHALLENGE
	}

	printf("OK CHALLENGE\n");
	if(sData!=8)
	{
		printf("ERROR LONGITUD RESPUESTA\n");
		return 0x14;	// ERROR GET CHALLENGE
	}
	for(i=0; i<8; i++)
		Challenge[i] = dataBuf[i];

	printf("Challenge=[%04x]->", Sam_Response);
	for(i=0; i<8; i++)
		printf("[%02x]",Challenge[i]);
	printf("\n");

	Three_DES_Encrypt(Challenge,RespChall,pKey);

	printf("OUT Challenge=[%04x]->", Sam_Response);
	for(i = 0 ; i < 8; i++)
		printf("%02hhX ",RespChall[i]);
	printf("\n\n\n");


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


	//**************************************************************************
	printf("\n GET CHALLENGE\n");
	sAux = CreaTramaAPDU(tmpBuf,ExternAuth, 13);
	re = SendCmd(CMD_SAM1CardDirectControl, tmpBuf, sAux, TO_EJECT_HOLD );
	if( re != 0)
	{
		printf("ERROR GET CHALLENGE\n");
		return 0x14;	// ERROR GET CHALLENGE
	}
	printf("OK CHALLENGE\n");

	
	//**************************************************************************
	printf("\nSAM UID\n");
	sAux = CreaTramaAPDU(tmpBuf,APDU_GET_UID_SAM_HEX, sizeof(APDU_GET_UID_SAM_HEX));
	re = SendCmd(CMD_SAM1CardDirectControl, tmpBuf, sAux, TO_EJECT_HOLD );
	if( re != 0)
	{
		printf("ERROR GET SAM UID\n");
		return 0x14;	// ERROR GET SAM UID
	}

	printf("OK GET SAM UID");
	if(sData!=8)
	{
		printf("ERROR LONGITUD RESPUESTA\n");
		return 0x14;	// ERROR GET CHALLENGE
	}
	for(i=0; i<8; i++)
		SAMUID[i] = dataBuf[i];

	for(i=0; i<8; i++)
		printf("[%02X]",SAMUID[i]);
	printf("\n");

	return 0x00;		// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::GetCardKey( unsigned char mfuid[], unsigned char MFKEY[16][6], unsigned char readblock[64][16] )
{	unsigned char tmpBuf[100];
	unsigned char ExternAuth[20];
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

	//**************************************************************************
	printf("\n GET CARD KEY\n");
	sAux = CreaTramaAPDU(tmpBuf,ExternAuth, 13);
	if( SendCmd(CMD_SAM1CardDirectControl, tmpBuf, sAux, TO_EJECT_HOLD ) != 0)
	{
		printf("ERROR GET CARD KEY\n");
		return 0x14;	// ERROR GET CHALLENGE
	}

	printf("OK GET CARD KEY\n");

	//**************************************************************************
	printf("\nSAM GET LAST KEY\n");
	sAux = CreaTramaAPDU(tmpBuf,APDU_READ_LAST_KEY_HEX, sizeof(APDU_READ_LAST_KEY_HEX));
	if( SendCmd(CMD_SAM1CardDirectControl, tmpBuf, sAux, TO_EJECT_HOLD )!=0)
	{
		printf("ERROR GET LAST KEY\n");
		return 0x14;	// ERROR GET SAM UID
	}

	printf("OK GET LAST KEY\n");
	if(sData!=16)
	{
		printf("ERROR LONGITUD RESPUESTA\n");
		return 0x14;	// ERROR GET CHALLENGE
	}

	for(k=0; k<16; k++)
	{	for(i=0; i<6; i++)
//			if( k!=1 )
				MFKEY[k][i] = dataBuf[i];
	}

	for(i=0; i<6; i++)
		printf("[%02X]",dataBuf[i]);
	printf("\n");

	return 0x00;	//OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::ReadAllSector(	int TypeKey, 
									unsigned char pPASS[16][6],
									unsigned char ReadBlocks[64][16] )
{	int k, re;

	for(k=0; k<16;k++)
	{
		// Sector 1 -> Block: 4-4
		if( RF_LoadKey(TypeKey, k*4, pPASS[k]) != 0)
		{
			printf("Error Load Key\n");
			return 0x10001;
		}

		if( (re=RF_Autentifica()) != 0)
		{
			printf("Error Autentifica Sector 1A\n");
			return 0x10002;
		}

		if( RF_ReadSector(k*4, ReadBlocks[k*4]) != 0)
		{
			printf("Error Read Sector=%d\n", k);
			return 0x10003;
		}
	}
	return 0x00;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::ReadAllMIA(	unsigned char TypeKey,
					unsigned char mfuid[],
					unsigned char LlavesMIA[16][6],
					unsigned char DatosMIA[64][16])
{
	int i;

	if( CardPresent( mfuid ) !=0 )
	{
		printf("Error en Read UID\n");
		return 0x0400;		// ERROR EN UID
	}

	// Lllave del primer sector
	for(i=0; i<6; i++)
		LlavesMIA[1][i] = mfuid[i];

	// Read Sector 1
	if( Read_Sector1A(mfuid, LlavesMIA, DatosMIA) != 0)
	{
		printf("Error en Read Sector 1\n");
		return 0x0401;		// ERROR READ SECTOR 1
	}


	if( GetCardKey ( mfuid, LlavesMIA, DatosMIA) != 0)
	{
		printf("Error en Get Keys\n");
		return 0x0402;	// Error al obtener las llaves de la MIA
	}


	if( ReadAllSector(ISAM, LlavesMIA, DatosMIA ) !=0)
	{
		printf("Error en Get Keys\n");
		return 0x0402;	// Error al obtener las llaves de la MIA
	}

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::ReadListError(void)
{	int re;

	memset( kty_error, 0, sizeof(kty_error) );		// borra la lista de errores/status

	//**************************************************************************
	if( (re=SendCmd(CMD_GetAllSensorStatus, auxBuf, 0, TO_GENERAL )) < 0)
	{
		printf("Error en GetStatus\n");
		return KYT_ERROR;
	}
	if(re == KYT_POSITIVE)
	{
		ObtieneErrores();
	}
	else
	{	
		printf("Negative=%02X%02X\n", rxBuf[0x08], rxBuf[0x09]);
		return KYT_NEGATIVE;
	}
	return KYT_OK;

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::Identifica(void)
{

	if( ReadModel() == KYT_OK)
		printf("********************\nModelo=%s\n", Model);
	else
		return KYT_ERROR;

	if( ReadFirmware() == KYT_OK)
		printf("********************\nVersion=%s\n", Version);
	else
		return KYT_ERROR;

	if( ReadStatusStacker() == KYT_OK)
	{	
		printf("********************\nStacker=%02x\n", Stacker);
		if(Stacker <=3)
			printf("%s\n", kyt_stk_status[Stacker].status);
	}
	else
		return KYT_ERROR;

	if( ReadListError() == KYT_OK)
		PrintError();
	else
		return KYT_ERROR;

	if( RF_EnableAntenna(RF_Antenna12)==0)
		printf("********************\nAntenna OK\n");
	else
		return KYT_ERROR;

	return KYT_OK;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::Close(void)
{
	RS232_Close();
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::InitComm(char tport, char namecomm[], int baud)
{	int k;

	if(tport==0)
	{	// USB
		printf("Error USB Dispensador\n");
		return 0x101;
	}
	else
	{	// RS232
		printf("Openning Dispensador\n");
		if( RS232_OpenWithBaudRate(namecomm, baud)== 0)
		{	
			printf("OK OPEN Comm Dispensador\n");
			return 0x00;	// Open OK
		}
		else
		{
			printf("Error Comm Dispensador\n");
		}	
	}
	return 0x01;
}

//
// ********************** Read/write de MIA ************************************
//
/******************************************************************************/
/* Lee el Sector 1 completo  llave UID                                        */
/******************************************************************************/
int CLASS_CHM29XX::Read_Sector1A(BYTE MFUID[], BYTE pPASS[16][6], BYTE readblock[64][16])
{	unsigned char key[6];
	int i;

	for(i=0; i<6; i++)
		pPASS[1][i] = MFUID[i];

	// Sector 1 -> Block: 4-4
	if( RF_LoadKey(KEYA, 0x04, MFUID) != 0)
	{
		printf("Error Load Key\n");
		return 0x10001;
	}
	
	if( RF_Autentifica() != 0)
	{ 
		printf("Error Autentifica Sector 1A\n");
		return 0x10002;
	}

	if( RF_ReadSector(0x04, readblock[4]) != 0)
	{
		printf("Error Read Sector 1\n");
		return 0x10003;
	}

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::READ_ALL_WITH_KEY(int tKey, BYTE pPASS[16][6], BYTE ReadBlocks[64][16] )
{	int k, re;

	for(k=0; k<16;k++)
	{
		// Sector 1 -> Block: 4-4
		if( RF_LoadKey(tKey, k*4, pPASS[k]) != 0)
		{
			printf("Error Load Key\n");
			return 0x10001;
		}

		if( (re=RF_Autentifica()) != 0)
		{
			printf("Error Autentifica Sector 1A\n");
			return 0x10002;
		}
//		printf("Autentifica [k=%d -> %d -> %d]\n",k,re, rbuf);

		if( RF_ReadSector(k*4, ReadBlocks[k*4]) != 0)
		{
			printf("Error Read Sector=%d\n", k);
			return 0x10003;
		}
	}
	return 0x00;
}


/******************************************************************************/
/* Graba un Block                                                             */
/******************************************************************************/
int CLASS_CHM29XX::WriteBlock(int tKey, int nblk, BYTE pPASS[16][6], BYTE wrblock[64][16] )
{	int i, nsec, re;

	nsec = nblk/4;

	if(nblk==0)				// no escribe en el block 0
	{	printf("blk0\n");
		return 0x00;	
	}
	if(	(nblk % 4) == 0x03 )	// no escribe el 3er block de cada sector
	{	printf("blk3\n");
		return 0x00;	// no escribe en el block 0
	}
	

	// Sector x
	if(tKey==0)
		re = RF_LoadKey(KEYA, nblk, pPASS[nsec]);
	else
		re = RF_LoadKey(KEYB, nblk, pPASS[nsec]);

	if(re!= 0)
	{
		printf("Error Load Key\n");
		return 0x10001;
	}

	if( RF_Autentifica() != 0)
	{
		printf("Error Autentifica Block=%d\n", nblk);
		return 0x10002;
	}

	if( RF_WriteBlock(nblk, wrblock[nblk]) != 0)
	{
		printf("Error Write Block =%d\n", nblk);
		return 0x10003;
	}

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
	int CLASS_CHM29XX::WriteSectorMIA(	unsigned char TypeKey,
						unsigned char nsector,
						unsigned char MFUID[],
						unsigned char LlavesMIA[16][6],
						unsigned char DatosMIA[64][16])
	{	int nblk,k;

		nblk = nsector * 4;
		
		for(k=0; k<4; k++)
		{
			if( WriteBlock(TypeKey, nblk, LlavesMIA, DatosMIA )!=0)
			{
				// Error write block
				return 0x01;
			}
			nblk++;
		}

		return 0x00;
	}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::ReadPosicionMIA(void)
{	int re;

	if( (re=SendCmd(CMD_GetCardPosition, auxBuf, 0, TO_GENERAL )) < 0)
	{
		printf("Error en Posicion\n");
		return KYT_ERROR;
	}
	if(re == KYT_POSITIVE)
	{
//		printf("Pos=%02X\n", rxBuf[0x0B]); 	// Posicion
	}
	else
	{	
		printf("Negative=%02X%02X\n", rxBuf[0x08], rxBuf[0x09]);
		return KYT_NEGATIVE;
	}
	return KYT_OK;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::CardPresent(BYTE UID[])
{	int k;

	RF_EnableAntenna(RF_Antenna12);
	usleep(100000);

	if( RF_CardPresent( UID ) == 0)
	{
		printf("Card Present ok\n");
		ECode = 0x000;
		return 0x0;	// OK
	}
	else
		printf("Card NO Present\n");

	ECode = (rxBuf[8]*0x100) + rxBuf[9]; 

	return 0x01;
}

//
// **************************** MODULO RF ************************************** 
//
//
//
/******************************************************************************/
/*  Habilita Antena                                                           */
/******************************************************************************/
int CLASS_CHM29XX::RF_EnableAntenna(unsigned char nAntenna)
{	int re;

	auxBuf[0] = RF_CmdModeISO14443A;
	auxBuf[1] = nAntenna;

	if( (re=SendCmd(CMD_ExternalRfControl, auxBuf, 2, TO_GENERAL )) < 0 )
	{
		printf("Error Enable Antena\n");
		return re;
	}
	if(re == KYT_POSITIVE)
	{
		
	}
	else
	{

	}		
	return re;
}

/******************************************************************************/
/* RF Card Present                                                            */
/******************************************************************************/
int CLASS_CHM29XX::RF_CardPresent(unsigned char MFUID[])
{	int re;

	auxBuf[0] = RF_CmdTypeA_ReqStdToSelect;
	if( (re=SendCmd(CMD_ExternalRfControl, auxBuf, 1, TO_GENERAL )) < 0 )
	{
		printf("Error en RF Cart Present\n");
		return 0x01;
	}
	if(re == KYT_POSITIVE)
	{			
		printf("RF_CardPresent: rbuf:%d Data:\n",rbuf);
		for(int i = 0; i < rbuf ; i++){printf("%02x ", rxBuf[i]);}
		printf("\n");
		if(rbuf==21)
		{
			// Detecto una Tarjeta y se obtuvo su informacion
			for(re=0; re<4; re++)
				MFUID[re] = rxBuf[14+re];
			MFUID[4] = MFUID[0];
			MFUID[5] = MFUID[1];
			re = 0x00;	// Presente ok
		}
		else
		{
			// Tarjeta Presente, Error en lectura
			re = 0x01;	// presente error
		}
	}
	else
	{
		printf("RF_CardPresent: Negative=%02X%02X\n", rxBuf[0x08], rxBuf[0x09]);
	}
	return re;
}

/******************************************************************************/
/*  Load Key A o B                                                                          */
/******************************************************************************/
int CLASS_CHM29XX::RF_LoadKey(unsigned char tkey, unsigned char nblock, unsigned char key[])
{	int re;

	if(tkey == 0x00)
		auxBuf[0] = RF_CmdAuthenticationA;
	else
		auxBuf[0] = RF_CmdAuthenticationB;

	auxBuf[1] = nblock;

	for(re=0; re<6; re++)
		auxBuf[2+re] = key[re];

//	printf("Load Key\n");
	if( (re=SendCmd(CMD_ExternalRfControl, auxBuf, 8, TO_GENERAL )) < 0 )
	{
		printf("Error LoadKey\n");
		return re;
	}
	re = rxBuf[11];	//STATUS
	return re;
}

/******************************************************************************/
/*  Autentifica despues de cargar las llaves                                  */
/******************************************************************************/
int CLASS_CHM29XX::RF_Autentifica(void)
{	int re, i;

	auxBuf[0] = RF_CmdAuthentication2;

	if( (re=SendCmd(CMD_ExternalRfControl, auxBuf, 1, TO_GENERAL )) < 0 )
	{
		printf("Error Autentifica 2\n");
		return re;
	}
	re = rxBuf[11];	//STATUS

	return re;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::RF_ReadBlock(unsigned char nblock, unsigned char datos[])
{	int re;

	auxBuf[0] = RF_CmdReadBlock;
	auxBuf[1] = nblock;

	if( (re=SendCmd(CMD_ExternalRfControl, auxBuf, 2, TO_GENERAL )) < 0 )
	{
		printf("Error ReadBlock\n");
		return re;
	}
	if(re == KYT_POSITIVE)
	{

	}
	else
	{
		
	}		
	return re;	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::RF_ReadSector(unsigned char nblock, unsigned char datos[])
{	int re,i;

	auxBuf[0] = RF_CmdReadSector;
	auxBuf[1] = nblock;

	if( (re=SendCmd(CMD_ExternalRfControl, auxBuf, 2, TO_RF_GENERAL )) < 0 )
	{
		printf("Error ReadBlock\n");
		return re;
	}
	re = rxBuf[11];		//Status

	if(re == KYT_POSITIVE)
	{
		// revisa el total de datos RX
		if(rbuf != 78)
			return 0x01;	// ERROR

		re = (int)rxBuf[2] * 256;
		re = rxBuf[3];
		if(re==71)
		{	// respuesta con datos
			for(i=0; i<63; i++)
				datos[i] = rxBuf[ 12 + i];
		}
		return 0x00;
	}
	else
	{	// respondio con Error
		return 0x01;
	}
	return re;		// No respondio
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::RF_WriteBlock(unsigned char nblock, unsigned char datos[])
{	int re;

	auxBuf[0] = RF_CmdWriteBlock;
	auxBuf[1] = nblock;

	for(re=0; re<16; re++)
		auxBuf[2+re] = datos[re];

	if( (re=SendCmd(CMD_ExternalRfControl, auxBuf, 18, TO_GENERAL )) < 0 )
	{
		printf("Error WriteBlock\n");
		return re;
	}

//	printf("re=%d  tdat=%d Status=%02X\n", re , rbuf, rxBuf[11]);

	re = rxBuf[11];		// OK = 0  ERROR != 0 	
	return re;	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CHM29XX::RF_Halt(void)
{	int re;

	auxBuf[0] = RF_CmdTypeA_Halt;

	if( (re=SendCmd(CMD_ExternalRfControl, auxBuf, 1, TO_GENERAL )) < 0 )
	{
		printf("Error Halt\n");
		return re;
	}
	if(re == KYT_POSITIVE)
	{

	}
	else
	{
		
	}		
	return re;	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLASS_CHM29XX::PrintError(void)
{	int k;

	printf("********************\n");
	printf("KYT Errores Presentes\n");
	for(k=0; k<KYT_TOTAL_ERRORES; k++)
	{	if(kty_error[k]!= 0)
			printf("Error=%2d-[%04X = %s]\n", k, kyt_errores[k].codigo, kyt_errores[k].error);
	}
//	printf("*******************************\n\n");
}
////////////////////////////////////////////////////////////////////////////////