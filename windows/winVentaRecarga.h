/* 
 * File:   winVentaRecarga.h
 * Author: famisa
 *
 * Created on January 2, 2023, 12:26 PM
 */

#ifndef WINVENTARECARGA_H
#define WINVENTARECARGA_H


#define PROCESO_RECARGA		0
#define PROCESO_VENTA		1


// ********* variables para el proceso de venta/recarga de tarjetas ************
GtkWidget	*winprocess;	// ventana principal
GtkWidget	*fxProcess;	// contenedor

GtkWidget	*lblDate;	// label de fecha y hora
GtkWidget	*lblDatos;	// label de IP, NM y Version
GtkWidget	*lblMensajes;	// mensaje de alarma

GtkWidget	*lblMsgBill;	// mensaje de billetero
int			fgMsgBill = 0;	// Hide lblMsgBill



// Menu de Venta/Recarga
GtkWidget	*fxBtnII, *lblBtnII;	// Boton Inferior Izquierdo
GtkWidget	*fxBtnSD, *lblBtnSD;	// Boton Superior Derecho;
GtkWidget	*fxBtnID, *lblBtnID;	// Boton Inferior Derecho
GtkWidget	*fxBtnSI, *lblBtnSI;	// Boton Superior Izquierdo;

// Menu Principal
GtkWidget	*fxIniBtnSI, *lblIniBtnSI;	// Inicio Boton Superior Izquierdo
GtkWidget	*fxIniBtnIC, *lblIniBtnIC;	// Inicio Boton Inferior Central



// Posicion de los mensajes de los botones
const int x_Izquierdo = 0;
const int x_Derecho = 524;

const int y_Superior = 86;
const int y_Inferior = 520;

const int x_centro = 460;
const int y_centro = 540;




// ****************   VENTANA SALDO   ***************
GtkWidget	*fxSaldo;	// contenedor
GtkWidget	*entCostoTarjeta;		// Costo de la Tarjeta
GtkWidget	*entSaldoInicial;		// Saldo Inicial
GtkWidget	*entBono;				// Bono Inicial
GtkWidget	*entSuPago;				// Cantidad Depositada
GtkWidget	*entSaldoActual;		// Saldo Actual-> Inicial + SuPago

// Etiquetas Balance
GtkWidget *lblEncabezado;
GtkWidget *lblCosto;
GtkWidget *lblInicial;
GtkWidget *lblSuPago;
GtkWidget *lblActual;
GtkWidget *lblBono;
GtkWidget *lblMIA;

char NoPasoProceso=0;

#define PASO_INICIAL		0
#define PASO_MOV_FIN		1
#define PASO_MOV_REGRESAR	2
#define PASO_IMP_FIN		3
#define PASO_IMP_CONFIRMA	4
#define PASO_FALTANTE		5

void win_show_botones( char win);


#endif /* WINVENTARECARGA_H */

