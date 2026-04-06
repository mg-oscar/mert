/*
 * File:   readers.cpp
 * Author: famisa
 *
 * Created on October 19, 2022, 10:24 AM
 */

#include "readers.h"
//#include "readers/class_cim6903.h"
#include "readers/class_chm29xx.h"
#include "libMIA.hpp"
#include "readers/class_civintec.h"
#include "readers/class_crt285.h"
#include "readers/class_crt603vx.hpp"
#include "readers/class_acr1222l.h"
#include "serialport.h"
#include "kytv26xx.h"

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
readers::readers()
{

}
//
//
// ************* FUNCIONES EXCLUSIVAS DEL DISPENSADOR  ************************* 
//
//
/******************************************************************************/
/* Toma una tarjeta del Stacker y al nuevo al lector                          */
/******************************************************************************/
int readers::GetCard(BYTE stk)
{
	switch(modelo)

		
	{
		// case RDR_CIM6903:		// Kytronics
		// 		if( reader_cim6903.MoveMiaToReader(stk) == 0)
		// 		{
		// 			return 0x00;
		// 		}
		// 		return 0x1000;	// error al mover la tarjeta
		// 		break;
		case RDR_CHM2901RF:		// Kytronics
				if( reader_chm2901.GetCard() == 0)
				{
					return 0x00;
				}
				return 0x1000;	// error al mover la tarjeta
				break;
			
		case DIS_KYT2664:		// Kytronics
				if(   reader_kyt2664.KytDispensa(stk) == 0)
				{
					return 0x00;
				}
				return 0x1000;	// error al mover la tarjeta

				break;


		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x00;	// OK
}








//
//
// *************** FUNCIONES EXCLUSIVAS DE LOS LECTORES ************************
//
//

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::LedOnOff(int NoLed, int Opcion)
{
	switch(modelo)
	{	case 0:		// NO HAY READER CONECTADO
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				reader_crt285.Led(NoLed, Opcion);		
				break;

		case RDR_CN670:
//				printf("LED CIVINTEC\n");				
				break;

		case RDR_CIM6903:		// Kytronics
				break;

		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			break;
		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::Buzzer(BYTE opcion)
{
	switch(modelo)
	{	case 0:					// NO HAY READER CONECTADO
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				break;

		case RDR_CN670:
				reader_cn670.Buzzer(opcion);
				break;

		case RDR_CIM6903:		// Kytronics
				break;

		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			break;
		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::initComm( char marca, char tport, char namecomm[], int baud )
{
	modelo = marca;
	switch(modelo)
	{	case 0:					// NO HAY READER CONECTADO
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				printf("CRT-285\n");
				if( reader_crt285.InitComm(tport, namecomm, baud) != 0)
				{	printf("error crt \n");
					return 0x101;
				}
				reader_crt285.Led(CRT_LED_ALL,CRT_LED_OFF);		
				break;

		case RDR_CN670:		// Civintec
				printf("CIVINTEC CN670=[%s]\n",namecomm);
				return 0x101;
				break;

		//case RDR_CIM6903:		// Kytronics
		//		printf("KYTRONICS CIM6903=[%s]\n",namecomm);
		//		if( reader_cim6903.InitComm(tport, namecomm, baud) != 0)
		//		{	printf("error crt \n");
		//			return 0x101;
		//		}
		//		break;
		case RDR_CHM2901RF: // Kytronics CHM2901
				printf("KYTRONICS CHM2901=[%s]\n",namecomm);
				if( reader_chm2901.InitComm(tport, namecomm, baud) != 0)
				{	printf("error crt \n");
					return 0x101;
				}

		break;
		case RDR_ACR1222L:
				printf("OPEN ACR1222L\n");
				break;

		case RDR_CRT603:
				printf("OPEN CRT603 = [%s] [%d]\n",namecomm, baud);
				if( reader_crt603.InitComm(namecomm, baud) != 0)
				{	printf("error crt603 \n");
					return 0x101;
				}			
			break;

		case DIS_KYT2664:
			printf("OPEN KYTRONICS KYT2664 = [%s] [%d]\n",namecomm, baud);
				if( reader_kyt2664.OpenComm(namecomm, baud)!=0)
				if( reader_crt603.InitComm(namecomm, baud) != 0)
				{	printf("error kyt \n");
					return 0x101;
				}			
			break;
		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x00;	// OK
}

/******************************************************************************/
/* para dispensadores con modulos separados                                   */
/* Equipos con RS232 y USB                                                    */
/******************************************************************************/
int readers::initPorts( char marca, char tport, char namecomm[], int baud )
{
	modelo = marca;
	switch(modelo)
	{	case 0:					// NO HAY READER CONECTADO
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				printf("CRT-285\n");
				if( reader_crt285.InitComm(tport, namecomm, baud) != 0)
				{	printf("error crt \n");
					return 0x101;
				}
				reader_crt285.Led(CRT_LED_ALL,CRT_LED_OFF);		
				break;

		case RDR_CN670:		// Civintec
				printf("CIVINTEC=[%s]\n",namecomm);
				return 0x101;
				break;

		// case RDR_CIM6903:		// Kytronics
		// 		printf("KYTRONICS=[%s]\n",namecomm);
		// 		if( reader_cim6903.InitComm(tport, namecomm, baud) != 0)
		// 		{	printf("error crt \n");
		// 			return 0x101;
		// 		}
		// 		break;
		case RDR_CHM2901RF:		// Kytronics
				printf("KYTRONICS=[%s]\n",namecomm);
				if( reader_chm2901.InitComm(tport, namecomm, baud) != 0)
				{	printf("error crt \n");
					return 0x101;
				}
				break;

		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
				printf("OPEN CRT = [%s] [%d]\n",namecomm, baud);
			
				if( reader_crt603.InitComm(namecomm, baud) != 0)
				{	printf("error crt \n");
					return 0x101;
				}			
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::initUSB(char marca,  libusb_device_handle *nusb )
{
	modelo = marca;
	switch(modelo)
	{	case 0:					// NO HAY READER CONECTADO
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				printf("CRT-285\n");
				if( reader_crt285.InitUSB(nusb) != 0)
				{	printf("error crt \n");
					return 0x101;
				}
				reader_crt285.Led(CRT_LED_ALL,CRT_LED_OFF);		
				break;

		case RDR_CN670:		// Civintec
				return 0x101;
				break;

		case RDR_CIM6903:		// Kytronics
				return 0x101;
				break;

		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			break;
		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x00;	// OK
}




/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::Identifica(void)
{
		switch(modelo)
	{	case 0:		// NO HAY READER CONECTADO
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				break;

		case RDR_CN670:
				break;

//		case RDR_CIM6903:		// Kytronics
//				return reader_cim6903.Identifica();
//				break;
		case RDR_CHM2901RF:		// Kytronics
				return reader_chm2901.Identifica();
				break;		

		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			break;
		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::AutentificaSAM(char slot )
{	int re=0;

	switch(modelo)
	{	case 0:						// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:			// China Creator
				if(slot == 0)
				{	// NO hay SAM para AUTENTIFICAR
					return 0x100;
				}
				if(slot==PSAM)
					re = reader_crt285.AutentificaSAM(1, PSAM);
				else if(slot==ISAM)
					re = reader_crt285.AutentificaSAM(1, ISAM);
				else if(slot==DEF_FF)
				{	
					// carga llaves default
					re = 0x00;
				}
				if(re!=0)
				{	printf("Error en Autentificacion[]\n");
					return 0x101;
				}
				break;

		case RDR_CN670:			// Civintec
				if(slot==DEF_FF)
				{	// Se usaran llaves x default 0xFFFFFFFFFFFF
					printf("SAM DEFAULT\n");
					return 0x00;
				}
				if( re=reader_cn670.AutentificaSAM(1, PSAM) != 0)
				{	printf("Error en Autentificacion[%d]\n", re);
					return 0x101;
				}
				break;

		case RDR_CIM6903:		// Kytronics
				break;

		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::StackerStatus(void)
{
//	printf("modelo%d\n", modelo);

	switch(modelo)
	{
		// case RDR_CIM6903:		// Kytronics
		// 		// verifica el estado de stacker
		// 	if( reader_cim6903.ReadStatusStacker()!=0)
		// 	{	
		// 		// error no vende
		// 		return 0x01;
		// 	}
		// 	if	(	(	(reader_cim6903.Stacker[0]==STACKER_EMPTY) &&  
		// 				(reader_cim6903.Stacker[1]==STACKER_EMPTY) )  ||
		// 			(	(reader_cim6903.Stacker[0]==STACKER_ERROR) ||
		// 				(reader_cim6903.Stacker[1]==STACKER_ERROR) )
		// 		)
		// 	{
				
		// 		return 0x01;	// No HAY TARJETAS DISPENSADOR
		// 	}
		// 	return 0x00;		// SI HAY TARJETAS EN EL DISPENSADOR
		// 	break;
		case RDR_CHM2901RF:		// Kytronics
				// verifica el estado de stacker
			if( reader_chm2901.ReadStatusStacker()!=0)
			{	
				// error no vende
				return 0x01;
			}
			if	(	(	(reader_chm2901.Stacker ==STACKER_EMPTY)  )  ||
					(	(reader_chm2901.Stacker ==STACKER_UNKNOW) )//to do handling of Stacker status 
				)
			{
				
				return 0x01;	// No HAY TARJETAS DISPENSADOR
			}
			return 0x00;		// SI HAY TARJETAS EN EL DISPENSADOR
			break;

		case DIS_KYT2664:		// Kytronics
			if( reader_kyt2664.KytStatus()==0)
			{
				if(reader_kyt2664.kytNoCard!=0)
					return 0x00;	// si hay tarjetas en el dispensador
			}
			return 0x01;	// No hay tarjetas en el dispensador
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::GetStacker(unsigned char nstk)
{
	switch(modelo)
	{

		// case RDR_CIM6903:		// Kytronics
		// 	return reader_cim6903.Stacker[nstk];
		// 	break;
		case RDR_CHM2901RF:		// Kytronics
			return reader_chm2901.Stacker;
			break;


		case DIS_KYT2664:		// Kytronics
			return reader_kyt2664.Stacker[nstk];
			break;
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::EjectCard(void)
{
	switch(modelo)
	{

		// case RDR_CIM6903:		// Kytronics
		// 	return reader_cim6903.EjectDrop();
		// 	break;
		case RDR_CHM2901RF:		// Kytronics
			return reader_chm2901.EjectCard();
			break;
		case DIS_KYT2664:		// Kytronics
			return reader_kyt2664.KytEject();
			break;
	}
	return 0x01;	// Error
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::CaptureCard(void)
{
	switch(modelo)
	{

		// case RDR_CIM6903:		// Kytronics
		// 	return reader_cim6903.Captura();
		// 	break;
		case RDR_CHM2901RF:		// Kytronics
			return reader_chm2901.CapturaCard(); //To do Check differences in Captura handling
			break;
		case DIS_KYT2664:		// Kytronics
			return reader_kyt2664.KytCaptura();
			break;
	}
	return 0x01;	// Error
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::CardPresent()
{
	switch(modelo)
	{

		case RDR_CRT285:		// China Creator
				return reader_crt285.CardPresent();
				break;

		case RDR_CN670:			// Civintec
				//return reader_cn670.CardPresent(MIA.MifareUID);
				break;

		// case RDR_CIM6903:
		// 		return reader_cim6903.CardPresent(MIA.MifareUID);
		// 		break;
		case RDR_CHM2901RF:
				return reader_chm2901.CardPresent(/*MIA.MifareUID*/);//To do handling of reading Mifare UID
				break;
				
		case DIS_KYT2664:
			if (modelo_exp==RDR_ACR1222L)
			{
				if( reader_acr1222.GetUID(MIA.MifareUID) != 0)
				{
					return 0x01;
				}
				return 0x00;
			}
			break;
		

		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			break;


		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x01;	// OK

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::SetMode(void)
{
	switch(modelo)
	{	case 0:		// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				break;

		case RDR_CN670:			// Civintec
				return reader_cn670.ConfigAutoRead1();

				break;

		case RDR_CIM6903:		// KYT
				break;

		case RDR_ACR1222L:		// ACS
				break;

		case RDR_ACR1252L:		// ACS
				break;

		case RDR_CRT603:		// China Creator
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x00;	// OK
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::Read_MFUID( unsigned char UID[] )
{
	switch(modelo)
	{	case 0:	// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:	// China Creator
				reader_crt285.LockedProcess(CRT_LOCKCARD_LOCKED);
				if(reader_crt285.Read_MFUID( UID) !=0 )
				{
					reader_crt285.Close();
					return 0;
				}
				break;

		case RDR_CN670:		// Civintec
				if( reader_cn670.ReadUID(UID) != 0)
				{
					return 0x01;
				}
				return 0x00;
				break;

		case RDR_CIM6903:
//			reader_acr1222.
				break;

		case RDR_ACR1222L:
				if( reader_acr1222.GetUID(UID) != 0)
				{
					return 0x01;
				}
				return 0x00;

		case RDR_ACR1252L:
				if( reader_acr1222.GetUID(UID) != 0)
				{
					return 0x01;
				}
				return 0x00;

		case RDR_CRT603:
			if( reader_crt603.MifareGetUID(UID)!=0)
			{
				return 0x01;
			}
			return 0x00;
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x01;	// OK

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::Read_Sector(int TypeKey, int isec, int fsec, BYTE pPASS[16][6], BYTE readblock[64][16])
{
	switch(modelo)
	{	case 0:	// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				return reader_crt285.Read_Sector(TypeKey, isec, fsec, pPASS, readblock);
				break;

		case RDR_CN670:			// Civintec
				break;
				
		case RDR_CIM6903:		// Kytronics
				break;

		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			return reader_crt603.MF_RdSector(isec, TypeKey, pPASS[isec], readblock);
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x01;	// OK

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::READ_ALL_WITH_KEY(int tKey, BYTE pPASS[16][6], BYTE ReadBlocks[64][16] )
{	long rv;

	switch(modelo)
	{
		// case RDR_CIM6903:
		// 	if( reader_cim6903.READ_ALL_WITH_KEY( tKey, pPASS, ReadBlocks) != 0)
		// 	{
		// 		printf("Error Read All\n");
		// 		return 0x0101;
		// 	}
		// 		return 0x00;	//OK
		// 	break;
		case RDR_CHM2901RF:
			if( reader_chm2901.READ_ALL_WITH_KEY( tKey, pPASS, ReadBlocks) != 0)
			{
				printf("Error Read All\n");
				return 0x0101;
			}
				return 0x00;	//OK
			break;
			
		case DIS_KYT2664:
			
			if (modelo_exp==RDR_ACR1222L)
			{

				rv = reader_acr1222.READ_ALL_WITH_KEY(ACS_RDR0, ISAM, pPASS, ReadBlocks);
				if( rv != 0 )
				{
					PCSC_ERROR(rv, (char *)"Error Read Sector 1");
					return rv;
				}
				return 0x0;	//ok
			}
			return 0x01;

	case RDR_ACR1252L:
			
			rv = reader_acr1222.READ_ALL_WITH_KEY(ACS_RDR0, ISAM, pPASS, ReadBlocks);
			if( rv != 0 )
			{
				PCSC_ERROR(rv, (char *)"Error Read Sector 1");
				return rv;
			}
			return 0x0;	//ok
			break;
	}

	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::Read_Sector1A(BYTE MFUID[], BYTE pPASS[16][6], BYTE readblock[64][16])
{	long rv;

	switch(modelo)
	{
		// case RDR_CIM6903:
		// 	if( reader_cim6903.Read_Sector1A(	MIA.MifareUID, 
		// 											MIA.KeyA, 
		// 											MIA.ReadBlocks )!= 0)
		// 	{
		// 		printf("Error Sector 1A\n");
		// 		return 0x0101;
		// 	}
		// 		return 0x00;	//OK
		// 	break;
		case RDR_CHM2901RF:
			if( reader_chm2901.Read_Sector1A(	MIA.MifareUID, 
													MIA.KeyA, 
													MIA.ReadBlocks )!= 0)
			{
				printf("Error Sector 1A\n");
				return 0x0101;
			}
				return 0x00;	//OK
			break;

		case DIS_KYT2664:

			if (modelo_exp==RDR_ACR1222L)
			{
				// Read Sector 1A
				rv = reader_acr1222.MF_RdSector (ACS_RDR0, 1, KEYA, reader_acr1222.MFUID, readblock);
				if( rv != 0 )
				{
					PCSC_ERROR(rv, (char *)"Error Read Sector 1");
					return rv;
				}
				return 0x0;	//ok
			}
				return 0x01;
			break;

		case RDR_ACR1252L:
				rv = reader_acr1222.MF_RdSector (ACS_RDR0, 1, KEYA, MFUID, readblock);
				if( rv != 0 )
				{
					PCSC_ERROR(rv, (char *)"Error Read Sector 1");
					return rv;
				}
				return 0x0;	//ok
			break;

	}
	return 0x01;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::GetCardKey( BYTE TypeKey, BYTE mfuid[], BYTE MFKEY[16][6], BYTE readblock[64][16] )
{
	switch(modelo)
	{	case 0:	// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				return reader_crt285.GetCardKey(mfuid, MFKEY, readblock );
				break;

		case RDR_CN670:			// Civintec
				return reader_crt285.GetCardKey(mfuid, MFKEY, readblock );
				break;

		case RDR_CIM6903:		// Kytronics
				return reader_crt285.GetCardKey(mfuid, MFKEY, readblock );
				break;

		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
				return reader_crt285.GetCardKey(mfuid, MFKEY, readblock );
				break;
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x01;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::ReadValue(int TypeKey, int PurseBlock, BYTE pPASS[16][6] )
{
	switch(modelo)
	{	case 0:	// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				return reader_crt285.ReadValue( TypeKey, PurseBlock, pPASS );
				break;

		case RDR_CN670:			// Civintec
				break;

		case RDR_CIM6903:		// Kytronics
				break;
				
		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x01;	// OK

}

/******************************************************************************/
/* Input   1=No Latch   2= Locked                                             */
/* Return: 1=No Latch   2=locked                                              */
/* 9=device is offline, the card latch status unknown                         */
/******************************************************************************/
int readers::UNLOCK(char lock)
{	int k, re;
	int latch;

	latch = (lock)?CRT_LOCKCARD_LOCKED:CRT_LOCKCARD_NOTLOCK;

	for(k=0;k<5; k++)
	{
		usleep(100000);
		reader_crt285.LockedProcess(latch);

		usleep(100000);
		re = reader_crt285.LockedStatus();
//		printf("LockedStatus = %d\n", re);
		if(re == latch)
		{
//			printf("Latch OK\n");
			return 0x00;
		}
	}
	printf("Error al LOCK/UNLOCK CRT\n");
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::Close(void)
{
	switch(modelo)
	{	case 0:	// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				return reader_crt285.Close();
				break;

		case RDR_CN670:			// Civintec
				break;

		case RDR_CIM6903:		// Kytronics
				break;
				
		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			return 0x00;		//eader_crt603.
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x01;	// OK

}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::ReadAllData(int TypeKey)
{	int i=1;
//	unsigned int tread;

	switch(modelo)
	{	case 0:	// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				// lee MIA
				if(TypeKey == KEYA)
					i=reader_crt285.READ_ALL(KEYA, MIA.MifareUID, MIA.KeyA, MIA.ReadBlocks );
				else
					i=reader_crt285.READ_ALL(KEYB, MIA.MifareUID, MIA.KeyB, MIA.ReadBlocks );
				if(i != 0)
				{
					printf("Error ALL=[%04X]\n",i);
					return i;
				}
				if( (i=MIA.TransferVerifica(mert_identidad)) != 0 )
				{
					printf("Error al Transferir y Veriricar=[%04X]\n",i);
					return i;
				}

				return 0x00;
				break;

		case RDR_CN670:			// Civintec
				printf("Civintec KEY=%d\n", TypeKey);

//				printf("Sector 1A OK\n");
				if( (i=reader_cn670.Read_Sector1A(MIA.MifareUID, MIA.KeyA, MIA.ReadBlocks ))!= 0)
				{
					printf("Error Sector 1A=[%04X]\n",i);
					return i;
				}

//				printf("GetCardKey OK\n");
				if( ( i = GetCardKey(KEYB, MIA.MifareUID, MIA.KeyA, MIA.ReadBlocks)) !=0 )
				{
					printf("Error GetCardKey=[%04X]\n",i);
					return i;
				}

				if( (i=reader_cn670.READ_ALL_WITH_KEY(KEYB, MIA.MifareUID, MIA.KeyA, MIA.ReadBlocks ))!=0)
				{
					printf("Error ALL=[%04X]\n",i);
					return i;
				}
				return 0x00;
				break;
#if 0
		case RDR_CIM6903:		// Kytronics
//				tread = milisegundos();
				if( (i=reader_cim6903.Read_Sector1A(MIA.MifareUID, MIA.KeyA, MIA.ReadBlocks ))!= 0)
				{
					printf("Error Sector 1A=[%04X]\n",i);
					return i;
				}

				if(TypeKey == PSAM)
				{
					if( ( i = GetCardKey(KEYA, MIA.MifareUID, MIA.KeyA, MIA.ReadBlocks)) !=0 )
					{
						printf("Error GetCardKey=[%04X]\n",i);
						return i;
					}

					if( (i=reader_cim6903.READ_ALL_WITH_KEY(KEYA, MIA.KeyA, MIA.ReadBlocks ))!=0)
					{
						printf("Error ALL=[%04X]\n",i);
						return i;
					}
				}
				else
				{
					if( ( i = GetCardKey(KEYB, MIA.MifareUID, MIA.KeyB, MIA.ReadBlocks)) !=0 )
					{
						printf("Error GetCardKey=[%04X]\n",i);
						return i;
					}
					if( (i=reader_cim6903.READ_ALL_WITH_KEY(KEYB, MIA.KeyB, MIA.ReadBlocks ))!=0)
					{
						printf("Error ALL=[%04X]\n",i);
						return i;
					}
				}				
				if(i != 0)				{
					printf("Error ALL=[%04X]\n",i);
					return i;
				}
				return 0;

				break;
#endif	
		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			// Lee el Sector 1
				if( reader_crt603.MF_RdSector(1, KEYA, MIA.KeyA[1], MIA.ReadBlocks ) )
				{	printf("Error Sector 1A=[%04X]\n",i);
					return i;
				}
//				printf("Sector 1A OK = %d\n", TypeKey);
//				MIA.PrintReadData();
				
//				printf("GetCardKey OK\n");
				if( ( i = GetCardKey(KEYB, MIA.MifareUID, MIA.KeyA, MIA.ReadBlocks)) !=0 )
				{
					printf("Error GetCardKey=[%04X]\n",i);
					return i;
				}

//				printf("ReadAll Data\n");
				if( (i=reader_crt603.READ_ALL_WITH_KEY(KEYB, MIA.MifareUID, MIA.KeyA, MIA.ReadBlocks ))!=0)
				{
					printf("Error ALL=[%04X]\n",i);
					return i;
				}
				return 0x00;
			break;

	default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x01;	// 
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::WriteBlock(int block)
{
	switch(modelo)
	{	case 0:	// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				return  reader_crt285.WriteBlock(KEYB, block, MIA.KeyB, MIA.WriteBlocks);
				break;

		case RDR_CN670:			// Civintec
				break;

		// case RDR_CIM6903:		// Kytronics
		// 	return reader_cim6903.WriteBlock(KEYB, block, MIA.KeyB, MIA.WriteBlocks);
		// 	return 0x00;
		// 		break;
		case RDR_CHM2901RF:		// Kytronics
			return reader_chm2901.WriteBlock(KEYB, block, MIA.KeyB, MIA.WriteBlocks);
			return 0x00;
				break;
		case DIS_KYT2664:
			if (modelo_exp==RDR_ACR1222L)
			{
				return reader_acr1222.WriteBlock(KEYB, block, MIA.KeyB, MIA.WriteBlocks);
			}
			break;
				
		case RDR_ACR1222L:
			
			return reader_acr1222.WriteBlock(ISAM, block, MIA.KeyB, MIA.WriteBlocks);
			break;

		case RDR_CRT603:
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x01;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::WriteAllBlock(void)
{	int blk;

	switch(modelo)
	{	case 0:	// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				for(blk=1; blk<64; blk++)
					if(reader_crt285.WriteBlock(KEYB, blk, MIA.KeyB, MIA.WriteBlocks) != 0)
						return 0x400;
				return 0x00;
				break;

		case RDR_CN670:			// Civintec
				break;

		case RDR_CIM6903:		// Kytronics
				break;
				
		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x01;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::WriteBlocks(int TypeKey, char iblock, char fblock)
{	int blk;

	switch(modelo)
	{	case 0:	// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				for(blk=iblock; blk<fblock; blk++)
					if(reader_crt285.WriteBlock(KEYB, blk, MIA.KeyB, MIA.WriteBlocks) != 0)
						return 0x400;
				return 0x00;
				break;

		case RDR_CN670:			// Civintec
				for(blk=iblock; blk<fblock; blk++)
				{	switch(TypeKey)
					{	case PSAM:
							//int CLASS_CIVINTEC::WriteBlock(int TypeKey, int isec, int fsec, BYTE MFUID[], BYTE pPASS[16][6], BYTE writeblock[64][16])
							if(reader_cn670.WriteBlock(KEYA, blk, MIA.MifareUID, MIA.KeyA, MIA.WriteBlocks) != 0)
								return 0x400;
							break;
						case ISAM:
							if(reader_cn670.WriteBlock(KEYB, blk, MIA.MifareUID, MIA.KeyB, MIA.WriteBlocks) != 0)
								return 0x400;
						case DEF_FF:
							if(reader_cn670.WriteBlock(KEYA, blk, MIA.MifareUID, MIA.KeyA, MIA.WriteBlocks) != 0)
								return 0x400;
					}
				}
				return 0x00;
				break;
		case RDR_CIM6903:		// Kytronics
				break;

		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x00;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::IncrementaMonedero(int TypeKey, int block, int incremento)
{	int i;

	switch(modelo)
	{	case 0:	// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				//IncrementaBalance(int TypeKey, int PurseSector, int Purse1o2, BYTE pPASS[16][6], int valor );
				if(TypeKey == PSAM)
					i = reader_crt285.IncrementaBalance(KEYA, Purse_1,  MIA.KeyA, incremento );
				else
					i = reader_crt285.IncrementaBalance(KEYB, Purse_1,  MIA.KeyB, incremento );
				if(i != 0)
				{
					printf("Error al incrementar monedero\n");
					return 0x400;
				}	
				break;

		case RDR_CN670:			// Civintec
				break;

		case RDR_CIM6903:		// Kytronics
				break;
				
		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x01;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::DecrementaMonedero(int sector, int block, int increment)
{
	switch(modelo)
	{	case 0:	// Sin Reader
				return 0x100;
				break;

		case RDR_CRT285:		// China Creator
				break;

		case RDR_CN670:			// Civintec
				break;

		case RDR_CIM6903:		// Kytronics
				break;

		case RDR_ACR1222L:
				break;

		case RDR_CRT603:
			break;

		default:
				return 0x101;	// lector no implementado
				break;
	}
	return 0x01;	// OK
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::ReadMonedero(int block,int *vPurse1, int *vPurse2)
{   int sector;
    unsigned char *p;
    unsigned char StartBlockID;
    unsigned char Buffer[16];
    int RTN,i;

    sector = block / 4;
    block  = 0;

    if(RTN==0)
    { 
		for(int I=0; I < 4; I++)
        {   StartBlockID = sector * 4 + block;// Formula que arroja que Bloque se leera
            memset(Buffer,0x00,16); //Pone en Cero el bufer
            if (RTN==0)
            {
                if(block==1)
                {   p = (unsigned char *)vPurse1;
                    *p++ = Buffer[0]; *p++ = Buffer[1]; *p++ = Buffer[2]; *p++ = Buffer[3];
                }
                if(block==2)
                {   p = (unsigned char *)vPurse2;
                    *p++ = Buffer[0]; *p++ = Buffer[1]; *p++ = Buffer[2]; *p++ = Buffer[3];
                }
            }
            else
            {   //printf("%d ",StartBlockID);
                printf("Read data is fails !\n");
            }
            block +=1;//Incrementa su valor de 0 a 3
        }
		return 0;   // ok
    }    
    return 1;   // error
}   

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int readers::WriteAllCard(int ctrl)
{	unsigned char rBlocks[16];
	unsigned char wBlocks[16];
	int flag_write[64];
	int i, k = 0;

	memset(flag_write,0x00,64);

	for(i = 4; i < 42; i++)
	{	memset(rBlocks,0x00,16);
		memset(wBlocks,0x00,16);

		memcpy(rBlocks, MIA.ReadBlocks[i], 16);
		memcpy(wBlocks, MIA.WriteBlocks[i],16);

		//Compara los bloques y guarda los bloques que han cambiado
		if((memcmp(rBlocks,wBlocks,16) != 0) )
		{
			flag_write[k++] = i;
		}
	}

	for(i = 0;  i < MIA.OperationCardIn.NumberBlocks2Write; i++)
	{
		memset(wBlocks,0x00,16);	// pone en 0x00 todo el vector
		memcpy(wBlocks, MIA.WriteBlocks[MIA.OperationCardIn.Blocks[i]],16);		// copia la informacion al vector
		MIA.sector = MIA.OperationCardIn.Blocks[i] / 4;
		MIA.block  = MIA.OperationCardIn.Blocks[i] % 4;

		//Validacion para no escribir en los bloques de los monederos Mifare
		if(ctrl == 1)
		{
			if((MIA.OperationCardIn.Blocks[i] != Purse_1) && (MIA.OperationCardIn.Blocks[i] != Purse_2))
			{

			}
			else
			{
				if(MIA.OperationCardIn.Blocks[i] == Purse_1)
				{
					//Copiar contenido de monedero 2 al monedero 1 e incrementar el monedero 1
				}
				if(MIA.OperationCardIn.Blocks[i] == Purse_2)
				{
					//Copiar contenido de monedero 1 al monedero 2 e incrementar el monedero 2
				}
			}
		}
		else
		{
			if(MIA.OperationCardIn.Blocks[i]!=0)
			{
				if( WriteBlock(MIA.OperationCardIn.Blocks[i]) != 0)
				{	printf("error en write block=%d\n", MIA.OperationCardIn.Blocks[i]);
					MIA.OperationCardIn.Msg = ErrorEscritura;
					return 0x400;
				}
			}
		}
	}

	if(MIA.OperationCardIn.Msg == ErrorEscritura)
	{
//		MIA.ValuesTrans();
		//MIA.Buzzer(argc,argv,100,1174); //Re7
		return 0x01;;
	}
	else
	{
		return 0x00;
	}
	return 0x00;
}

/******************************************************************************/
/* Write 0's desde el block 5 hasta el fin                                    */
/******************************************************************************/
int readers::WriteInitCard(void)
{	int i;

	memset(MIA.WriteBlocks, 0x00, sizeof(MIA.WriteBlocks));

	for(i = 1;  i < 63; i++)
	{
		if(i!=4)
		{
			if( (i%4) !=3 )
			{
//				printf("process write block=%d\n", i);

				if( WriteBlock(i) != 0)
				{	printf("error en write block=%d\n", i);
					MIA.OperationCardIn.Msg = ErrorEscritura;
					return 0x400;
				}
//				else
//					printf("OK write block=%d\n", i);

			}
		}
	}

	if(MIA.OperationCardIn.Msg == ErrorEscritura)
	{
		return 0x01;
	}
	else
	{
		return 0x00;
	}
	return 0x00;
}

/******************************************************************************/
/* Prepara datos de movimientos para mostarlos                                */
/******************************************************************************/
int readers::HistMovements(void)
{



	return 0x00;
}
////////////////////////////////////////////////////////////////////////////////