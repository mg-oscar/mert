



/******************************************************************************/

#include "kytv26xx.h"

/*                                                                            */
/******************************************************************************/
CLASS_KYT26xx::CLASS_KYT26xx()
{
	Version = 0x0000;
}

/******************************************************************************/
// desgloza las banderas del KYT
/******************************************************************************/
void CLASS_KYT26xx::showKyt(unsigned char b1,unsigned char b2)
{
	fgKyt.fgDummy0				= (b1>>7) & 0x01;       // ST1      b7
	fgKyt.fgReady				= (b1>>6) & 0x01;       // ST1      b6
	fgKyt.fgStack2FinishSensor	= (b1>>5) & 0x01;       // ST1      b5
	fgKyt.fgStack1FinishSensor	= (b1>>4) & 0x01;       // ST1      b4     
	fgKyt.fgDummy1				= (b1>>3) & 0x01;       // ST1      b3
	fgKyt.fgFinishSensor		= (b1>>2) & 0x01;       // St1      b2
	fgKyt.fgMoveSensor			= (b1>>1) & 0x01;       // St1      b1
	fgKyt.fgBinEmpty			= (b1>>0) & 0x01;       // St1      b0
	fgKyt.fgDummy2				= (b2>>7) & 0x01;       // ST2      b7
	fgKyt.fgMotor3				= (b2>>6) & 0x01;       // ST2      b6
	fgKyt.fgMotor2				= (b2>>5) & 0x01;       // ST2      b5
	fgKyt.fgMotor1				= (b2>>4) & 0x01;       // ST2      b4
	fgKyt.fgStack2Warning		= (b2>>3) & 0x01;       // ST2      b3
	fgKyt.fgStack1Warning		= (b2>>2) & 0x01;       // ST2      b2
	fgKyt.fgStack2Empty			= (b2>>1) & 0x01;       // ST2      b1
	fgKyt.fgStack1Empty			= (b2>>0) & 0x01;       // ST2      b0 
	fgKyt.fgKytW = (b1<<8) + b2;

	printf("%04X = ",fgKyt.fgKytW);
	binary(b1);
	printf(" ");
	binary(b2);
	printf("\n");
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytDispensa1(void)
{	int k;

	for(k=0; k<5; k++)
	{
		if( KytStatus() ==0 )
		{
			// Revisa si tiene tarjetas el stacker indicado
			if( fgKyt.fgStack1Empty != KYT_STACKER_EMPTY)
			{
				// dispensa tarjeta stk 1
					
					if( wrCmdKyt(CMD_STANDBY1)==0)
					{				
						return 0x00;
					}
			}
			else
				return 0x10001;			
		}
	}
	return 0x0001;		// No Contesta
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytDispensa2(void)
{	int k;

	printf("Dispensa Stacker 2\n");

	for(k=0; k<5; k++)
	{
		if( KytStatus() ==0 )
		{
			// Revisa si tiene tarjetas el stacker indicado
			if( fgKyt.fgStack2Empty != KYT_STACKER_EMPTY)
			{
				// dispensa tarjeta
					printf("Dispensa Stacker 2\n");
					
					if( wrCmdKyt(CMD_STANDBY2)==0)
					{				
						return 0x00;
					}
			}
			else
				return 0x10001;			
		}
	}
	return 0x0001;		// No Contesta
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytDispensa(unsigned char NoStk)
{	int k;

	for(k=0; k<5; k++)
	{
		if( KytStatus() ==0 )
		{
			// Revisa si tiene tarjetas el stacker indicado
			if(NoStk ==KYT_STK1)
			{
				// Stacker 1
				if( fgKyt.fgStack1Empty != KYT_STACKER_EMPTY)
				{
					// dispensa tarjeta
					
					if(KytDispensa1()==0)
						return 0x00;
				}
				else
				{
					// Stacker 1 EMPTY
					printf("Stacker 1 VACIO\n");
					return 0x1001;
				}
			}

			else if(NoStk ==KYT_STK2)
			{
				printf("Dispensa Stacker 2\n");
				// Stacker 2
				if( fgKyt.fgStack2Empty != KYT_STACKER_EMPTY)
				{
					// dispensa tarjeta
					printf("Dispensa Stacker 2\n");
					if(KytDispensa2()==0)
						return 0x00;					
					return 0x00;
				}
				else
				{
					// Stacker 2 EMPTY
					printf("Stacker 2 VACIO\n");
					return 0x1002;
				}
			}
			else if(NoStk ==KYT_STK0)
			{
				// Stacker Auto
				printf("Dispensa Staker Auto\n");
				return 0x00;
			}
			else
				return 0x1000;	// error en NoStk
			
		}
	}
	return 0x2000;		// No Contesta
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytCaptura(void)
{	int k;

	printf("Captura\n");

	for(k=0; k<5; k++)
	{
		if( KytStatus() ==0 )
		{
			if( wrCmdKyt(CMD_Capture)==0)
				{
					return 0x00;
				}
		}
	}
	return 0x0001;		// No Contesta
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytEject(void)
{	int k;

	printf("Eject\n");

	for(k=0; k<5; k++)
	{
		if( KytStatus() ==0 )
		{
			if( wrCmdKyt(CMD_FeedOut)==0)
				{
					return 0x00;
				}
		}
	}
	return 0x0001;		// No Contesta
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytRdACK(void)
{	int rdlen;
	static unsigned char buf[20];
	unsigned int mseg;

	
	buf[0] = 0x00;

	mseg = milisegundos();
//	tm = tcnt;
	// espera un ACK, NAK CAN ...
	while(1)
	{	ioctl(fd, FIONREAD, &rdlen);
		if(rdlen>0)
		{	//printf(" len =%d",rdlen);
			rdlen = read(fd, &buf[0], 1);

//			printf("-%02X-\n", buf[0]);
			switch(buf[0])
			{
				case	ACK:
						//	printf("ACK\n");
						return 0x00;
						break;
				case	NAK:
				printf("NAK\n");
						return NAK;
						break;
				case CAN:
//				printf("CAN\n");
						return CAN;
						break;
				default:
					return 0x01;

			}
		}
		if ( (milisegundos() - mseg) > KYT_TIMEOUT)				
		{	printf("\nError No Response\n");
			return 0x02;   // timeout, no response
		}
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytrdResponse(unsigned char *buf)
{
	unsigned int mseg;
//	unsigned char pd;
	int rdlen,pd;

	buf[0] = 0x00;

	pd =0;
	mseg = milisegundos();

	do
	{	ioctl(fd, FIONREAD, &rdlen);
		if(rdlen>0)
		{
			rdlen = read(fd, &buf[pd], 1);
			pd++;
		}

		if ( (milisegundos() - mseg) > KYT_TIMEOUT)				
		{
			return 0x02;   // timeout, no response
		}
	} while(pd != 5);
	pd = buf[0] ^ buf[1] ^ buf[2] ^ buf[3];
	if(pd != buf[4])
	{
		printf("\nLRC BAD\n");
		return 1;
	}
	KytWrACK();
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::wrCmdKyt(int cmd)
{	int wlen;
	unsigned char buf[20];

//	printf("fd: %d\n", fd);


	// limpia el buffer de recepcion
	ioctl(fd, FIONREAD, &wlen);
	if(wlen>0)
		wlen = read(fd, buf, wlen);

	buf[0] = STX;
	buf[1] = cmd;
	buf[2] = ETX;
	buf[3] =  buf[0] ^ buf[1] ^ buf[2];

	wlen = write(fd, buf, 4);

	if (wlen != 4)
	{
		printf("Error from write: %d, %d\n", wlen, errno);
		return 1;
	}
	// espera un ACK
	if( KytRdACK()==0 )
	{
		return 0x00;
	}

	return 0x01;	// error
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytWrACK(void)
{	int wlen;
	unsigned char buf[4];

	buf[0] = ACK;
	wlen = write(fd, buf, 1);
	if (wlen != 1)
		return 1;

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::mCMD_B9600(void)
{
	printf("B9600\n");
	wrCmdKyt(CMD_B9600);

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::mCMD_B19200(void)
{
	printf("B19200\n");
	wrCmdKyt(CMD_B19200);

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytVersion(void)
{	unsigned char buf[10],k;

	printf("Version\n");
	
	for(k=0; k<5; k++)
	{
		if ( wrCmdKyt(CMD_Version)==0)
		{
			if( wrCmdKyt(CMD_StatusRequest)==0)
			{
				// ACK OK, Request status

				if( KytrdResponse(buf)==0)
				{
					Version = buf[2] + (buf[1]*0x100);
					printf("\nVersion [%04X]\n", Version);
					return 0x00;
				}
			}
		}
	}
	return 0x01;
}
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytStatus(void)
{	unsigned int mseg;
unsigned char buf[10];


	kytNoCard=0;	// no hay tarjetas

//	printf("Request Status\n");

	mseg = milisegundos();

	if( wrCmdKyt(CMD_StatusRequest) !=0)
		return 0x01;	// Error en Status
	
	// ACK OK, Request status

	if( KytrdResponse(buf) !=0)
	{	//rdResponse();
		printf("\nError No Response\n");
		return 1;   // timeout, no response
	}

	showKyt(buf[1],buf[2]);
		st0Kyt = buf[1];
		st1Kyt = buf[2];


	Stacker[0] = st1Kyt&0x01;		// Status del Stacker 1
	Stacker[1] = st1Kyt&0x02;		// Status del Stacker 2

//		printf( "Status -> %02x %02X\n", st0Kyt,st1Kyt);

		Stacker[0] = (st1Kyt&0x01)?0:1;	// Status del Stacker 1
		Stacker[1] = (st1Kyt&0x02)?0:1;	// Status del Stacker 2

		if( (st1Kyt&0x01) && ((st1Kyt&0x02)) )
		{	kytNoCard = 0x00;   // no hay tarjetas
//			printf("NO hay tarjetas\n");
			kytNoCard = 0;		// no hay tarjetas
		}
		else
		{
//			printf("SI hay tarjetas\n");
//		printf("STK1= %d\n", Stacker[0]);
//		printf("STK2= %d\n", Stacker[1]);

			kytNoCard = 1;		// si hay tarjetas
		}

//		printf("finish sensor =%d\n", fgKyt.fgFinishSensor );

	return 0x00;
}
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytInit(char *namecomm, int baudrate)
{
	if( OpenComm((char *)"/dev/ttyS3", B9600) != 0)
	{
		printf("open comm error\n");
		return 0x01;
	}

	KytStatus();

	if ( KytVersion() == 0)
	{

	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int CLASS_KYT26xx::KytClear(void)
{
	if( wrCmdKyt(CMD_Clear) ==0)
	{
		// ACK OK, Request status
		if( KytStatus() ==0)
			printf("Status OK--------\n");
		else
			printf("Status ERROR\n");
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////