//
//   ************ Rutinas para los procesos de Venta y Recarga
//
//
#include "bd.h"
#include "cctalk.h"
#include "xmls.h"
#include "libMIA.hpp"
#include "printer.h"
#include "readers.h"
//#include "readers/class_cim6903.h"
#include "windows/dispensador.h"
#include "mert.h"
#include "kytv26xx.h"
#include "readers/class_acr1222l.h"

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void rep_alarma( int nv)
{	int i;

	for( i=0; i<nv; i++)
	{
		while(stModIO->NoControl !=0 );

		stModIO->OnOFF     = R_PULSE;
		stModIO->tiempo    = 2;	//
		stModIO->NoControl = ALARMA_AUDIBLE_VISIBLE;	
		usleep(400000);
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void RevisaPermisoVenta(void)
{
	mert_operacion.fgVENTA		= 0;	// No Vende Tarjetas
	mert_operacion.fgRECARGA	= 0;	// NO Recarga		

	if( mert_identidad.TIPO == MERT_EXPENDEDORA)
	{
		printf("\nMAQUINA EXPENDEDORA\n");
		mert_operacion.fgVENTA		= 1;	// Si Vende Tarjetas
		mert_operacion.fgRECARGA	= 1;	// Si Recarga
		// verifica el estado de stacker

//		printf("modelo =%d\n", MertPort.EXP_Marca);
		if(MertPort.EXP_Marca==DIS_KYT2664)
		{
//			if( dispensador.KytStatus() ==0)
			if( venta.reader_kyt2664.KytStatus() ==0)
			{
				if( venta.reader_kyt2664.kytNoCard==0)
				{
					mert_operacion.fgVENTA = 0;	// No Vende Tarjetas
				}
			}	
		}
		else if( venta.StackerStatus() !=0)
		{
			mert_operacion.fgVENTA		= 0;	// No Vende
			return;
		}
		else
		{
				printf("HAY TARJETAS EN STACKERS\n");
			// // HAY TARJETAS EN LOS DISPENSADORES
			// Revisar la BD
			if	(detStackers[MAGAZINE1].TotalTarjetas==0)
			{
				printf("NO VENDE\n");
				mert_operacion.fgVENTA = 0;	// No Vende Tarjetas
				return;
			}
		}
	}
	else if( mert_identidad.TIPO == MERT_RECARGADORA)
	{
		printf("\nMAQUINA RECARGADORA\n");
		mert_operacion.fgVENTA		= 0;	// No Vende Tarjetas
		mert_operacion.fgRECARGA	= 1;		// Si Recarga
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void fillDatosTrans(void)
{
	datImprimir.linea		= mert_identidad.LINEA;
	datImprimir.estacion	= mert_identidad.ESTACION;
	datImprimir.equipo		= mert_identidad.NUMDISP;

	strcpy(datImprimir.nlinea, Lineas.name[mert_identidad.LINEA-1]);
	strcpy(datImprimir.nestacion, 
			Lineas.estaciones[mert_identidad.LINEA-1].name[mert_identidad.ESTACION-1]);

	strcpy(datImprimir.nequipo, MertID.code);

	datImprimir.saldo_inicial = TotalCantidadInicial;
	datImprimir.bono_inicial  = TotalBonosInicial;
	datImprimir.saldo_final   = TotalCantidadFinal;
	datImprimir.bono_final    = TotalBonosFinal;
	datImprimir.deposito      = TotalCantidadDepositado;

	datImprimir.folio         = detTransaccion.NoMov;

	strcpy(datImprimir.nlogico, recarga.MIA.LogicNumber);
//	strcpy(datImprimir.nlogico, recarga.MIA.ReturnTSC.LogicNumber);
	sprintf((char *)datImprimir.UID, "%02X%02X%02X%02X",recarga.MIA.MifareUID[0],
														recarga.MIA.MifareUID[1],
														recarga.MIA.MifareUID[2],
														recarga.MIA.MifareUID[3]);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int imprime_transaccion(char status)
{
	fillDatosTrans();
	impTicketTransaccion(1, datImprimir, status);
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int buscar_perfil(int perfil)
{	int i;

	for(i=0; i<BonosProfile.tot_reg; i++)
	{
		if(perfil== BonosProfile.profile[i])
			return BonosProfile.code[i];
	}
	return 0; // no se encontro el perfil
}

/******************************************************************************/
/*             Calcula la bonificacion del deposito                           */
/******************************************************************************/
int calcula_bonificacion(int perfil, int cantidad)
{	int k, tbonos;

	if( (tbonos=buscar_perfil(perfil))==0)
		return 0;  // Sin Bonificacion

	tbonos--;
	for(k=0;k< BonosRango[tbonos].tot_reg;k++)
	{
		if(cantidad>= BonosRango[tbonos].value[k])
			;
		else
			return BonosRango[tbonos].bono[k-1];	// bonificacion
	}
	// bonificacion maxima
	return BonosRango[tbonos].bono[BonosRango[tbonos].tot_reg-1];

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int RecargaMaxima(int perfil)
{	int i;

	for(i=0; i< BalanceDatos.tot_reg; i++)
		if(perfil == BalanceDatos.profile[i])
			return BalanceDatos.max_balance[i]*100;
	return 0x00;	// No existe el perfil
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void MuestraMovimientos(void)
{	int i;
	char msg[2048], line[140];

	sprintf(msg, "                    Fecha de Caducidad [%s]  Mov=[%2d]\n\n", 
												movMIA.fCaducidad, movMIA.tot_reg);

	sprintf(line, "   Fecha    Hora  Estacion             Eq. Rec/Ent      Valor    Bono   Saldo  Bono R\n");
	strcat(msg, line);

	for(i=0; i<movMIA.tot_reg; i++)
	{
		movMIA.estacion[i][19] = 0x00;	// limita nombre a 20 caracteres

		sprintf(line, "%2d %s %-20s %03d %-10s %7.2f %7.2f %7.2f %7.2f\n", 
			(i+1), movMIA.fecha[i], movMIA.estacion[i], movMIA.equipo[i], movMIA.tmov[i],
			double(movMIA.valor[i])/100, double(movMIA.bono[i])/100,
			double(movMIA.saldoMov[i])/100, double(movMIA.bonoMov[i])/100 );
		strcat(msg, line);
	}

	winMsgMovimientos( msg, "Ubuntu Mono", 400, 20);
}

/******************************************************************************/
/* OPEN/CLOSE ILUMINA_BILLETERO ILUMINA_MONEDERO ILUMINA_BANDEJA_CAMBIO       */
/******************************************************************************/
void ILUMINA3(char open_close)
{
	//
	stModIO->OnOFF     = open_close;
	stModIO->NoControl = ILUMINA_BILLETERO;	
	while(stModIO->NoControl !=0 );

	//
	stModIO->OnOFF     = open_close;
	stModIO->NoControl = ILUMINA_MONEDERO;
	while(stModIO->NoControl !=0 );

	//
	stModIO->OnOFF     = open_close;
	stModIO->NoControl = ILUMINA_BANDEJA_CAMBIO;
	while(stModIO->NoControl !=0 );
}

/******************************************************************************/
/* Prepara la condicion inicial para la venta/recarga                         */
/******************************************************************************/
int condiciones_iniciales(void)
{	int ne, i;

	ILUMINA3(R_CLOSE);

	//
	stModIO->OnOFF     = R_OPEN;
	stModIO->NoControl = ILUMINA_LECTOR_TARJETA;
	while(stModIO->NoControl !=0 );

	billsprocesos(0,0);
//	bills.Posicion = 0x00;
//	bills.Path     = 0x00;

	while(stModIO->cmdBill !=0 );
	stModIO->cmdBill = CMD_BILL_ENABLE;

	// Regresa cualquier billete en ESCROW
	if(BilleteroON==1)
	{
		//expulsa cualquier billete en ESCROW
		while(stModIO->cmdBill !=0 );
		stModIO->cmdBill = CMD_BILL_RETURN;
	}

	// Habilitar Monedero
	while(stModIO->cmdCoin !=0 );
	stModIO->cmdCoin = CMD_COIN_ENABLE;	

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void final_io(void)
{
	ILUMINA3(R_OPEN);

	stModIO->OnOFF     = R_CLOSE;
	stModIO->NoControl = ILUMINA_LECTOR_TARJETA;
	while(stModIO->NoControl !=0 );
}

/******************************************************************************/
/* Deshabilita los diferentes perifericos                                     */
/******************************************************************************/
int condiciones_finales(void)
{
	while(stModIO->cmdCoin !=0 );
	stModIO->cmdCoin = CMD_COIN_DISABLE;	

	if(BilleteroON==1)
	{
		//expulsa cualquier billete en ESCROW
		while(stModIO->cmdBill !=0 );
		stModIO->cmdBill = CMD_BILL_RETURN;
	}

	usleep(100000);

	while(stModIO->cmdBill !=0 );
	stModIO->cmdBill = CMD_BILL_DISABLE;

	return 0x00;
}

/******************************************************************************/
/* Proceso para grabar la MIA, actualizar BD y generar archivo de transaccion */
/******************************************************************************/
int UpdateMIA(void)
{	int re=0;

	if( recarga.MIA.UpdateBlocksPointers(TotalCantidadDepositado, TotalBonosDepositado) != 0)
	{	// Error al actualizar informacion
		printf("Error al actualizar informacion\n");
		re = 0x401;
	}
	else
	{	printf("WRITE CARD\n");
		if( recarga.WriteAllCard(0)== 0)
		{
			printf("WRITE MIA OK\n");
			re =  0x00;
		}
		else
		{
			printf("WRITE MIA BAD\n");
			re = 0x403;
		}	
	}
	return re;
}

/******************************************************************************/
/*  operacion:   1=VENTA 2=RECARGA 3=VENTA_ERR                                */
/******************************************************************************/
int insTransaccion(int operacion, int status)
{	DATA_TRANS dtrans;

	dtrans.Status = status;	// OK

	dtrans.Linea_TDE		= MertID.lineaTDE;
	dtrans.Estacion_TDE		= MertID.estacionTDE;
	dtrans.Linea_TVT		= MertID.lineaTV;
	dtrans.Estacion_TVT		= MertID.estacionTV;
	dtrans.NoDisp			= MertID.NumDisp;

	dtrans.VentaRecarga		= operacion;
		dtrans.ValorMIA			= TotalFianza;
	strcpy(dtrans.ID_Name,  MertID.code);

	if(status == DISP_ERR_VENTA_CANCEL)
	{
		dtrans.SaldoInicial		= 0;
		dtrans.Deposito			= TotalReal;
		dtrans.SaldoFinal		= 0;
		dtrans.BonoInicial		= 0;
		dtrans.BonoDepositado	= 0;
		dtrans.BonoFinal		= 0;

		strcpy(dtrans.UID_SAM,  "");
		strcpy(dtrans.UID_MIA,  "");
		strcpy(dtrans.NoLogico,  "");		
	}
	else
	{
		dtrans.SaldoInicial		= TotalCantidadInicial;
		dtrans.Deposito			= TotalCantidadDepositado;
		dtrans.SaldoFinal		= TotalCantidadFinal;
		dtrans.BonoInicial		= TotalBonosInicial;
		dtrans.BonoDepositado	= TotalBonosDepositado;
		dtrans.BonoFinal		= TotalBonosFinal;

		unsigned char *p;
		p = &recarga.MIA.SamUID[0];
		sprintf(dtrans.UID_SAM,"%02X%02X%02X%02X%02X%02X%02X%02X", 
						p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7]);

		if(operacion == PROC_RECARGA)	//OPERACION_RECARGA)
		{
			p = &recarga.MIA.MifareUID[0];
			sprintf(dtrans.UID_MIA,"%02X%02X%02X%02X", p[0],p[1],p[2],p[3]);
			strcpy(dtrans.NoLogico, recarga.MIA.LogicNumber);
		}
		else
		{
			p = &venta.MIA.MifareUID[0];
			sprintf(dtrans.UID_MIA,"%02X%02X%02X%02X", p[0],p[1],p[2],p[3]);
			strcpy(dtrans.NoLogico, venta.MIA.LogicNumber);
		}
	}
	//**************************************************************************
	// cambiar cuando se tengas los datos de numero de cajas
	dtrans.NoCajaBilletes = detBilletes.CajaID;
	dtrans.NoCajaMonedas  = detMonedas.CajaID;
	//**************************************************************************

	insBD_Transaccion(dtrans);
	dtrans.NoMov = detTransaccion.NoMov;
	// genera xml de transaccion
	xmlTransaccion(dtrans);
	xmlCorteParcial();	// actualiza corte parcial
	return 	0x00;	// OK
}

/******************************************************************************/
/* Actualiza las BD de Monedas                                                */
/******************************************************************************/
int updateBD_Coins(void)
{	int k,i;

	for(k=0; k<coins.TotalDenominaciones;k++)
	{
		if(coins.depositado.counter[k] > 0)
		{
//			printf("Valor=%04d Cantidad=%02d\n", 
//						coins.depositado.valor[k], coins.depositado.counter[k]);

			// busca valor en BD
			for(i=0; i < TOTAL_DENOM_BM; i++)
			{
				if(coins.depositado.valor[k] == detMonedas.Valor[i])
				{
					// valor encontrado
					detMonedas.Total[i] += coins.depositado.counter[k];
					break;
				}
			}
		}
	}
	detMonedas.CantidadTotal += coins.depositado.cantidad;
	wrLastTotal(dbMonedas, detMonedas);
	return 0;
}

/******************************************************************************/
/* Actualiza las BD de Monedas                                                */
/******************************************************************************/
int updateBD_Bills(void)
{	int k;

	for(k=0; k<TOTAL_DENOM_BM;k++)
		detBilletes.Total[k] += bills.depositado.counter[k];

	detBilletes.CantidadTotal += bills.depositado.cantidad;
	wrLastTotal(dbBilletes, detBilletes);
	
	return 0;
}

/******************************************************************************/
/* Actualiza BD                                                               */
/******************************************************************************/
int updateBD(int tOperacion, int status)
{	int re=0;

	updWin();

	if( fgUpdate != 0x00)
		return re;

	if( updateBD_Coins() != 0)
		re |= 0x01;		// Error BD Monedas

	updWin();

	if( updateBD_Bills() != 0)
		re |= 0x02;		// Erroe BD Billetes

	updWin();

	if(tOperacion == PROC_RECARGA)		//OPERACION_RECARGA)
	{	// solo en recarga
		if(GRABA_MIA)
		{	if( UpdateMIA() != 0)
				re |= 0x10;		// Error en MIA
		}
	}

	updWin();

	if( insTransaccion(tOperacion, status)!= 0)
		re |= 0x20;		// Error BD Transacciones

	updWin();

	if(re!=0)
		printf("Error al actualizar BD[%02x]\n]", re);

	fgUpdate = 0x01;	// bandera de actualizacion

	return re;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void msg_limite_excedido(void)
{
	// mostrar mensaje durante 1 segundo
	winMsgLimite();
}

/******************************************************************************/
/*  Antes de finalizar revisa si hay monedas pendientes de procesar           */
/******************************************************************************/
int revLastEvent(int profile)
{	int k, ipos, ivalor;

	for(k=0; k<5; k++)
	{	if( (coins.Posicion>0) && (coins.Path>=MONEDA_EN_CAJA) )
		{	// moneda aceptado
			ipos   = coins.Posicion;
			ivalor = coins.Value(ipos);
			coins.Posicion=0;       // elimina evento

			acumula(profile, ivalor);

			coins.depositado.counter[ipos]++;
			coins.depositado.cantidad += ivalor;
		}
		updWin();
		usleep(100000);
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void msg_casilleno(int nm)
{
	winMsgCasiLLeno(nm);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void msg_lleno(int nm)
{
	ILUMINA3(R_OPEN);

	stModIO->OnOFF     = R_OPEN;
	stModIO->NoControl = ILUMINA_LECTOR_TARJETA;
	while(stModIO->NoControl !=0 );
	
	recarga.LedOnOff(CRT_LED_RED, CRT_LED_ON);
	winMsgLLeno(nm);

	stModIO->StatusProceso=MODO_CAJA_LLENA;

//	printf("Proceso=%d\n", stModIO->StatusProceso);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int posiciona_destrabador(void)
{	unsigned char pant;
	int lapso;

	// posicionar destrabador
	pant = moduloES.Entradas[SENSOR_MONEDERO];
	pant =  INPUT_IN_1;

	stModIO->OnOFF     = R_CLOSE;
	stModIO->NoControl = MOTOR_DESATORAR;
	while(stModIO->NoControl !=0 );

	lapso = milisegundos();
	while(pant != 0xFF)
	{
		if(pant != moduloES.Entradas[SENSOR_MONEDERO])
		{	pant = moduloES.Entradas[SENSOR_MONEDERO];
			if(pant==INPUT_IN_1)
			{
				stModIO->OnOFF     = R_OPEN;
				stModIO->NoControl = MOTOR_DESATORAR;
				while(stModIO->NoControl !=0 );
				pant = 0xff;
				return 0;	// OK
			}
		}
		if( (milisegundos()-lapso) >= 3000 )
		{	// apaga el motor
			printf("Error Destrabador, no se detecto cambio en la entrada\n");
			stModIO->OnOFF     = R_OPEN;
			stModIO->NoControl = MOTOR_DESATORAR;
			while(stModIO->NoControl !=0 );
			return 0x01;	//error
		}
	}
	return 0x01;	// Error
}

/******************************************************************************/
/*  Procedimiento para destrabar                                              */
/******************************************************************************/
int procDestrabar(void)
{
	// Deshabilitar Monedero
	coins.SendWaitMasterInhibit(CCTALK_OFF);

	winMsgDestrabar( (char *)"Destraba\nMonedero", "SANS", 400, 60);
	posiciona_destrabador();
	winExitDestrabar();

	// Habilitar Monedero
	coins.SendWaitMasterInhibit(CCTALK_ON);

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void SaveEvent(int ivalor)
{	char fname[] ={"/home/user/mert/events.txt"};
	char date[100], da[30];

	FILE *fptr=NULL;
	fptr = fopen(fname, "a");

	datetime(da);
	sprintf(date,"%s -> %.2f\n", da, double(ivalor)/100);
	fprintf ( fptr,"%s", date);

	fclose(fptr);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int WaitBillToX( char ruta )
{	unsigned int tm= milisegundos();
	unsigned int tout;
	int i;

	unsigned int tiempos[5] = {15000, 8000, 4000, 3000, 3000};

	for(i=0; i<5; i++)
	{
		tout = tiempos[i];

		while(stModIO->cmdBill !=0 );
		if(ruta == CMD_BILL_ACCEPT)
		{
			stModIO->cmdBill = CMD_BILL_ACCEPT;
		}
		else
			stModIO->cmdBill = CMD_BILL_RETURN;

		while(stModIO->cmdBill !=0 );
		usleep(50000);
 
		tm= milisegundos();

		// espera evento de billete en caja, durante 5 segundos maximo
		while( (milisegundos()-tm) < tout)
		{
			if(ruta==CMD_BILL_ACCEPT)
			{
				if( bills.Posicion > 0 )
				{
					printf("bill to box Posicion = %d PATH = %d\n",bills.Posicion, bills.Path );
					if(bills.Path == BILLETE_EN_CAJA)
						return 0x00;	// billete aceptado
					if(bills.Path == BILLETE_RECHAZADO)
						return 0x01;	// billete rechazado
				}
				else
				{
//					printf("--Proceso Error = %d\n",bills.error);
					if(bills.error != 0xFF)
					{
						printf("--Proceso Error = %d\n",bills.error);
						while(flagBill==1);
						flagBill=1;
						bills.error = 0xFF;
						flagBill=0;
					}
				}
			}
			else
			{
				if( (bills.Posicion==BILLETE_POS_DUMMY) && (bills.Path==BILLETE_RECHAZADO) )
				{
//5/02/2025			return 0x00;	// billete regresado
					return 0x01;	// billete regresado
				}
			}
		}
	}
	printf("Error al enrutar el billete\n");
	return -1;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int WaitBillReturn( void )
{	unsigned int tm= milisegundos();
	unsigned int tout;
	int i;

	unsigned int tiempos[4] = {7000,5000,4000,3000};

	for(i=0; i<4; i++)
	{
		tout = tiempos[i];

		while(stModIO->cmdBill !=0 );
		stModIO->cmdBill = CMD_BILL_RETURN;

		usleep(50000);

		tm = milisegundos();

		// espera evento de billete en caja, durante 5 segundos maximo
		while( (milisegundos()-tm) < tout)
		{
			if( (bills.Posicion==BILLETE_POS_DUMMY) && (bills.Path == BILLETE_RECHAZADO) )
			{
				printf("billete regresado\n");
				return 0x00;	// billete regresado
			}
		}
	}
	printf("No lo pudo regresar\n");
	return -1;	// ERROR NO SE REGRESO EL BILLETE
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int BillToBox(int valor)
{	int re= 1;

	coins.SendWaitMasterInhibit(CCTALK_OFF);

	winValidaBill(valor);

	re = WaitBillToX( CMD_BILL_ACCEPT);

	switch(re)
	{
		case 0:	// billete aceptado
			re = 0x00;	// se acredita el saldo
			break;

		case 1:	// billete retornado
			re = 0x01;	// No se acredita el Saldo
			break;

		case -1:	// no se recibio el evento
			SaveEvent(valor);
			re = 0x00;	// se acredita el saldo
			break;
			
		default:
			printf("evento desconocido = %d\n", re);
	}

	billsprocesos(0, bills.Path);
//	bills.Posicion=0;
	coins.SendWaitMasterInhibit(CCTALK_ON);
	winValidaExit();
	return re;
}

/******************************************************************************/
/* Acumula Deposito y actualiza totales en pantalla                           */
/******************************************************************************/
void acumula(int profile, int ivalor)
{
	TotalReal += ivalor;

	if( mert_operacion.proceso == PROC_VENTA)
	{
		if(TotalReal<=TotalFianza)
		{	//printf("menor TR=%d TF=%d\n", TotalReal, TotalFianza);
			TotalCantidadInicial = (TotalFianza - TotalReal) * -1;
			gtk_setcolorbg(entSaldoInicial, rgb_red);
		}
		else
		{	//printf("mayor\n");
			gtk_setcolorbg(entSaldoInicial, rgb_white);
			TotalCantidadInicial = 0;
			TotalCantidadDepositado = TotalReal - TotalFianza;
			TotalBonosDepositado = calcula_bonificacion(profile, TotalCantidadDepositado);
			TotalCantidadFinal = TotalCantidadInicial + TotalCantidadDepositado;
			TotalBonosFinal = TotalBonosInicial + TotalBonosDepositado;
		}
	}
	else
	{
		gtk_setcolorbg(entSaldoInicial, rgb_white);
		TotalCantidadDepositado += ivalor;
		TotalBonosDepositado = calcula_bonificacion(profile, TotalCantidadDepositado);

		TotalCantidadFinal = TotalCantidadInicial + TotalCantidadDepositado;
		TotalBonosFinal = TotalBonosInicial + TotalBonosDepositado;
	}

	win_saldo_inicial(TotalFianza, TotalCantidadDepositado, 
											TotalCantidadInicial, TotalBonosInicial);			
	win_saldo_final(TotalCantidadFinal, TotalBonosFinal, TotalCantidadDepositado);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int total_billcoin(DET_BILL_COIN detalle)
{	int i, total;

	total = 0;
	for(i=0; i<TOTAL_DENOM_BM; i++)
	{	if(detalle.Valor[i] != 0)
			total += detalle.Total[i];
	}
	return total;
}

/******************************************************************************/
/* Selecciona el STACKER en uso                                               */
/******************************************************************************/
int SeleccionaStacker(void)
{
	if( venta.StackerStatus() != 0)
	{
		// NO Hay tarjetas en los STACKERs
		return -1;
	}

	// MAGAZINE 1 -> ACTIVO
	//if(Config.Stacker == STACKER1)
	//{	// HAY TARJETAS EN BASE DE DATOS
		if(	detStackers[MAGAZINE1].TotalTarjetas>0)
		{	// STATUS NO EMPTY
			if( venta.GetStacker(0x00) != 0)
				return MAGAZINE1;	// OK- STACKER1
		}
	//}

	#if 0
	// MAGAZINE 2 -> ACTIVO
	if(Config.Stacker == STACKER2)
	{	// HAY TARJETAS EN BASE DE DATOS
		if(	detStackers[MAGAZINE2].TotalTarjetas>0)
		{	// STATUS NO EMPTY
			if( venta.GetStacker(MAGAZINE2) != 0)
				return MAGAZINE2;	// OK- STACKER2
		}
	}
	#endif

	// NO CUMPLEN LAS CONDICIONES LOS STACKERS
	if(	detStackers[MAGAZINE1].TotalTarjetas>0)
	{	// STATUS NO EMPTY
		if( venta.GetStacker(MAGAZINE1) != STACKER_EMPTY)
			return MAGAZINE1;	// OK- STACKER1
	}

	#if 0
	if(	detStackers[MAGAZINE2].TotalTarjetas>0)
	{	// STATUS NO EMPTY
		if( venta.GetStacker(MAGAZINE2) != STACKER_EMPTY)
			return MAGAZINE2;	// OK- STACKER2
	}
	#endif

	// ERROR, NO PROCEDE LA VENTA
	return -1;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void ret_faltante(void)
{
	gtk_fixed_move ( GTK_FIXED (fxProcess), lblMensajes, 112, 200);
	gtk_setsize(lblMensajes, 800, 400);
	gtk_hide(lblMensajes);

	win_show_botones(WIN_PROCESO);
	gtk_widget_show(fxSaldo);
	
	updWin();
}

/******************************************************************************/
/*  Incrementa BinBox                                                         */
/******************************************************************************/
int UpdateBinBox(void)
{
	Config.Captura++;
	UpdateConfig();

	// Mejor revisar en Limites
	if(Config.Captura>= Config.BinBoxMax)
	{
		// Alarma
	}
	return TRUE;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int ReadAllNewCard(int tkey)
{	int k;

	updWin();

	memset(venta.MIA.ReadBlocks, 0x00, sizeof(venta.MIA.ReadBlocks));
	memset(venta.MIA.KeyB, 0x00, sizeof(venta.MIA.KeyB));
	

	for(k=0; k<6; k++)
	{
		venta. MIA.KeyA[1][k] = venta.MIA.MifareUID[k];
	}

	if( venta.Read_Sector1A(	venta.MIA.MifareUID, 
								venta.MIA.KeyA, 
								venta.MIA.ReadBlocks )!= 0)
	{
		printf("Error Sector 1A\n");
		return 0x0101;
	}

	updWin();
	
	if( recarga.GetCardKey(tkey,	venta.MIA.MifareUID, 
									venta.MIA.KeyB, 
									venta.MIA.ReadBlocks ) != 0)
	{
		printf("Error en Get Keys\n");

		return 0x0102;
	}

//	printf("OK en Get Keys\n");

	updWin();

	if( venta.READ_ALL_WITH_KEY(KEYB,venta.MIA.KeyB, venta.MIA.ReadBlocks )!=0)
	{
		printf("Error READ ALL\n");
		return 0x0103;
	}
	
//	printf("VENTA->OK READ ALL\n");
//	venta.MIA.PrintReadData();
//	venta.MIA.PrintKeys();
		
	updWin();
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void billsprocesos(unsigned char posicion, unsigned char path )
{
	while(flagBill==1);
	flagBill = 1;
	bills.Posicion = posicion;
	bills.Path = path;
	flagBill = 0;
}
/******************************************************************************/
/* SMOL                                                                       */
/******************************************************************************/
int proceso_venta(long tcarga, long tbonos, long tfianza)
{	int err, stk,k;
		unsigned int tmGetCard = milisegundos();

	for(k=0; k<2; k++)
	{
		updWin();

		if( (stk = SeleccionaStacker()) == -1)
			goto ExitError;
		updWin();

		if( venta.GetCard(stk+1) != 0x00)
		{
			printf("ERROR AL TOMAR LA TARJETA\n");
			err = DISP_ERR_GETCARD;
			goto ExitError;
		}
		updWin();

		tmGetCard = milisegundos();
		while     (  venta.CardPresent() != 0  )
		{	
			usleep(10000);

			if (  (milisegundos() - tmGetCard) >= 5000  )
			{
				printf("ERROR NO DETECTO LA TARJETA\n");
				err = DISP_ERR_NOCARD;
				goto ExitError;		
			}
		};

		updWin();

		if( ReadAllNewCard(ISAM) != 0)
		{
			// error al leer la informacion de la MIA
			err = DISP_ERR_READCARD;
			goto ExitError;
		}
		updWin();

		// Prepara la informacion a Grabar
		if(venta.MIA.VentaMIA(mert_identidad, tcarga, tbonos, tfianza) != TRUE)
		{	// error al leer la informacion de la MIA
			printf("*******************Error en MIA********************\n");
			err = DISP_ERR_MIA;
			goto ExitError;
		}
		updWin();

		if(GRABA_MIA)
		{	if(venta.WriteAllCard(0x00)!=0)
			{
				//printf("Error en Grabar MIA\n");
				err = DISP_ERR_WRITECARD;
				goto ExitError;
			}
		}
		updWin();

		if( venta.EjectCard() != 0)
		{
			printf("Error al mover a Bandeja\n");
			err = DISP_ERR_BINBOX;
			goto ExitError;
		}

		updWin();

		// actualizar BD de tarjetas
		DecrementaStacker(stk);
		//printf("*****************************\n\n");
		return  0x00;

ExitError:

		if( err != DISP_ERR_STACKER )
		{
			// Inicializa la tarjeta
			if( (err == DISP_ERR_MIA) && (InitMIA==1) )
				venta.WriteInitCard();

			if( venta.CaptureCard() != 0)
			{
				printf("Error al Capturar\n");
			}
			UpdateBinBox();
			DecrementaStacker(stk);
		}
	}

	updWin();

	// Imprime Ticket
	printVentaCancelada(tcarga+tfianza, disp_err[err+1].msg);
	destroyGif();
	winErrorDispensador();

	//printf("*****************************\n\n");
	return err;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void CambiaMsgBotones(void)
{
	if( NoPasoProceso == PASO_MOV_FIN)
	{	// se recibio, dinero cambiar mensaje en botones
		NoPasoProceso = PASO_IMP_FIN;
		win_show_botones(WIN_PROCESO);									
	}
}

/******************************************************************************/
/*  limitar nombre a 40 caracteres                                            */
/******************************************************************************/
char *buscaEstacion(char linea, char estacion)
{
	char *nombre = (char*)malloc(50);

	// validar rango de linea y estacion, si no cumplen reportar en su lugar
	// Linea=### Estacion=###

	if( (linea>0) && ( linea <= Lineas.tot_reg) )
	{	//printf("Linea=%d -> Estaciones=%2d\n", linea, Lineas.estaciones[linea-1].tot_reg);
		if( (estacion>0) && ( estacion <= Lineas.estaciones[linea-1].tot_reg) )
		{
			strcpy(nombre, Lineas.estaciones[linea-1].name[estacion-1]);
			return nombre;
		}
	}

	sprintf(nombre, "L =%2d E =%3d", linea, estacion);
	return nombre;
}

/******************************************************************************/
/*	    recarga.MIA.PrintHistoricalMovements();                               */
/******************************************************************************/
void readMovimientos(void)
{	int HistMovBlocks[12];
	int count, i;
	HistoricalMovementsbits HistMovTemp;
	int saldoMov, cargoMov; 
	int bonoMov,  bcargoMov ;

	char fecha[20];

	memset( &movMIA, 0x00, sizeof(movMIA) );

	saldoMov = TotalCantidadInicial;
	cargoMov = 0;

	bonoMov  = TotalBonosInicial;
	bcargoMov = 0;

	count = recarga.MIA.OperationEnd.PointerMove - 1;

	// Ordenammos los bloques del historico de movimientos
	for(i = 0; i < 12; i++)
	{
		HistMovBlocks[i] = recarga.MIA.HistoricalMovementsBlocks[count--];
		if(count == -1)
			count = 11;
	}

	for(i=0; i<TotalMovements; i++)
	{
		if( recarga.MIA.BlockInZero(HistMovBlocks[i]) != 0x00)
		{
			// el block tiene informacion
			recarga.MIA.PassingHistorical( &HistMovTemp, HistMovBlocks[i]);
			saldoMov = saldoMov - cargoMov;
			bonoMov  = bonoMov - bcargoMov;

			strcpy(fecha, DecadeMonth2Date(HistMovTemp.DecMonth) );

			sprintf( movMIA.fecha[i], "%02d/%s %02d:%02d",
				HistMovTemp.Day, fecha, HistMovTemp.Hour, HistMovTemp.Minute);

			strcpy( movMIA.tmov[i], nameOperaciones[HistMovTemp.SubOperation]);
			
			strcpy( movMIA.estacion[i],
								buscaEstacion(HistMovTemp.Line, HistMovTemp.Station));
			movMIA.equipo[i] = HistMovTemp.EquipmentSell;
			movMIA.bono[i]   = HistMovTemp.Way;
			movMIA.valor[i]  = HistMovTemp.AmountCharge;

			movMIA.saldoMov[i] = saldoMov;
			movMIA.bonoMov[i]  = bonoMov;

			cargoMov = HistMovTemp.AmountCharge;
			bonoMov  = HistMovTemp.Way;
			if( HistMovTemp.SubOperation == OPER_ENTRADA)
			{	cargoMov  = cargoMov  * -1;
				bcargoMov = bcargoMov * -1;
			}
		}
		else
		{
			// block en Zeros, se termina la busqueda
			break;
		}
	}
	movMIA.tot_reg = i;

	sprintf(movMIA.fCaducidad, "%02d/%02d/%02d",	recarga.MIA.ReturnTSC.ExpirationDay, 
													recarga.MIA.ReturnTSC.ExpirationMonth,
													recarga.MIA.ReturnTSC.ExpirationYear);
	sprintf(movMIA.fEfectiva,  "%02d/%02d/%02d",	recarga.MIA.ReturnTSC.BirthDay,
													recarga.MIA.ReturnTSC.BirthMont,
													recarga.MIA.ReturnTSC.BirthYear);
}

/******************************************************************************/
/*   //250206                                                                 */
/******************************************************************************/
int recarga_process(void)
{	static char  imprime, cntLibera;
	static int unsigned lapso, rec_maxima;
	static int profile, mia, tmOperacion, tmMov;
	int lim, ivalor, ipos;
	int tcoins, tbills, err;

	switch(mnu_opcion)
	{	case PROCESO_INICIAL:
				// incializa contador de billetes/monedas
				bills.ClearDepositado();
				coins.ClearDepositado();

				if( recarga.UNLOCK(LOCK_ON)!=0)	// bloquea la tarjeta
				{	// error al bloquear la tarjeta
					printf("Error al bloquear CRT\n");
				}
				
				fgUpdate = 0x00;

				if( mert_operacion.proceso == PROC_RECARGA)
				{
					printf("RECARGA\n");
					// lee MIA
					if( recarga.ReadAllData(ISAM) != 0)
					{	// error al leer la informacion de la MIA
						mnu_opcion = PROCESO_ERROR;
						break;
					}

					// Trasnfiere todos los datos leidos a las estructuras de la MIA
					mia = recarga.MIA.TransferVerifica(mert_identidad);
					if( (mia != 0) || (recarga.MIA.ReturnTSC.Category==EMPLEADOS) ) 
					{
						// error en tarjeta, se regresa
						mnu_opcion = PROCESO_ERROR;
						break;
					}
					profile = recarga.MIA.ReturnTSC.TitleCode;

					recarga.MIA.ReadBalanceCard();
					
					TotalCantidadInicial = recarga.MIA.MiaBalance.Purse;

					TotalBonosInicial = recarga.MIA.MiaBalance.Bonus;
					TotalFianza = 0;

					readMovimientos();
				}
				else
				{	// PROCESO DE VENTA
					printf("VENTA \n");
					recarga.LedOnOff(CRT_LED_RED, CRT_LED_ON);

					profile = 111;		// Titulo Tarjeta en Venta
					TotalBonosInicial		= 0;
					TotalFianza             = MertLmt.cost_card;
					TotalCantidadInicial	= -TotalFianza;

					// No hay movimientos
					memset( &movMIA, 0x00, sizeof(movMIA) );					
				}

				if( (rec_maxima=RecargaMaxima(profile)) == 0 )
				{	// no existe el perfil, rechaza el billete
					//printf("B-Perfil No existe=%d\n", profile);
					mnu_opcion = PROCESO_ERROR;
					break;
				}
				//printf("Recarga Maxima=%d\n", rec_maxima);

				moduloES.EntradasOld[ BOTON_INFERIOR_IZQUIERDO] = moduloES.Entradas[ BOTON_INFERIOR_IZQUIERDO];
				moduloES.EntradasOld[ BOTON_INFERIOR_DERECHO]   = moduloES.Entradas[ BOTON_INFERIOR_DERECHO];
				moduloES.EntradasOld[ BOTON_SUPERIOR_IZQUIERDO] = moduloES.Entradas[ BOTON_SUPERIOR_IZQUIERDO];

				winSaldo_show();

				// Totales en 0's
				TotalCantidadFinal		= 0;
				TotalReal				= 0;
				TotalCantidadDepositado = 0;
				TotalBonosDepositado	= 0;
				imprime					= 0;	// NO

				acumula(profile, 0);

				win_saldo_inicial(TotalFianza, TotalCantidadDepositado, 
										TotalCantidadInicial, TotalBonosInicial);

				condiciones_iniciales();

				// incializa contador de billetes/monedas
				bills.ClearDepositado();
				coins.ClearDepositado();

				tmOperacion = milisegundos();
				lapso = milisegundos();

				// Validar que no este sobrepasado el limite de saldo
				if( TotalCantidadFinal >= rec_maxima)
				{
					// limite excedido
					msg_limite_excedido();
//					deshabilita_coin_bill(profile);
					// termina venta/recarga
					mnu_opcion = PROCESO_ERROR;
					break;
				}
				mnu_opcion = PROCESO_VENTA_RECARGA;
				break;

		case PROCESO_VENTA_RECARGA:	// recibe saldo: Bill & Coins

				// pregunta si estan las cajas de valores
				if( (fgCajaMonedas==0) || (fgCajaBilletes==0) )
				{
					// falta alguna caja de valores
					//printf("Error en Caja de Valores\n");

					winErrorMsg(MSG_CAJAS, 1500);
					mnu_opcion = PROCESO_CONDICION_FINAL;
					tmOperacion = milisegundos();
					break;
				}

				// monedero
				if( (coins.Posicion>0) && (coins.Path>=MONEDA_EN_CAJA) )
				{	// moneda aceptado
					tmOperacion = milisegundos();	// reinicia tiempo de operacion

					ipos   = coins.Posicion;
					ivalor = coins.Value(ipos);

					coins.Posicion=0;	// elimina evento

					CambiaMsgBotones();
					acumula(profile, ivalor);
					tmOperacion = milisegundos();

					coins.depositado.counter[ipos]++;
					coins.depositado.cantidad += ivalor;

					if( (TotalCantidadFinal) > rec_maxima)
					{
						// limite excedido
						msg_limite_excedido();
						mnu_opcion = PROCESO_CONDICION_FINAL;
						break;
					}
				}

				// Billetero
				if( (bills.Posicion > 0) && (bills.Path == BILLETE_EN_ESCROW) )
				{	// billete aceptado
					tmOperacion = milisegundos();	// reinicia tiempo de operacion

					ipos   = bills.Posicion;
					ivalor = bills.Value(ipos);

					printf("\n\n***********************************************\n"
						"pos=%d- ivalor=%d\n", ipos, ivalor);

					billsprocesos(0,BILLETE_NADA);

					if( (TotalCantidadFinal + ivalor) > rec_maxima)
					{
						// limite excedido
						msg_limite_excedido();
						WaitBillToX( CMD_BILL_RETURN);
					}
					else
					{
						// Dentro de Rango, Manda el Billete a la Caja y Acumula
						if( BillToBox(ivalor) == 0)
						{	
							switch(ivalor)
							{
								case 2000:
											bills.depositado.counter[0]++;
											break;
								case 5000:
											bills.depositado.counter[1]++;
											break;
								case 10000:
											bills.depositado.counter[2]++;
											break;
								case 20000:
											bills.depositado.counter[3]++;
											break;
								case 50000:
											bills.depositado.counter[4]++;
											break;
								case 100000:
											bills.depositado.counter[5]++;
											break;
							}
							
							bills.depositado.cantidad += ivalor;

							CambiaMsgBotones();
							acumula(profile, ivalor);
						}
					}
				}
				////////////////////////////////////////////////////////////////

				if(bills.fg_master_inhibit==0)
				{
					if( fgMsgBill==0)
					{
						printf("************************************************solo monedas\n");
						winMsg(lblMsgBill, rgb_orange_red, rgb_black, (char *)"--> SOLO MONEDAS <--", "Sans", 700,50);
						fgMsgBill = 1;	//show
					}
				}
				else
				{
					if( fgMsgBill==1)
					{
						printf("************************************************oculta monedas\n");
						gtk_hide(lblMsgBill);
						fgMsgBill=0;
					}
				}

				// revisar tiempo de operacion
				if( (milisegundos()- tmOperacion) >= MertLmt.time_ope_refill)
				{	// terminar operacion
					printf("TIEMPO EXCEDIDO, TERMINA OPERACION\n");
					mnu_opcion = PROCESO_CONDICION_FINAL;
					break;
				}

				// Revisa boton destrabar
				if( moduloES.BotonOprimido(BOTON_SUPERIOR_IZQUIERDO) == BOTON_OPRIMIDO_SI)
				{	printf("DESTRABAR\n");
					procDestrabar();
					tmOperacion = milisegundos();
				}

				// REVISAR SI OPRIMIO ALGUN BOTON Y EJECUTAR SU ACCION
				switch(NoPasoProceso)
				{	case PASO_MOV_FIN:		// SIN SALDO
							// Puede revisar sus movimientos o terminar, NO ABONO
							// FINALIZAR
//							if( moduloES.BotonOprimido(BOTON_INFERIOR_DERECHO) == BOTON_OPRIMIDO_SI)
							if( moduloES.Botones[BOTON_INFERIOR_DERECHO] == 0x01)
							{
								printf("0-FINALIZAR=%d\n", TotalCantidadFinal);
								mnu_opcion = PROCESO_CONDICION_FINAL;
								tmOperacion = milisegundos();
								break;
							}

							// MOVIMIENTOS
							if( mert_operacion.proceso == PROC_RECARGA)
							{	// solo en recarga hay movimientos
								if( moduloES.BotonOprimido(BOTON_INFERIOR_IZQUIERDO) == BOTON_OPRIMIDO_SI)
								{	printf("MOVIMIENTOS \n");
								
									// Deshabilitar Monedero/Billetero
									condiciones_finales();
									ILUMINA3(R_OPEN);
									MuestraMovimientos();

									mnu_opcion = PROCESO_MUESTRA_MOVIMIENTOS;
									tmOperacion = milisegundos();
								}
							}
							break;

					case PASO_IMP_FIN:	// CON SALDO
							if( moduloES.Botones[BOTON_INFERIOR_DERECHO] == 0x01)
							{
								printf("1-FINALIZAR=%d\n", TotalCantidadFinal);
								tmOperacion = milisegundos();
								// revisar si estamos en venta y que lo depositado
								// sea mayor a la fianza
								if(TotalCantidadInicial < 0)
								{	// no ha completado la fianza
//******************
									printf("FALTA\n");
									NoPasoProceso = PASO_FALTANTE;
									winMsgFaltante(TotalFianza, TotalCantidadInicial);
									mnu_opcion = PROCESO_FALTA;
									break;
								}
//******************
								printf("Grabar MIA [%d][%d]\n", 
												TotalCantidadDepositado, TotalBonosDepositado);

								mnu_opcion = PROCESO_CONDICION_FINAL;
							}
							break;
				}
				break;

		case PROCESO_FALTA:	// Falta $ para completar la fianza
				if( moduloES.BotonOprimido(BOTON_INFERIOR_IZQUIERDO) == BOTON_OPRIMIDO_SI)
				{
					printf(" SI FINALIZA**********************\n");
					mnu_opcion = PROCESO_CONDICION_FINAL;
					tmOperacion = milisegundos();
				}
				if( moduloES.BotonOprimido(BOTON_INFERIOR_DERECHO) == BOTON_OPRIMIDO_SI)
				{
					printf("CONTINUA DEPOSITANDO\n");
					winButtonMainHide();
					mnu_opcion = PROCESO_VENTA_RECARGA;
					NoPasoProceso = PASO_IMP_FIN;
					condiciones_iniciales();
					ret_faltante();
					tmOperacion = milisegundos();
				}
				break;

		case PROCESO_MUESTRA_MOVIMIENTOS:
				// REGRESAR
				if( moduloES.BotonOprimido(BOTON_INFERIOR_DERECHO) == BOTON_OPRIMIDO_SI)
//				if( moduloES.Botones[BOTON_INFERIOR_DERECHO] == 0x01)
				{
					printf("REGRESAR AL MENU ANTERIOR\n");
					winExitMovimientos();
					winSaldo_show();
//					opcion = PROCESO_VENTA_RECARGA;	// REGRESA A RECIBIR SALDO
					mnu_opcion = PROCESO_REGRESA_MOVIMIENTOS;	// REGRESA A RECIBIR SALDO
					tmOperacion = tmMov = milisegundos();
					//  Habilitar Monedero/Billetero
					ILUMINA3(R_CLOSE);
					condiciones_iniciales();
				}
				break;

		case PROCESO_REGRESA_MOVIMIENTOS:
				if( moduloES.Botones[BOTON_INFERIOR_DERECHO] == 0x00)
				{
					if( (milisegundos() - tmMov) > 500)
					{
						printf("SALIR DE MOVIMIENTOS\n");
						mnu_opcion = PROCESO_VENTA_RECARGA;	// REGRESA A RECIBIR SALDO
					}
					tmOperacion = milisegundos();
				}
				else
					tmMov = milisegundos();	
				break;

		case PROCESO_ERROR:		// smol
				printf("Proceso Error\n");
				winButtonMainHide();
				gtk_hide(fxSaldo);

				winMsgWaitMomment(MSG_CERRANDO);

				updWin();

				winGifWait();
			
				condiciones_finales();
			
				updWin();

				stModIO->StatusProceso = 0x00;	// Proceso de Venta/Recarga finalizado


				recarga.LedOnOff(CRT_LED_ALL, CRT_LED_FLASHING);

				final_io();

				lapso = milisegundos();
				mnu_opcion = PROCESO_LIBERA_TARJETA;
				cntLibera = 0;
				break;

		// PROCESO DE FINALIZAR VENTA o RECARGA
		case PROCESO_CONDICION_FINAL:	//smol
				winButtonMainHide();
				gtk_hide(fxSaldo);

				updWin();

				winGifWait();

				if(mert_operacion.proceso == PROC_VENTA)
				{
					if(TotalCantidadInicial >= 0)
						winMsgWaitMomment(MSG_SALDO);
					else
						winMsgWaitMomment(MSG_CERRANDO);
				}
				else
				{	if(TotalCantidadDepositado>0)
						winMsgWaitMomment(MSG_SALDO);					
					else
						winMsgWaitMomment(MSG_CERRANDO);
				}

				revLastEvent(profile);

				condiciones_finales();

				updWin();

				stModIO->StatusProceso = 0x00;	// Proceso de Venta/Recarga finalizado

				updWin();

				if(mert_operacion.proceso == PROC_VENTA)
				{
					// VENTA
					if(TotalReal==0)
					{	// todo en cero, no hace nada

						mnu_opcion = PROCESO_INICIAL;

						updWin();

						final_io();

						if( recarga.UNLOCK(LOCK_OFF)!=0)	// se libera la tarjeta
						{	// error al bloquear la tarjeta
							printf("Error al liberar la CRT\n");
						}
						destroyGif();
						winmain_show();
						return 0x01;
						break;
					}
					else
					{
						if(TotalCantidadInicial < 0)
						{
							// no completo la fianza, solo se le imprime un ticket
							// actualiza BD de billetes, monedas y transacciones
							printVentaCancelada(TotalReal, disp_err[DISP_ERR_VENTA_CANCEL].msg );
							destroyGif();
							winErrorVentaIncompleta();
							updateBD(PROC_VENTA_ERR, DISP_ERR_VENTA_CANCEL+1);

							final_io();

							if( recarga.UNLOCK(LOCK_OFF)!=0)	// se libera la tarjeta
							{	// error al desbloquear la tarjeta
								printf("Error al liberar la CRT\n");
							}
							mnu_opcion = PROCESO_REVISAR_LIMITES;
							break;
						}
						else
						{	// Completo minimo la fianza, se le despacha su tarjeta
							// inicia proceso del DISPENSADOR
							winMsgWaitMomment(MSG_SALDO);
							printf("DISPENSA TARJETA\n");
							if( (err=proceso_venta(	TotalCantidadDepositado, 
												TotalBonosDepositado, 
											TotalFianza)) == 0x00)
							{
								printf("FIN PROCESO DE VENTA OK\n");
								updateBD(PROC_VENTA, 0x01);
							}
							else
							{
								// cambia ruta de salida
								printf("ERROR EN PROCESO DE VENTA\n");
								updateBD(PROC_VENTA, err+1);

								final_io();

								if( recarga.UNLOCK(LOCK_OFF)!=0)	// se libera la tarjeta
								{	// error al desbloquear la tarjeta
									printf("Error al liberar la CRT\n");
								}
								mnu_opcion = PROCESO_REVISAR_LIMITES;
								break;
							}
						}
					}
				}
				else
				{	// RECARGA
					printf("TotalCantidadDepositado=%d\n", TotalCantidadDepositado);
					if(TotalCantidadDepositado>0)
					{	winMsgWaitMomment(MSG_SALDO);
						if( (updateBD(PROC_RECARGA, 0x01) & 0x10) == 0x10)
						{
							printf("ERROR EN GRABAR MIA\nIMPRIME TICKET\n");
							imprime_transaccion(0x01);
						}
					}
				}

				if(imprime == 1)	// SI
				{	imprime_transaccion(0x00);
					imprime = 0;
				}

				lapso = milisegundos();

				recarga.LedOnOff(CRT_LED_ALL, CRT_LED_FLASHING);

				final_io();

				mnu_opcion = PROCESO_LIBERA_TARJETA;
				cntLibera = 0;
				break;

		case PROCESO_LIBERA_TARJETA:
				if( (milisegundos() - lapso) >= TiempoDeBarrido)
				{	// se deshabilita monedero, billetero, se apagan los indicadores
					// de ambos, asi como la luz de la charola
					lapso = milisegundos();
					if( recarga.UNLOCK(LOCK_OFF)!=0)	// se libera la tarjeta
					{	// error al desbloquear la tarjeta, implementar contador
						// 
						printf("Error al liberar la CRT\n");
						cntLibera++;
						if(cntLibera<2)
							break;
					}
					mnu_opcion = PROCESO_WAIT_RETIRED;
					destroyGif();
					winMsgRemoveMIA();

					recarga.LedOnOff(CRT_LED_ALL, CRT_LED_FLASHING);					
				}
				break;

		case PROCESO_WAIT_RETIRED:	// Espera a que se retire la tarjeta
				if( (milisegundos() - lapso) >= TiempoDeBarrido)
				{
					if( (err=recarga.CardPresent()) == 1)
					{
						// ho hay tarjeta
						// Revisar los limites de Monedas/Billetes
						mnu_opcion = PROCESO_REVISAR_LIMITES;
						break;
					}
					else
					{
						if(err == CRT_CARDST_UNKNOW)
						{
							printf("***********Device offline**********\n");
							// poner MERT fuera de servicio
							fgCRT285 = 0;	// bandera de error del CRT285
							mnu_opcion = PROCESO_REVISAR_LIMITES;// avanza a la siguiente opcion
						}
					}
				}
				break;

		case PROCESO_REVISAR_LIMITES:
				printf("Revisar Limites\n");
				tcoins = total_billcoin(detMonedas);
				tbills = total_billcoin(detBilletes);

				lim = 0;
				if( tcoins >= MertLmt.coin_capacity_stored )
					lim = lim | 0x01;
				if( tbills >= MertLmt.bill_capacity_stored )
					lim = lim | 0x02;
				if(Config.Captura>= Config.BinBoxMax)
					lim = lim | 0x04;
				if( fgCRT285 == 0)
					lim = lim | 0x08;	// bloquea MERT

				if(lim!=0)
				{	// alguna caja llena, alarma y se pone en
					// fuera de operacion
					printf("Caja Llena o Alarmas=%d\n", lim);

					msg_lleno(lim);
					lapso = milisegundos();
					mnu_opcion = PROCESO_LIMITES;
					break;
				}

				lim = 0;
				if( tcoins >= MertLmt.coin_threshold_stored )
					lim = lim | 0x01;
				if( tbills >= MertLmt.bill_threshold_stored )
					lim = lim | 0x02;
				if(lim!=0)
				{	// alguna caja casi llena, alarma y continua 
					printf("Caja casi Llena=%d\n", lim);

					msg_casilleno(lim);

				}
				mnu_opcion = PROCESO_INICIAL;
				RevisaPermisoVenta();
				winmain_show();
				return 0x01;	// exit recarga process

		case PROCESO_LIMITES:
				if( (milisegundos() - lapso) >= 2000)
				{	// revisar cada 5 segundos si se restablecieron los limites

					lapso = milisegundos();

					tcoins = total_billcoin(detMonedas);
					tbills = total_billcoin(detBilletes);
					lim = 0;
					if( tcoins >= MertLmt.coin_capacity_stored )
						lim = lim | 0x01;
					if( tbills >= MertLmt.bill_capacity_stored )
						lim = lim | 0x02;
					if(Config.Captura>= Config.BinBoxMax)
						lim = lim | 0x04;
					if( fgCRT285 == 0)
						lim = lim | 0x08;	// bloquea MERT

					if(lim==0)
					{	// OK
						printf("TODO NORMAL = %d\n", lim);
						winmain_show();
						mnu_opcion = PROCESO_INICIAL;
						return 0x01;	// exit recarga process
						break;
					}
				}
				if(fgCRT285 == 0)
				{
					if( moduloES.Entradas[BOTON_INFERIOR_IZQUIERDO] == BOTON_OPRIMIDO_SI)
					{
						if( moduloES.fgLatch == 0)
						{	moduloES.fgLatch=1;
							moduloES.tmBtnLatch = milisegundos();
						}
						if( (milisegundos() - moduloES.tmBtnLatch) > 5000)
						{
							printf("Inicializa Lector\n");
							rep_alarma(1);
							initRecarga();
							// Inicializa el lector
							moduloES.fgLatch = 0;
						}
					}
					else
						moduloES.fgLatch = 0;
				}
				break;

		case PROCESO_MENSAJES:
			break;
		default:
				printf("E R R O R OPCION NO VALIDA\n");
				break;
	}
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int puerta_process(void)
{	int profile, mia,i;
	unsigned int uid;

	printf("PROCESANDO TARJETA EN PUERTA\n");

	if(puerta.Read_Sector1A(puerta.reader_acr1222.MFUID, puerta.reader_acr1222.MF_KEYA,
		puerta.MIA.ReadBlocks))
	{	
		printf("Error Sector 1A=[%04X]\n",i);
		return i;
	}


//	printf("Sector 1A OK\n");
//	puerta.MIA.PrintReadData();
				
//	printf("Read Sector 1A OK\n");

	// obtiene las llaves de la tarjeta en puerta
	if( ( i = recarga.GetCardKey(	KEYB,
									puerta.MIA.MifareUID, 
									puerta.MIA.KeyA, 
									puerta.MIA.ReadBlocks)) !=0 )
	{
		printf("Error GetCardKey=[%04X]\n",i);
		return i;
	}

//	puerta.MIA.PrintKeys();

	
//	printf("ReadAll Data\n");

	if( (i=puerta.READ_ALL_WITH_KEY(KEYB, 
											puerta.MIA.KeyA, 
											puerta.MIA.ReadBlocks ))!=0)
	{
		printf("Error ALL=[%04X]\n",i);
		return i;
	}
//	puerta.MIA.PrintReadData();

	// Trasnfiere todos los datos leidos a las estructuras de la MIA
	mia = puerta.MIA.TransferVerifica(mert_identidad);

	if( (mia != 0) || (puerta.MIA.ReturnTSC.Category != EMPLEADOS) ) 
	{	// error en algun dato de la MIA o no es tarjeta de EMPLEADO
		printf("ERROR datos y/o Tarjeta de Usuario\n");
		return 0x101;	// error en datos
	}

	printf("Tarjeta de Empleado\n");
	profile = puerta.MIA.ReturnTSC.TitleCode;

	printf("TitleCode      =%d\n", puerta.MIA.ReturnTSC.TitleCode);	// Codigo de Titulo de la tarjeta
	printf("Category       =%d\n", puerta.MIA.ReturnTSC.Category);	// Categoria del Titulo
	printf("Profile        =%d\n", puerta.MIA.ReturnTSC.EmployeeProfile);	// Perfil del empleado
	printf("Personalization=%d\n", puerta.MIA.ReturnTSC.PersonalizationFlag);	// Flag de personalizacion

	card_puerta.nousuario = -1;
	uid = cnvuidhex(puerta.MIA.MifareUID);

	if( (mia=SearchUser(Usuarios, uid))==-1)
	{
		printf("UID NO ENCONTRADO=[%08x]\n", uid);
		return 0x102;
	}

	card_puerta.nousuario = mia;
	card_puerta.noempleado = puerta.MIA.ReturnTSC.EmployeeCode;
	card_puerta.nouid = uid;
	card_puerta.noperfil = Usuarios.profile[mia];
	strcpy(card_puerta.name, Usuarios.name[mia]);
	strcpy(card_puerta.pass, Usuarios.pwd[mia]);

	printf("UID ENCONTRADO = %d->[%08x]\n", card_puerta.nousuario, card_puerta.nouid);
	printf("Nombre         = [%s]\n", card_puerta.name);
	printf("No. Empleado   = [%d]\n", card_puerta.noempleado);
	printf("Perfil         = [%d]\n", card_puerta.noperfil);
//	printf("Pass           = [%s]\n", card_puerta.pass);

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void move_data_door(int nuser)
{
	data_user[nuser].nousuario  = card_puerta.nousuario;
	data_user[nuser].noperfil   = card_puerta.noperfil;
	data_user[nuser].noempleado = card_puerta.noempleado;
	data_user[nuser].nouid      = card_puerta.nouid;
	strcpy(data_user[nuser].name, card_puerta.name);
	strcpy(data_user[nuser].pass, card_puerta.pass);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int ReadTxt_MIA(void)
{	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	FILE *fptr = NULL; 
	char fname[100];
	int k,i, hex;

	char hexa[6];	// = {"0x"};

	// LEE ARCHIVO PROFILE DE MANTENIMIENTO
	strcpy( fname, "/home/sisamo/mias/R0001-84D37E0E.txt");

	if(access(fname, F_OK)==0)
	{	printf("SI existe[%s]\n",fname);

		fptr = fopen(fname, "r");
		k=0;
		while ((read = getline(&line, &len, fptr)) != -1) 
		{
			for(i=0; i<16; i++)
			{
				memcpy(hexa, &line[i*2], 2);
				hex = strtol(hexa, NULL, 16);
				printf("%02X ", hex);
				recarga.MIA.WriteBlocks[k][i]= hex;
			}
			printf("\n");
			k++;
		}
		fclose(fptr);
		recarga.MIA.PrintWriteData();
		recarga.WriteAllBlock();
		return 0;
	}
	if (line)
        free(line);
	printf("NO existe [%s]\n",fname);
	return 1;
}
/******************************************************************************/