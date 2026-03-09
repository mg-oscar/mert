/******************************************************************************/
/*  Rutinas para imprimir los diferentes tickets                              */
/******************************************************************************/
//
#include "serialport.h"
#include "printer.h"
#include "xmls.h"
#include "bd.h"
#include "windows/retirocajas.h"
#include "mert.h"
//
//
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int printer_status(void)
{	unsigned char da;	

	printer.WriteComm( (unsigned char *)PRINTER_STATUS, sizeof(PRINTER_STATUS));
	usleep(1000000);
	printer.ReadComm();

	printf("Pointers %02x=%02x\n,", printer.apIniBufRx, printer.apFinBufRx);

	while( printer.apIniBufRx != printer.apFinBufRx)
	{
		da = printer.getBufferRx();
		printf("%02x,",da);
		if( (da & 0x60) == 0x60)
			return 0x01;	// sin papel
	}
	return 0x00;	// todo bien
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void print_enter(int nenter)
{	char enter[40];
	memset(enter,'\n',sizeof(enter));

	printer.WriteComm( (unsigned char *)enter, nenter);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void printer_cut(void)
{


	if(mert_famisa==0)
	{
		// DTE, agrega varios Enter antes del corte
		print_enter(12);
	}


	printer.WriteComm( (unsigned char *)"\x0C",1);
	print_enter(4);
	printer.WriteComm( (unsigned char *)PRINTER_CUT, sizeof(PRINTER_CUT) );
	print_enter(2);


	
	
	usleep(50000);
	printer.CloseComm();
}
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int printEncabezado(void)
{	char line[60];

	printer.WriteComm( (unsigned char *)PRINTER_INITIALIZE,sizeof(PRINTER_INITIALIZE));
	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_CENTER) );
		
	print_enter(1);
	printer.WriteComm( (unsigned char *)ENCABEZADO1, sizeof(ENCABEZADO1) );
	print_enter(1);
	printer.WriteComm( (unsigned char *)ENCABEZADO2, sizeof(ENCABEZADO2) );
	print_enter(1);
	printer.WriteComm( (unsigned char *)ENCABEZADO3, sizeof(ENCABEZADO3) );
	print_enter(2);

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_LEFT, sizeof(PRINTER_ALIGN_LEFT) );
	// Imprimir Datos de Estacion
	snprintf(line,70,"      Linea:     %s\n", Lineas.name[mert_identidad.LINEA-1]);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	strcpy(line, Lineas.name[mert_identidad.LINEA-1]);
	snprintf(line,70,"      Estacion:  %s\n", MertID.name);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,70,"      Equipo:    %s\n", MertID.code);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	print_enter(2);
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int ImprimeStacker(int nstk, DET_STACKERS detalle)
{	char line[60], fechahora[40];

	if( printer.OpenComm((char *)"/dev/ttyS5", 9600)!= 0)
	{
		printf("Error al abrir el puerto serie /dev/ttyS5\n");
		return 0x01;
	}

	printEncabezado();

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_CENTER) );
//	print_enter(1);
	printer.WriteComm( (unsigned char *)"DISPENSADOR", 11 );
	print_enter(2);

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_LEFT, sizeof(PRINTER_ALIGN_LEFT) );
//	print_enter(1);

	snprintf(line,60,"           Stacker:  %d\n", nstk+1);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,60,"           No. Mov:  %d\n", detalle.NoMov);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,60,"             Fecha:  %s\n", detalle.Fecha);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,60,"    Tipo Operacion:  %d\n", detalle.TipoOperacion);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,60,"Tarjetas Operacion:  %d\n", detalle.TotalOperacion);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,60," Total de Tarjetas:  %d\n", detalle.TotalTarjetas);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,60,"  Tarjetas en Hold:  %d\n", Config.Captura);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,60,"      No. Empleado:  %d\n", detalle.NoEmpleado);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	print_enter(2);
	datetime(fechahora);
	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_RIGHT) );
	printer.WriteComm( (unsigned char *)fechahora, strlen(fechahora) );
	print_enter(1);

	printer_cut();
	
//	printer.WriteComm( (unsigned char *)"\x0C",1);
//	print_enter(4);
//	printer.WriteComm( (unsigned char *)PRINTER_CUT, sizeof(PRINTER_CUT) );
//	printer.CloseComm();
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int ImprimeCaptura(void)
{	char line[70], fechahora[40];

	if( printer.OpenComm((char *)"/dev/ttyS5", 9600)!= 0)
	{
		printf("Error al abrir el puerto serie /dev/ttyS5\n");
		return 0x01;
	}

	datetime(fechahora);


	printEncabezado();

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_CENTER) );
//	print_enter(1);
	printer.WriteComm( (unsigned char *)"BANDEJA DE CAPTURA", 18 );
	print_enter(2);

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_LEFT, sizeof(PRINTER_ALIGN_LEFT) );
//	print_enter(1);

	snprintf(line,70,"           No. Mov:  %d\n", Config.NoMov);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,70,"             Fecha:  %s\n", fechahora);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,70,"Tarjetas Retiradas:  %d\n", Config.Captura);
	printer.WriteComm( (unsigned char *)line, strlen(line) );


	snprintf(line,70,"      No. Empleado:  %d\n", data_user[NOUSER_1].noempleado);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,70,"      No. Empleado:  %d\n", data_user[NOUSER_2].noempleado);
	printer.WriteComm( (unsigned char *)line, strlen(line) );


	print_enter(2);
	datetime(fechahora);
	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_RIGHT) );
	printer.WriteComm( (unsigned char *)fechahora, strlen(fechahora) );
	print_enter(2);

	printer_cut();

//	printer.WriteComm( (unsigned char *)"\x0C",1);
//	print_enter(4);
//	printer.WriteComm( (unsigned char *)PRINTER_CUT, sizeof(PRINTER_CUT) );
//	printer.CloseComm();

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int printVentaCancelada(int saldo, char merr[])
{	char line[60], fechahora[40];

	if( printer.OpenComm((char *)"/dev/ttyS5", 9600)!= 0)
	{
		printf("Error al abrir el puerto serie /dev/ttyS5\n");
		return 0x01;
	}


printf("printer 12\n");

	printEncabezado();
	print_enter(2);
	
	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_CENTER) );

	strcpy(line, merr);
//	snprintf(line,60,"VENTA CANCELADA POR FONDOS INSUFICIENTES\n");
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	print_enter(2);

	snprintf(line,60,"     Saldo:     %6.2f\n", (double)saldo/100);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	
	print_enter(2);
	datetime(fechahora);
	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_RIGHT) );
	printer.WriteComm( (unsigned char *)fechahora, strlen(fechahora) );
	print_enter(1);

	printer_cut();

	return 0x00;	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int testEncabezado(void)
{	char line[60], fechahora[40];

	if( printer.OpenComm((char *)"/dev/ttyS5", 9600)!= 0)
	{
		printf("Error al abrir el puerto serie /dev/ttyS5\n");
		return 0x01;
	}

	printer.WriteComm( (unsigned char *)PRINTER_INITIALIZE,sizeof(PRINTER_INITIALIZE));
	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_CENTER) );
		
	printer.WriteComm( (unsigned char *)ENCABEZADO1, sizeof(ENCABEZADO1) );
	print_enter(1);
	printer.WriteComm( (unsigned char *)ENCABEZADO2, sizeof(ENCABEZADO2) );
	print_enter(1);
	printer.WriteComm( (unsigned char *)ENCABEZADO3, sizeof(ENCABEZADO3) );
	print_enter(2);

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_LEFT, sizeof(PRINTER_ALIGN_LEFT) );
	// Imprimir Datos de Estacion
	snprintf(line,70,"      Linea:     %s\n", Lineas.name[mert_identidad.LINEA-1]);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	strcpy(line, Lineas.name[mert_identidad.LINEA-1]);
	snprintf(line,70,"      Estacion:  %s\n", MertID.name);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,70,"      Equipo:    %s\n", MertID.code);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	
	print_enter(4);
	datetime(fechahora);
	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_RIGHT) );
	printer.WriteComm( (unsigned char *)fechahora, strlen(fechahora) );
	print_enter(2);

	printer_cut();
	
//	printer.WriteComm( (unsigned char *)"\x0C",1);
//	print_enter(4);
//	printer.WriteComm( (unsigned char *)PRINTER_CUT, sizeof(PRINTER_CUT) );
//	print_enter(2);

//	usleep(50000);
//	printer.CloseComm();
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int impCaja(char tipo, IMP_DETALLE datos)
{	char fechahora[40];
	char line[70];
	double v;
	int i;

	if( printer.OpenComm((char *)"/dev/ttyS5", 9600)!= 0)
	{
		printf("Error al abrir el puerto serie /dev/ttyS5\n");
		return 0x01;
	}

	printer.WriteComm( (unsigned char *)PRINTER_INITIALIZE,sizeof(PRINTER_INITIALIZE));
	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_CENTER) );
		
	printer.WriteComm( (unsigned char *)ENCABEZADO1, sizeof(ENCABEZADO1) );
	print_enter(1);
	printer.WriteComm( (unsigned char *)ENCABEZADO2, sizeof(ENCABEZADO2) );
	print_enter(1);
	printer.WriteComm( (unsigned char *)ENCABEZADO3, sizeof(ENCABEZADO3) );
	print_enter(2);

	if(tipo == 1)
		printer.WriteComm( (unsigned char *)ENCABEZADO4, sizeof(ENCABEZADO4) );
	else
		printer.WriteComm( (unsigned char *)ENCABEZADO5, sizeof(ENCABEZADO5) );
	print_enter(1);

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_LEFT, sizeof(PRINTER_ALIGN_LEFT) );
	snprintf(line,60,"      Linea:       %s\n", datos.nlinea);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,60,"      Estacion:    %s\n", datos.nestacion);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,60,"      Equipo:      %s\n", datos.nequipo);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,60,"     Usuario1:     %6d\n", datos.usuario1);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,60,"     Usuario2:     %6d\n", datos.usuario2);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,60,"      Ficha:       %06d\n", datos.ficha);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,60,"      Sello:       %06d\n", datos.sello);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,60,"      Alcancia:    %u\n", datos.alcancia);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,60,"      Folio:       %06d\n", datos.folio);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	
	if(mert_famisa)
	{
		snprintf(line,60,"      Creacion:    %s\n", datos.fecha);
		printer.WriteComm( (unsigned char *)line, strlen(line) );

		datetime(fechahora);
		snprintf(line,60,"      Fecha Corte: %s\n", fechahora);
		printer.WriteComm( (unsigned char *)line, strlen(line) );
	}
	else
	{
		snprintf(line,60,"Creacion:    %s\n", datos.fecha);
		printer.WriteComm( (unsigned char *)line, strlen(line) );

		datetime(fechahora);
		snprintf(line,60,"Fecha Corte: %s\n", fechahora);
		printer.WriteComm( (unsigned char *)line, strlen(line) );
	}
		
	print_enter(1);

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_CENTER) );
	printer.WriteComm( (unsigned char *)"------------------------------", 30 );
	print_enter(1);

	for(i=0; i<TOTAL_DENOM_BM; i++)
	{
		if(datos.valor[i]!= 0)
		{	v = datos.valor[i];
			snprintf(line,60,"%6.2f        %4d\n", (v/100) , datos.cantidad[i]);
			printer.WriteComm( (unsigned char *)line, strlen(line) );
		}	
	}
	print_enter(1);

	snprintf(line,60,"Total:       %6d\n", datos.total);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	printer.WriteComm( (unsigned char *)"------------------------------", 30 );
	print_enter(2);

	printer.WriteComm( (unsigned char *)PRINTER_BOLD_ON, sizeof(PRINTER_BOLD_ON) );
	v = datos.monto;
	snprintf(line,60,"Monto Total:      $ %6.2f\n", (v/100));
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	print_enter(1);
	printer.WriteComm( (unsigned char *)PRINTER_B_S_OFF, sizeof(PRINTER_B_S_OFF) );

	print_enter(3);

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_RIGHT) );
	printer.WriteComm( (unsigned char *)fechahora, strlen(fechahora) );
	print_enter(2);

	printer_cut();

//	printer.WriteComm( (unsigned char *)"\x0C",1);
//	print_enter(4);
//	printer.WriteComm( (unsigned char *)PRINTER_CUT, sizeof(PRINTER_CUT) );
//	print_enter(1);
//	usleep(100000);
//	printer.CloseComm();

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int impTicketTransaccion(char tipo, IMP_DETALLE datos, char status)
{	char fechahora[40], line[60];
//	double v;
//	int i;

	if( printer.OpenComm((char *)"/dev/ttyS5", 9600)!= 0)
	{
		printf("Error al abrir el puerto serie /dev/ttyS5\n");
		return 0x01;
	}

	printer.WriteComm( (unsigned char *)PRINTER_INITIALIZE,sizeof(PRINTER_INITIALIZE));
	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_CENTER) );

	
	printer.WriteComm( (unsigned char *)ENCABEZADO1, sizeof(ENCABEZADO1) );
	print_enter(1);
	printer.WriteComm( (unsigned char *)ENCABEZADO2, sizeof(ENCABEZADO2) );
	print_enter(1);
	printer.WriteComm( (unsigned char *)ENCABEZADO3, sizeof(ENCABEZADO3) );
	print_enter(2);

	if(tipo == 1)
		printer.WriteComm( (unsigned char *)ENCABEZADO6, sizeof(ENCABEZADO6) );
	else
		printer.WriteComm( (unsigned char *)ENCABEZADO7, sizeof(ENCABEZADO7) );
	print_enter(1);
	
	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_LEFT, sizeof(PRINTER_ALIGN_LEFT) );
	snprintf(line,60,"      Linea:       %s\n", datos.nlinea);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,60,"      Estacion:    %s\n", datos.nestacion);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,60,"      Equipo:      %s\n", datos.nequipo);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,60,"      Folio:       %d\n", datos.folio);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
//	print_enter(2);
	datetime(fechahora);
	snprintf(line,60,"      Fecha: %s\n", fechahora);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	print_enter(1);

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_CENTER) );

	if(status==0)
		printer.WriteComm( (unsigned char *)ENCABEZADO10, sizeof(ENCABEZADO10) );
	else
		printer.WriteComm( (unsigned char *)ENCABEZADO12, sizeof(ENCABEZADO12) );

	print_enter(1);
	printer.WriteComm( (unsigned char *)datos.nlogico, strlen(datos.nlogico) );
	print_enter(2);

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_LEFT, sizeof(PRINTER_ALIGN_LEFT) );

	snprintf(line,60,"     Tarjeta:            %s\n", datos.UID);
	printer.WriteComm( (unsigned char *)line, strlen(line) );

	snprintf(line,60,"     Saldo Anterior:     %6.2f\n", (double)datos.saldo_inicial/100);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	snprintf(line,60,"     Bono Anterior:      %6.2f\n", (double)datos.bono_inicial/100);
	printer.WriteComm( (unsigned char *)line, strlen(line) );
	print_enter(1);

	if(status==0)
	{
		snprintf(line,60,"     Saldo Actual:       %6.2f\n", (double)datos.saldo_final/100);
		printer.WriteComm( (unsigned char *)line, strlen(line) );
		snprintf(line,60,"     Bono Actual:        %6.2f\n", (double)datos.bono_final/100);
		printer.WriteComm( (unsigned char *)line, strlen(line) );
		print_enter(1);

		printer.WriteComm( (unsigned char *)ENCABEZADO11, sizeof(ENCABEZADO11) );
		snprintf(line,60,"     Recarga de Saldo:   %6.2f\n", (double)datos.deposito/100);
		printer.WriteComm( (unsigned char *)line, strlen(line) );
		print_enter(1);
		snprintf(line,60,"     TOTAL:              %6.2f\n", (double)datos.deposito/100);
		printer.WriteComm( (unsigned char *)line, strlen(line) );
		print_enter(2);
	}
	else
	{
		printer.WriteComm( (unsigned char *)ENCABEZADO11, sizeof(ENCABEZADO11) );
		snprintf(line,60,"  %s   %6.2f\n", ENCABEZADO13, (double)datos.deposito/100);
		printer.WriteComm( (unsigned char *)line, strlen(line) );
		print_enter(1);
		print_enter(2);
	}

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_CENTER) );
	printer.WriteComm( (unsigned char *)ENCABEZADO8, sizeof(ENCABEZADO8) );
	print_enter(1);
	printer.WriteComm( (unsigned char *)ENCABEZADO9, sizeof(ENCABEZADO9) );
	print_enter(1);

	printer.WriteComm( (unsigned char *)PRINTER_ALIGN_CENTER, sizeof(PRINTER_ALIGN_CENTER) );
	printer.WriteComm( (unsigned char *)fechahora, strlen(fechahora) );
	print_enter(1);
	
	printer_cut();

//	printer.WriteComm( (unsigned char *)"\x0C",1);
//	print_enter(4);
//	printer.WriteComm( (unsigned char *)PRINTER_CUT, sizeof(PRINTER_CUT) );
//	printer.CloseComm();

	return 0x00;	
}

////////////////////////////////////////////////////////////////////////////////
