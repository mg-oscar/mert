/* 
 * File:   procesosMty.h
 * Author: famisa
 *
 * Created on October 24, 2022, 8:52 AM
 */

#ifndef PROCESOSMTY_H
#define PROCESOSMTY_H



#define PROCESO_INICIAL					0
#define PROCESO_VENTA_RECARGA			1

#define PROCESO_LIBERA_TARJETA			10
#define PROCESO_WAIT_RETIRED			11
#define PROCESO_CONDICION_INICIAL		12
#define PROCESO_CONDICION_FINAL			13
#define PROCESO_LIMITES					14
#define PROCESO_MENSAJES				15
#define PROCESO_FALTA					16
#define PROCESO_REVISAR_LIMITES			17
#define PROCESO_ERROR					20

#define PROCESO_MUESTRA_MOVIMIENTOS		9
#define PROCESO_REGRESA_MOVIMIENTOS		18

#define TiempoEntrePreguntas		2000
#define TiempoEntreCivintec			4000
#define TiempoEntreCctalk			2000


#define TiempoDeBarridoCCTALK		150		//100
#define TiempoDeBarrido				200

int TotalFianza;
int TotalReal;					// Deposito Real

int TotalCantidadInicial;		// venta=0		recarga= saldo en tarjeta
int TotalBonosInicial;			// venta=0		recarga= saldo en tarjeta

int TotalCantidadDepositado;	//	
int TotalBonosDepositado;		//

int TotalCantidadFinal;			// TotalCantidadInicial + TotalCantidadDepositado;
int TotalBonosFinal;			// TotalBonosInicial + TotalBonosDepositado


void impDetalle( DET_BILL_COIN detalle, char op);
int wrLastTotal(sqlite3 *db, DET_BILL_COIN detalle);
int posiciona_destrabador(void);
void acumula(int profile, int ivalor);
int acepta_moneda(int perfil, int saldo_actual, int *limite);

#endif /* PROCESOSMTY_H */

