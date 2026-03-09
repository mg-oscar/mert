/*
 * File:   Inicializa los Perifericos
 * Author: FAMISA
 *
*/
#include "bd.h"
#include "cctalk.h"
#include "xmls.h"
#include "libMIA.hpp"
#include "printer.h"
#include "readers.h"
#include "readers/class_cim6903.h"
#include "readers/class_civintec.h"
#include "readers/class_CV_CN670.h"
#include "readers/class_acr1222l.h"
#include "serialport.h"
#include "kytv26xx.h"

void idACR1222(void);
void idACR1252(void);

/******************************************************************************/
/* Valida el SAM                                                              */
/******************************************************************************/
int SAM_Init(void)
{	static int tmRecarga = milisegundos() - 2001;

	if( MertPort.REC_SAM_SLOT0!=0)
	{
		if( MERT_RECARGA_SAM ==1)
			return 0x00;	// ya esta validado

		// validar cada 2 segundos
		if( (milisegundos() - tmRecarga) <= 2000)
			return 0x01;
			
		tmRecarga = milisegundos();

		if( recarga.AutentificaSAM(MertPort.REC_SAM_SLOT0 ) == 0)
		{
			printf("Autentificacion OK\n");
			// por mientras aqui
			memcpy(recarga.MIA.SamUID, recarga.reader_crt285.SamUID,8);
			MERT_RECARGA_SAM = 1;
			printf("Recarga OK  SAM=[%d]\n", MERT_RECARGA_SAM);
			return 0x00;
		}
		return 0x01;	// No pudo validar el SAM
	}
	printf("SIN SAM EN LECTOR DE RECARGA\n");
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	Recarga_Init(void)
{	static int tmRecarga = milisegundos() - TiempoEntrePreguntas -10;

	if( (milisegundos() - tmRecarga) >= TiempoEntrePreguntas)
	{	tmRecarga = milisegundos();

		MERT_RECARGA_SAM = 0;

		if( lUSBCRT == NULL)
		{
			printf("busca usb crt\n");
			if( (lUSBCRT=find_usb_device(CRT_VENDOR, CRT_PRODUCT)) == NULL)
			{	printf("USB CRT NO SE ENCONTRO\n");
				return 0x01;
			}
		}

		if(recarga.initUSB(MertPort.REC_Marca, lUSBCRT) != 0)
		{
			printf("Open Error Reader Recarga\n");
			return 0x0101;
		}
		if( MertPort.REC_SAM_SLOT0!=0)
		{
			if( recarga.AutentificaSAM(MertPort.REC_SAM_SLOT0 ) == 0)
			{
				printf("Autentificacion OK\n");
				// por mientras aqui
				memcpy(recarga.MIA.SamUID, recarga.reader_crt285.SamUID,8);
				MERT_RECARGA_SAM = 1;
			}
		}
		printf("Recarga OK  SAM=[%d]\n", MERT_RECARGA_SAM);
		return 0x00;
	}
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	Venta_Init(unsigned char usb)
{	static int tmVenta = milisegundos() - TiempoEntrePreguntas - 10;
	static char op = 0;
	long rv;

//	printf("Marca   =%d\n", MertPort.EXP_Marca);
//	printf("MarcaExp=%d\n", MertPort.EXP_USBMarca);
	
	if( (milisegundos() - tmVenta) >= 500)
	{
		tmVenta = milisegundos();

		if( MertPort.EXP_Marca ==RDR_CIM6903)
		{
			// KYT CIM6903
			switch(op)
			{
				case 0:

					if(venta.initComm(MertPort.EXP_Marca, MertPort.EXP_TipoPuerto,
								MertPort.EXP_SerialName, MertPort.EXP_SerialBaud) != 0)
					{
						printf("Open Error Dispensador KYT-CIM6903\n");
						return 0x0101;
					}
					op = 1;

				case 1:
					// ID del Dispensador y Status del Stacker
					if (venta.Identifica() == 0)
					{	printf("Stacker1=%d   Stacker2=%d\n", 
								venta.reader_cim6903.Stacker[0], venta.reader_cim6903.Stacker[0]);
					op = 0;
						return 0x00;
					}
			}
		} else if( MertPort.EXP_Marca==DIS_KYT2664)
		{
			// KYT2664
			printf("KYT2664\n");
			switch(op)
			{
				case 0:
					// Open RS232
					if(venta.initComm(MertPort.EXP_Marca, MertPort.EXP_TipoPuerto,
								MertPort.EXP_SerialName, MertPort.EXP_SerialBaud) != 0)
					{
						printf("Open Comm Error Dispensador\n");
						return 0x0101;
					}
					printf("Open CommDispensador OK\n");

					op++;
				case 1:
					if(venta.reader_kyt2664.KytStatus() !=0)
					{
						printf("KYT2664 Status Error\n");
						return 0x0101;
					}
					
					venta.reader_kyt2664.KytCaptura();
					
					op++;

				case 2:
					if (usb==1)
					{
						if( MertPort.EXP_USBMarca == RDR_ACR1222L)
						{
							printf("Id=%d\n",idDispensador );
							rv = venta.reader_acr1222.ReaderContext(ghContext, grdracs, indDispensador);
							if(rv != SCARD_S_SUCCESS)
							{
								printf("Error en COntext = %d\n",indDispensador);
								return rv;
							}
							rv = venta.reader_acr1222.ActiveBuzzer(1);
							printf("rv = %08lx\n", rv );
//							return rv;
							op++;
						}
					}
					break;
				case 3:
					venta.modelo	= MertPort.EXP_Marca;		// dispensador
					venta.modelo_exp= MertPort.EXP_USBMarca;	// reader

					printf("modelo exp=%d\n", venta.modelo_exp);
					printf("modelo    =%d\n", venta.modelo);

					op =0;
					return 0x00;
					break;
			}
		}
	}
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	Puerta_Init()
{	int addr, k;
	char SerialNUM[10], name[50];

	static int tmPuerta = milisegundos() - TiempoEntreCivintec -10;

	puerta.modelo = MertPort.PUERTA_Marca; 

	if( (milisegundos() - tmPuerta) >= TiempoEntreCivintec)
	{	tmPuerta = milisegundos();

		memset(name, 0, sizeof(name));
		memset(SerialNUM, 0, sizeof(SerialNUM));

		k=0;
		for(k=0; k< NO_MAX_USB; k++)
		{
			if( (lUSB[k]==NULL) )
				continue;

			name[0] = '\0';
			puerta.reader_cn670.cn670.nUSB = lUSB[k];
			if( puerta.reader_cn670.cn670.GetUserInfo(0, name) == 0)
			{
				if(  strcmp(name, MertPort.PUERTA_SerialName)==0)
				{
					printf("***************\nReader Encontrado=[%s]\n", name);
					puerta.reader_cn670.cn670.GetSerialNum (0, &addr, SerialNUM);
					printf("CurrentAddress=[%d] %d  SerialNUM=%s \n", k, addr, SerialNUM);
					usleep(100000);
					printf("Set LED\n");
					puerta.reader_cn670.cn670.SetLED(0, LED_ALL_ON);
					usleep(100000);
					puerta.reader_cn670.Buzzer(2);
					usleep(100000);
					puerta.reader_cn670.cn670.SetAutoPowerSave(0, 0x00);	// siempre activo
					printf("***************************************\n");
					break;
				}
			}
		}
		if(k == NO_MAX_USB)
		{	
			puerta.reader_cn670.cn670.nUSB = NULL;
			return 0x0101;	// no se encontro
		}

		// Habilitar Modo de AutoRead
		if( puerta.SetMode() != 0)
		{
			printf("Error AutoRead PUERTA\n");
			return 0x0101;
		}
		printf("OK AutoRead PUERTA\n");

		if(MertPort.PUERTA_SAM_SLOT0 != 0)
		{	// HAY UN SAM EN SLOT 0, AUTENTIFICA
			puerta.TipoSAM = MertPort.PUERTA_SAM_SLOT0;
//			printf("SAM=%d\n", MertPort.PUERTA_SAM_SLOT0);
			if( puerta.AutentificaSAM(MertPort.PUERTA_SAM_SLOT0 ) != 0)
			{	printf("Error en Autentificacion Puerta[]\n");
				return 0x101;
			}
		}
		printf("Puerta OK\n");
		return 0x00;
	}
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	Puerta_Init_ACR()
{	long rv;

	printf("Init RDR Puerta\n");
	if( (puerta.modelo = MertPort.PUERTA_Marca) == RDR_ACR1252L)
	{
		printf("Id=%d\n",idPuerta );
		rv = puerta.reader_acr1222.ReaderContext(ghContext, grdracs, indPuerta);
		if(rv != SCARD_S_SUCCESS)
		{
			printf("Error en Context = %d\n",indPuerta);
		}
		rv = puerta.reader_acr1222.ActiveBuzzer(1);
		printf("rv => %08lx\n", rv );
		return rv;
	}
	printf("Error en modelo\n");
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int Puerta_Init_CRT(void)
{	static int tmCrt = milisegundos() - TiempoEntrePreguntas - 10;
	static char op = 0;


	if( (milisegundos() - tmCrt) >= 500)
	{	tmCrt = milisegundos();
		switch(op)
		{
			case 0:	// Open Comm
					printf("open Puerta\n");
					if(puerta.initComm(MertPort.PUERTA_Marca, MertPort.PUERTA_TipoPuerto, 
												MertPort.PUERTA_SerialName,MertPort.PUERTA_SerialBaud) != 0)
					{
						printf("Open Comm Error Puerta CRT\n");
						return 0x101;
					}
					op = 1;
					return 0x00;
					break;
			case 1:
					if( puerta.Buzzer(1) != 0)
					{
						printf("Error Init Puerta CRT603\n");
						puerta.Close();
						op = 0;
						return 0x1021;
					}
					printf("OK Init Puerta\n");
					return 0x00;	// OK
		}
	}
	return 0x01;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	CoinBox_Init()
{	int addr, k;
	char SerialNUM[10], name[50];

	static int tmBarrido = milisegundos() - TiempoEntreCivintec -10;

	coin_box.modelo = MertPort.COINBOX_Marca; 

	if( (milisegundos() - tmBarrido) >= TiempoEntreCivintec)
	{	tmBarrido = milisegundos();

		memset(name, 0, sizeof(name));
		memset(SerialNUM, 0, sizeof(SerialNUM));

		for(k=0; k< NO_MAX_USB; k++)
		{
			name[0] = '\0';
			coin_box.reader_cn670.cn670.nUSB = lUSB[k];
			if( coin_box.reader_cn670.cn670.GetUserInfo(0, name) == 0)
			{
				if(  strcmp(name, MertPort.COINBOX_SerialName)==0)
				{
					printf("***************\nReader Encontrado=[%s]\n", name);
					coin_box.reader_cn670.cn670.GetSerialNum (0, &addr, SerialNUM);
					printf("CurrentAddress=[%d] %d  SerialNUM=%s \n", k, addr, SerialNUM);
					usleep(100000);
					printf("Set LED\n");
					coin_box.reader_cn670.cn670.SetLED(0, LED_ALL_ON);
					usleep(100000);
					coin_box.reader_cn670.Buzzer(2);
					usleep(100000);
					coin_box.reader_cn670.cn670.SetAutoPowerSave(0, 0x00);	// siempre activo
					printf("***********************************************\n");
					break;
				}
			}
		}
		if(k == NO_MAX_USB)
		{	
			coin_box.reader_cn670.cn670.nUSB = NULL;
			return 0x0101;	// no se encontro
		}

		// Habilitar Modo de AutoRead
		if( coin_box.SetMode() != 0)
		{
			printf("Error AutoRead PUERTA\n");
			return 0x0101;
		}
		printf("OK AutoRead CAJA MONEDAS\n");
	
		if(MertPort.COINBOX_SAM_SLOT0 != 0)
		{	// HAY UN SAM EN SLOT 0, AUTENTIFICA
			coin_box.TipoSAM = MertPort.COINBOX_SAM_SLOT0;
//			printf("SAM=%d\n", MertPort.COINBOX_SAM_SLOT0);
			if( coin_box.AutentificaSAM(MertPort.COINBOX_SAM_SLOT0 ) != 0)
			{	printf("Error en Autentificacion Puerta[]\n");
				return 0x101;
			}
		}
		printf("CoinBox OK\n");
		return 0x00;
	}
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	CoinBox_Init_ACS()
{	long rv;
unsigned char uid[6];

	printf("Init CoinBox %d-%d\n", coin_box.modelo,MertPort.COINBOX_Marca);
	if( (coin_box.modelo = MertPort.COINBOX_Marca) == RDR_ACR1222L)
	{
		printf("Id=%d\n",indCoinBox );
		rv = coin_box.reader_acr1222.ReaderContext(ghContext, grdracs, indCoinBox);
		rv = coin_box.reader_acr1222.ActiveBuzzer(1);
		printf("rv = %08lx\n", rv );
		return rv;
	}
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	BillBox_Init_ACS()
{	long rv;

	printf("Init BillBox\n");
	if( (bill_box.modelo = MertPort.BILLBOX_Marca) == RDR_ACR1222L)
	{
		printf("Id=%d\n",idBillBox );
		rv = bill_box.reader_acr1222.ReaderContext(ghContext, grdracs, indBillBox);
		if(rv != SCARD_S_SUCCESS)
		{
			printf("Error en COntext = %d\n",indBillBox);
		}
		rv = bill_box.reader_acr1222.ActiveBuzzer(1);
		printf("rv = %08lx\n", rv );
		return rv;
	}
	
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	BillBox_Init()
{	int addr, k;
	char SerialNUM[10], name[50];
	static int tmBarrido = milisegundos() - TiempoEntreCivintec -10;

	bill_box.modelo = MertPort.BILLBOX_Marca; 

	if( (milisegundos() - tmBarrido) >= TiempoEntreCivintec)
	{	tmBarrido = milisegundos();

		memset(name, 0, sizeof(name));
		memset(SerialNUM, 0, sizeof(SerialNUM));

		for(k=0; k< NO_MAX_USB; k++)
		{
			name[0] = '\0';
			bill_box.reader_cn670.cn670.nUSB = lUSB[k];
			if( bill_box.reader_cn670.cn670.GetUserInfo(0, name) == 0)
			{
				if(  strcmp(name, MertPort.BILLBOX_SerialName)==0)
				{
					printf("***************\nReader Encontrado=[%s]\n", name);
					bill_box.reader_cn670.cn670.GetSerialNum (0, &addr, SerialNUM);
					printf("CurrentAddress=[%d] %d  SerialNUM=%s \n", k, addr, SerialNUM);
					usleep(100000);
					printf("Set LED\n");
					bill_box.reader_cn670.cn670.SetLED(0, LED_ALL_ON);
					usleep(100000);
					bill_box.reader_cn670.Buzzer(2);
					usleep(100000);
					bill_box.reader_cn670.cn670.SetAutoPowerSave(0, 0x00);	// siempre activo
					printf("************************************************\n");
					usleep(100000);
					break;
				}
			}
		}
		if(k == NO_MAX_USB)
		{	
			bill_box.reader_cn670.cn670.nUSB = NULL;
			return 0x0101;	// no se encontro
		}

		// Habilitar Modo de AutoRead
		if( bill_box.reader_cn670.ConfigAutoRead() != 0)
		{
			printf("Error AutoRead PUERTA\n");
			return 0x0101;
		}
		printf("OK AutoRead CAJA BILLETES\n");

		if(MertPort.BILLBOX_SAM_SLOT0 != 0)
		{	// POSIBLEMENTE HAY UN SAM EN SLOT 0, AUTENTIFICA
			bill_box.TipoSAM = MertPort.BILLBOX_SAM_SLOT0;
			if( bill_box.AutentificaSAM(MertPort.BILLBOX_SAM_SLOT0 ) != 0)
			{	printf("Error en Autentificacion Puerta[]\n");
				return 0x101;
			}
		}
		printf("BillBox OK\n");

		return 0x00;
	}
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int Monedero_Init(void)
{	static int tmMonedero = milisegundos() - TiempoEntrePreguntas - 10;
	static char op = 0;
	int re;
	
	if( (milisegundos() - tmMonedero) >= 500)		//TiempoEntrePreguntas)
	{	tmMonedero = milisegundos();
		switch(op)
		{
			case 0:	// Open Comm
					printf("open coin\n");

					if(fd_cctalk == -1)
					{
						if(coins.OpenComm(MertPort.COIN_COMM, 9600) != 0)
						{
							printf("Open Comm Error Monedero\n");
							return 0x101;
						}
						op = 1;
						fd_cctalk = coins.GetFD();
					}
					else
					{
						
						coins.SetFD(fd_cctalk);
					}
//					break;
			case 1:
					if( (re=coins.Inicializa(COIN_ADDRESS, MertPort.COIN_VERIFICACION, MertPort.COIN_ECHO )) != 0)
					{
						printf("Error Init Monedero=%d\n",re);
						coins.CloseComm();
						fd_cctalk = -1;
						op = 0;
						return 0x1021;
					}
					printf("OK Init Monedero\n");
					//					coins.chnmsb = 0xFF;	// Habilita todas las monedas
//					coins.chnlsb = 0XFF;	//
					return 0x00;	// OK
		}
	}
	return 0x01;
}

/******************************************************************************/
/* Forma la mascara de habilitacion de billetes                               */
/******************************************************************************/
unsigned int mskBill(char billcoin)
{	int k,i;
	unsigned int bmk = 0x0000;
	unsigned int mask= 0x0001;

	for(k=1; k<bills.TotalDenominaciones; k++)
	{
		if( bills.depositado.valor[k] != 0x00)
		{
			printf("k=%d v=%d\n", k, bills.depositado.valor[k]);
			// buscarlo en el xml
			
			for(i=0; i<BankNotes[billcoin].tog_reg; i++)
			{	if( bills.depositado.valor[k] == BankNotes[billcoin].denominacion[i])
				{	// denominacion encontrada
					if(BankNotes[0].enable[i])
						bmk = bmk | mask;
					break;	
				}
			}
		}
		mask = mask << 1;
	}
	printf("Mask final= %04x\n", bmk);
	return bmk;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	Billetero_Init(void)
{	static int tmBilletero = milisegundos() - TiempoEntreCctalk - 10;
	static char op = 0;
	unsigned int mask;
	int re,i;

	if( (milisegundos() - tmBilletero) >= 500)		//TiempoEntreCctalk)
	{	tmBilletero = milisegundos();
	

		switch(op)
		{
			case 0:	// Open Comm
				if(fd_cctalk==-1)
				{
					printf("open bill\n");
					if(bills.OpenComm(MertPort.BILL_COMM, 9600) != 0)
					{
						printf("Open Comm Error BILLETERO\n");
						return 0x101;
					}
					printf("Open Comm OK BILLETERO\n");
					fd_cctalk = bills.GetFD();
				}
				else
				{
					bills.SetFD(fd_cctalk);
				}
				op = 1;
			case 1:
					if( (re=bills.Inicializa(BILL_ADDRESS, MertPort.BILL_VERIFICACION, MertPort.BILL_ECHO )) != 0)
					{
						printf("Error Init Billetero=%d\n",re);
						bills.CloseComm();
						fd_cctalk = -1;
						op = 0;	
						return 0x1021;
					}
					
					if(!mert_famisa)
					{
						// recorre posiciones
						
						for(i=0; i<32; i++)
							bills.depositado.valor[31-i] = bills.depositado.valor[30-i];
						bills.depositado.valor[0] = 0;
						
						for(i=0; i<32; i++)
							
							printf("Pos=%2d= %d\n", i, bills.depositado.valor[i] );
					}

					printf("OK Init Billetero\n");
					bills.ReadAllCounters();
					printf("Read All Counters OK\n");

					mask = mskBill(0x00);

					//
//					bills.chnlsb = mask & 0xFF;	//
//					bills.chnmsb = (mask>>8) & 0xFF;	//
//					printf("MSB=%02x LSB=%02x\n\n", bills.chnmsb, bills.chnlsb);


/*					if( bills.SendWaitInhibitStatus(BILLETES_DISABLE) != 0)
						printf("E R R O R\n");
					else
						printf("G O O D\n");
*/
					
//					unsigned char dat[] = {0xff, 0xff, 0xff,0xff};
//					bills.SendModifyInhibitBillStatus(dat, 2 );
//					bills.SendWaitMasterInhibit(0x01);
//					bills.SendMasterInhibitStatus(BILLETES_ENABLE);
//					while(1)
//					{
//						usleep(300000);
//
//						bills.ReadEvents(0x00);
//						printf("1111\n");
//
//					}
//
					return 0x00;	// OK
		}
	}
	return 0x01;
}

/******************************************************************************/
/*           Comprueba que todos los perifericos esten en linea               */
/******************************************************************************/
int perifericosOK(void)
{	char msg[100];
	char k;

	static int tusb=0, iUSB=0;		// total de Reader CRT
	static int tacs=0, iACS=0;		// total de Reader 
	// USB -> READER
	static int re_venta=0x01;		// status Reader Venta
									// VENTA
	static int re_com_venta=0x01;	// RS232-DISPENSADOR
	static int re_usb_venta=0x01;	// USB-READER



	static int re_recarga=1;		// status Reader Recarga
	static int re_coinbox=1;		// Status Reader Caja de Monedas
	static int re_billbox=1;		// Status Reader Caja de Billetes
	static int re_puerta=1;			// Status Reader Puerta

	// COM's
	// monedero & billetero
	static int re_monedero=1;		// status Monedero
	static int re_billetero=1;		// status Billetero


	// DISPENSADOR
	if( MertLmt.type == MERT_EXPENDEDORA)
	{
		if(re_venta != 0)
		re_venta = Venta_Init(iUSB);
	}
		else
			re_venta = 0;


	if(mert_famisa)
	{
		if(iUSB==0)
		{
			if (libusb_init(&context) != 0)
			{
				printf("error: intializing libusb \n");
				iUSB = 0;
			}
			else
			{	
				printf("Init USB OK\n");
				iUSB = 1;	// contexto inicializado
			}
		}

		if(tRdrCV != 0)
		{
			if( tusb != tRdrCV)
			{
				tusb=SearchAllUSB();
			}
			else
			{
					// ******** Lectores Civintec: Caja Monedas, BIlletes y Puerta**************
					// Puerta
					if(re_puerta != 0)
						re_puerta = Puerta_Init();

					// Caja del Monedero
					if(re_coinbox != 0)
						re_coinbox = CoinBox_Init();

					// Caja del Billetero
					if(re_billbox!=0)
						re_billbox = BillBox_Init();
			}
		}
	}
	else
	{
		if(iUSB==0)
		{
			// buscar lectores de ACS (ACR1222L)
			SearchAllACSReaders();
			idACR1222();	// identifica los ACR1222
			idACR1252();	// identifica los ACR1252
			

			indBillBox		= indPtr1222[idBillBox];
			indCoinBox		= indPtr1222[idCoinBox];
			indDispensador	= indPtr1222[idDispensador];
			indPuerta		= indPtr1252[0];	//indPtr1252[idPuerta];



			printf("IndCoinBox     = %d\n",indCoinBox);
			printf("Ind BillBox    = %d\n", indBillBox);
			printf("IndDispensador = %d\n", indDispensador);
			printf("IndPuerta      = %d\n", indPuerta);

			iUSB=1;
		}


		if(idBillBox >= 0)
		{
			if(re_billbox != 0)
				re_billbox = BillBox_Init_ACS();

		}
		if(idCoinBox >= 0)
		{
			if(re_coinbox != 0)
				re_coinbox = CoinBox_Init_ACS();
		}

		if( idDispensador >= 0)
		{
			
		}

		// Puerta
		if(re_puerta != 0)
			re_puerta = Puerta_Init_ACR();		//Puerta_Init_CRT();
	}

	// Lector de Recarga
	if(re_recarga != 0)
		re_recarga = Recarga_Init();

	if(re_recarga==0)
		SAM_Init();

	// *********************** MONEDERO Y BILLETERO CCTALK *********************
	// Monedero
	if(mert_operacion.fgMONEDERO!=0)
	{	if(re_monedero!=0)
			if( (re_monedero = Monedero_Init()) == 0)
			{	// ya Inicializo el Monedero, revisar si la BD tiene los CVF grabados
//				revCVFMonedas();
				if( posiciona_destrabador()!=0)
					printf("Error al destrabar el monedero\n");
			}
	}
	else
		re_monedero = 0;

	// Billetero
	if(mert_operacion.fgBILLETERO!=0)
	{	if(re_billetero!=0)
			if( (re_billetero = Billetero_Init()) == 0 )
			{	// ya Inicializo el Monedero, revisar si la BD tiene los CVF grabados
//				revCVFBilletes();
			}
	}
	else
		re_billetero=0;

	strcpy(msg, "INICIALIZANDO\nPERIFERICOS\n");

	if(!MERT_RECARGA_SAM)
						strcat(msg,"[SAM]\n");
	if( re_venta )      strcat(msg,"[RV]");
	if( re_recarga )    strcat(msg,"[RR]");
	if( re_billetero )  strcat(msg,"[Bill]");
	if( re_monedero )   strcat(msg,"[Coin]");
						strcat(msg,"\n");
	if( re_billbox )    strcat(msg,"[RBB]");
	if( re_coinbox )    strcat(msg,"[RCB]");
	if( re_puerta != 0) strcat(msg,"[RP]");
	if( stModIO->status_comm != 0x00) strcat(msg,"[ModIO]");

	winMsg(msgMain, rgb_lime_green, rgb_black, msg, "SANS", 700, 55);
	
	if(	(MERT_RECARGA_SAM==1) && 
		(re_venta==0) && 
		(re_recarga==0) && 
		(re_billetero==0) && 
		(re_monedero==0) && 
		(re_billbox==0) && 
		(re_puerta==0) && 
		(re_coinbox==0) && 
		(stModIO->status_comm == 0x00) )
	{	// todos los dispositivos en linea
		printf("Todos los dispositivos en linea\n");
		fgCRT285 = 1;	// USB RECARGA OK, x mientras aqui

//		proceso_venta(2000, 0, 2000);
	
		
		return 0x00;
	}
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int initRecarga(void)
{	char k=0;
	int re_recarga = 1;

	MERT_RECARGA_SAM = 0;
	close_usb(lUSBCRT);
	lUSBCRT = NULL;
	while(k<20)
	{	printf("k=%d\n",k);
		// Lector de Recarga
		if(re_recarga != 0)
			re_recarga = Recarga_Init();
		if(re_recarga==0)
			SAM_Init();

		if( (re_recarga==0) && (MERT_RECARGA_SAM==1) )
		{
			fgCRT285=1;
			return 0;
		}
		usleep(200000);
		k++;
	}
	return 0x01;
}	

/******************************************************************************/
/*  Lee el ID de las cajas de monedas y de billetes                           */
/******************************************************************************/
int readIdCajas(void)
{	static int idMonedas  = 1;
	static int idBilletes = 1;
	char msg[100];

	if(idMonedas != 0)
	{
		if( idCajaMonedas != 0)
			idMonedas = 0;
	}

	if(idBilletes != 0)
	{
		if( idCajaBilletes != 0)
			idBilletes = 0;
	}

	strcpy(msg, "IDENTIFICANDO\nCAJAS\nDE\nVALORES\n");
	if( idMonedas )  strcat(msg,"[Monedas]");
	if( idBilletes ) strcat(msg,"[Billetes]");

	winMsg(msgMain, rgb_lime_green, rgb_black, msg, "SANS", 700, 60);

	if( (idMonedas==0) && (idBilletes==0))
		return 0x00;

	return 1;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
LONG PCSC_ERROR(LONG rv, char *text)
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
/*   // identifica los ACR1222                                                */
/******************************************************************************/
void idACR1222(void)
{	LONG rv;
	char id[100],k;

	printf("****************SEARCH ALL USB ACS\n");

	CLASS_ACR1222L acr1222;	// Reader ACS ACR1222L

	for(k=0; k<tRdrACR1222; k++)
	{
			printf("AXR1222L=%d\n",k);

			rv = acr1222.GetUserId(ghContext, grdracs, indPtr1222[k],AREA1,20, id);

			printf("K=%d IND= %d ID=[%s]\n", k, indPtr1222[k], id);
			
			if( strcmp(id,"BILLETERO")==0)
			{
				idBillBox=k;
			} else if( strncmp(id,"MONEDERO",8)==0)
			{
				idCoinBox=k;
			} else if( strcmp(id,"DISPENSADOR")==0)
				idDispensador=k;
	}

}

/******************************************************************************/
/*   // identifica los ACR1252                                                */
/******************************************************************************/
void idACR1252(void)
{	LONG rv;
	char id[100],k;

	printf("****************SEARCH ALL ACR1252 USB ACS =%d\n",indPtr1252[0]);

	CLASS_ACR1222L acr1222;	// Reader ACS ACR1252U
	

	if(tRdrACR1252==1)
	{
		idPuerta = 0;

		rv = acr1222.GetSerialNum(ghContext, grdracs, indPtr1252[0], id);
		printf("K=%d IND= %d ID=[%s]\n", k, indPtr1252[k], id);
		
//		if( strcmp(id,"RR554-17316")==0)
		
		if (rv == SCARD_S_SUCCESS)
			idPuerta = 0;
	}
}

/******************************************************************************/
/* Busca todos los Readers de ACS                                             */
/******************************************************************************/
LONG SearchAllACSReaders(void)
{
	LONG rv;
	DWORD dwReaders;
	char *ptr;
	int nbReaders;
	grdracs = NULL;
	tReaders=0;

int totRDR1222=0;
int totRDR1252=0;

	rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &ghContext);
	if (rv != SCARD_S_SUCCESS)
	{
		printf("SCardEstablishContext: Cannot Connect to Resource Manager %lX\n", rv);
		return EXIT_FAILURE;
	}

	/* Retrieve the available readers list. */
	dwReaders = SCARD_AUTOALLOCATE;
	rv = SCardListReaders(ghContext, NULL, (LPSTR)&mszReaders, &dwReaders);
	PCSC_ERROR(rv, (char *)"SCardListReaders");

	printf("dwReaders=%ld\n", dwReaders);

	nbReaders = 0;
	ptr = mszReaders;
	while (*ptr != '\0')
	{
		ptr += strlen(ptr)+1;
		nbReaders++;
	}

	printf("nbReaders=%d\n", nbReaders);

	if (nbReaders == 0)
	{
		printf("No reader found\n");
		return 0xFFFF;
	}

	/* allocate the readers table */
	grdracs = (char **)calloc(nbReaders, sizeof(char *));
	if (NULL == grdracs)
	{
		printf("Not enough memory for readers[]\n");
		return -1;
	}

	/* fill the readers table */
	nbReaders = 0;
	ptr = mszReaders;
	while (*ptr != '\0')
	{
		printf("--%2d: %s\n", nbReaders, ptr);

//		printf("[%s]\n", substr(ptr, 0, 11) );
		if ( strcmp( substr(ptr, 0, 11), "ACS ACR1222") ==0 )
		{
			printf("---ACS1222----\n" );
		printf("[%s]\n", substr(ptr, 30, 2) );
			if ( strcmp( substr(ptr, 30, 2), "00") ==0 )
				
			{
				indPtr1222[totRDR1222] = nbReaders; 
				totRDR1222++;
				printf("ptr =%d\n",nbReaders);
			}
		}
		else if ( strcmp( substr(ptr, 0, 11), "ACS ACR1252") ==0 )
		{
			printf("ACS1252\n" );
		printf("[%s]\n", substr(ptr, 46, 4) );
			if ( strcmp( substr(ptr, 46, 4), "PICC") ==0 )
			{
				indPtr1252[totRDR1252] = nbReaders; 
				totRDR1252++;
				printf("ptr =%d\n",nbReaders);
			}
		}
		else
		printf("ACR NO IDENTIFICADO\n" );

		grdracs[nbReaders] = ptr;
		ptr += strlen(ptr)+1;
		nbReaders++;
	}

	tReaders = nbReaders/4;
	printf("Total de Readers =%u 1222=%d 1252=%d\n", tReaders,totRDR1222, totRDR1252);

	printf("Total de Readers ACR1222L =%u 1222=%d\n", tReaders,totRDR1222);
	for(int k=0; k<6; k++)
		printf("indices=%d\n", indPtr1222[k]);


	printf("Total de Readers ACR1252L =%u 1222=%d\n", tReaders,totRDR1252);
	for(int k=0; k<6; k++)
		printf("indices=%d\n", indPtr1252[k]);

	return rv;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CLOSE_ACS(void)
{	LONG rv;

	/* free allocated memory */
	if (mszReaders)
		SCardFreeMemory(ghContext, mszReaders);

	/* We try to leave things as clean as possible */
	rv = SCardReleaseContext(ghContext);
	if (rv != SCARD_S_SUCCESS)
		printf("SCardReleaseContext: %s (0x%lX)\n", pcsc_stringify_error(rv), rv);

	if (grdracs)
		free(grdracs);
}
////////////////////////////////////////////////////////////////////////////////