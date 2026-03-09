/* 
 * File:   bd.h
 * Author: famisa
 *
 * Created on January 10, 2023, 11:58 AM
 */

#ifndef BD_H
#define BD_H

#include <sqlite3.h>

typedef struct
{//	Fecha	DATETIME  DEFAULT CURRENT_TIMESTAMP
	int  NoMov;
	char fecha[30];
	char ID_Name[20];
	int  Linea_TDE;
	int  Estacion_TDE;
	int  Linea_TVT;
	int  Estacion_TVT;
	int  NoDisp;
	int  Status;
	char UID_SAM[20];
	char UID_MIA[10];
	char NoLogico[28];
	int SaldoInicial;
	int Deposito;
	int SaldoFinal;
	int BonoInicial;
	int BonoDepositado;
	int BonoFinal;
	int VentaRecarga;
	int ValorMIA;
	int NoCajaBilletes;
	int NoCajaMonedas;
} DATA_TRANS;

typedef struct
{
	int		NoMov;
	unsigned int	CajaID;
	int		Usuario1;
	int		Usuario2;
	int		Ficha;
	int		Sello;
	int		Status;
	char	FechaCreacion[30];
	char 	FechaCorte[30];
	int		Valor[10];
	int		Total[10];
	int		CantidadTotal;
}DET_BILL_COIN;

typedef struct
{
	int		NoMov;
	char 	Fecha[30];
	int		TipoOperacion;
	int		TotalOperacion;
	int		TotalTarjetas;
	int		NoEmpleado;
	char 	UID_MIA[10];
}DET_STACKERS;

// *****************************************************************************
sqlite3 *dbTransacciones;
DATA_TRANS detTransaccion;
int re_bdTrans;

sqlite3 *dbMonedas;
DET_BILL_COIN detMonedas;
int re_bdCoins;
int lastMovCoins;

sqlite3 *dbBilletes;
DET_BILL_COIN detBilletes;
int re_bdBills;
int lastMovBills;

sqlite3 *dbDispensador;
DET_STACKERS detStackers[2];
int re_bdDispensador;
int lastMovStackers[2];
int lastStacker;

int lastRec;

#define CAJA_MONEDAS		1
#define CAJA_BILLETES		2

#define TOTAL_DENOM_BM		10

#endif /* BD_H */