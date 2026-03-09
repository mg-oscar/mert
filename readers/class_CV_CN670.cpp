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
CLASS_CV_CN670::CLASS_CV_CN670()
{
	tot_reader = 0;
	memset(num_reader, 0, sizeof(num_reader) );
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	CLASS_CV_CN670::CV_CloseUSB(void)
{
//	close(rdr_fd);
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::GetUserInfo(int addr, char *name)
{	unsigned char dat[30], dRx[50];
	int lrx,k;

	dat[0] = CMD_GetUserInfo;
	dat[1] = 0x01;
	dat[2] = 0x00;

	lrx = SendCmd(addr, dat, 3, dRx);
	if(lrx != 0)
	{
		for(k=0; k<32;k++)
			name[k] = dRx[k+5];
		return 0x00;	// OK
	}
	return lrx;	// ERROR
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::SetUserInfo(int addr, char *name)
{	unsigned char dat[40], dRx[40];
	int lrx,k;

	memset(dRx,0, sizeof(dRx));
	strcpy((char *)dRx, name);

	dat[0] = CMD_SetUserInfo;
	dat[1] = 0x21;
	dat[2] = 0x00;

	for(k=0; k<32;k++)
		dat[3+k] = dRx[k];

	lrx = SendCmd(0x00, dat, 35, dRx);

	if(lrx != 0)
	{
		return 0x00;	// OK
	}
	return lrx;	// ERROR
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	CLASS_CV_CN670::ActiveBuzzer(unsigned char addr, unsigned char modo, unsigned char *buf)
{	unsigned char dat[10], dRx[10], len, i;

	len = 4;
	dat[0] = CMD_ActiveBuzzer;
	dat[1] = 0x02;
	dat[2] = 0x00;
	dat[3] = modo;

	if(modo==4)
	{	for(i=0; i<5; i++)
			dat[4+i] = buf[i];
		dat[1] = 0x07;
		len = 9;
	}

	SendCmd(0x00, dat, len, dRx);
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::CardPresent( unsigned char *MFUID )
{	unsigned char dat[5], dRx[50];
	int lrx,k;

	dat[0] = CMD_PN_InListPassive;
	dat[1] = 0x04;	// total de datos
	dat[2] = 0x01;	// time Out

	dat[3] = 0x00;	// MIFARE
	dat[4] = 0x01;	// 1 tarjeta
	dat[5] = 0x00;	// Baud rate, baud rate, 0—106kbps, 1—212kbps, 2—424kbps.

	lrx = SendCmd(0x00, dat, 6, dRx);
	if(lrx != 0)
	{	//printf("CardLen=%d\n", lrx);
		if( (lrx == 17) && (dRx[4]==00) )
		{
			for(k=0; k<4; k++)
				MFUID[k] = dRx[11+k];
			MFUID[4] = MFUID[0];
			MFUID[5] = MFUID[1];
//			printf("tarjeta presente\nDatos = [");
//			for(k=0; k<17; k++)
//				printf("%02X", dRx[k]);
//			printf("]\n");
			return 0;
		}
		else
		{
//			printf("NO HAY TARJETA\n");
		}
	}
	return 0x100;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::GetSerialNum(unsigned char addr, int *Address, char *SerialNUM)
{	unsigned char dat[30], dRx[50];
	int lrx,k;

	dat[0] = CMD_GetSerialNum;
	dat[1] = 0x01;
	dat[2] = 0x00;

	lrx = SendCmd(0x00, dat, 3, dRx);
	if(lrx != 0)
	{
		for(k=0; k<8;k++)
			SerialNUM[k] = dRx[k+5];

		*Address = dRx[13] + (dRx[14]*0x100);
		
		return 0x00;	// OK
	}
	return lrx;	// ERROR
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::SetLED(int addr, unsigned char modo)
{	unsigned char dat[5], dRx[10];
	int lrx;

	dat[0] = CMD_SetLED;
	dat[1] = 0x02;
	dat[2] = 0x00;
	dat[3] = modo;

	lrx = SendCmd(0x00, dat, 4, dRx);
	if( (lrx == 7) && (dRx[4]==00) )
	{
//		printf("OK SetLed\n");
		return 0x00;	// OK
	}
	return 0x01;	// error
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::SetAutoPowerSave(unsigned char addr, unsigned char modo)
{	unsigned char dat[5], dRx[10];
	int lrx;

	dat[0] = CMD_SetAutoPowerSave;
	dat[1] = 0x02;	// total de datos
	dat[2] = 0x00;	// time Out
	dat[3] = modo;	// power 0=Disable  1=activo

	lrx = SendCmd(addr, dat, 4, dRx);
	if( (lrx == 7) && (dRx[4]==00) )
	{
//		printf("OK POWER SAVE\n");
		return 0x00;	// OK
	}
	return 0x01;	// error
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::SendSetWiegandMode(unsigned char addr)
{	unsigned char dat[25], dRx[25];
	int lrx;

//printf("18\n");
	dat[ 0] = CMD_SetWiegandMode;
	dat[ 1] = 0x0E;		// Total de datos
	dat[ 2] = 0x00;		// TimeOut
	dat[ 3] = 0x00;		// 	d0
	dat[ 4] = 0x00;		//  d1	block 0
	dat[ 5] = 0x52;		//  d2	ALL
	dat[ 6] = 0x10;		//  d3      0001 0011  
	dat[ 7] = 0x55;		// 	d4 
	dat[ 8] = 0xaa;		// 	d5
	dat[ 9] = 0x01;		//  d6 	Key Format 1=4 bit
	dat[10] = 0x04;		// 	d7 	Output  b0=0  b1=UART->1  b2=Press key-> 1 
	dat[11] = 0x00;		//  d8	Read Card Type
	dat[12] = 0x00;		// 	d9	Cart Type
	dat[13] = 0x00;		//  d10	reserved
	dat[14] = 0x00;		//  d11	reserved
	dat[15] = 0x00;		// 	d12	reserved

	lrx =  SendCmd( 0x00, dat, 16, dRx);
	if( (lrx == 7) && (dRx[4]==00) )
	{
		return 0x00;	// OK
	}
	return lrx;

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::SendGetWiegandMode(unsigned char addr)
{	unsigned char dat[5], dRx[20];
	int lrx;

//printf("1D\n");
	dat[0] = CMD_GetWiegandMode;
	dat[1] = 0x02;	// total de datos
	dat[2] = 0x00;	// time Out
	dat[3] = 0x00;	//

	lrx = SendCmd( 0x00, dat, 4, dRx);
	if( (lrx == 17) && (dRx[4]==00) )
	{
		return 0x00;	// OK
	}
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::SendSetWiegandModeLB(unsigned char addr, unsigned char stat)
{	unsigned char dat[5], dRx[20];
	int lrx;

	dat[0] = CMD_SetWiegandModeLB;
	dat[1] = 0x02;	// total de datos
	dat[2] = 0x00;	// time Out
	dat[3] = stat;	//

	lrx = SendCmd( 0x00, dat, 4, dRx);
	if( (lrx == 7) && (dRx[4]==00) )
	{
		return 0x00;	// OK
	}

	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::MF_Read (BYTE addr, BYTE nblk, BYTE tblk, BYTE *buffer)
{	unsigned char dat[10], dRx[100];
	int lrx,i;

	dat[0] = CMD_MF_Read;
	dat[1] = 0x03;	// total de datos
	dat[2] = 0x00;	// time Out
	dat[3] = nblk;	// numero inicial de block
	dat[4] = tblk;	// total de block

	lrx = SendCmd(0x00, dat, 5, dRx);
	if(lrx == (7 + tblk*16) )
	{	// sacar datos
		if(dRx[4]==00)
		{
//			printf("OK READ\n");
			for(i=0; i<(tblk*16); i++)
				buffer[i] = dRx[5 + i];
			return 0x00;	// OK
		}
	}
	return lrx;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::MF_Write(BYTE addr, BYTE nblock, BYTE tblock, BYTE *buffer)
{	unsigned char dat[100], dRx[10];
	int lrx,k;

	// validar que no grabe el 4to block de cada sector

	dat[0] = CMD_MF_Write;
	dat[1] = (tblock*16)+3;	// total de datos
	dat[2] = 0x00;			// time Out
	dat[3] = nblock;			// numero inicial de block
	dat[4] = tblock;			// total de block

	for(k=0; k<(tblock*16); k++)
		dat[5+k] = buffer[k];

	lrx = SendCmd(0x00, dat, (tblock*16) + 5, dRx);
	if( (lrx == 7) && (dRx[4]==00) )
	{
//		printf("OK WRITE\n");
		return 0x00;	// OK
	}
	return 0x01;	// ERROR
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::PN_SelectTarget(unsigned char addr, unsigned char ncard)
{	unsigned char dat[10], dRx[10];
	int lrx;

	dat[0] = CMD_PN_SelectTarget;
	dat[1] = 0x02;	// total de datos
	dat[2] = 0x00;	// time Out
	dat[3] = ncard;	// Tarjeta a Seleccionar

	lrx = SendCmd(0x00, dat, 4, dRx);
	if( (lrx == 7) && (dRx[4]==00) )
	{
//		printf("OK SELECT TARGET\n");
		return 0x00;	// OK
	}
	return 0x01;	// ERROR
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::MF_LoadKey(unsigned char addr, unsigned char *Key)
{	unsigned char dat[10], dRx[10];
	int lrx;

	dat[0] = CMD_MF_LoadKey;
	dat[1] = 0x07;	// total de datos
	dat[2] = 0x00;	// time Out

	for(lrx=0; lrx<6; lrx++)
		dat[3+ lrx] = Key[lrx];	// time Out

	lrx = SendCmd(0x00, dat, 9, dRx);
	if( (lrx == 7) && (dRx[4]==00) )
	{
//		printf("OK Load Key\n");
		return 0x00;	// OK
	}
	return lrx;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::MF_Auth(BYTE addr, BYTE tkey, BYTE *MFUID, BYTE nblk)
{	unsigned char dat[10], dRx[10];
	int lrx;

	dat[0] = CMD_MF_Auth;
	dat[1] = 0x07;	// total de datos
	dat[2] = 0x00;	// time Out
	dat[3] = tkey;	// KeyA o KeyB

	for(lrx=0; lrx<4; lrx++)
		dat[4 + lrx] = MFUID[lrx];	// time Out

	dat[8] = nblk;	// no de block

	lrx = SendCmd(0x00, dat, 9, dRx);
	if( (lrx == 7) && (dRx[4]==00) )
	{
//		printf("OK AUTH\n");
		return 0x00;	// OK
	}
	return lrx;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::MF_Decrement(BYTE addr, BYTE PurseBlock, int valor)
{
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::MF_Transfer(BYTE addr, BYTE PurseBlock)
{
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::PN_InListPassiveTarget(int addr, unsigned char CardType, unsigned char MaxTg, 
				unsigned char BaudRate, unsigned char IniDataLen, 
				unsigned char *IniData, unsigned char *NbTg, 
				unsigned char *RDataLen,unsigned char *RData)
{

	
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::ISO7816_ATR(BYTE addr, BYTE var1, BYTE *byRecvData , BYTE iRecvDataLen, int nmax)
{
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::ISO7816_APDU_Exchange(BYTE addr, BYTE var1, BYTE *apdu, BYTE var2, 
			BYTE *byRecvData, BYTE iRecvDataLen, int nmax)
{
	
	
	
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_CV_CN670::rdResponse(unsigned char *bufRx, unsigned char seq)
{	int re,k, len, op;
	unsigned char brx[1024], ap;

	memset(bufRx, 0, sizeof(bufRx));
	memset(brx, 0, sizeof(brx));

	ap = op = 0;
	unsigned int tm = milisegundos();
	while( (milisegundos()-tm) < 1000)
	{
//		re = read(rdr_fd, brx, 100);
		re = USB_Read2( 100, brx, 40);
//		printf("rx=%d\n", re);
		if(re>0)
		{	// recibio datos, verificar sequencia
			for(k=0; k<re; k++, ap++)
			{	
				bufRx[ap] = brx[k];
			}
			
			if(bufRx[1] == seq)
			{	// seq ok
//				printf("seq ok\n");
				if(ap > 4 )
				{	len = bufRx[3] + 6;
//					printf("len total=%d-%d\n",len, ap);
					if( (ap) == len)
					{	op = 1;
						break;
					}
				}
			}
			else
			{	ap = 0;
				printf("seq bad\n");
			}
		}
	}

	return re;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	CLASS_CV_CN670::SendCmd(unsigned char addr, unsigned char *datos, int len, unsigned char *dRx)
{	unsigned char dTx[100], dResponse[100], bcc;
	static unsigned char seq=0xF0;
	int i, k;

	seq += 0x10;
	seq |= 0x80;

	dTx[0] = STX;
	dTx[1] = seq;
	dTx[2] = addr;

	for(i=0; i<len; i++)
		dTx[i+3] = datos[i];
	len +=3;

	bcc = 0;
	for(i=1; i< len; i++)
		bcc = bcc ^ dTx[i];

	dTx[i] = bcc;
	dTx[i+1] = ETX;

	len = i+2;

//	printf("Dtx=[%d]\n", len);
//	for(i=0; i<len; i++)
//		printf("[%02x]",dTx[i]);
//	printf("\n");

	i = USB_Write(len, dTx);
	i = rdResponse(dResponse, seq);
	if(i != 0)
	{
		for(k=0; k<i; k++)
			dRx[k] = dResponse[k];
	}
	return  i;
}
////////////////////////////////////////////////////////////////////////////////