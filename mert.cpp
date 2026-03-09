/*
 * File:   mert.cpp
 * Author: FAMISA
 *
 * Created on October 15, 2022, 11:12 AM
*/
/******************************************************************************/
/* VERSIONES                                                                  */
/*             Version Instalada, EN OPERACION                                */
/* v321 230524 No genera archivo de eventos del billetero ni del monedero     */
/******************************************************************************/
/* v322 230613 Correccion en Mensaje de Saldo Insuficiente y en error en mia  */
/* v325 230613 Cambio en Base de Datos de Monedas y Billetes                  */
/* v326 230905 Uso de 2 tarjetas para abrir la META o 1 de SUPERADMINISTRADOR */
/* v330 230911 Validacion de Perfiles permitidos                              */
/* v331 230913 Arreglo para almacenar los usuarios y modificacion de los      */
/*             de los reportes                                                */
/* v332 230921 Continuan los cambios                                          */
/* v333 231213 Correcion de fecha en historico de movimientos                 */
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
/* v400 240929 Actualizacion de las MERT de DTE                               */
/*             Se agregan lectores ACS y CRT603                               */
/* v401 241009 ACS Agregados, incluir CRT285 y CRT603                         */
/* V402 241010 Se agrega lector CRT603 para la PUERTA                         */
/* v405 241011 Cambios en CCTALK para manejar 2 disp en un mismo serial       */
/* v406 241012 05-se corrige problema de comm con billetero                   */
/*             se habilita el uso de un solo puerto serie para coin/bill      */
/* v407 241021 Proceso de lector de puerta                                    */
/* v410 241022 Se agrega dispensador KYT2664                                  */
/* v411 241025 Se modifica rutina de KYT2664                                  */
/* v412 241026 Se Modifica Rx del dispensador                                 */
/* v413 241028 continuo con el KYT2664                                        */
/* v415 241028 Pruebas                                   de Venta de Tarjetas */
/* v416 241030 Determina total de canles en billetero 16/32                   */
/* v420 241031 Integrar Dispensador y Lector en una sola clase                */
/* v421 241112 Se qgrega lector ACR1252 en Puerta                             */
/* v425 241114 Se integra maquina completa de TDE, se agrega class ACR1252    */
/* v426 241119 Revision Stackers                                              */
/* v427 241121 Se continua proceso de Venta                                   */
/* v428 241124 modificacion indices contexto de ACS1222 y QCS1252             */
/*                                                                            */
/* v430 241211 proceso de venta                                               */
/* v431 241214 Se agrega delay en GetCard                                     */
/* v435 241221 Delay y Timeout en GetCard                                     */
/* v436 2412223 Se agregan msg de error en venta de tarjetas                  */
/* v437 241220 Revision de impresio                                           */
/*                                                                            */
/******************************************************************************/
/* v440 250102 Cambio de Resolucion de monitor al EC Line                     */
/* v441 250102 Relocalizacion de msg/entradas en cada ventana de mantto       */
/* v442 250104 Acomodo Mensajes MERT DTE                                      */
/* v443 250108 Lector de Puerta ACR y acomodo de mas mensajes DTE             */
/* v444 250110 Acomodo ventanas de mantto                                     */
/* v445 250113 Archivos separados de configuracion                            */
/* v446 250113 Mantto Dispensador                                             */
/* v447 250115 Correccion en LimiteExcedido de queda con el billete           */
/* v448 250116 DTE-> Modo Recarga, solo 2 lectores ACR1222                    */
/* v449 250117 Error en Venta, se quedo comentada una linea                   */
/* v450 250117 Mantto Billetero                                               */
/* v451 250118 Correccion no deshabilta el billetero al finalizar transaccion */
/* v452 250118 TouchScreen ECLINE                                             */
/* v453 250120 Cambios en Mantto Billetero                                    */
/* v454 250121 Mantto Dispensador                                             */
/* v455 250123 Pruebas de error en billetero                                  */
/* v456 250129 Modificacion al rechazar un billete del stacker                */
/* v457 250131 Se agrega ventana de "Billetero Fuera de Servicio"             */
/* v458 250131 Reacomodo de Msg en ventana de saldo                           */
/* v459 250201 Correccion al terminar de desatorar el billetero               */
/* v460 250204 Se envia un reset al billetero al recibir:                     */
/*             evento=6 Bill jammed in transport ( unsafe mode ) Fatal Error  */
/*             evento=15 / Stacker jammed Fatal Error                         */
/* v461 250205 Implementa Reset-Billetero cuando se reciba un ERROR FATAL     */
/* v462 250205 Correccion ene vento de billete $20.00 de los azules (viejo)   */
/* v463 250206 Correccion en mensaje de "solo monedas" no se quita            */
/* v464 250206 Se continua revisando los eventos del billetero                */
/*                                                                            */
/******************************************************************************/
const char VERSION[]={"v464"};
//
// System Includes
#include <cstdlib>
#include <stdio.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <signal.h>
#include <string>
#include <memory.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "libusb-1.0/libusb.h"

#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>


#define WRITE_EVENTS		1

// datos generales
#include "mert.h"
#include "mensajes.h"

#include "xmls.h"

unsigned int milisegundos(void);

// Rutinas varias
#include "rutinas.cpp"

// Puerto Serial -> Open, Close, Read y Write
#include "serialport.h"
#include "serialport.cpp"

// Puerto Serial -> Open, Close, Read y Write
#include "readers/rs232.h"	 
#include "readers/rs232.cpp"

#include "readers/rs232_603.hpp"	 
#include "readers/rs232_603.cpp"

// USB -> Open, Close, Read y Write
#include "readers/usbdev.h"	 
#include "readers/usbdev.cpp"

// Protocolo Modbus
#include "modbusclass.h"
#include "modbusclass.cpp"

// Modulo de E/S
#include "moduloio.h"		
#include "moduloio.cpp"

// Base de Datos: Transacciones,
#include "bd.h"
#include "bd.cpp"

// Impresora
#include "printer.h"		
#include "printer.cpp"

// Triple DEA
#include "SAM.h"
#include "tripleDEA.h"
#include "tripleDEA.cpp"

// Manejo de los Datos de la MIA
#include "bitmap.h"
#include "libMIA.hpp"
#include "libMIA.cpp"

// Manejo de Tokens de Cajas
#include "libBOX.h"
#include "libBOX.cpp"

//****************************READERS ******************************************
// Reader CRT-285 de CHINA CREATOR
#include "readers/crt_288x_ur.h"	// libreria CRT-285
#include "readers/crt_288x_ur.cpp"
#include "readers/class_crt285.h"
#include "readers/class_crt285.cpp"

// Reader CRT-603vc de CHINA CREATOR
#include "readers/class_crt603vx.hpp"
#include "readers/class_crt603vx.cpp"


// Reader CIVINTEC
#include "readers/class_CV_CN670.h"
#include "readers/class_CV_CN670.cpp"
#include "readers/class_civintec.h"
#include "readers/class_civintec.cpp"

// Reader Kytronics CIM6903 y Dispensador
#include "readers/class_cim6903.h"
#include "readers/class_cim6903.cpp"

// Reader ACR1222L -> Advanced Card System
#include "readers/class_acr1222l.h"
#include "readers/class_acr1222l.cpp"

#include "kytv26xx.h"
#include "kytv26xx.cpp"

int proceso_venta(long tcarga, long tbonos, long tfianza);



LONG SearchAllACSReaders(void);
void CLOSE_ACS(void);
LONG ReadAllMIA(int tkey);
LONG WriteSector(BYTE nsec, int tkey);
LONG PCSC_ERROR(LONG rv, char *text);

// Protocolo CCTALK
#include "cctalk.h"
#include "cctalk.cpp"

// Seleccion entre los diferentes lectores
#include "readers.h"
#include "readers.cpp"		// seleccion entre los diferentes lectores

#include "xmls.h"
#include "xmls.cpp"

moduloio moduloES;		// Modulos E/S Modbus
cctalk coins;			// monedero
cctalk bills;			// billetero

readers recarga;		// lector recarga
readers venta;			// lector venta

readers coin_box;		// Caja de Monedas
readers bill_box;		// Caja de Billetes
readers puerta;			// Puerta

//CLASS_KYT26xx dispensador;

// windows
#include "windows/winmain.h"
#include "windows/winmain.cpp"

// Carga la Configuracion de la MERT
#include "configuracion.h"
#include "configuracion.cpp"

// Proceso para venta y recarga
#include "procesosMty.h"
#include "perifericos.cpp"
#include "procesosMty.cpp"

#include "aceptator.cpp"
#include "modEntradas.cpp"


// Hilo -> Barrido del Modulo de E/S
pthread_t modulos_es;	// Comunicacion con Modulo de E/S
pthread_t mod_billcoin;	// Comunicacion Billetero y Monedero

int	Billetero_Init(void);

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int main(int argc, char** argv)
{	int re_conf;	// Puertos, Lectores, SAM

	argumentos(argc, argv);	// parametros de operacion
	libusb_init(&context);

	EventosMert((char *)"Inicio de la APP");

	mert_operacion.modo			= MERT_MODO_RECARGA;
	mert_operacion.proceso		= 0;		// espera proceso venta/recarga
	mert_operacion.fgIdioma		= SPANISH;	// Español
	mert_operacion.password		= 0;

	mert_operacion.fgVENTA		= 0;	// No Vende Tarjetas
	mert_operacion.fgRECARGA	= 0;	// Si Recarga		

	strncpy(mert_operacion.path,(char *)"/home/user/mert/",50);

	ReadConfigXML();
	creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_INICIA, 0, 0);

	iniPathLed();

	mert_operacion.fgBILLETERO	= Config.Billetero;		// Billetero
	mert_operacion.fgMONEDERO	= Config.Monedero;		// Monedero
	mert_operacion.fgVENTA		= Config.Dispensador;	// Dispensador

	LoadBaseDatos();

	ldXMLs();

	// muestra la ventana principal de la MERT
	wininicial( argc, argv);

	// carga la configuracion de la MERT
	re_conf = revPuertos();
	
	// habilita comm con el Modulo de E/S
	memset(stModIO, 0x00, sizeof(st_io));

	stModIO->op = 0;				// no barre al modulo
	stModIO->opVentaRecarga = 0;	// No empieza venta/recarga

	stModIO->NoControl   = 0;		// No
	stModIO->status_comm = 0x01;
	stModIO->Baud        = MertPort.MODIO_BAUD;	// BAUD
	strcpy( stModIO->CommName, MertPort.MODIO_COMM );

	stModIO->startCB = 0;
	stModIO->startCajas = 0x00;
	stModIO->op = 1;	// inicia barrido del modulo de E/S

	pthread_create(&modulos_es,   NULL, mod_EntSal,    (void *)stModIO);
	pthread_create(&mod_billcoin, NULL, mod_Aceptator, (void *)stModIO);

	mert_operacion.no_menu = MODO_ARCHIVOS;
	mert_operacion.no_rec_venta = MODO_ARCHIVOS;

	obtieneIP_MASK((char *)"enp2s0", mert_operacion.ip, mert_operacion.nm);

	mnu_opcion = PROCESO_INICIAL;

	// ciclo principal
	while(1)
	{
		proc_administrativo(re_conf);

		proceso_venta_recarga();

		while (gtk_events_pending ())
			gtk_main_iteration_do (TRUE);
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void proc_administrativo(int re_conf)
{	int i;
	static unsigned int uid_old=0, tmo, tmp=0;
//	static unsigned char perfil=0;
	unsigned int uid_new;
	char msg[200];

	gtk_seconds_main();

	switch(mert_operacion.no_menu)
	{
		case MODO_ARCHIVOS:	// verifica que la configuracion este completa
				if(re_conf != 0)
					re_conf = revPuertos();

				if( (re_conf==0) )
				{	// Se cargo el archivo de puertos
					// habilita comm con el Modulo de E/S
					stModIO->status_comm = 0x01;
					stModIO->Baud        = MertPort.MODIO_BAUD;	// BAUD
					strcpy( stModIO->CommName, MertPort.MODIO_COMM );
					stModIO->op = 1;	// inicia barrido del modulo de E/S

					winMsg(msgMain, rgb_lime_green, rgb_black, 
								(char *)"INICIALIZANDO\n\nPERIFERICOS", "SANS", 700, 65);
					// Establece comunicacion con los diferentes perifericos
					mert_operacion.no_menu = MODO_PERIFERICOS;
				}
				break;

		case MODO_PERIFERICOS:		// Establece comunicacon con todos los perifericos
				if ( perifericosOK() == 0)
				{	// Todos los perifericos OK
					winMsg(msgMain, rgb_lime_green, rgb_black,
								(char *)"IDENTIFICANDO\nCAJAS\nDE\nVALORES\n[Monedas][Billetes]", "SANS", 700, 60);
					mert_operacion.no_menu = MODO_READ_CAJAS;

					stModIO->startCB = 1;		// Inicia Barrido del Billetero
					stModIO->startCajas = 0x01;	// inicia lectura de Cajas de Valores
				}
				break;

		case MODO_READ_CAJAS:
				if( readIdCajas() == 0)
				{	fgCajaMonedas=1;
					// compara numero de cajas, en BD y fisico
					if(idCajaMonedas==detMonedas.CajaID)
					{
						printf("****************Caja Monedas OK**************\n");
					}
					else
					{
						// Revisar procedimiento
						printf("ERROR Caja Monedas\n");
						EventBox((char *)" MONEDAS", detMonedas.CajaID, idCajaMonedas);
						creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_BOX_COIN_DIF, detMonedas.CajaID, idCajaMonedas);
					}

					fgCajaBilletes = 1;
					if(idCajaBilletes==detBilletes.CajaID)
					{
						printf("****************Caja Billetes OK*************\n");
					}
					else
					{
						// Revisar procedimiento
						printf("ERROR Caja Billetes\n");
						EventBox((char *)"BILLETES", detBilletes.CajaID, idCajaBilletes);
						creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_BOX_BILL_DIF, detBilletes.CajaID, idCajaBilletes);
					}

					// validar en BD numeros de ID de las cajas de Monedas y Billetes
					if(detMonedas.CajaID==0)
					{	// actualizar ID en BD
						printf("ID Caja Monedas =%u -> %u\n", detMonedas.CajaID, idCajaMonedas);
						detMonedas.CajaID = idCajaMonedas;
						UpCajaIDM();
					}
					if(detBilletes.CajaID==0)
					{	// actualizar ID en BD
						printf("ID Caja Billetes =%u -> %u\n", detBilletes.CajaID, idCajaBilletes);
						detBilletes.CajaID = idCajaBilletes;
						UpCajaIDB();
					}

					RevisaPermisoVenta();

					// habilita proceso de venta/recarga
					stModIO->opVentaRecarga = MODO_OPERATIVO;
					gtk_widget_hide(msgMain);
					mert_operacion.no_menu = MODO_TARJETA;
					printf("Modo Tarjeta\n");
					
					// inicializa datos de usuarios
					memset( &data_user,  0, sizeof(data_user) );

					if( puerta.SetMode() == 0)
					{
						
					}
				}
				break;

		case MODO_TARJETA:	// ESPERA A QUE SE LEA UNA TARJETA 

				if(moduloES.Entradas[SENSOR_PUERTA] != moduloES.EntradasOld[SENSOR_PUERTA] )
				{	if( moduloES.BotonOprimido(SENSOR_PUERTA) == PUERTA_ABIERTA)
					{
						tmo = milisegundos();	// inicia timer para aceptar MIA
						tmp=1;	// timeout habilitado

						printf("Puerta Abierta\n");
						stModIO->StatusMantto = 0x01;
						EventosMert((char *)"Puerta Abierta");
						creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_PUERTA_ABIERTA, 0, 0);
					}
				}

				if(tmp==1)
				{	if( (milisegundos() - tmo) >  TM_PASSWORD)
					{	// tiene 30 segundos para capturar el password
						mert_operacion.no_menu = MODO_TARJETA;
						Alarma(1);	// ALARMA
						tmp = 2;	// alarma activada
						creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_ALARMADA, 1, 0);
					}
				}

				if( puerta.Read_MFUID(puerta.MIA.MifareUID) == 0)
				{
					uid_new = cnvuidhex( puerta.MIA.MifareUID );

					if(uid_new==uid_old)
						break;

					printf("Puerta, Tarjeta Presente\n");
					printf("No.=[");
					for(i=0; i<4; i++)
						printf("%02X,", puerta.MIA.MifareUID[i]);
					printf("]\n");

					for(i=0; i<6; i++)
						puerta.MIA.KeyA[1][i] = puerta.MIA.MifareUID[i];

					uid_old = uid_new;
					if( puerta_process() == 0x00)
					{
						// valida el perfil, en este nivel solo se permite el
						// Perfil=5 SuperAdministrador	Perfil=3 Jefe de Estacion
						if( (card_puerta.noperfil == PERFIL_SUPERADMIN) ||
							(card_puerta.noperfil == PERFIL_JEFE_ESTACION) )
						{
							printf("admin\n");
							move_data_door(NOUSER_1);
							// MIA OK, pide password
							if(tmp!=2)
								puerta.Buzzer(2);
							tmo = milisegundos();
							mert_operacion.no_menu = MODO_PUERTA_ABIERTA;
						}
						else
						{	// rechazar tarjeta
							if(tmp!=2)
								puerta.Buzzer(3);
							
							if(puerta.SetMode() != 0)
							{
								printf("Error AutoRead PUERTA\n");
							}
						}
					}
					else
					{	// rechazar tarjeta
						if(tmp!=2)
							puerta.Buzzer(3);

						if( puerta.SetMode() != 0)
						{
							printf("Error AutoRead PUERTA\n");
						}
					}
				}
				else
				{	//printf("TARJETA NO PRESENTE\n");
					uid_old = 0;
				}

				break;

		case MODO_PUERTA_ABIERTA:
				if( moduloES.Entradas[SENSOR_PUERTA] == PUERTA_ABIERTA)
//				if( moduloES.Entradas[SENSOR_PUERTA] != PUERTA_ABIERTA)
				{	// Puerta Abierta

					printf("Puerta Abierta\n");
					stModIO->StatusMantto = 0x01;
					EventosMert((char *)"Puerta Abierta");
					creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_PUERTA_ABIERTA, 0, 0);

//					winPasswd(Usuarios.pwd[NoUsuario[NOUSER_1]]);
					winPasswd(data_user[NOUSER_1].pass);
					
					gtk_widget_hide(msgMain);
					tmo = milisegundos();
					tmp = 0;
					mert_operacion.no_menu = MODO_PASSWORD;

					printf("sh touch\n");
					
					if(mert_famisa)
						system("sh /home/user/mert/bin/touch.sh");
					else
						system("sh /home/user/mert/bin/touch_dte.sh");
				}
				else
				{	// Puerta Cerrada
					if( (milisegundos() - tmo) >  TM_PUERTA)
					{	// tiene 10 segundos para abrir la puerta, termina operacion
						// si no la abre						
						data_user[NOUSER_1].noempleado = 0;
						mert_operacion.no_menu = MODO_TARJETA;
						puerta.Buzzer(4);
						if( puerta.SetMode() != 0)
						{
							printf("Error AutoRead PUERTA\n");
						}
					}
				}
				break;

		case MODO_PASSWORD:
				if( (milisegundos() - tmo) >  TM_PASSWORD)
				{	// tiene 30 segundos para capturar el password
					tmo = milisegundos();
					Alarma(1);	// alarma
					creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_ALARMADA, 2, 0);
				}

				if(mert_operacion.password == TRUE)
				{
					// se recibio el password correctamente, muestra el sig menu
					// de acuerdo a los previlegios del usuario

					printf("******PASS ok***************\n");
					gtk_widget_destroy(GTK_WIDGET (fxPass) );

					creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_PASSWORD, 
								   data_user[NOUSER_1].nouid, 0);

					Alarma(0);	// apaga la alarma

					mert_operacion.password = FALSE;
					// antes de mostrar el menu revisar si no esta en un proceso
					// de venta/recarga o en Caja Llena

//					if( NoPerfil[NOUSER_1] == PERFIL_SUPERADMIN)
					if( data_user[NOUSER_1].noperfil == PERFIL_SUPERADMIN)
					{
						move_data_door(NOUSER_2);

						// mostrar menu de ESPERA FIN DE TRANSACCION
						winMsg(msgMain, rgb_lime_green, rgb_black,
									(char *)"ESPERA\nFIN DE \nTRANSACCION", "SANS", 700, 60);

						if( (stModIO->StatusProceso==0) || (stModIO->StatusProceso==MODO_CAJA_LLENA) )
						{
							winMsgMantenimiento();					
						}

						mert_operacion.no_menu = MODO_ESPERA_FIN;

						sprintf(msg, "Puerta Abierta, %d, %s", data_user[NOUSER_1].noempleado,
															   data_user[NOUSER_1].name);
						EventosMert(msg);
					}
					else
					{
						// pedir 2da tarjeta
						//#define MODO_SDA_TARJETA			109
						//#define MODO_SDO_PASSWORD			110
						winMsg(msgMain, rgb_lime_green, rgb_black,
								(char *)"PASE\nSEGUNDA \nTARJETA", "SANS", 700, 60);

						printf("Configura AutoRead PUERTA 2da Tarjeta\n");
						if( puerta.SetMode() != 0)
						{
							printf("Error AutoRead PUERTA\n");
						}
						tmo = milisegundos();
						mert_operacion.no_menu = MODO_SDA_TARJETA;
					}
				}
				break;

		case MODO_SDA_TARJETA:
				if( (milisegundos() - tmo) >  TM_PASSWORD)
				{	// tiene 30 segundos para capturar el password
					tmo = milisegundos();
					Alarma(1);	// alarma
					creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_ALARMADA, 2, 0);
				}

				if( puerta.Read_MFUID(puerta.MIA.MifareUID) == 0)
				{
					uid_new = cnvuidhex( puerta.MIA.MifareUID );

					if(uid_new==uid_old)
						break;

					printf("Puerta, Tarjeta Presente\n");
					printf("No.=[");
					for(i=0; i<4; i++)
						printf("%02X,", puerta.MIA.MifareUID[i]);
					printf("]\n");

					uid_old = uid_new;

					if( puerta_process() == 0x00)
					{
						// valida el perfil, en este nivel solo se permite el
						// Perfil=2 PERFIL_SUPERVISOR
						// Perfil=6 PERFIL_MANTENIMIENTO
						if( (card_puerta.noperfil == PERFIL_SUPERVISOR) ||
							(card_puerta.noperfil == PERFIL_MANTENIMIENTO) )
						{
							move_data_door(NOUSER_2);
							printf("PERFIL VALIDO\n");
							gtk_widget_hide(msgMain);
							// MIA OK, pide password
							if(tmp!=2)
								puerta.Buzzer(2);
							tmo = milisegundos();

							winPasswd(data_user[NOUSER_2].pass);							
							gtk_widget_hide(msgMain);
		
							mert_operacion.no_menu = MODO_SDO_PASSWORD;							
						}
						else
						{	// rechazar tarjeta
							if(tmp!=2)
								puerta.Buzzer(3);
							
							if(puerta.SetMode() != 0)
							{
								printf("Error AutoRead PUERTA\n");
							}
						}	
					}
					else
					{	// rechazar tarjeta
						if(tmp!=2)
							puerta.Buzzer(3);

						if( puerta.SetMode() != 0)
						{
							printf("Error AutoRead PUERTA\n");
						}
					}
				}
				break;

		case MODO_SDO_PASSWORD:
				if( (milisegundos() - tmo) >  TM_PASSWORD)
				{	// tiene 30 segundos para capturar el password
					tmo = milisegundos();
					Alarma(1);	// alarma
					creaXmlEventos(data_user[NOUSER_1].noempleado, EVT_ALARMADA, 2, 0);
				}

				if(mert_operacion.password == TRUE)
				{	
					// se recibio el password correctamente, muestra el sig menu
					// de acuerdo a los previlegios del usuario

					printf("******2do. PASS ok***************\n");
					gtk_widget_destroy(GTK_WIDGET (fxPass) );

					creaXmlEventos(data_user[NOUSER_2].noempleado, EVT_PASSWORD, 
								   data_user[NOUSER_2].nouid, 0);

					Alarma(0);	// apaga la alarma

					mert_operacion.password = FALSE;
					// antes de mostrar el menu revisar si no esta en un proceso
					// de venta/recarga o en Caja Llena

					// mostrar menu de ESPERA FIN DE TRANSACCION
					winMsg(msgMain, rgb_lime_green, rgb_black,
							(char *)"ESPERA\nFIN DE \nTRANSACCION", "SANS", 700, 60);

					if( (stModIO->StatusProceso==0) || (stModIO->StatusProceso==MODO_CAJA_LLENA) )
					{
						winMsgMantenimiento();					
					}

					mert_operacion.no_menu = MODO_ESPERA_FIN;

					sprintf(msg, "Puerta Abierta, %d, %s", data_user[NOUSER_2].noempleado, 
														   data_user[NOUSER_2].name);
//						Usuarios.name[NoUsuario[NOUSER_1]]);
					EventosMert(msg);
				}
			break;

		case MODO_ESPERA_FIN:
				if( (stModIO->StatusProceso==0) || (stModIO->StatusProceso==MODO_CAJA_LLENA) )
				{
					winMsgMantenimiento();
					// el siguiente menu depende del usuario
//					gtk_nextmenu(Usuarios.profile[NoUsuario[NOUSER_1]],0);
					gtk_nextmenu(data_user[NOUSER_2].noperfil,0);
					gtk_widget_hide(msgMain);
					mert_operacion.no_menu = MODO_MENU;
					stModIO->StatusMantto = 0x01;
					uid_old=0;
					tmo = milisegundos();
				}
				break;

		case MODO_MENU:
				if( moduloES.Entradas[SENSOR_PUERTA] == PUERTA_CERRADA)
//				if( moduloES.Entradas[SENSOR_PUERTA] != PUERTA_CERRADA)
				{	// Puerta Cerrada

					if( fgMenu == 0x00)
					{	// sale de Mantto
						win_menu = WIN_MENU1_EXIT;
						exeMenu();		// ejecuta Menu segun perfil
						tmo = milisegundos();
						mert_operacion.no_menu = MODO_CIERRA_PUERTA;
						break;
					}
					else
					{
						if( (milisegundos() - tmo) >  TM_TERMINA)
						{
							// se deben terminar los procesos antes de cerrar la puerta
							tmo = milisegundos();
							puerta.Buzzer(5);
						}
					}
				}
				exeMenu();		// ejecuta opciones Menu 1
				break;

		case MENU_REGRESO:
				// Aqui regresa del cualquier menu, y en base al usuario se le
				// muestra el menu de su perfil
//				gtk_nextmenu(Usuarios.profile[NoUsuario[NOUSER_1]], 1);
				gtk_nextmenu(data_user[NOUSER_2].noperfil,1);
//				gtk_widget_hide(msgMain);

				mert_operacion.no_menu = MODO_MENU;
				win_menu = 100;
				fgMenu = 0x00;	// Menu Principal
				break;

		case MODO_CIERRA_PUERTA:
				if( (milisegundos() - tmo) >  TM_CERRAR)
				{
					printf("Configura AutoRead PUERTA\n");
					if( puerta.SetMode() != 0)
					{
						printf("Error AutoRead PUERTA\n");
					}

					// termina sesion de mantto, borra no empleado

					RevisaPermisoVenta();

					if(BilleteroON==0)
					{
						sprintf(msg, "Habilita Billetero");
						EventosMert(msg);
					}

					BilleteroON=1;	// Habilita billetero

					sprintf(msg, "Puerta Cerrada, %d, %s", data_user[NOUSER_2].noempleado, 
														   data_user[NOUSER_2].name);
					EventosMert(msg);

					creaXmlEventos(data_user[NOUSER_2].noempleado, EVT_PUERTA_CERRADA, 
								   data_user[NOUSER_2].nouid, 0);

					data_user[NOUSER_2].noempleado = 0;

					// permite entrar Mantenimiento o Venta/Recarga
					stModIO->StatusMantto = 0x00;
					mert_operacion.no_menu= MODO_TARJETA;

					// inicializa datos de usuarios
					memset( &data_user,  0, sizeof(data_user) );

					if( mnu_opcion == PROCESO_LIMITES)
							mnu_opcion = PROCESO_REVISAR_LIMITES;
				}
				break;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void proceso_venta_recarga( void )
{	int re;

	gtk_seconds();

	switch(mert_operacion.no_rec_venta )
	{
		case MODO_INICIAL:
				printf("Proceso Venta/Recarga\n");
				stModIO->OnOFF     = R_CLOSE;
				stModIO->NoControl = ILUMINA_LECTOR_TARJETA;
				while(stModIO->NoControl !=0 );

				winButtonMainHide();
				mert_operacion.no_rec_venta = MODO_ARCHIVOS;		// muestra la ventana principal
				break;

		case MODO_ARCHIVOS:
				if(stModIO->opVentaRecarga != 0)
				{
					gtk_hide(lblMensajes);
					NoPasoProceso = PASO_INICIAL;
					win_show_botones(WIN_INICIO);
					recarga.LedOnOff(CRT_LED_BLUE, CRT_LED_FLASHING);
					mert_operacion.no_rec_venta = MODO_DECIDE;

					stModIO->OnOFF     = R_CLOSE;
					stModIO->NoControl = ILUMINA_LECTOR_TARJETA;
					while(stModIO->NoControl !=0 );
				}
				break;

		case MODO_DECIDE:	// Espera->Boton Venta, Boton Lenguaje, Tarjeta Presente
				if( (fgShowCoin==1) && (bills.num_intentos==0) )
				{
					printf("\n\n**** ENVIA UN RESET AL BILLETERO *******\n\n");

					while(stModIO->cmdBill !=0 );
					stModIO->cmdBill = CMD_BILL_RESET;
					winMsgReset();
					bills.num_intentos=1;
				}

				if( fgShowCoin==1 )
				{
					gtk_widget_show(lblMsgBill);
					fgMsgBill = 1;	//show
				}
				else
				{
					gtk_widget_hide(lblMsgBill);
					fgMsgBill = 0;	//hide
				}

				if( (fgCajaMonedas==0) || (fgCajaBilletes==0) )
				{
					// Se retiro alguna caja de recaudacion
					printf("RETIRO CAJA DE RECAUDACION\n");
					Alarma(1);	// Alarma ON
					// MSG Equipo Alarmado

					stModIO->StatusMantto = 0x01;
					winMsgCajas();
					mert_operacion.proceso = PROC_NONE;
					mert_operacion.no_rec_venta = MODO_MANTTO;	
					break;
				}

				if(	stModIO->StatusMantto != 0)	// Proceso de Venta/Recarga finalizado
				{	// inicio un proceso administrativo donde no puede vender/recargar
					winMsgMantenimiento();
					if( recarga.UNLOCK(LOCK_ON)!=0)	// bloquea la tarjeta
					{	// error al bloquear la tarjeta
						printf("Error al bloquear CRT\n");
					}

					stModIO->OnOFF     = R_OPEN;
					stModIO->NoControl = ILUMINA_LECTOR_TARJETA;
					while(stModIO->NoControl !=0 );

					recarga.LedOnOff(CRT_LED_RED, CRT_LED_ON);
					mert_operacion.no_rec_venta = MODO_MANTTO;
					break;
				}

				re = recarga.CardPresent();
				if(re==CRT_CARDST_UNKNOW)
				{
					printf("ERROR CRT=%d\n", re);
					winMsgOut();
					fgCRT285 = 0;	// error del CRT285
					mert_operacion.no_rec_venta = MODO_ERROR_CRT;
					break;
				}
				if( re==0)
				{	moduloES.fgLatch = 0;
					// recarga
					printf("Tarjeta Presente----->\n");
					winMsgWaitMomment(MSG_INI_RECARGA);
					winButtonMainHide();
					recarga.LedOnOff(CRT_LED_RED, CRT_LED_ON);
					mert_operacion.no_rec_venta = MODO_RECARGA;
					stModIO->StatusProceso = 0x01;	// Proceso de Venta/Recarga iniciado
					mert_operacion.proceso = PROC_RECARGA;
				}
				else if( moduloES.BotonOprimido(BOTON_SUPERIOR_DERECHO) == BOTON_OPRIMIDO_SI)
				{
					// Boton Superior Derecho-> VENTA
					if( (mert_identidad.TIPO == MERT_EXPENDEDORA) &&
						(mert_operacion.fgVENTA != 0) )
					{
						printf("**********VENTA************\n");
						mert_operacion.proceso = PROC_VENTA;
						mert_operacion.no_rec_venta = MODO_RECARGA;
						winButtonMainHide();
						winMsgWaitMomment(MSG_INI_VENTA);
						stModIO->StatusProceso = 0x01;	// Proceso de Venta/Recarga iniciado
					}					
				}
				else if( moduloES.BotonOprimido(BOTON_SUPERIOR_IZQUIERDO) == BOTON_OPRIMIDO_SI)
				{
					// Cambio de Idioma
					winCambioIdioma(mert_operacion.fgIdioma);
				}
				else if( moduloES.Entradas[BOTON_INFERIOR_IZQUIERDO] == BOTON_OPRIMIDO_SI)
				{
					if( moduloES.fgLatch==0)
					{	moduloES.fgLatch=1;
						moduloES.tmBtnLatch = milisegundos();
					}
					if( (milisegundos() - moduloES.tmBtnLatch) > 3000)
					{
						printf("3000 mseg cmd libera tarjeta\n");
						// envia liberar la tarjeta
						rep_alarma(1);
						if( recarga.UNLOCK(LOCK_OFF)!=0)	// libera la tarjeta
						{	// error al bloquear la tarjeta
							printf("Error al bloquear CRT\n");
						}
						moduloES.fgLatch=0;
					}
				}
				else
					moduloES.fgLatch = 0;
				break;

		case MODO_RECARGA:	// Recarga
				if( recarga_process() == 0x01)
				{
					if( (fgCajaMonedas==0) || (fgCajaBilletes==0) )
					{
						Alarma(1);	// Alarma ON
						// MSG Equipo en Mantenimiento
						gtk_widget_hide(msgMain);
						stModIO->StatusMantto = 0x01;
						winMsgMantenimiento();
						mert_operacion.no_rec_venta = MODO_DECIDE;
						mert_operacion.proceso = PROC_NONE;
					}
					else
					{
						// fin de la recarga/venta
						recarga.LedOnOff(CRT_LED_BLUE, CRT_LED_FLASHING);
						// Revisar si conviene mas Desbloquear la Recarga aqui
						stModIO->OnOFF     = R_CLOSE;
						stModIO->NoControl = ILUMINA_LECTOR_TARJETA;
						while(stModIO->NoControl !=0 );
						mert_operacion.no_rec_venta = MODO_DECIDE;
						mert_operacion.proceso = PROC_NONE;
					}
				}
				break;

		case MODO_MANTTO:
				if(	stModIO->StatusMantto == 0)	// Proceso de Venta/Recarga finalizado
				{
					// inicio un proceso administrativo donde no puede vender/recargar
					winMsgMantenimiento();
					if( recarga.UNLOCK(LOCK_OFF)!=0)	// desbloquea la tarjeta
					{	// error al desbloquear la tarjeta
						printf("Error al desbloquear CRT\n");
					}
					mert_operacion.no_rec_venta = MODO_ARCHIVOS;
				}
				break;
				
		case MODO_ERROR_CRT:
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
				else
				{
					// Sale del Error
					mert_operacion.no_rec_venta = MODO_ARCHIVOS;
				}
			break;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void Alarma(char status)
{
	if(TipoAlarma)
	{
		printf("Alarma=%d\n", status);
		// ALARMA SONORA DE LA MERT
		stModIO->OnOFF = status;
		stModIO->NoControl = ALARMA_AUDIBLE_VISIBLE;
		while(stModIO->NoControl !=0 );
	}
	else
	{
		// BUZZER DEL LECTOR DE PUERTA
		puerta.Buzzer(status);	// ALARMA
	}
}
////////////////////////////////////////////////////////////////////////////////