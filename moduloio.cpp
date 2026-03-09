/*
 * File:   moduloio.cpp
 * Author: FAMISA
 * 
 * Created on October 10, 2022, 4:41 PM
 */
#include "moduloio.h"

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
moduloio::moduloio()
{	int k;

	OnLine = FUERA_DE_LINEA;
	fgLatch = 0;

	for(k=0; k<TOTAL_ENTRADAS; k++)
	{	Entradas[k] = 0x80;		// fuera de linea
		EntradasOld[k] = 0x00;
	}

	for(k=0; k<TotalBytesEntrada; k++)
	{	entradas_new[k] = 0x00;
		entradas_old[k] = 0x00;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void moduloio::desgloza_entradas(void)
{	unsigned char mask, i;

	mask = 0x01;
	for(i=0; i<8; i++)
	{
		if(entradas_new[0] & mask)
			Entradas[i] = 0x81;
		else
			Entradas[i] = 0x80;
		mask = mask << 1;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void moduloio::desgloza_salidas(void)
{	unsigned char mask, i;

	mask = 0x01;
	for(i=0; i<8; i++)
	{
		if(salidas_new[0] & mask)
			Salidas[i] = 0x81;
		else
			Salidas[i] = 0x80;
		mask = mask << 1;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int moduloio::ReadEntradas(void)
{
	if( readinput() != 0)		//
	{	// error al leer las entradas
		return 0x01;
	}

	if(  (entradas_new[0] != entradas_old[0]) )
	{	//printf("Entradas Nuevas\n");
		desgloza_entradas();
		entradas_old[0] = entradas_new[0];
	}

	if(  (salidas_new[0] != salidas_old[0]) )
	{	//printf("Entradas Nuevas\n");
		desgloza_salidas();
		salidas_old[0] = salidas_new[0];
	}
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int moduloio::relay(int no_relay, int valor)
{
//	printf("NoControl=%d\n", no_relay);
	
	if( wrcmd(dirmodulo, WRITE_SINGLE_COIL,  no_relay , valor) != 0)
	{
		printf("Error send relay= %d %d\n", dirmodulo, no_relay );
		return 0x01;
	}
	if( response(dirmodulo, WRITE_SINGLE_COIL, 8) != 0)
	{
		printf("Relay -> Error en respuesta=%d\n", dirmodulo);
		return 0x02;
	}
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int moduloio::relay_all(int dir_modulo, unsigned char valor)
{

	if( wrcmdcoils(dir_modulo, WRITE_MULTIPLE_COILS, 0x0000, 0x0008, valor) != 0)
	{
		printf("Error send multiple relay= %d %d\n", dir_modulo, valor );
		return 0x01;
	}
	if( response(dir_modulo, WRITE_MULTIPLE_COILS, 8) != 0)
	{
		printf("Relay -> Error en respuesta\n");
		return 0x01;
	}
	return 0x00;	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int moduloio::pulse(int no_relay, int valor, int tiempo)
{
	no_relay = RELAY_PULSO[no_relay];

	if(valor==RELAY_OPEN)
		valor = 0x0004;
	else
		valor = 0x0002;

	if( wrcmddat(dirmodulo, WRITE_MULTIPLE_HOLDING_REGISTER,  no_relay , valor, tiempo) != 0)
	{
		printf("Error send relay= %d %d\n", dirmodulo, no_relay );
		return 0x01;
	}
	if( response(dirmodulo, WRITE_MULTIPLE_HOLDING_REGISTER, 8) != 0)
	{
		printf("Relay -> Error en respuesta\n");
		return 0x01;
	}
	return 0x00;
}

/******************************************************************************/
/*  Lee los Modulos de E/S                                                    */
/******************************************************************************/
int moduloio::readinput(void)
{
	entradas_new[0] = 0x00;
	if( wrcmd(dirmodulo, READ_DISCRETE_INPUTS, 0x0000 , 0x0008) != 0)
	{
		printf("Error send Discrete Input\n");
		return 0x01;
	}
	if( response(dirmodulo, READ_DISCRETE_INPUTS, 6) != 0)
	{
		printf("Input -> Error en Mod. IO READ DISCRETE INPUT\n");
		return 0x01;
	}
	entradas_new[0] = readbuffer(3);

	if( wrcmd(dirmodulo, READ_COILS, 0x0000 , 0x0008) != 0)
	{
		printf("Error Send ReadCoils \n");
		return 0x01;
	}
	if( response(dirmodulo, READ_COILS, 6) != 0)
	{
		printf("Input -> Error en Mod. I/O READ COILS\n");
		return 0x01;
	}
	salidas_new[0] = readbuffer(3);
	return 0x00;
}

/******************************************************************************/
/*  Revisa si se oprimio un boton                                             */
/******************************************************************************/
unsigned char moduloio::BotonOprimido(unsigned char nboton)
{
	if( Entradas[nboton] != EntradasOld[nboton] )
	{	EntradasOld[nboton] = Entradas[nboton];
		if(Entradas[nboton] == BOTON_OPRIMIDO_SI)
			return BOTON_OPRIMIDO_SI;
	}
	return BOTON_OPRIMIDO_NO;
}
/******************************************************************************/