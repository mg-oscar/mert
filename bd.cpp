/******************************************************************************/
/*   RUTINAS PARA EL MANEJO DE LAS DIFERENTES BASES DE DATOS                  */
/*                 OPEN, CLOSE, CREACION, READ, WRITE                         */
/******************************************************************************/
//
#include "bd.h"
#include "mert.h"

/******************************************************************************/
/*             Abre o Crea la base de Datos de Transacciones                  */
/******************************************************************************/
int bd_OpenDispensador(char *dbname, sqlite3 **db )
{	int rc;
	char *err_msg = 0;
	char *sql;

	char path_bd[100];
	sprintf(path_bd, "%sbd/%s",mert_operacion.path, dbname);

	rc = sqlite3_open(path_bd, db);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return 0x01;
	}

	sql = (char *)"CREATE TABLE IF NOT EXISTS stacker1"
		"( NoMov INTEGER NOT NULL UNIQUE PRIMARY KEY AUTOINCREMENT, "
		"Fecha	DATETIME DEFAULT  (datetime('now','localtime')), "
		"TipoOperacion  INTEGER NOT NULL, "
		"TotalOperacion	INTEGER NOT NULL, "
		"TotalTarjetas	INTEGER NOT NULL, "
		"NoEmpleado	    INTEGER NOT NULL, "
		"UID_MIA VARCHAR(10) NOT NULL );";

	rc = sqlite3_exec(*db, sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK)
	{
		// error al open/create table
		fprintf(stderr, "Dispensador, Cannot open/create table: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return 0x01;
	}

	sql = (char *)"CREATE TABLE IF NOT EXISTS stacker2"
		"( NoMov INTEGER NOT NULL UNIQUE PRIMARY KEY AUTOINCREMENT, "
		"Fecha	DATETIME DEFAULT  (datetime('now','localtime')), "
		"TipoOperacion  INTEGER NOT NULL, "
		"TotalOperacion	INTEGER NOT NULL, "
		"TotalTarjetas	INTEGER NOT NULL, "
		"NoEmpleado	    INTEGER NOT NULL, "
		"UID_MIA VARCHAR(10) NOT NULL );";

	rc = sqlite3_exec(*db, sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK)
	{
		// error al open/create table
		fprintf(stderr, "Dispensador, Cannot open/create table: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return 0x01;
	}

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int lastBDDispensador(void *ns, int argc, char **argv, char **azColName)
{	int NoStk;

	NoStk = *((int *)ns);

	for (int i = 0; i < argc; i++)
	{	//printf("i=%d\n", i);
		switch(i)
		{	case 0:
				detStackers[NoStk].NoMov = atoi(argv[i]);
				lastRec = detStackers[NoStk].NoMov;
//				printf("Movimiento=%d=%d\n", NoStk, detStackers[NoStk].NoMov);
				break;
			case 1:
				strcpy(detStackers[NoStk].Fecha, argv[i]);
				break;
			case 2:
				detStackers[NoStk].TipoOperacion = atoi(argv[i]);
				break;
			case 3:
				detStackers[NoStk].TotalOperacion = atoi(argv[i]);
				break;
			case 4:
				detStackers[NoStk].TotalTarjetas = atoi(argv[i]);
				break;
			case 5:
				detStackers[NoStk].NoEmpleado = atoi(argv[i]);
				break;
			case 6:
				strcpy(detStackers[NoStk].UID_MIA, argv[i]);
				break;
		}
	}
	return 0;
}

/******************************************************************************/
/* Lee el ultimo registro de la base de datos                                 */
/******************************************************************************/
int rdLastDispensador(sqlite3 *db, int NoStk)
{	int rc, re;
	char *err_msg = 0;
	char *sql;
	
	if(NoStk==0)
		sql = (char *)"SELECT * FROM stacker1 WHERE NoMov = "
										"(SELECT MAX(NoMov) FROM stacker1) LIMIT 1";
	else
		sql = (char *)"SELECT * FROM stacker2 WHERE NoMov = "
										"(SELECT MAX(NoMov) FROM stacker2) LIMIT 1";

	lastRec = -1;
	re = 1;
	rc = sqlite3_exec(db, sql, lastBDDispensador, &NoStk, &err_msg);
	if (rc != SQLITE_OK )
	{	printf("Failed to select data\n");
		printf("SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(db);
		return -1;
	}
    return lastRec;
}

/******************************************************************************/
/* Inserta una transaccion en la base de datos del Dispensador                */
/******************************************************************************/
int insBD_Dispensador(int NoStk, DET_STACKERS dtstacker)
{	sqlite3_stmt *stmt;
	char *err_msg = 0;
	int rc;

	if(NoStk==0)
		sqlite3_prepare_v2(dbDispensador, "INSERT INTO stacker1 (TipoOperacion, TotalOperacion, "
		"TotalTarjetas, NoEmpleado, UID_MIA) VALUES (?,?,?,?,?);", -1, &stmt, NULL);
	else
		sqlite3_prepare_v2(dbDispensador, "INSERT INTO stacker2 (TipoOperacion, TotalOperacion, "
		"TotalTarjetas, NoEmpleado, UID_MIA) VALUES (?,?,?,?,?);", -1, &stmt, NULL);
	
	if(stmt != NULL) 
	{	//printf("OK STMT STACKER\n");
		sqlite3_bind_int (stmt, 1, dtstacker.TipoOperacion );
		sqlite3_bind_int (stmt, 2, dtstacker.TotalOperacion);
		sqlite3_bind_int (stmt, 3, dtstacker.TotalTarjetas);
		sqlite3_bind_int (stmt, 4, dtstacker.NoEmpleado);
		sqlite3_bind_text(stmt, 5, dtstacker.UID_MIA, strlen(dtstacker.UID_MIA), NULL);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
	else
	{
		printf(" ERROR  STMT STACKER%d \n", NoStk);
	}

	rc = sqlite3_exec(dbDispensador, (const char *)stmt, 0, 0, &err_msg);

	rdLastDispensador(dbDispensador, NoStk);

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void impStacker( int ns)
{
	printf("\n*****************************************************\n");	
	printf("STACKER%d\n", ns);	
	printf("NoMov              = %d\n", detStackers[ns].NoMov);
	printf("Fecha              = %s\n", detStackers[ns].Fecha);
	printf("Tipo de Operacion  = %d\n", detStackers[ns].TipoOperacion);
	printf("Total Operacion    = %d\n", detStackers[ns].TotalOperacion);
	printf("Total de tarjetas  = %d\n", detStackers[ns].TotalTarjetas);
	printf("No de Empleado     = %d\n", detStackers[ns].NoEmpleado);
	printf("UID MIA            = %s\n", detStackers[ns].UID_MIA);
	printf("*****************************************************\n");	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int UpdateStacker(int nstk)
{
	insBD_Dispensador( nstk, detStackers[nstk]);

	impStacker( nstk);

	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int DecrementaStacker(int nstk)
{
	if( (nstk==0) || (nstk==1) )
	{
		detStackers[nstk].TipoOperacion  = 3;		// VENTA
		detStackers[nstk].TotalOperacion  = 1;
		detStackers[nstk].TotalTarjetas -= 1;
		detStackers[nstk].NoEmpleado     = 0;
		strcpy(detStackers[nstk].UID_MIA, "");

		UpdateStacker(nstk);
	}
	else
		printf("Error en Numero de Stacker\n");
	return 0x00;
}

//******************************************************************************
//******************************************************************************
//
/******************************************************************************/
/*             Abre o Crea la base de Datos de Transacciones                  */
/******************************************************************************/
int bd_OpenTransaccion(char *dbname, sqlite3 **db )
{	int rc;
	char *err_msg = 0;

	char path_bd[100];
	sprintf(path_bd, "%sbd/%s",mert_operacion.path, dbname);

	rc = sqlite3_open(path_bd, db);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return 0x01;
	}
	char *sql = (char *)"CREATE TABLE IF NOT EXISTS movimientos"
		"( NoMov INTEGER NOT NULL UNIQUE PRIMARY KEY AUTOINCREMENT, "
		"Fecha	DATETIME DEFAULT  (datetime('now','localtime')), "
		"ID_NAME VARCHAR(20) NOT NULL,"
		"Linea_TDE	INTEGER NOT NULL, "
		"Estacion_TDE	INTEGER NOT NULL, "
		"Linea_TV	INTEGER NOT NULL, "
		"Estacion_TV	INTEGER NOT NULL, "
		"NoDisp	INTEGER NOT NULL, "
		"Status	INTEGER NOT NULL, "
		"UID_SAM VARCHAR(20) NOT NULL, "
		"UID_MIA VARCHAR(10) NOT NULL, "
		"NoLogico VARCHAR(28) NOT NULL, "
		"SaldoInicial INTEGER NOT NULL, "
		"Deposito	INTEGER NOT NULL, "
		"SaldoFinal INTEGER NOT NULL, "
		"BonoInicial INTEGER NOT NULL, "
		"BonoDepositado INTEGER NOT NULL, "
		"BonoFinal  INTEGER NOT NULL, "
		"VentaRecarga INTEGER NOT NULL, "
		"ValorMIA	INTEGER NOT NULL, "
		"NoCajaBilletes	INTEGER NOT NULL, "
		"NoCajaMonedas	INTEGER NOT NULL);" ;

	rc = sqlite3_exec(*db, sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK)
	{
		// error al open/create table
		fprintf(stderr, "Transacciones, Cannot open/create table: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return 0x01;
	}
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int lastBDTrans(void *detvoid, int argc, char **argv, char **azColName)
{
	for (int i = 0; i < argc; i++)
	{
		switch(i)
		{	case 0:
				detTransaccion.NoMov = atoi(argv[i]);
				lastRec = detTransaccion.NoMov;
				break;
			case 1:
				strcpy(detTransaccion.fecha, argv[i]);
				break;
			case 2:
				strcpy(detTransaccion.ID_Name, argv[i]);
				break;
			case 3:
				detTransaccion.Linea_TDE = atoi(argv[i]);
				break;
			case 4:
				detTransaccion.Estacion_TDE = atoi(argv[i]);
				break;
			case 5:
				detTransaccion.Linea_TVT = atoi(argv[i]);
				break;
			case 6:
				detTransaccion.Estacion_TVT = atoi(argv[i]);
				break;
			case 7:
				detTransaccion.NoDisp = atoi(argv[i]);
				break;
			case 8:
				detTransaccion.Status = atoi(argv[i]);
				break;
			case 9:
				strcpy(detTransaccion.UID_SAM, argv[i]);
				break;
			case 10:
				strcpy(detTransaccion.UID_MIA, argv[i]);
				break;
			case 11:
				strcpy(detTransaccion.NoLogico, argv[i]);
				break;
			case 12:
				detTransaccion.SaldoInicial = atoi(argv[i]);
				break;
			case 13:
				detTransaccion.Deposito = atoi(argv[i]);
				break;
			case 14:
				detTransaccion.SaldoFinal = atoi(argv[i]);
				break;
			case 15:
				detTransaccion.BonoInicial = atoi(argv[i]);
				break;
			case 16:
				detTransaccion.BonoDepositado = atoi(argv[i]);
				break;
			case 17:
				detTransaccion.BonoFinal = atoi(argv[i]);
				break;
			case 18:
				detTransaccion.VentaRecarga = atoi(argv[i]);
				break;
			case 19:
				detTransaccion.ValorMIA = atoi(argv[i]);
				break;
			case 20:
				detTransaccion.NoCajaBilletes = atoi(argv[i]);
				break;
			case 21:
				detTransaccion.NoCajaMonedas = atoi(argv[i]);
				break;
		}
	}
	return 0;
}

/******************************************************************************/
/* Lee el ultimo registro de la base de datos                                 */
/******************************************************************************/
int rdLastTrans(sqlite3 *db, DATA_TRANS *detTransaccion)
{	int rc, re;
	char *err_msg = 0;

	char *sql = (char *)"SELECT * FROM movimientos WHERE NoMov = "
										"(SELECT MAX(NoMov) FROM movimientos) LIMIT 1";

	lastRec = -1;
	re = 1;
	rc = sqlite3_exec(db, sql, lastBDTrans, 0, &err_msg);
	if (rc != SQLITE_OK )
	{	printf("Failed to select data\n");
		printf("SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(db);
		return -1;
	}
    return lastRec;
}

/******************************************************************************/
/* Inserta una transaccion en la base de datos de transacciones               */
/******************************************************************************/
int insBD_Transaccion(DATA_TRANS dtrans)
{	sqlite3_stmt *stmt;
	char *err_msg = 0;
	int rc;

	sqlite3_prepare_v2(dbTransacciones, "INSERT INTO movimientos (ID_NAME, Linea_TDE, "
		"Estacion_TDE, Linea_TV, Estacion_TV, NoDisp, Status, UID_SAM, UID_MIA, NoLogico,"
		"SaldoInicial, Deposito, SaldoFinal, BonoInicial, BonoDepositado, BonoFinal, "
		"VentaRecarga, ValorMIA, NoCajaBilletes, NoCajaMonedas) "
		"VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);", -1, &stmt, NULL);

	if(stmt != NULL) 
	{	//printf("OK STMT\n");
		sqlite3_bind_text(stmt, 1, dtrans.ID_Name, strlen(dtrans.ID_Name), NULL);
		sqlite3_bind_int (stmt, 2, dtrans.Linea_TDE);
		sqlite3_bind_int (stmt, 3, dtrans.Estacion_TDE);
		sqlite3_bind_int (stmt, 4, dtrans.Linea_TVT);
		sqlite3_bind_int (stmt, 5, dtrans.Estacion_TVT);
		sqlite3_bind_int (stmt, 6, dtrans.NoDisp);
		sqlite3_bind_int (stmt, 7, dtrans.Status);
		sqlite3_bind_text(stmt, 8, dtrans.UID_SAM,  strlen(dtrans.UID_SAM), NULL);
		sqlite3_bind_text(stmt, 9, dtrans.UID_MIA,  strlen(dtrans.UID_MIA), NULL);
		sqlite3_bind_text(stmt,10, dtrans.NoLogico, strlen(dtrans.NoLogico),NULL);
		sqlite3_bind_int (stmt,11, dtrans.SaldoInicial);
		sqlite3_bind_int (stmt,12, dtrans.Deposito);
		sqlite3_bind_int (stmt,13, dtrans.SaldoFinal);
		sqlite3_bind_int (stmt,14, dtrans.BonoInicial);
		sqlite3_bind_int (stmt,15, dtrans.BonoDepositado);
		sqlite3_bind_int (stmt,16, dtrans.BonoFinal);
		sqlite3_bind_int (stmt,17, dtrans.VentaRecarga);
		sqlite3_bind_int (stmt,18, dtrans.ValorMIA);
		sqlite3_bind_int (stmt,19, dtrans.NoCajaBilletes);
		sqlite3_bind_int (stmt,20, dtrans.NoCajaMonedas);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
	else
	{
		printf("ERROR  STMT TRANSACCION\n");
	}

	rc = sqlite3_exec(dbTransacciones, (const char *)stmt, 0, 0, &err_msg);

	// regresar el Numero de Ticket, este lo asigna automaticamente
	// al crear el registro, leer el ultimo registro y tomar el Numero de Ticket	
//	printf("No Mov = %d\n", detTransaccion.NoMov);
	
	rdLastTrans(dbTransacciones, &detTransaccion);

//	printf("No Mov = %d\n", detTransaccion.NoMov);
	
	return 0;
}

//******************************************************************************
//**************************          CAJAS      *******************************
//******************************************************************************
//
/******************************************************************************/
/*             Abre o Crea la base de Datos de Caja de Monedas                */
/******************************************************************************/
int bd_OpenCajas(char *dbname, sqlite3 **db )
{	int rc;
	char *err_msg = 0;

	char path_bd[100];
	sprintf(path_bd, "%sbd/%s",mert_operacion.path, dbname);

	rc = sqlite3_open(path_bd, db);

	if (rc != SQLITE_OK)
	{
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return 0x01;
	}

	char *sql = (char *)"CREATE TABLE IF NOT EXISTS detalle"
		"( NoMov	INTEGER NOT NULL UNIQUE PRIMARY KEY AUTOINCREMENT, "
		"CajaID		INTEGER DEFAULT 0, "
		"FechaCreacion	DATETIME DEFAULT  (datetime('now','localtime')), "
		"FechaCorte	DATETIME DEFAULT  (datetime('now','localtime')), "
		"Status		INTEGER DEFAULT 0, "
		"Usuario1	INTEGER DEFAULT 0, "
		"Usuario2	INTEGER DEFAULT 0, "
		"Ficha		INTEGER DEFAULT 0, "
		"Sello		INTEGER DEFAULT 0, "
		"Valor1		INTEGER DEFAULT 0, "
		"Valor2		INTEGER DEFAULT 0, "
		"Valor3		INTEGER DEFAULT 0, "
		"Valor4		INTEGER DEFAULT 0, "
		"Valor5		INTEGER DEFAULT 0, "
		"Valor6		INTEGER DEFAULT 0, "
		"Valor7		INTEGER DEFAULT 0, "
		"Valor8		INTEGER DEFAULT 0, "
		"Valor9		INTEGER DEFAULT 0, "
		"Valor10	INTEGER DEFAULT 0, "
		"Total1		INTEGER DEFAULT 0, "
		"Total2		INTEGER DEFAULT 0, "
		"Total3		INTEGER DEFAULT 0, "
		"Total4		INTEGER DEFAULT 0, "
		"Total5		INTEGER DEFAULT 0, "
		"Total6		INTEGER DEFAULT 0, "
		"Total7		INTEGER DEFAULT 0, "
		"Total8		INTEGER DEFAULT 0, "
		"Total9		INTEGER DEFAULT 0, "
		"Total10	INTEGER DEFAULT 0, "
		"CantidadTotal	INTEGER DEFAULT 0);";

	rc = sqlite3_exec(*db, sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK)
	{
		// error al open/create table
		fprintf(stderr, "Cannot open/create table: %s\n", sqlite3_errmsg(*db));
		sqlite3_close(*db);
		return 0x01;
	}
	return 0;
}

/******************************************************************************/
/* Inserta un registro en la base de datos de las Cajas                       */
/******************************************************************************/
int insBDDetalle(sqlite3 *db, DET_BILL_COIN detalle)
{	sqlite3_stmt *stmt;
	char *err_msg = 0;
	int rc,i ;

	sqlite3_prepare_v2(db, "INSERT INTO detalle "
			"(CajaID, Status,  "
			"Valor1, Valor2, Valor3,  Valor4,  Valor5,  "
			"Valor6, Valor7, Valor8,  Valor9,  Valor10, "
			"Total1, Total2, Total3, Total4, Total5,  "
			"Total6, Total7, Total8, Total9, Total10, "
			"CantidadTotal) "
			"VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);", 
			1000, &stmt, NULL);

	if(stmt != NULL)
	{	//printf("OK STMT\n");
		sqlite3_bind_int (stmt, 1, detalle.CajaID);
		sqlite3_bind_int (stmt, 2, detalle.Status);

		for(i=0; i<TOTAL_DENOM_BM; i++)
			sqlite3_bind_int (stmt, i+3, detalle.Valor[i]);
		for(i=0; i<TOTAL_DENOM_BM; i++)
			sqlite3_bind_int (stmt,i+13, detalle.Total[i]);

		sqlite3_bind_int (stmt,23, detalle.CantidadTotal);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
	else
	{
		printf("ERROR  STMT Inserta Nuevo Registro\n");
		return  0x01;	//error
	}
	rc = sqlite3_exec(db, (const char *)stmt, 0, 0, &err_msg);
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void impDetalle( DET_BILL_COIN detalle, char op)
{	int i;

	if(op==0)
		printf("MONEDAS\n");
	else
		printf("BILLETES\n");

	printf("NoMov          = %d\n", detalle.NoMov);
	printf("CajaID         = %d\n", detalle.CajaID);
	printf("F. Creacion    = %s\n", detalle.FechaCreacion);
	printf("F. Corte       = %s\n", detalle.FechaCorte);
	printf("Status         = %d\n", detalle.Status);
	printf("Usuario1       = %d\n", detalle.Usuario1);
	printf("Usuario2       = %d\n", detalle.Usuario2);
	printf("Ficha          = %d\n", detalle.Ficha);
	printf("Sello          = %d\n", detalle.Sello);
	printf("Cantidad Total = %d\n", detalle.CantidadTotal);

	for(i=0;i<TOTAL_DENOM_BM;i++)
		printf("%2d -> Valor = %4d - Total = %4d\n", i, detalle.Valor[i], detalle.Total[i]);
}

/******************************************************************************/
/* Lee el ultimo registro de Monedas/Billetes                                 */
/******************************************************************************/
int lastBDMov(void *detvoid, int argc, char **argv, char **azColName)
{
	DET_BILL_COIN *detalle;

//	printf("LastBDMov = %d\n", *(int *)detvoid);
	
	if( *(int *)detvoid == 0)
		detalle = (DET_BILL_COIN *)&detMonedas;
	else
		detalle = (DET_BILL_COIN *)&detBilletes;

	for (int i = 0; i < argc; i++)
	{
		switch(i)
		{	case 0:
				detalle->NoMov = atoi(argv[i]);	
				lastRec = atoi(argv[i]);
				break;
			case 1:
				detalle->CajaID = atoi(argv[i]);
				break;
			case 2:
				strcpy(detalle->FechaCreacion, argv[i]);
				break;
			case 3:
				strcpy(detalle->FechaCorte, argv[i]);
				break;
			case 4:
				detalle->Status = atoi(argv[i]);
				break;
			case 5:
				detalle->Usuario1 = atoi(argv[i]);
				break;
			case 6:
				detalle->Usuario2 = atoi(argv[i]);
				break;
			case 7:
				detalle->Ficha = atoi(argv[i]);
				break;
			case 8:
				detalle->Sello = atoi(argv[i]);
				break;
			case 29:
				detalle->CantidadTotal = atoi(argv[i]);
				break;
		}
		if( (i>=9) && (i<=18))
				detalle->Valor[i-9] = atoi(argv[i]);
		if( (i>=19) && (i<=28))
				detalle->Total[i-19] = atoi(argv[i]);
	}
	return 0;
}

/******************************************************************************/
/* Lee el ultimo registro de la base de datos                                 */
/******************************************************************************/
int rdLastCaja(sqlite3 *db, DET_BILL_COIN *detalle, int opdb)
{	int rc, re;
	char *err_msg = 0;

	char *sql = (char *)"SELECT * FROM detalle WHERE NoMov = "
										"(SELECT MAX(NoMov) FROM detalle) LIMIT 1";

	lastRec = -1;
	re = 1;
	rc = sqlite3_exec(db, sql, lastBDMov, &opdb, &err_msg);
	if (rc != SQLITE_OK )
	{	printf("Failed to select data\n");
		printf("SQL error: %s\n", err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(db);
		return -1;
	}
//	printf("1---CajaID=%u\n", detalle->CajaID);
    return lastRec;
}

/******************************************************************************/
/*  Update ultimo registro con CVF de las monedas                             */
/******************************************************************************/
int wrDenominaciones(sqlite3 *db, DET_BILL_COIN detalle)
{	sqlite3_stmt *stmt;
//	char *err_msg = 0;
	int rc,i;

	printf("Update DENOMINACIONES\n");
	rc = sqlite3_prepare_v2(db, "UPDATE detalle SET "
		"Status = ?, Valor1 = ?, Valor2 = ?, Valor3 = ?, Valor4 = ?, Valor5 = ?, "
		"Valor6 = ?, Valor7 = ?, Valor8 = ?, Valor9 = ?, Valor10= ? "
		"WHERE NoMov = ? ;", -1, &stmt, NULL);
	if (rc != SQLITE_OK) 
	{
		printf("ERROR QUERY CVF=%d\n", rc);
		return  0x01;	// ERROR
	}

	if(stmt != NULL)
	{
		printf("OK STMT CVF, NoMov=%d\n", detalle.NoMov);
		sqlite3_bind_int (stmt,1, detalle.Status);

		for(i=0; i< TOTAL_DENOM_BM; i++)
			sqlite3_bind_int (stmt, i+2,   detalle.Valor[i]);

		sqlite3_bind_int (stmt,12, detalle.NoMov);
		rc = sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		if(rc==SQLITE_DONE)
		{
			printf("OK UPDATE Caja CVF\n");
			return  0x00;	//OK
		}
		else
		{
			printf("ERROR UPDATE Caja Denominaciones=%d\n", rc);
			return  0x01;	// ERROR
		}
	}
	else
	{
		printf("ERROR  STMT Caja Denominaciones\n");
		return  0x01;	//error
	}

	return 0;	// OK
}

/******************************************************************************/
/*  Update ultimo registro con Totales                                        */
/******************************************************************************/
int xwrLastCajasTotal(sqlite3 *db, DET_BILL_COIN detalle)
{	sqlite3_stmt *stmt;
	char *err_msg = 0;
	int rc,i;

	sqlite3_prepare_v2(db, "UPDATE detalle set "
		"Total1 = ?, Total2 = ?, Total3 = ?, Total4 = ?, Total5 = ?, Total6 = ?, "
		"Total7 = ?, Total8 = ?, Total9 = ?, Total10= ?, CantidadTotal = ? "
		"where NoMov = ?; ", 1000, &stmt, NULL);

	if(stmt != NULL) 
	{	//printf("OK STMT\n");		
		for(i=0; i<TOTAL_DENOM_BM; i++)
			sqlite3_bind_int (stmt, i+1, detalle.Total[i]);
		sqlite3_bind_int (stmt,11, detalle.CantidadTotal);
		sqlite3_bind_int (stmt,12, detalle.NoMov);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		return 0;
	}
	else
	{
		printf("ERROR  STMT Update Total\n");
		return  0x01;	//error
	}
	return 0;
}

/******************************************************************************/
/*  Update ultimo registro con CVF de las monedas                             */
/******************************************************************************/
// Agregar Fecha de Corte, Status-> Cerrar Caja
int wrLastCorte(sqlite3 *db, DET_BILL_COIN detalle)
{	sqlite3_stmt *stmt;
	char *err_msg = 0;
	int rc;

	sqlite3_prepare_v2(db, "UPDATE detalle set "
		"FechaCorte = ?, Status = ?, Usuario1 = ?, Usuario2 = ?, Ficha = ?, Sello = ? "
		"where NoMov = ?; SELECT * from detalle", 1000, &stmt, NULL);
		
	if(stmt != NULL) 
	{	//printf("OK STMT\n");
		sqlite3_bind_text(stmt, 1, detalle.FechaCorte, strlen(detalle.FechaCorte), NULL);
		sqlite3_bind_int (stmt, 2, detalle.Status);
		sqlite3_bind_int (stmt, 3, detalle.Usuario1);
		sqlite3_bind_int (stmt, 4, detalle.Usuario2);
		sqlite3_bind_int (stmt, 5, detalle.Ficha);
		sqlite3_bind_int (stmt, 6, detalle.Sello);
		sqlite3_bind_int (stmt, 7, detalle.NoMov);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
	else
	{
		printf("ERROR  STMT UPDATE CORTE\n");
		return  0x01;	//error
	}

	rc = sqlite3_exec(db, (const char *)stmt, 0, 0, &err_msg);
	return 0;
}

/******************************************************************************/
/*  Update CajaID                                                             */
/******************************************************************************/
int wrLastCajaID(sqlite3 *db, DET_BILL_COIN detalle)
{	sqlite3_stmt *stmt;
	char *err_msg = 0;
	int rc;

	rc = sqlite3_prepare_v2(db, "UPDATE detalle set "
		"CajaID = ? where NoMov = ?; SELECT * from detalle", 1000, &stmt, NULL);
	if (rc != SQLITE_OK) 
	{
		printf("ERROR QUERY CajaID=%d\n", rc);
		return  0x01;	// ERROR
	}

	if(stmt != NULL) 
	{	//printf("OK STMT\n");		
		sqlite3_bind_int (stmt, 1, detalle.CajaID);
		sqlite3_bind_int (stmt, 2, detalle.NoMov);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
	else
	{
		printf("ERROR  STMT CajaID\n");
		return  0x01;	//error
	}

	return 0;
}

/******************************************************************************/
/*  Update ultimo registro con CVF de las monedas                             */
/******************************************************************************/
int wrLastTotal(sqlite3 *db, DET_BILL_COIN detalle)
{	sqlite3_stmt *stmt;
	char *err_msg = 0;
	int rc,i;

	sqlite3_prepare_v2(db, "UPDATE detalle set "
		"Total1 = ?, Total2 = ?, Total3 = ?, Total4 = ?, Total5 = ?, Total6 = ?, "
		"Total7 = ?, Total8 = ?, Total9 = ?, Total10= ?, CantidadTotal = ? "
		"where NoMov = ?; SELECT * from detalle", 1000, &stmt, NULL);
		
	if(stmt != NULL) 
	{	//printf("OK STMT\n");		
		for(i=0; i<TOTAL_DENOM_BM; i++)
			sqlite3_bind_int (stmt, i+1, detalle.Total[i]);
		sqlite3_bind_int (stmt,11, detalle.CantidadTotal);
		sqlite3_bind_int (stmt,12, detalle.NoMov);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
	else
	{
		printf("ERROR  STMT Update Total\n");
		return  0x01;	//error
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void updateCorteMonedas(void)
{	int i, valor[TOTAL_DENOM_BM];

	// agregar nuevo registro
	for(i=0; i<TOTAL_DENOM_BM; i++ )
		valor[i] = detMonedas.Valor[i];

	memset(&detMonedas, 0, sizeof(detMonedas) );
	for(i=0; i<TOTAL_DENOM_BM; i++ )
		detMonedas.Valor[i] = valor[i];

	detMonedas.Status = 1; // BD con CVF
	detMonedas.CajaID = idCajaMonedas;
	if(insBDDetalle(dbMonedas, detMonedas) != 0)
		printf("Error al Insertar Registro Monedas\n");
	lastMovCoins = rdLastCaja(dbMonedas, &detMonedas, 0);	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void updateCorteBilletes(void)
{	int i, valor[TOTAL_DENOM_BM];

	// agregar nuevo registro
	for(i=0; i<TOTAL_DENOM_BM; i++ )
		valor[i] = detBilletes.Valor[i];

	memset(&detBilletes, 0, sizeof(detBilletes) );
	for(i=0; i<TOTAL_DENOM_BM; i++ )
		detBilletes.Valor[i] = valor[i];

	detBilletes.Status = 1; // BD con CVF
	detBilletes.CajaID = idCajaBilletes;
	if(insBDDetalle(dbBilletes, detBilletes) != 0)
		printf("Error al Insertar Registro Billetes\n");
	lastMovCoins = rdLastCaja(dbBilletes, &detBilletes, 1);	
}

/******************************************************************************/
/* Update ID Caja Monedas                                                     */
/******************************************************************************/
int UpCajaIDM(void)
{
	wrLastCajaID(dbMonedas, detMonedas);
	
	return 0x00;
}

/******************************************************************************/
/* Update ID Caja Monedas                                                     */
/******************************************************************************/
int UpCajaIDB(void)
{
	wrLastCajaID(dbBilletes, detBilletes);
	
	return 0x00;
}

//******************************************************************************
//
//                 REVISA Y ABRE LAS BASES DE DATOS
//
//******************************************************************************

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int revBD_CajaMonedas(void)
{
	memset(&detMonedas, 0 , sizeof(detMonedas) );	
	re_bdCoins  = bd_OpenCajas((char *)"CorteMonedas.db", &dbMonedas);

	// lee el ultimo registro y si esta cerrado crea una Caja Nueva
	lastMovCoins = rdLastCaja(dbMonedas, &detMonedas, 0);
	if(lastMovCoins==-1)
	{	// no hay registros, inserta 1		
		memset(&detMonedas, 0, sizeof(detMonedas));
		if(insBDDetalle(dbMonedas, detMonedas) != 0)
		{
			printf("Error al Insertar Registro Monedas\n");
			return 0x01;	// Error en Base de Datos
		}
		printf("OK al insertar Registro Monedas\n");
		lastMovCoins = rdLastCaja(dbMonedas, &detMonedas, 0);

		if( detMonedas.Status == 0)
			detMonedas.Status = 1;
		detMonedas.Valor[0] = 50;
		detMonedas.Valor[1] = 100;
		detMonedas.Valor[2] = 200;
		detMonedas.Valor[3] = 500;
		detMonedas.Valor[4] = 1000;
		detMonedas.Valor[5] = 2000;
		detMonedas.Valor[6] = 0;
		detMonedas.Valor[7] = 0;
		detMonedas.Valor[8] = 0;
		detMonedas.Valor[9] = 0;
	
		wrDenominaciones(dbMonedas, detMonedas);
		lastMovCoins = rdLastCaja(dbMonedas, &detMonedas, 0);
	}
	impDetalle( detMonedas, 0);

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int revBD_CajaBilletes(void)
{
	memset(&detBilletes, 0 , sizeof(detBilletes) );

	re_bdBills  = bd_OpenCajas((char *)"CorteBilletes.db", &dbBilletes);

	// lee el ultimo registro y si esta cerrado crea una Caja Nueva
	lastMovBills = rdLastCaja(dbBilletes, &detBilletes, 1);
	if(lastMovBills==-1)
	{	// no hay registros, inserta 1		
		memset(&detBilletes, 0, sizeof(detBilletes));
		if(insBDDetalle(dbBilletes, detBilletes) != 0)
		{
			printf("Error al Insertar Registro Monedas\n");
			return 0x01;	// Error en Base de Datos
		}
//		printf("OK al insertar Registro Billetes\n");
		lastMovBills = rdLastCaja(dbBilletes, &detBilletes, 1);
		
		if(detBilletes.Status == 0)
			detBilletes.Status = 1;
		detBilletes.Valor[0] =  2000;
		detBilletes.Valor[1] =  5000;
		detBilletes.Valor[2] = 10000;
		detBilletes.Valor[3] = 20000;
		detBilletes.Valor[4] = 50000;
		detBilletes.Valor[5] = 0;
		detBilletes.Valor[6] = 0;
		detBilletes.Valor[7] = 0;
		detBilletes.Valor[8] = 0;
		detBilletes.Valor[9] = 0;
	
		wrDenominaciones(dbBilletes, detBilletes);
		lastMovCoins = rdLastCaja(dbBilletes, &detBilletes, 0);
	}
	impDetalle( detBilletes, 0);

//	sqlite3_close(dbBilletes);

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int revBD_Transacciones(void)
{
	memset(&detTransaccion, 0 , sizeof(detTransaccion) );

	re_bdTrans  = bd_OpenTransaccion( (char *)"Transacciones.db", &dbTransacciones);

	return re_bdTrans;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int revBD_Dispensador(void)
{	int i;

	memset(&detStackers, 0 , sizeof(detStackers) );

	re_bdDispensador  = bd_OpenDispensador( (char *)"Dispensador.db", &dbDispensador);

	// lee el ultimo registro STACKER
	for(i=0; i<2; i++)
	{
		lastMovStackers[i] = rdLastDispensador(dbDispensador, i);
//		printf("Last Mov=%d\n",lastMovStackers[i] );
		if(lastMovStackers[i] == -1)
		{	// no hay registros, inserta 1		
			memset(&detStackers[i], 0, sizeof(detStackers[i]));
			if(insBD_Dispensador(i, detStackers[i]) != 0)
			{
				printf("Error al Insertar Stacker%d\n", i);
				return 0x01;	// Error en Base de Datos
			}
//			printf("OK al insertar Stacker%d\n", i);
			lastMovStackers[i] = rdLastDispensador(dbDispensador, i);
		}
		impStacker( i);
	}

	return re_bdDispensador;
}

/******************************************************************************/
/*                   Carga todas las bases de datos                           */
/******************************************************************************/
void LoadBaseDatos(void)
{
	// Base de Datos del Dispensador
	re_bdDispensador = revBD_Dispensador();

	// Base de Datos de la Caja de Transacciones
	re_bdTrans = revBD_Transacciones();

	// Base de Datos de la Caja de Monedas
	re_bdCoins = revBD_CajaMonedas();

	// Base de Datos de la Caja de Billetes
	re_bdBills = revBD_CajaBilletes();

}
////////////////////////////////////////////////////////////////////////////////
