
#include "bd.h"
#include "cctalk.h"
#include "readers.h"

void PrintID(char msg[], unsigned char UID[]);
void ReadCajaValores(void);

int ReqMonedero (int cmdCoin);
int ReqBilletero(int cmdBill);

void wrNeEventBill(unsigned char ne);
void wrEventBill(char pro[], int pos, int path);
void wrErrorBill(int evento);

void wrNeEventCoin(unsigned char ne);
void wrEventCoin(char pro[], int pos, int path);
void wrErrorCoin(int evento);

/******************************************************************************/
/* Hilo para interrogar al Billetero y/o Monedero                             */
/* y reportar los evntos al proceso principal                                 */
/******************************************************************************/
void *mod_Aceptator(void *inp)
{	unsigned int lapso;
	unsigned int tmBill, tmCoin;
	unsigned char bc=0;

	struct st_io *mod;
	mod = (struct st_io*)inp;

	tmBill = tmCoin = milisegundos();

	while(1)
	{
		// Monedero/Billetero
		switch(mod->startCB)
		{		case 0:		// Espera a tener la configuracion del puerto
				usleep(100000);
				break;

			case 1:	// Espera a tener la configuracion del puerto
				lapso = milisegundos();

				coins.SendDisable();
				coins.SendWaitMasterInhibit(MONEDAS_DISABLE);


				bills.SendDisable();

				bills.SendWaitMasterInhibit(BILLETES_DISABLE);

				mod->cmdCoin = mod->cmdBill = 0;
				mod->startCB = 2;
				break;

			case 2:		// OPEN
				if( (milisegundos() - lapso) >= TiempoDeBarridoCCTALK )
				{
					lapso = milisegundos();
					if(bc==0)
					{
						bc = 1;
						if( ReqMonedero(mod->cmdCoin)==1)
							mod->cmdCoin=0;
					}
					else
					{
						bc = 0;

						if( ReqBilletero(mod->cmdBill)==1)
							mod->cmdBill=0;
					}
				}
				break;
		}

		if( mod->startCajas )
			ReadCajaValores( );
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void prtErrorBill(int noerror)
{
	printf("ERROR [%d = %s]\n", noerror, CCTAK_BILL_ERROR[noerror]);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void bill_clrflags(void)
{
	fgMsgBill=0;
	bills.fg_master_inhibit = 0; // fuera de servicio
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void billschange(unsigned char posicion, unsigned char path )
{
	while(flagBill==1);
	flagBill =1;
	bills.Posicion = posicion;
	bills.Path = path;
	flagBill = 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int ReqBilletero(int cmdBill)
{	static int tmBill = milisegundos();
	static byte fgCmd=100;	//primero status billetero
	int ne,k,evento,re;
	char event[100];

	// consulta billetero
	if(mert_operacion.fgBILLETERO!=0)
	{
		switch(cmdBill)
		{
			case CMD_BILL_POLL:	// Eventos
				if(fgCmd<=4)
				{
					fgCmd++;
					if( (ne=bills.ReadEvents(0x00)) > 0)
					{	
						//printf("Total Eventos=%d =%d \n", ne, mert_operacion.proceso);
						wrNeEventBill(ne);
						k = ne -1;
						while(k>=0)
						{
							evento = bills.RdEvent(k);

//						printf("Eventos=%d =%04d \n", ne, evento);
							if ( evento >= 0x0100)
							{
								tmBill = milisegundos();

								if( (mert_operacion.proceso != PROC_NONE) || (testBill) )
								{
									billschange((evento>>8) & 0xFF, (evento   ) & 0xFF);
									if(mert_famisa)
									{	re = bills.Posicion - 1;
										billschange(re, bills.Path);
//										bills.Posicion--;
									}
									if( mert_operacion.proceso == PROC_VENTA)
										wrEventBill( (char *)"VEN", bills.Posicion, bills.Path);
									else if( mert_operacion.proceso == PROC_RECARGA)
										wrEventBill( (char *)"REC", bills.Posicion, bills.Path);
									else
										wrEventBill( (char *)"OUT", bills.Posicion, bills.Path);
								}
								else
								{
									printf("se recibio billete fuera del proceso de venta/recarga\n");
									// se recibio billete fuera del proceso de venta/recarga
									// regresar billete( si entro al ESCROW) y deshabilitar el billetero
									wrEventBill( (char *)"OUT", (evento>>8)&0xFF, evento&0xFF);
								}
							}
							else
							{
								printf("Error = %04d\n", evento);
								prtErrorBill(evento);

								while(flagBill==1);
								flagBill=1;
								bills.error = evento & 0x00FF;
								flagBill=0;

								// ERROR
								if( evento != 0x00A0)
									wrErrorBill(evento);

								if( (evento==0x0001) || 
									( evento == CCTALK_ERROR_INVALID_BILL_1) ||
									( evento == CCTALK_ERROR_INVALID_BILL_2) ||
									( evento == CCTALK_ERROR_INVALID_BILL_2) ||
									( evento == CCTALK_ERROR_BILL_JAMMED_TRANS1) )
								{
									// billete rechazado
									printf("Billete Rechazado\n");
									billschange(BILLETE_POS_DUMMY,BILLETE_RECHAZADO);
								}

								while(flagBill==1);
								flagBill=1;
								bills.error = evento;
								flagBill=0;

								switch(evento)
								{
									case CCTALK_MASTER_INHIBIT:
										//0	Master inhibit active Status
										printf(" 0 - Evento de AUTO Master INHIBIT\n");
										break;
	
									case CCTALK_ERROR_BILL_RETURNED:		
										//1 Bill returned from escrow Status
										printf(" 1 - Evento Bill returned from escrow Status\n");
										break;

									case  CCTALK_ERROR_INVALID_BILL_1:		
										//2 Invalid bill ( due to validation fail ) Reject
										printf(" 2 - Invalid bill ( due to validation fail ) Reject\n");
										break;

									case  CCTALK_ERROR_INVALID_BILL_2:
										//3	Invalid bill ( due to transport problem ) Reject
										
										printf("3 - Invalid bill ( due to transport problem ) Reject\n");
										// termina ciclo, NO SE ACREDITA EL SALDO
										break;

									case CCTALK_ERROR_INVALID_BILL_3:		
										//4 Inhibited bill ( on serial ) Status
										printf("4 - Inhibited bill ( on serial ) Status\n");
										break;

									case CCTALK_ERROR_INVALID_BILL_4:
										//5 Inhibited bill ( on DIP switches ) Status
										printf("5 - Inhibited bill ( on DIP switches ) Status\n");
										break;
									case  CCTALK_ERROR_BILL_JAMMED_TRANS1:
										//6 Bill jammed in transport ( unsafe mode ) Fatal Error
										printf("6 - Bill jammed in transport ( unsafe mode ) Fatal Error\n");
										bill_clrflags();
										billschange(bills.Posicion, BILLETE_EN_CAJA);

										// se deshabilita billetero
//										bills.fg_master_inhibit = 0; // billetero fuera de servicio
										// termina ciclo, NO SE ACREDITA EL SALDO
										// al parecer aqui queda dentro del billetero, pero fuera de la caja
										// el usuario NO tiene acceso al billete, pero los operadores SI
										break;

									case  CCTALK_ERROR_BILL_JAMMED_STK:
										//7	Bill jammed in stacker Fatal Error
										printf("7 - Bill jammed in stacker Fatal Error\n");
										bill_clrflags();
										break;

									case CCTALK_ERROR_BILL_PULLED:
										//8	 Bill pulled backwards Fraud Attempt
										printf("8 - Bill pulled backwards Fraud Attempt\n");
										break;

									case CCTALK_ERROR_BILL_TAMPER:
										//9 Bill tamper Fraud Attempt
										printf("9 - Bill tamper Fraud Attempt\n");
										break;

									case CCTALK_STACKER_OK:
										//10 Stacker OK Status
										printf("10 - Stacker OK Status\n");
										break;

									case CCTALK_ERROR_STACKER_REMOVED:
										//11 Stacker removed Status
										printf("11 - Stacker removed Status\n");
										break;

									case CCTALK_ERROR_STACKER_INSERTED:
										//12 Stacker inserted Status
										printf("12 - Stacker inserted Status\n");
										break;
									case CCTALK_ERROR_STACKER_FAULT:
										//13 Stacker faulty Fatal Error
										printf("13 - Stacker faulty Fatal Error\n");
										bill_clrflags();
										break;

									case CCTALK_ERROR_STACKER_FULL:
										//14 Stacker full Status
										printf("14 - Stacker full Status\n");
										break;

									case CCTALK_ERROR_STACKER_JAMMED:
										//15 Stacker jammed Fatal Error
										printf("15 - Stacker jammed Fatal Error\n");
										// se deshabilita billetero
										bill_clrflags();
										billschange(BILLETE_POS_DUMMY,BILLETE_RECHAZADO);
										break;

									case CCTALK_ERROR_BILL_JAMMED_TRANS2:
										//16 Bill jammed in transport ( safe mode ) Fatal Error
										printf("16 - Bill jammed in transport ( safe mode ) Fatal Error\n");
										// termina ciclo, SE ACREDITA EL SALDO, billetero deshabilitado
										bill_clrflags();
										billschange(bills.Posicion,BILLETE_EN_CAJA);
									break;

									case CCTALK_ERROR_OPTO_FRAUD:
										//17 Opto fraud detected Fraud Attempt
										printf("17 - Opto fraud detected Fraud Attempt\n");
										break;
									case CCTALK_ERROR_STRING_FRAUD:
										//18 String fraud detected Fraud Attempt
										printf("18 - String fraud detected Fraud Attempt\n");
										break;

									case CCTALK_ERROR_ANTI_STRING:
										//19 Anti-string mechanism faulty Fatal Error
										printf("19 - Anti-string mechanism faulty Fatal Error\n");
										bill_clrflags();
										break;

									case CCTALK_ERROR_BARCODE_DETECTED:
										//20 Barcode detected Status
										printf("20 - Barcode detected Status\n");
										break;
									case CCTALK_ERROR_UNKNOW_BILL:
										//21 Unknown bill type stacked Status
										printf("21 - nknown bill type stacked Status\n");
										break;
								}

							}
							k--;
						}
					}
					else
					{
						// No Hay Eventos
					}
				}
				else
				{
					fgCmd = 0;
					re=bills.SendRequestSelfTest();
					if(re==0)
					{
						printf("Billetero No Contesta\n");
						return 0x100;
					}
					re--;
					if(re==0)
					{
						bills.fg_master_inhibit=1;	// OK
//						printf("Billetero EN LINEA=%d\n",fgMsgBill);
						fgShowCoin = 0;	// No lo muestra
						bills.num_intentos = 0;	// Billetero OK
					}
					else
					{
						bills.fg_master_inhibit=0;	//ERROR
//						printf("Billetero FUERA DE SERVICIO--%d- %d\n",re,fgMsgBill);
						fgShowCoin = 1;	// Si lo muestra 
					}
				}

				if( (milisegundos() - tmBill) > 180000)		// 1 min
				{
					tmBill = milisegundos();
					if( mert_operacion.proceso == PROC_NONE)
					{
						// enviar un BILL DISABLE
						bills.SendDisable();
						EventCntBill();
					}
				}
				return 0x00;
				break;

			case CMD_BILL_ENABLE:		// Enable
					if(BilleteroON==1)
					{	bills.RouteBill(BILL_RETURN);	//expulsa cualquier billete en stacker
						usleep(10000);
						bills.SendEnable();

						unsigned char dat[] = {0xff, 0xff, 0xff,0xff};
						bills.SendModifyInhibitBillStatus(dat, 4 );
						bills.SendMasterInhibitStatus(0x01);

						bills.SendWaitMasterInhibit(0x01);

						if( mert_operacion.proceso == PROC_VENTA)
							sprintf(event, "VEN HABILITA BILLETERO********************");
						else if( mert_operacion.proceso == PROC_RECARGA)
							sprintf(event, "REC HABILITA BILLETERO********************");
						else
							sprintf(event, "OUT HABILITA BILLETERO********************");

						EventBill(event);
						EventCntBill();

					}
					tmBill = milisegundos();
					return 0x01;
					break;

			case CMD_BILL_DISABLE:		// Disable
					bills.SendDisable();

					if(!mert_famisa)
						bills.SendMasterInhibitStatus(0);

					if( mert_operacion.proceso == PROC_VENTA)
						sprintf(event, "VEN DESHABILITA BILLETERO********************\n");
					else if( mert_operacion.proceso == PROC_RECARGA)
						sprintf(event, "REC DESHABILITA BILLETERO********************\n");
					else
						sprintf(event, "OUT DESHABILITA BILLETERO********************\n");

					EventDetBill(bills);
					EventBill(event);

					tmBill = milisegundos();
					return 0x01;
					break;

			case CMD_BILL_ACCEPT:
					bills.RouteBill(BILL_ACCEPT);

					if( mert_operacion.proceso == PROC_VENTA)
						sprintf(event, "   VEN CMD ACEPTA BILL");
					else if( mert_operacion.proceso == PROC_RECARGA)
						sprintf(event, "   REC CMD ACEPTA BILL");
					else
						sprintf(event, "   OUT CMD ACEPTA BILL");

					EventBill(event);

					tmBill = milisegundos();
					return 0x01;
					break;

			case CMD_BILL_RETURN:
					if(mert_famisa)
						bills.RouteBill(BILL_RETURN);
					else
						bills.RouteBill(0);

					if( mert_operacion.proceso == PROC_VENTA)
						sprintf(event, "   VEN CMD RETURN BILL");
					else if( mert_operacion.proceso == PROC_RECARGA)
						sprintf(event, "   REC CMD RETURN BILL");
					else
						sprintf(event, "   OUT CMD RETURN BILL");
					EventBill(event);

					tmBill = milisegundos();
					return  0x01;
					break;
					
			case CMD_BILL_RESET:		// RESET

					bills.SendResetDevice();
					if( mert_operacion.proceso == PROC_VENTA)
							sprintf(event, "VEN RESET BILLETERO********************");
					else if( mert_operacion.proceso == PROC_RECARGA)
							sprintf(event, "REC RESET BILLETERO********************");
					else
						sprintf(event, "OUT RESET BILLETERO********************");
					EventBill(event);
					EventCntBill();

				tmBill = milisegundos();
				return 0x01;
				break;

			case CMD_BILL_SENSOR:		// SENSORES

					bills.SendResetDevice();

					EventBill(event);
					EventCntBill();

				tmBill = milisegundos();
				return 0x01;
				break;


		}
	}
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int ReqMonedero(int cmdCoin)
{	static int tmCoin = milisegundos();
	int ne,k,evento;
	char event[100];

	// consulta billetero
	if(mert_operacion.fgMONEDERO!=0)
	{
		switch(cmdCoin)
		{
			case CMD_COIN_POLL:	// Eventos
				if( (ne=coins.ReadEvents(0x00)) > 0)
				{
					wrNeEventCoin(ne);

					for(k=0; k<ne; k++)
					{
						evento = coins.RdEvent(k);
						if ( evento > 0x100)
						{
							if( (mert_operacion.proceso != PROC_NONE) || (testCoin) )
							{
								coins.Posicion = (evento>>8) & 0xFF;
								coins.Posicion--;
								coins.Path     = (evento   ) & 0xFF;
//								printf("moneda = %d  %d\n", coins.Posicion, coins.Path);
								wrEventCoin((char *)"PRO", coins.Posicion, coins.Path);
							}
							else
							{
								wrEventCoin((char *)"OUT", (evento>>8)&0xFF, evento&0xFF);
							}
						}
						else
						{							
							// revisar error, revisar si procede una inhabilitacion
//							printf("Coin Event Error = %04x\n", evento);
							if( mert_operacion.proceso != PROC_NONE)
								sprintf(event, "   PRO No. Error = %04x", evento);
							else
								sprintf(event, "   OUT No. Error = %04x", evento);
							EventCoin(event);
						}
					}
				}
				else
				{
					// No responde el Monedero
				}

				if( (milisegundos() - tmCoin) > 60000)		// 1 min
				{
					tmCoin = milisegundos();
					if( mert_operacion.proceso == PROC_NONE)
					{
						// enviar un COIN DISABLE
					}
				}
				return 0x00;	// POLL
				break;

			case CMD_COIN_ENABLE:		// Enable
					coins.SendEnable();
					usleep(10000);
					coins.SendWaitMasterInhibit(CCTALK_ON);
					usleep(10000);

					if( mert_operacion.proceso != PROC_NONE)
						sprintf(event, "PRO HABILITA MONEDERO********************");
					else
						sprintf(event, "OUT HABILITA MONEDERO********************");
					EventCoin(event);
					tmCoin = milisegundos();
					break;

			case CMD_COIN_DISABLE:		// Disable
					coins.SendDisable();
					usleep(10000);
					coins.SendWaitMasterInhibit(MONEDAS_DISABLE);
					usleep(10000);

					if( mert_operacion.proceso != PROC_NONE)
						sprintf(event, "PRO DESHABILITA MONEDERO********************\n");
					else
						sprintf(event, "OUT DESHABILITA MONEDERO********************\n");
					EventCoin(event);

					tmCoin = milisegundos();
					break;

			case CMD_COIN_WAIT:
					return 0x00;
					break;
		}
	}
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void wrNeEventBill(unsigned char ne)
{	char event[200];
	char line[20], k;

	sprintf(event, "Total Eventos=%d old=%3d new=%3d ",
					ne, bills.cctalk_event_old, bills.cctalkevents.events);

	if( (ne==1) && (bills.cctalkevents.datos[0]==0) && 
		( (bills.cctalkevents.datos[1]==0xA0) || (bills.cctalkevents.datos[1]==0xA1) ) )
		return ;

	for(k=0; k<ne; k++)
	{
		sprintf(line, " %d-[%02X-%02X] ", k+1, 
					bills.cctalkevents.datos[k*2], bills.cctalkevents.datos[k*2 + 1]);
		strcat(event,line);
	}
	EventBill(event);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void wrEventBill(char pro[], int pos, int path)
{	int eventval;
	char event[100];

	eventval = bills.Value(pos);
	sprintf(event, "   %s Val=%6.2f Ruta=%d NoBill=%4d", pro, (double)eventval/100, path, 
					(detBilletes.Total[pos] + bills.depositado.counter[pos] + 1) );
	EventBill(event);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void wrErrorBill(int evento)
{	char event[100];

	if( mert_operacion.proceso != PROC_NONE)
		sprintf(event, "   PRO No. Error = %04x", evento);
	else
		sprintf(event, "   OUT No. Error = %04x", evento);
	EventBill(event);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void wrNeEventCoin(unsigned char ne)
{	char event[200];
	char line[20], k;

	sprintf(event, "Total Eventos=%d old=%3d new=%3d ",
					ne, coins.cctalk_event_old, coins.cctalkevents.events);

	for(k=0; k<ne; k++)
	{
		sprintf(line, " %d-[%02X-%02X] ", k+1,
					coins.cctalkevents.datos[k*2], coins.cctalkevents.datos[k*2 + 1]);
		strcat(event,line);
	}
	EventCoin(event);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void wrEventCoin(char pro[], int pos, int path)
{	int eventval;
	char event[100];

	eventval = coins.Value(pos);
	sprintf(event, "   %s Val=%6.2f Ruta=%d NoCoin=%4d Old=%02X  New=%02X", pro, 
					(double)eventval/100, path,
					(detMonedas.Total[pos] + coins.depositado.counter[pos] + 1),
					coins.cctalk_event_old, coins.cctalkevents.events);
	EventCoin(event);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void wrErrorCoin(int evento)
{	char event[100];

	if( mert_operacion.proceso != PROC_NONE)
		sprintf(event, "   PRO No. Error = %04x", evento);
	else
		sprintf(event, "   OUT No. Error = %04x", evento);
	EventCoin(event);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void ReadCajaValores( void )
{	static char fgStCM=0;
	static char fgStCB=0;
	static unsigned int intentos_cb=0;
	static unsigned int intentos_bb=0;

	if( coin_box.Read_MFUID(coin_box.MIA.MifareUID) == 0)
	{	fgStCM = 1;
		idCajaMonedas = cnvuidhex( coin_box.MIA.MifareUID );
		intentos_cb = niCajaMonedas = 0;
	}
	else
	{  
		intentos_cb++;
		if( (intentos_cb > NO_REINTENTOS_CAJAS) || (fgStCM==0) )
		{
			fgStCM = 1;
			// CoinBox, Caja NO Presente
			if( coin_box.SetMode() == 0)
				intentos_cb=0;
//			else
//				printf("Error AutoRead CajaMonedas\n");
			niCajaMonedas++;
		}

		if(niCajaMonedas >= NO_INTENTOS_CAJAS)
		{	// Error en Caja de Monedas
			idCajaMonedas=0;
			if( fgCajaMonedas == 1 )
			{	
				printf("Error en ID Caja de Monedas\n");
				EventosMert( (char *)"Caja de Monedas No Presente");
			}
			fgCajaMonedas = 0;
			intentos_cb = niCajaMonedas = 0;
		}
	}

	if( bill_box.Read_MFUID(bill_box.MIA.MifareUID) == 0)
	{	fgStCB = 1;
		idCajaBilletes = cnvuidhex( bill_box.MIA.MifareUID );
		intentos_bb = niCajaBilletes = 0;
	}
	else
	{
		intentos_bb++;
		if( (intentos_bb > NO_REINTENTOS_CAJAS) || (fgStCB==0) )
		{
			fgStCB = 1;
			// BillBox, Caja NO Presente
			if( bill_box.SetMode() == 0)
				intentos_bb=0;
//			else
//				printf("Error AutoRead CajaBilletes\n");
			niCajaBilletes++;
		}

		if(niCajaBilletes >= NO_INTENTOS_CAJAS)
		{	// Error en Caja de Billetes
			idCajaBilletes=0;
			if( fgCajaBilletes == 1 )
			{
				EventosMert( (char *)"Caja de Billetes No Presente");
			}
			fgCajaBilletes = 0;
			intentos_bb = niCajaBilletes = 0;
		}
	}

}
////////////////////////////////////////////////////////////////////////////////

