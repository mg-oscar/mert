/* File:   kyt_cim6903.cpp
 * Author: user
 * 
 * Created on March 2, 2023, 2:17 PM
 */

#include "class_cim6903.h"

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
CLASS_CIM6903::CLASS_CIM6903()
{
	memset( kty_error, 0, sizeof(kty_error) );		// borra la lista de errores/status

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::AnalizaRespuesta(unsigned char rx)
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
int CLASS_CIM6903::waitResponse(unsigned char resp)
{	unsigned int tm= milisegundos();

	while( (milisegundos()-tm) < 1000)
	{
		RS232_Read();
		while( existDat() )
		{
			if( AnalizaRespuesta(getBufferRx()) == 0)
			{	// Respuesta completa OK
				// comprobar si es POSITIVE o NEGATIVE

//				printf("Respuesta Completa\n");
				send1B(ACK);
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
int CLASS_CIM6903::waitACK(void)
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
int CLASS_CIM6903::creaTrama(char NoCmd, unsigned char *datos, int tdat)
{	int i;
	unsigned char bcc;

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

//	if(NoCmd==55)
//	{
//		printf("Datos a Tx=%d\n[", sbuf);
//		for(i=0; i< sbuf; i++)
//			printf("%02X,", cmdBuf[i]);
//		printf("]\n");
//	}

	return 0;
}

/******************************************************************************/
/* Envia comaando de 1B, sin datos                                            */
/******************************************************************************/
int CLASS_CIM6903::send1B(unsigned char by)
{	unsigned char dat[2];
	dat[0] = by;
	RS232_Write( 1, dat);
//	printf("1=%02X\n", by);
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::Sincroniza(void)
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
int CLASS_CIM6903::SendCmd(char NoCmd, unsigned char *datos, int tdat)
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
				if( (trec=waitResponse(0)) >= 0)
				{
					return 0;
				}
				send1B(NAK);
//				printf("menos -1\n");
				return -1;
				break;
		}
	}
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::searchErr(int nerr)
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
int CLASS_CIM6903::ObtieneErrores(void)
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
int CLASS_CIM6903::Captura(void)
{	int k;

	for(k=0; k<5; k++)
	{
		usleep(200000);
		// captura la Tarjeta
		if( SendCmd(CMD_CardCapture, auxBuf, 0 ) != 0)
		{
			printf("Error en Capture\n");
			continue;
		}

		usleep(200000);
		if(ReadPosicionMIA()==0)
		{
//			if(rxBuf[0x0B]== SEN4)
			if(rxBuf[0x0B]== 0x00)
			{	// NO HAY TARJETA EN READER
//				printf("NO HAY TARJETA\n");
				return 0x00;	// OK
			}
		}	
	}
	return 0x01;	// error no se capturo la tarjeta
}

/******************************************************************************/
/* Dispensa la tarjeta y la Sostiene con los rodillos                         */
/******************************************************************************/
int CLASS_CIM6903::EjectHold(void)
{
	if( SendCmd(CMD_CardEjectHold, auxBuf, 0 ) != 0)
	{
		printf("Error en Dispensa & Suelta\n");
		return 0x01;
	}
//	printf("OK Dispensa y Suelta\n");

	return 0;
}

/******************************************************************************/
/* Mueve una tarjeta del Stacker al Modulo de RF                              */
/******************************************************************************/
int CLASS_CIM6903::CardMoveFromStack(char stk)
{
	// Mueve una tarjeta del Stacker al Reader
	auxBuf[0] = stk;		// 1=Stacker1   2=Stacker2
	auxBuf[1] = RF_MODULE;	// Move to RF Module
	if( SendCmd(CMD_CardMoveFromStack, auxBuf, 2 ) != 0)
	{
		printf("Error en Card move\n");
		return 0x01;
	}
	return 0;
}

/******************************************************************************/
/* Toma la tarejeta del frente y la mueve al RF Module                        */
/******************************************************************************/
int CLASS_CIM6903::CardTakeMoveToRF(void)
{
	auxBuf[0] = 0x03;	// La mueva al RF Module
	if( SendCmd(CMD_CardTakeMoveTo, auxBuf, 1 ) != 0)
	{
		printf("Error en Take\n");
		return 0x01;
	}
//	printf("OK Take Card\n");

	return 0;
}

/******************************************************************************/
/* Dispensa la tarjeta y la suelta fuera del dispensador                      */
/******************************************************************************/
int CLASS_CIM6903::EjectDrop(void)
{
	// Dispensa la tarjeta y la suelta 
	if( SendCmd(CMD_CardEjectDrop, auxBuf, 0 ) != 0)
	{
		printf("Error en Dispensa & Suelta\n");
		return 0x01;
	}
//	printf("OK Dispensa y Suelta\n");

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::ReadModel(void)
{	int re;

	memset(Model, 0, sizeof(Model));

	if( (re=SendCmd(CMD_GetModel, auxBuf, 0 )) < 0)
	{
		printf("Error en GetModel\n");
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
int CLASS_CIM6903::ReadFirmware(void)
{	int re;

	memset(Version, 0, sizeof(Version));

	//**************************************************************************
	if( (re=SendCmd(CMD_GetFirmware, auxBuf, 0 )) < 0)
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
int CLASS_CIM6903::ReadStatusStacker(void)
{	int re;

	Stacker[0] = STACKER_ERROR;
	Stacker[1] = STACKER_ERROR;

	if( (re=SendCmd(CMD_GetStacker, auxBuf, 0 )) < 0)
	{
		printf("Error en GetStatusStacker\n");
		return KYT_ERROR;
	}

	if(re == KYT_POSITIVE)
	{
		Stacker[0] = rxBuf[0x0B];
		Stacker[1] = rxBuf[0x0C];
	}
	else
	{
		printf("Negative->=%02X%02X\n", rxBuf[0x08], rxBuf[0x09]);
		return KYT_NEGATIVE;
	}
	return KYT_OK;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::ReadListError(void)
{	int re;

	memset( kty_error, 0, sizeof(kty_error) );		// borra la lista de errores/status

	//**************************************************************************
	if( (re=SendCmd(CMD_GetStatus, auxBuf, 0 )) < 0)
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
int CLASS_CIM6903::Identifica(void)	
{

	if( ReadModel() == KYT_OK)
		printf("Modelo=%s\n", Model);
	else
		return KYT_ERROR;

	if( ReadFirmware() == KYT_OK)
		printf("Version=%s\n", Version);
	else
		return KYT_ERROR;

	if( ReadStatusStacker() == KYT_OK)
		printf("Stacker=%02x-%02X\n", Stacker[0], Stacker[1]);
	else
		return KYT_ERROR;

	if( ReadListError() == KYT_OK)
		PrintError();
	else
		return KYT_ERROR;

//	if( Captura() == KYT_OK)
//		printf("Captura OK\n");
//	else
//		return KYT_ERROR;

	if( RF_EnableAntenna(RF_Antenna12)==0)
		printf("Antenna OK\n");
	else
		return KYT_ERROR;

	return KYT_OK;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::Close(void)
{
	RS232_Close();
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::InitComm(char tport, char namecomm[], int baud)
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
int CLASS_CIM6903::Read_Sector1A(BYTE MFUID[], BYTE pPASS[16][6], BYTE readblock[64][16])
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
int CLASS_CIM6903::ReadSector(int TypeKey, BYTE nSector, BYTE pPASS[], BYTE readblock[])
{
	
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::READ_ALL_WITH_KEY(int tKey, BYTE pPASS[16][6], BYTE ReadBlocks[64][16] )
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
int CLASS_CIM6903::WriteBlock(int tKey, int block, BYTE pPASS[16][6], BYTE wrblock[64][16] )
{	int i, nsec, re;

	nsec = block/4;

	// Sector x
	if(tKey==0)
		re = RF_LoadKey(KEYA, block, pPASS[nsec]);
	else
		re = RF_LoadKey(KEYB, block, pPASS[nsec]);

	if(re!= 0)
	{
		printf("Error Load Key\n");
		return 0x10001;
	}

	if( RF_Autentifica() != 0)
	{
		printf("Error Autentifica Block=%d\n", block);
		return 0x10002;
	}

	if( RF_WriteBlock(block, wrblock[block]) != 0)
	{
		printf("Error Write Block =%d\n", block);
		return 0x10003;
	}

	return 0x00;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::READ_ALL(int TypeKey,  BYTE UID[], BYTE pPASS[16][6], BYTE ReadBlocks[64][16] )
{	int i;

	// Sector 1, se lee con el KEYA = UID de password
	if( (i=Read_Sector1A(UID, pPASS, ReadBlocks ))!= 0)
	{
		printf("Error Sector 1A=[%04X]\n",i);
		return i;
	}
	printf("Sector 1A OK\n");

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::ReadPosicionMIA(void)
{	int re;

	memset(Model, 0, sizeof(Model));

	if( (re=SendCmd(CMD_GetCardPosition, auxBuf, 0 )) < 0)
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
int CLASS_CIM6903::MoveMiaToReader(BYTE stk)
{	int k;

	for(k=0; k<5; k++)
	{
		if( CardMoveFromStack(stk) == 0)
		{
			printf("Move card ok\n");
			return 0x0;	// OK
		}
		if(ReadPosicionMIA()==0)
		{
			if(rxBuf[0x0B]==SEN4)
			{	// MIA EN READER
				printf("MIA en Posicion ok\n");
				return 0x0;	// OK
			}
		}	
	}
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::CardPresent(BYTE UID[])
{	int k;

	for(k=0; k<5; k++)
	{
		RF_EnableAntenna(RF_Antenna12);

		if( RF_CardPresent( UID ) == 0)
		{
//			printf("Card Present ok\n");
			return 0x0;	// OK
		}
		usleep(300000);
	}
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
int CLASS_CIM6903::RF_EnableAntenna(unsigned char nAntenna)
{	int re;

	auxBuf[0] = RF_CmdModeISO14443A;
	auxBuf[1] = nAntenna;

	if( (re=SendCmd(CMD_ExternalRFModuleControl, auxBuf, 2 )) < 0 )
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
int CLASS_CIM6903::RF_CardPresent(unsigned char MFUID[])
{	int re;
//	auxBuf[0] = RF_CmdTypeA_ReqAllToSelect;
	auxBuf[0] = RF_CmdTypeA_ReqStdToSelect;
	if( (re=SendCmd(CMD_ExternalRFModuleControl, auxBuf, 1 )) < 0 )
	{
		printf("Error en RF Cart Present\n");
		return 0x01;
	}
	if(re == KYT_POSITIVE)
	{	if(rbuf==21)
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
		printf("Negative=%02X%02X\n", rxBuf[0x08], rxBuf[0x09]);
	}
	return re;
}

/******************************************************************************/
/*  Load Key A o B                                                                          */
/******************************************************************************/
int CLASS_CIM6903::RF_LoadKey(unsigned char tkey, unsigned char nblock, unsigned char key[])
{	int re;

	if(tkey == 0x00)
		auxBuf[0] = RF_CmdAuthenticationA;
	else
		auxBuf[0] = RF_CmdAuthenticationB;

	auxBuf[1] = nblock;

	for(re=0; re<6; re++)
		auxBuf[2+re] = key[re];

//	printf("Load Key\n");
	if( (re=SendCmd(CMD_ExternalRFModuleControl, auxBuf, 8 )) < 0 )
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
int CLASS_CIM6903::RF_Autentifica(void)
{	int re, i;

	auxBuf[0] = RF_CmdAuthentication2;

	if( (re=SendCmd(CMD_ExternalRFModuleControl, auxBuf, 1 )) < 0 )
	{
		printf("Error Autentifica 2\n");
		return re;
	}

	re = rxBuf[11];	//STATUS

//	printf("re=%d tdat=%d \n", re, rbuf);
//	for(i=0; i<rbuf; i++)
//		printf("%02x,",rxBuf[i]);
//	printf("\n");
	
//	if(re == KYT_POSITIVE)
//	{
//		return KYT_POSITIVE;
//	}
//	else
//	{
//		return KYT_NEGATIVE;
//	}
	return re;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::Autentifica(int TypeKey, int nblock, BYTE MFUID[], BYTE pPASS[])
{
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::RF_ReadBlock(unsigned char nblock, unsigned char datos[])
{	int re;

	auxBuf[0] = RF_CmdReadBlock;
	auxBuf[1] = nblock;

	if( (re=SendCmd(CMD_ExternalRFModuleControl, auxBuf, 2 )) < 0 )
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
int CLASS_CIM6903::RF_ReadSector(unsigned char nblock, unsigned char datos[])
{	int re,i;

	auxBuf[0] = RF_CmdReadSector;
	auxBuf[1] = nblock;

	if( (re=SendCmd(CMD_ExternalRFModuleControl, auxBuf, 2 )) < 0 )
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
int CLASS_CIM6903::RF_WriteBlock(unsigned char nblock, unsigned char datos[])
{	int re;

	auxBuf[0] = RF_CmdWriteBlock;
	auxBuf[1] = nblock;

	for(re=0; re<16; re++)
		auxBuf[2+re] = datos[re];

	if( (re=SendCmd(CMD_ExternalRFModuleControl, auxBuf, 18 )) < 0 )
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
int CLASS_CIM6903::RF_Halt(void)
{	int re;

	auxBuf[0] = RF_CmdTypeA_Halt;

	if( (re=SendCmd(CMD_ExternalRFModuleControl, auxBuf, 1 )) < 0 )
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
int CLASS_CIM6903::RF_PurseIncrementTransfer( int TypeKey, BYTE MFUID[], int PurseBlock, 
											BYTE pPASS[16][6], int valor)
{
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::RF_PurseDecrementTransfer(	int TypeKey, BYTE MFUID[], int PurseBlock, 
											BYTE pPASS[16][6], int valor)
{
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::RF_PurseRestore(int TKey, int PurseBlock, BYTE key[], int valor)
{	int re;

//	printf("\nLoad Key\n");
	re = RF_LoadKey(0x00, PurseBlock, key);

//	printf("\nAutentifica\n");
	re = RF_Autentifica();

	auxBuf[0] = RF_CmdRestore;
	auxBuf[1] = PurseBlock;

	auxBuf[2] = (valor>>  0) & 0xFF;
	auxBuf[3] = (valor>>  8) & 0xFF;
	auxBuf[4] = (valor>> 16) & 0xFF;
	auxBuf[5] = (valor>> 24) & 0xFF;

//	printf("\nRestore\n");
	if( (re=SendCmd(CMD_ExternalRFModuleControl, auxBuf, 6 )) < 0 )
	{
		printf("Error Restore\n");
		return re;
	}
	if(re == KYT_POSITIVE)
	{
		if(rxBuf[0x0B]==0)
		{	
//			printf("Restore OK\n");
		}
		else
		{
//			printf("Error Restore=%d \n", rxBuf[0x0B]);
		}

	}
	else
	{
		
	}		
	return re;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::RF_PurseTransfer(int TypeKey, BYTE MFUID[], int PurseBlock, 
									BYTE pPASS[16][6], int valor)
{
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::RF_PurseRestoreTransfer(	int TypeKey, BYTE MFUID[], int PurseBlock, 
											BYTE pPASS[16][6], int valor)
{
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CIM6903::RF_Buzzer(unsigned char tm)
{	int re;

	auxBuf[0] = RF_CmdBuzzer;
	auxBuf[1] = tm;

	if( (re=SendCmd(CMD_ExternalRFModuleControl, auxBuf, 2 )) < 0 )
	{
		printf("Error Buzzer\n");
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
void CLASS_CIM6903::PrintError(void)
{	int k;

	printf("\n*****************************\n");
	printf("KYT Errores Presentes\n");
	for(k=0; k<KYT_TOTAL_ERRORES; k++)
	{	if(kty_error[k]!= 0)
			printf("Error=%2d-[%04X = %s]\n", k, kyt_errores[k].codigo, kyt_errores[k].error);
	}
	printf("*******************************\n\n");
}
////////////////////////////////////////////////////////////////////////////////