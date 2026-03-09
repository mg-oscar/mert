/* 
 * File:   modbusclass.cpp
 * Author: FAMISA
 * 
 * Created on October 10, 2022, 3:30 PM
 */

#include "modbusclass.h"


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
modbusclass::modbusclass()
{
}

/******************************************************************************/
/*   Instruccion para escribir en un byte                                     */
/******************************************************************************/
int	modbusclass::wrcmd(unsigned char dirmod, unsigned char  cmd, int dir_registro, int valor)
{	unsigned char  i, cmddat[10];
	int crc;

	clear_rx();

	cmddat[0] = dirmod;		//dir_modulo;
	cmddat[1] = cmd;

	cmddat[2] = (dir_registro >>8) & 0xFF;
	cmddat[3] =  dir_registro & 0xFF;

	cmddat[4] = (valor >>8) & 0xFF;
	cmddat[5] =  valor & 0xFF;

	crc = 0xFFFF;
	for(i=0; i<6; i++)
		crc = crcModBus( cmddat[i], crc);

	cmddat[6] = crc & 0xFF;
	cmddat[7] = (crc >>8) & 0xFF;

	if( WriteComm(cmddat, 8) != 0)
	{
		printf("Error WriteComm\n");
		return 0x01;
	}
	return 0x00;
}

/******************************************************************************/
/*   Instruccion para escribir en un byte                                     */
/******************************************************************************/
int	modbusclass::wrcmddat(unsigned char  dirmod, unsigned char  cmd, int dir_registro, int valor, int tiempo)
{	unsigned char  i, cmddat[20];
	int crc;

	clear_rx();

	cmddat[0] = dirmod;		//dir_modulo;
	cmddat[1] = cmd;

	cmddat[2] = (dir_registro >>8) & 0xFF;
	cmddat[3] =  dir_registro & 0xFF;

	cmddat[4] =  0x00;
	cmddat[5] =  0x02;
	cmddat[6] =  0x04;

	cmddat[7] = (valor >>8) & 0xFF;
	cmddat[8] =  valor & 0xFF;

	cmddat[9] = (tiempo >>8) & 0xFF;
	cmddat[10] =  tiempo & 0xFF;

	crc = 0xFFFF;
	for(i=0; i<11; i++)
		crc = crcModBus( cmddat[i], crc);

	cmddat[11] = crc & 0xFF;
	cmddat[12] = (crc >>8) & 0xFF;

	if( WriteComm(cmddat, 13) != 0)
	{
		printf("Error WriteComm\n");
		return 0x01;
	}
	return 0x00;
}

/******************************************************************************/
/*   Instruccion para escribir en multiple coils                              */
/******************************************************************************/
int	modbusclass::wrcmdcoils(unsigned char  dirmod, unsigned char  cmd, int reg_ini, 
													int reg_fin, unsigned char  valor)
{	unsigned char i, cmddat[20];
	int crc;

	clear_rx();

	cmddat[0] = dirmod;		//dir_modulo;
	cmddat[1] = cmd;

	cmddat[2] = (reg_ini >>8) & 0xFF;
	cmddat[3] =  reg_ini & 0xFF;

	cmddat[4] = (reg_fin >>8) & 0xFF;
	cmddat[5] =  reg_fin & 0xFF;

	cmddat[6] =  0x01;

	cmddat[7] = valor;

	crc = 0xFFFF;
	for(i=0; i<8; i++)
		crc = crcModBus( cmddat[i], crc);

	cmddat[8] = crc & 0xFF;
	cmddat[9] = (crc >>8) & 0xFF;

	if( WriteComm(cmddat, 10) != 0)
	{
		printf("Error WriteComm\n");
		return 0x01;
	}
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int modbusclass::wrAddress(void)
{	unsigned char cmddat[20];

	clear_rx();

	//	DIR=0xFF    00 10 00 00 00 01 02 00 FF EB 80
	cmddat[ 0] = 0x00;		//dir_modulo;
	cmddat[ 1] = 0x10;
	cmddat[ 2] = 0x00;
	cmddat[ 3] = 0x00;
	cmddat[ 4] = 0x00;
	cmddat[ 5] = 0x01;
	cmddat[ 6] = 0x02;
	cmddat[ 7] = 0x00;
	cmddat[ 8] = 0xFF;	// DIR
	cmddat[ 9] = 0xEB;
	cmddat[10] = 0x80;
	// DIR=0x01    00 10 00 00 00 01 02 00 01 6A 00
/*	cmddat[ 0] = 0x00;		//dir_modulo;
	cmddat[ 1] = 0x10;
	cmddat[ 2] = 0x00;
	cmddat[ 3] = 0x00;
	cmddat[ 4] = 0x00;
	cmddat[ 5] = 0x01;
	cmddat[ 6] = 0x02;
	cmddat[ 7] = 0x00;
	cmddat[ 8] = 0x01;	; DIR
	cmddat[ 9] = 0x6A;
	cmddat[10] = 0x00;
*/
	if( WriteComm(cmddat, 11) != 0)
	{
		printf("Error WriteComm\n");
		return 0x01;
	}
	return 0x00;
}



/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int modbusclass::analizaRx(unsigned char dirmod, unsigned char nocmd, int totdat )
{	int crc,i;
	unsigned char op;

	if(	modbus_apuntador < totdat )
		return 0x01;

	op = 0;
	crc = 0xFFFF;
	for(i=0; i< totdat; )		//modbus_apuntador;)
	{
		switch(op)
		{	case 0:	// address
					if(modbus_buffer[0] == dirmod)
					{	op++;
						crc = crcModBus( modbus_buffer[0], crc);
					}
					else
						i = 0x400;	// no es la direccion, un byte
					break;
			case 1:	// cmd
					if(modbus_buffer[1] == nocmd)
					{	op++;
						crc = crcModBus( modbus_buffer[1], crc);
					}
					else
						i = 0x400;	// no es la direccion, un byte
					break;
			case 2:	// resto de los datos
					crc = crcModBus( modbus_buffer[i], crc);
					break;
		}
		i++;
	}

	if( i == totdat)
	{	// total de datos recibidos
		if(crc==0x0000)
		{	// OK
			return 0x00;
		}
	}

	// elimina el primer dato
	memcpy(modbus_buffer, &modbus_buffer[1], --modbus_apuntador );
	printf("ERRO MODBUS\n");
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int  modbusclass::readbuffer(int indice)
{
	return modbus_buffer[indice];
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int modbusclass::response(unsigned char dirmod, unsigned char cmd, int totdat)
{	int mTimeOut;

	modbus_apuntador=0;
	mTimeOut = milisegundos();
	while( (milisegundos()- mTimeOut) <  TimeOutModBus)
	{
		ReadComm ();
		if(apIniBufRx != apFinBufRx)
		{
			modbus_buffer[modbus_apuntador++]= getBufferRx();
			if( analizaRx(dirmod, cmd, totdat ) == 0x00)
				return 0x00;	// OK
		}
	}
//	printf("TIMEOUT RX\n");
	return 0x02;	// TimeOut
}

/******************************************************************************/
/*   Instruccion para leer "n" entradas                                       */
/******************************************************************************/
int	modbusclass::rdcmd(unsigned char  addr, unsigned char  cmd, int reg_inicial, int reg_total)
{
	return 0;
}

/******************************************************************************/
/* c_crc_MODBUS: Calcula el crc                                               */
/* Polinomio:  x16+x15+x2+1                                                   */
/******************************************************************************/
int modbusclass::crcModBus(unsigned char dato, int crc)
{
 	return (crc>>8) ^ t_crc_MODBUS[(crc ^ dato) & 0x00ff];
}

/**************************************************************/
