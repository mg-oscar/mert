/* 
 * File:   libMIA.cpp
 * Author: Isaac V./FAMISA
 * 
 * Created on 21 de enero de 2021, 09:33 AM
 * Version 3.0.4
*/

/****************************** HEADER FILES **********************************/
#include "libMIA.hpp"
#include "mert.h"
#include <stdlib.h>
#include <memory.h>
#include <cstdio>

/* ****************************************************************************
 * 
 *      MIA Aplication 
 *      Telvent
 *      Some constants
 *
 * ****************************************************************************/
#define METRORREYID			0x4A3BE83
#define STARTYEAR			2000
#define MAXDECADEYEAR		120     //


/******************************************************************************/
/* Constructor                                                                */
/******************************************************************************/
libMIA::libMIA()
{
	PursesBlocks[0] = Purse_1;
	PursesBlocks[1] = Purse_2;
    
	HistoricalRechargeBlocks[0]=HISTORICALPURSERECHARGE_1;
	HistoricalRechargeBlocks[1]=HISTORICALPURSERECHARGE_2;

	TransitBlocks[0]=LASTTRANSIT_1;
	TransitBlocks[1]=LASTTRANSIT_2;
	TransitBlocks[2]=LASTTRANSIT_3;
    
	TransitFinalBlocks[0]=LASTTRANSITFINAL_1;
	TransitFinalBlocks[1]=LASTTRANSITFINAL_2;
	TransitFinalBlocks[2]=LASTTRANSITFINAL_3;
	
	HistoricalMovementsBlocks[0] = HISTORICALMOVEMENTS_1;
	HistoricalMovementsBlocks[1] = HISTORICALMOVEMENTS_2;
	HistoricalMovementsBlocks[2] = HISTORICALMOVEMENTS_3;
	HistoricalMovementsBlocks[3] = HISTORICALMOVEMENTS_4;
	HistoricalMovementsBlocks[4] = HISTORICALMOVEMENTS_5;
	HistoricalMovementsBlocks[5] = HISTORICALMOVEMENTS_6;
	HistoricalMovementsBlocks[6] = HISTORICALMOVEMENTS_7;
	HistoricalMovementsBlocks[7] = HISTORICALMOVEMENTS_8;
	HistoricalMovementsBlocks[8] = HISTORICALMOVEMENTS_9;
	HistoricalMovementsBlocks[9] = HISTORICALMOVEMENTS_10;
	HistoricalMovementsBlocks[10] = HISTORICALMOVEMENTS_11;
	HistoricalMovementsBlocks[11] = HISTORICALMOVEMENTS_12;

	TransferFlag = 0; 
}

/******************************************************************************/
/* Verifica el estado inicial de la tarjeta, que no este en 0's               */
/******************************************************************************/
int libMIA::VerEstadoInicial(void)
{	int k,i;
	for(k=5; k<=63;k++)
	{	// brincamos el tercer block de cada sector
		if( (k%4) !=3 )
		{	for(i=0;i<16;i++)
				if(ReadBlocks[k][i] != 0)
					return 0;
		}
	}
//printf("MIA EN CEROS\n");
	return 1;
}

/******************************************************************************/
/* Verifica si un block esta en 0's                                           */
/******************************************************************************/
int libMIA::BlockInZero(unsigned char blk)
{	int i;

	for(i=0;i<16;i++)
		if(ReadBlocks[blk][i] != 0)
			return 0x01;
	return 0x00;
}

/******************************************************************************/
/* Verifica si un block esta en 0's                                           */
/******************************************************************************/
int libMIA::SetBlockInZero(void)
{	int k,i;

	for(k=5; k<=63;k++)
	{	// brincamos el tercer block de cada sector
		if( (k%4) !=3 )
		{	for(i=0;i<16;i++)
				ReadBlocks[k][i] = 0;
		}
	}
	return 0;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::InitValues(initvalues ID)
{
	init.LINEA		= ID.LINEA;
	init.ESTACION	= ID.ESTACION;
	init.NUMDISP	= ID.NUMDISP;
	init.TIPO		= ID.TIPO;

	init.COMP		= ID.COMP;
	init.TRAY		= ID.TRAY;
	init.TICKETVAL	= ID.TICKETVAL;

/*	printf("LIBMIA \n");
	printf("LINEA     = %d\n", init.LINEA);
	printf("ESTACION  = %d\n", init.ESTACION);
	printf("NUM. DISP = %d\n", init.NUMDISP);
	printf("TIPO      = %d\n", init.TIPO);
	printf("\n");
	printf("COMPANIA  = %d\n", init.COMP);
	printf("TRAY      = %d\n", init.TRAY);
	printf("TICKETVAL = %d\n", init.TICKETVAL);
	printf("\n");
*/	
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::TransferVerifica(initvalues id)
{	unsigned char *OperationCheck;
	int YearNow;
	Check   MiaCheck;
	time_t TimeNow = time(NULL);
	struct tm *Now = localtime(&TimeNow);  // Get Local TIme

	InitValues(id);

	YearNow = ((Now->tm_year + OFFSETYEAR) - STARTYEAR);  // Year 02d

	// Estas funciones borraran todos lo datos de las variables
	memset(WriteBlocks,0x00,64*16);

	PassingInitializationBlock();
	PassingCustomBlock();
	PassingPurseSellBlock();
	PassingOperationStartBlock();
	PassingOperationEndBlock();
	PassingPurseBlock();
	PassingContract_1_ReleaseBlock();
	PassingContract_1_Transit_0_Block();
	PassingHistoricalMovements();
	PassingHistoricalRechargeMovements();
	PassingCustomPrivateData(&CustomPrivateData);
//	PassingCustomPublicData(&CustomPublicData);
    //ReadBalanceCard(argc,argv);
    //ReadTravelCard(argc,argv);
	OperationCardIn.Msg = 0;
    //MIA.Purse.PurseA = 0;
    //MIA.Purse.PurseA1 = 0;

	memcpy(WriteBlocks, ReadBlocks, 1024);

	PassingInitializationBlock();	
	PassingCustomBlock();
	PassingPurseSellBlock();

	if(VerEstadoInicial() == 1)
	{	OperationCardIn.Msg = TarjetaEnCeros;
			return 0x0509;		// TarjetaEnCeros
	}

//	printf("flag=%d\n", SellCard.ActivationFlag);
	if( SellCard.ActivationFlag != Activated)
	{
		switch(SellCard.ActivationFlag)
		{	case Inactivated:
						//Mostrar Mensaje y Tono
						//printf("Activation Flag -> %d\n",MIA.SellCard.ActivationFlag);
						OperationCardIn.Msg = TarjetaInactiva;
						return 0x0500;		// Tarjeta Inactiva
						break;
			case Loaded:
						//Mostrar Mensaje y Tono
						//printf("Activation Flag -> %d\n",MIA.SellCard.ActivationFlag);
						OperationCardIn.Msg = TarjetaPrevaluada;
						return 0x0501;		// Tarjeta PreValuada
						break;
			case Canceled:
						//Mostrar Mensaje y Tono
						//printf("Activation Flag -> %d\n",MIA.SellCard.ActivationFlag);
						OperationCardIn.Msg = TarjetaCancelada;
						return 0x0502;		// Tarjeta Cancelada
						break;
			case BlackList:
						//Mostrar Mensaje y Tono
						//printf("Activation Flag -> %d\n",MIA.SellCard.ActivationFlag);
						OperationCardIn.Msg = TarjetaListaNegra;
						return 0x0502;		// Tarjeta en Lista Negra
						break;
		}
		return 0x0503;		// Tarjeta No Activada, Estado Desconocido
	}

//	printf("OK\n");
	if( Initialization.ApplicationCode != MetrorreyApp )
	{	OperationCardIn.Msg = AppCodeInvalido;
		printf("Aplication Code=%d\n", Initialization.ApplicationCode);
		return 0x0504;		//Tarjeta que no es de Metrorrey
	}

	if(CustomCard.ExpYear <= YearNow)  		
	{	if(CustomCard.ExpMonth <= (Now->tm_mon+1))
		{	if(CustomCard.ExpDay <= Now->tm_mday)
			{	/*if(strcmp(argv[argc - j],debug) == 0)
								printf("Titulo Caduco %02d/%02d/%02d \n",
															CustomCard.ExpYear,
															CustomCard.ExpMonth,
															CustomCard.ExpDay);*/
				OperationCardIn.Msg = TituloCaduco;
				return 0x0505;	// Titulo Caducado
			}
		}
	}

	PassingOperationStartBlock();

	// Check Backups
	OperationCheck = MakeOperationStartBlock();
	MiaCheck.Check8 = ChecksumNibble(OperationCheck,0,0x0f,3,0xff);
	MiaCheck.Check8 &= 0x0F;			//LowNibble;
	if(OperationStart.Check8 != MiaCheck.Check8)
	{	printf("Error en Checksum 8\n");
		OperationCardIn.Msg = ErrorChks8;
		return 0x0506;		// ErrorChks8
	}

	PassingOperationEndBlock();
	OperationCheck = MakeOperationEndBlock();
	MiaCheck.Check9 = ChecksumWord(OperationCheck,2,15);
	if(OperationEnd.Check9 != MiaCheck.Check9)
	{	printf("Error en Checksum 9\n");
		OperationCardIn.Msg = ErrorChks9;
		return 0x0507;		// ErrorChks9
	}

	//Chek OperationStart
	if( OperationStart.Operation == NOOPERATION )
	{	printf("Tarjeta Desactivada=====\n");
		OperationCardIn.Msg = TarjetaDesactivada;
		return 0x0508;	// TarjetaDesactivada
	}

	OperationCardIn.TitleCode = CustomCard.TitleCode;
	if(OperationStart.FlagRecordStartOperation != OperationEnd.EndRecordOperation)
	{	printf("\n\nError en la transaccion, se usaran los datos pasados\n\n");
		OperationEndAux = OperationEnd;			// Respaldo la informacion del fin de operacion
		OperationEnd = OperationStart.Backup;	// cargo en el fin de operacion los datos del backup dentro de inicio de operacion
		OperationEnd.Ruf = 0x00;
		OperationEnd.OpeAutCode = 0x00;
	}

	PassingPurseBlock();
	PassingContract_1_ReleaseBlock();
	PassingContract_1_Transit_0_Block();
	PassingHistoricalMovements();
	PassingHistoricalRechargeMovements();

	switch(Initialization.Category)		// check the type of the card
	{	case SIN_CATEGORIA:     //0
					OperationCardIn.Msg = TarjetaSinCategoria;
					return 0x0509;	// Tarjeta Sin Categoria
					break;
		case EMPLEADOS:			//2
//					ReadTravelCard(argc,argv);
					break;
		case ANONIMOS:          //1
		case EMPRESAS_EXTERNAS: //4
		case PERSONAL:          //5
//					ReadBalanceCard(argc,argv);
					break;
		case ESPECIALES:		//3
//					if(MIA.CustomCard.TitleCode == 151) // Estudiante hijo Metrorrey
//						ReadTravelCard(argc,argv);
//					else
//						ReadBalanceCard(argc,argv);
					break;
	}
	CreateLogicNumber();
	ValuesTSC();
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::VentaMIA(initvalues id, long tCarga, long  tBono,long tFianza)
{	int miaBono;
	int ReloadValue,PurseBalance;   // Cantidad total leida x billetero y monedero
	int Fianza;                 // cantidad dejada en fianza
	int pCat    = 1;
	int pTitle  = 111;

	printf("Total a depositar = %ld\n", tCarga);
	printf("Total de Bonos    = %ld\n", tBono);
	printf("Total de Fianza   = %ld\n", tFianza);

	InitValues(id);

	PassingInitializationBlock();
	PassingCustomBlock();
	PassingPurseSellBlock();
	PassingOperationStartBlock();
	PassingOperationEndBlock();
	PassingPurseBlock();
	PassingContract_1_ReleaseBlock();
	PassingContract_1_Transit_0_Block();
	PassingHistoricalMovements();
	PassingHistoricalRechargeMovements();
	PassingCustomPrivateData(&CustomPrivateData);
//	PassingCustomPublicData(&CustomPublicData);
    //ReadBalanceCard(argc,argv);
    //ReadTravelCard(argc,argv);
	OperationCardIn.Msg = 0;



//	PassingCustomBlock();
//	PassingPurseSellBlock();


	memcpy(WriteBlocks, ReadBlocks, 1024);

//	PassingInitializationBlock();

	if(VerEstadoInicial() == 1)
	{

		OperationCardIn.Msg = TarjetaEnCeros;
	}
	
	if(SellCard.ActivationFlag == 0)
	{	
		if(OperationCardIn.Msg == TarjetaEnCeros)
		{   
			OperationCardIn.Msg = 0;

			if(Initialization.Category != ANONIMOS)
			{
				OperationCardIn.Msg = TarjetaSinCategoria;
				return false;
			}

			CreateLogicNumber();     // Esta funcion calcula el Item_ID o Número lógico

			ReloadValue = tCarga; 
			miaBono     = tBono;
			Fianza      = tFianza;
			PurseBalance = 0;

			//Blocks to Write on Card
			OperationCardIn.NumberBlocks2Write = 0;		// Star Count of blocks to write

			// Crea los monederos Purse1 y Purse 2,block 5,6,8,9,10,24,40 y 41
			SetCustomBlock(); //5
			CopyWriteBlocks(MakeCustomBlock(),0,BLOCKSIZE,(CUSTOMBLOCK));
			OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = CUSTOMBLOCK;  	// Bloque de personalizacion
			SetSellBlock(ReloadValue+Fianza,Fianza); //6
			CopyWriteBlocks(MakeSellBlock(),0,BLOCKSIZE,(PURSESELLBLOCK));
			OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = PURSESELLBLOCK;	// Bloque de venta
 
			SetOperationEndBlock(pCat,pTitle); // 9
			CopyWriteBlocks(MakeOperationEndBlock(),0,BLOCKSIZE,(OPERATIONENDBLOCK));

			SetOperationStartBlock(); //8
			CopyWriteBlocks(MakeOperationStartBlock(),0,BLOCKSIZE,(OPERATIONSTARTBLOCK));
			OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = OPERATIONSTARTBLOCK;	// Bloque inicio de operacion
            
			SetHistoricalRecharge(ReloadValue, PurseBalance); //10
			CopyWriteBlocks(MakeHistoricalRecharge(),0,BLOCKSIZE,(HISTORICALPURSERECHARGE_1));
			OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = HISTORICALPURSERECHARGE_1;	// Bloque de historico de recarga

			SetValueBlock(ReloadValue,0x09);
			CopyWriteBlocks(MakePurseBlock(),0,BLOCKSIZE,(Purse_1));
			OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = Purse_1;             // Bloque de monedero 1
			SetValueBlock(0,0x0A);
			CopyWriteBlocks(MakePurseBlock(),0,BLOCKSIZE,(Purse_2));
			OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = Purse_2;             // Bloque de monedero 2

			SetHistoricalMovements(ReloadValue,miaBono); // 24
			CopyWriteBlocks(MakeHistoricalMovementBlock(),0,BLOCKSIZE,(HISTORICALMOVEMENTS_1));
			OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = HISTORICALMOVEMENTS_1;       // Bloque de historico de movimientos

			SetReleaseContractBlock(miaBono); //40
			CopyWriteBlocks(MakeReleaseContractBlock(),0,BLOCKSIZE,(CONTRACT_1_RELEASE));
			OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = CONTRACT_1_RELEASE;       // Bloque de historico de movimientos

			SetTransitContractBlock(0x00, miaBono, 0x01); //41
			CopyWriteBlocks(MakeTransitContractBlock(),0,BLOCKSIZE,(CONTRACT_1_TRANSIT_0));
			OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = CONTRACT_1_TRANSIT_0;       // Bloque de historico de movimientos
			OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = OPERATIONENDBLOCK;	// Bloque fin de operacion

			OperationCardIn.Increment = ReloadValue;
 
			return TRUE;;			
		}
	}
	else
	{
		switch(SellCard.ActivationFlag)
		{
			case Inactivated:
				//Mostrar Mensaje y Tono
				//printf("Activation Flag -> %d\n",MIA.SellCard.ActivationFlag);
				OperationCardIn.Msg = TarjetaInactiva;
				return false;
				break;

			case Loaded:
				//Mostrar Mensaje y Tono
				//printf("Activation Flag -> %d\n",MIA.SellCard.ActivationFlag);
				OperationCardIn.Msg = TarjetaPrevaluada;
				return false;
				break;

			case Activated:
				//Mostrar Mensaje y Tono
				//printf("Activation Flag -> %d\n",MIA.SellCard.ActivationFlag);
				OperationCardIn.Msg = TarjetaActiva;
				return false;
				break;    

			case Canceled:
				//Mostrar Mensaje y Tono
				//printf("Activation Flag -> %d\n",MIA.SellCard.ActivationFlag);
				OperationCardIn.Msg = TarjetaCancelada;
				return false;
				break;    
			case BlackList:
				//Mostrar Mensaje y Tono
				//printf("Activation Flag -> %d\n",MIA.SellCard.ActivationFlag);
				OperationCardIn.Msg = TarjetaListaNegra;
				return false;
				break;
		}
		return false;
	}
	return 0x00;
}

/******************************************************************************/
/*  actualiza apuntadores y datos antes de escribir en la MIA                 */
/******************************************************************************/
int libMIA::UpdateBlocksPointers(long miaSaldo, long miaBono)
{	int YearNow;
	unsigned char *OperationStartData;
	unsigned char *OperationEndData;
	unsigned char  BlockUidSam[16];

	OperationCardIn.Purse = 0;

	if(SellCard.ActivationFlag == Activated)
	{	if(Initialization.Category == EMPLEADOS)
		{
			OperationCardIn.Msg = TarjetaSinCategoria;
			return false;
		}
		else
		{	if((Initialization.Category == ESPECIALES) && (CustomCard.TitleCode == 151))
			{
				OperationCardIn.Msg = TarjetaSinCategoria;
				return false;
			}
		}

		time_t TimeNow = time(NULL);
		struct tm *Now = localtime(&TimeNow);  // Get Local TIme
		YearNow = ((Now->tm_year + OFFSETYEAR) - STARTYEAR);  // Year 02d

		printf("Saldo Disponible -> $%ld\n",MiaBalance.Purse);
		printf("Bono Disponible -> $%ld\n", MiaBalance.Bonus);

		MiaBalance.RemPurse = miaSaldo;
		MiaBalance.RemBonus = miaBono;

		printf("Saldo a cargar -> $%ld\n", MiaBalance.RemPurse);
		printf("Bono a cargar -> $%ld\n", MiaBalance.RemBonus);

		// Historical Movements Block, Usar SetHistoricalMovements
		HistoricalMovements.DecMonth			= Date2DecadeMonth();
		HistoricalMovements.Day					= Now->tm_mday;
		HistoricalMovements.Hour				= Now->tm_hour;
		HistoricalMovements.Minute				= Now->tm_min;
		HistoricalMovements.Operation			= TITLEPURSEOPERATION;
		HistoricalMovements.SubOperation		= RECHARGE;
		HistoricalMovements.Operator			= SellCard.Operator;
		HistoricalMovements.Line				= init.LINEA;
		HistoricalMovements.Station				= init.ESTACION;
		HistoricalMovements.TravelPeople		= 0;
		HistoricalMovements.InvTravelPeople		= 0;
		HistoricalMovements.EquipmentSell		= SellCard.EquipmentSell;
		HistoricalMovements.ValuedAcerCostumer	= ADDBONO;
		HistoricalMovements.Way					= MiaBalance.RemBonus;   
		HistoricalMovements.Transfer			= NODATA;
		HistoricalMovements.Balance_			= ADDBALANCE;
		HistoricalMovements.AmountCharge		= MiaBalance.RemPurse;    //MiaBalance.ToPay;	//Amout to disscountig

		SetHistoricalRecharge(MiaBalance.RemBonus, MiaBalance.RemPurse); //10    

		MiaBalance.ToPay = MiaBalance.Purse + MiaBalance.RemPurse; 

		Purse.PurseA	= MiaBalance.ToPay;
		Purse.NegPurseA	= ~Purse.PurseA;
		Purse.PurseA1	= Purse.PurseA;
		Purse.AddrA		= Purse.AddrA;
		Purse.NegAddrA	= Purse.NegAddrA;
		Purse.AddrB		= Purse.AddrA;
		Purse.NegAddrB	= Purse.NegAddrA; 

		MiaBalance.RemPurse = Purse.PurseA;

		printf("Nuevo Saldo=%lu\n", Purse.PurseA);

		OperationCardIn.Msg = SaldoRestante;

        // incrementa bono, si lo hay....
		printf("BONOS -> $%ld %ld \n", MiaBalance.RemBonus, miaBono);

		if(miaBono != NOCREDIT)
		{   // incrementa bono
			MiaBalance.RemBonus += MiaBalance.Bonus;

			printf("Nuevo Bono -> $%ld \n", MiaBalance.RemBonus);
			printf("BONO ANTES %d - %ld %ld\n", OperationEnd.ContractBalance1, TransitContract.Balance0, TransitContract.Balance1);

			if(OperationEnd.ContractBalance1)	//save the bonus in the balance
				TransitContract.Balance0 = MiaBalance.RemBonus;
			else
				TransitContract.Balance1 = MiaBalance.RemBonus;

			printf("BONO DESPUES %d - %ld %ld\n", OperationEnd.ContractBalance1, TransitContract.Balance0, TransitContract.Balance1);
		}
		OperationCardIn.Cantidad = miaSaldo;

		//Operation Start Block -> This is a BackUp before work with them
		OperationStart.EndPointers = (
					((OperationEnd.PointerPurse&0x03)*0x10000)			+ ((OperationEnd.PointerRechargePurse&0x03)*0x4000) +
					((OperationEnd.PointerPurseTraffic&0x03)*0x1000)	+ ((OperationEnd.PointerMove&0xf)*0x100) +
					((OperationEnd.PointerOpec&0x03)*0x40)				+ ((OperationEnd.ContractTraffic1&0x1)*0x20) + 
					((OperationEnd.ContractBalance1&0x1)*0x10)			+ ((OperationEnd.ContractTraffic2&0x1)*0x8) +
					((OperationEnd.ContractBalance2&0x1)*0x4)			+ ((OperationEnd.ContractTraffic3&0x1)*0x2) + 
					 (OperationEnd.ContractBalance3&0x1) );

		OperationStart.EndOperation			= OperationStart.Operation;
		OperationStart.EndSubOperation		= OperationStart.SubOperation; 
		OperationStart.EndEspecialFlagsCard	= OperationEnd.EspecialFlagCard;
		OperationStart.EndTransacionNumber	= OperationEnd.TransactionNumber;
		OperationStart.EndPendingPayment	= OperationEnd.AmountPendingPurse;
		OperationStart.MsgSellCardSend		= OperationEnd.MsgVESend;
		OperationEnd.EndRecordOperation		= ((~OperationEnd.EndRecordOperation)&0x01);

		OperationStart.FlagRecordStartOperation = OperationEnd.EndRecordOperation; // Toggle bit must be the same
		OperationStart.SubOperation = RECHARGE;
		OperationStart.RecordCount++; // Increment the Number of records on the card
		OperationStart.EndTransacionNumber++;
		OperationStart.EndEspecialFlagsContract |= 0x200;

		OperationStartData = MakeOperationStartBlock();

		OperationStart.Check8		= ChecksumNibble(OperationStartData,0,0x0f,3,0xff);
		OperationStart.Check8	   &= 0x0F;		//LowNibble;
		OperationStart.CheckBackup	= ChecksumByte(OperationStartData,5,15);

		//Operation End Block
		OperationEnd.SubOperation = RECHARGE;

		// historicos recargas, 2 blocks
		OperationEnd.PointerRechargePurse++;
		if( OperationEnd.PointerRechargePurse > TotalRecharge)
			OperationEnd.PointerRechargePurse = STARTPOINTER;

		// historicos, 12 blocks
		OperationEnd.PointerMove++;	//Increment the pointer
		if(OperationEnd.PointerMove > TotalMovements)
			OperationEnd.PointerMove = STARTPOINTER; //Movement Historic

		OperationEnd.TransactionNumber++;	//Increment the number of transactions
		if(miaBono != NOCREDIT)
			OperationEnd.ContractBalance1 = ((~OperationEnd.ContractBalance1)&0x1); // Toggle Contract Balance

		printf("TRANSIT FINAL %d - %ld %ld\n", OperationEnd.ContractBalance1,
												TransitContract.Balance0, TransitContract.Balance1);

		//Blocks to Write on Card
		OperationCardIn.NumberBlocks2Write = 0;		// Star Count of blocks to write
		OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = OPERATIONSTARTBLOCK;  	// Bloque Inicio de Operacion

		OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = UIDSAMBLOCK;

		if(MiaBalance.ToPay != NOCREDIT)
		{	//Move the Credit to the next Purse
			printf("ToPay=%ld\n", MiaBalance.ToPay);

			OperationCardIn.SourceCreditBlock = PursesBlocks[OperationEnd.PointerPurse-1];

			OperationEnd.PointerPurse++;
			if(OperationEnd.PointerPurse >TotalPurses)
				OperationEnd.PointerPurse = STARTPOINTER;	

			PurseBlock = PursesBlocks[OperationEnd.PointerPurse-1];
			OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = PurseBlock;	//Bloque Monedero
		}

		OperationCardIn.Credits =  MiaBalance;
		OperationEndData = MakeOperationEndBlock();

		OperationEnd.Check9 = ChecksumWord(OperationEndData,2,15);

		HistoricalRechargeBlock  = HistoricalRechargeBlocks[(OperationEnd.PointerRechargePurse-1)];
		HistoricalMovementsBlock = HistoricalMovementsBlocks[(OperationEnd.PointerMove-1)];
		OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = HistoricalRechargeBlock;		//Bloque de Historicos Recarga
		OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = HistoricalMovementsBlock;	//Bloque de Historicos
		OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = CONTRACT_1_TRANSIT_0;		//Bloque Contrato 1 Transito 0

		CopyWriteBlocks(MakeOperationStartBlock(), 0, BLOCKSIZE, OPERATIONSTARTBLOCK);

		//
		memset(BlockUidSam, 0x00, 16);
		memcpy(BlockUidSam, SamUID, 8);
		CopyWriteBlocks(BlockUidSam, 0, BLOCKSIZE, UIDSAMBLOCK);

		CopyWriteBlocks(MakeOperationEndBlock(), 0, BLOCKSIZE, (OPERATIONENDBLOCK));

		if(MiaBalance.ToPay != NOCREDIT)
			CopyWriteBlocks(MakePurseBlock(), 0, BLOCKSIZE, PurseBlock);

		CopyWriteBlocks(MakeHistoricalRecharge(),		0,BLOCKSIZE, HistoricalRechargeBlock);
		CopyWriteBlocks(MakeHistoricalMovementBlock(),	0,BLOCKSIZE, HistoricalMovementsBlock); 		
		CopyWriteBlocks(MakeTransitContractBlock(),		0,BLOCKSIZE, CONTRACT_1_TRANSIT_0);

		OperationCardIn.Blocks[OperationCardIn.NumberBlocks2Write++] = OPERATIONENDBLOCK;	// Bloque Fin de Operacion

		OperationCardIn.Increment = miaSaldo;
		return 0x00;
	}
	return 0x400;		// Error tarjeta no activa
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::ReadBalanceCard(void)
{
	if(Purse.PurseA == Purse.PurseA1)	// Verificar integridad del monedero
	{
		MiaBalance.Purse = Purse.PurseA;
		if(MiaBalance.Purse > MAXMONEDERO)
		{   
			printf("Error en Monedero, excede el maximo permitido-> $%ld\n", MiaBalance.Purse);
			OperationCardIn.Msg = MonederoInvalido;
			return 45;
		}

		if( MiaBalance.Purse < 0)
		{
			printf("Error en Monedero, valor negativo-> $%ld\n", MiaBalance.Purse);
			OperationCardIn.Msg = MonederoInvalido;
			return 46;
		}
	}
	else
	{   
		printf("Error en Monedero $%ld <-> $%ld\n", Purse.PurseA, Purse.PurseA1);
		OperationCardIn.Msg = MonederoInvalido;
		return 47;
	}

	// Guardar en Balance el valor del monedero
	MiaBalance.Purse = Purse.PurseA;
	MiaBalance.RemPurse = Purse.PurseA;
    
	if(!OperationEnd.ContractBalance1)	//load the bonus from the card
	{
		MiaBalance.Bonus = TransitContract.Balance0;
	}else
	{
		MiaBalance.Bonus = TransitContract.Balance1;
	}
	MiaBalance.RemBonus = MiaBalance.Bonus;  //Copy
    return true;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::CreateLogicNumber( void )
{
	sprintf(LogicNumber,"%d%03d%02d%02d%02d%02d%08lu",
			METRORREYID,
			Initialization.MachineInitializer*10 + 1,
			Initialization.Category,
			Initialization.InitializationYear,
			Initialization.InitializationMonth,
			Initialization.InitializationDay,
			Initialization.InitializationSerialNumber);

	return true;
}

//
//
//
int libMIA::PassingInitializationBlock()
{
	//InitializationBlock	4
	Initialization.MapVersion = ((ReadBlocks[InitializationBlock][0]&0xC0) >> 6);
	Initialization.SamVersion = ((ReadBlocks[InitializationBlock][0]&0x38) >> 3);
	Initialization.CodeTitleSam1 = ((ReadBlocks[InitializationBlock][0]&0x07));
	Initialization.CodeTitleSam2 = ((ReadBlocks[InitializationBlock][1]&0xE0) >> 5);
	Initialization.CodeTitleSam3 = ((ReadBlocks[InitializationBlock][1]&0x1C) >> 2);

	Initialization.CountryCode = ((ReadBlocks[InitializationBlock][1]&0x03) << 8) + ReadBlocks[InitializationBlock][2];

	Initialization.State[0] = ReadBlocks[InitializationBlock][3];
	Initialization.State[1] = ReadBlocks[InitializationBlock][4];

	Initialization.City[0] = ReadBlocks[InitializationBlock][5];
	Initialization.City[1] = ReadBlocks[InitializationBlock][6];

	Initialization.ApplicationCode = ReadBlocks[InitializationBlock][7];
	Initialization.MachineInitializer = ReadBlocks[InitializationBlock][8];
	Initialization.InitializationYear = (ReadBlocks[InitializationBlock][9]&0xFE) >> 1;
	Initialization.InitializationMonth = ReadBlocks[InitializationBlock][9]&0x01;
	Initialization.InitializationMonth <<= 3;
	Initialization.InitializationMonth += ((ReadBlocks[InitializationBlock][10]&0xE0) >> 5);
	Initialization.InitializationDay = (ReadBlocks[InitializationBlock][10]&0x1F);
	Initialization.Category = ReadBlocks[InitializationBlock][11];
	Initialization.InitializationSerialNumber = ReadBlocks[InitializationBlock][12];
	Initialization.InitializationSerialNumber <<= 16;
	Initialization.InitializationSerialNumber += ReadBlocks[InitializationBlock][13] << 8; // antes *100
	Initialization.InitializationSerialNumber += ReadBlocks[InitializationBlock][14];
	Initialization.Check4 = ReadBlocks[InitializationBlock][15];

	//CUSTOMBLOCK 5
	CustomCard.ActYear = (ReadBlocks[CUSTOMBLOCK][0]>>1);
	CustomCard.ActMont = ((ReadBlocks[CUSTOMBLOCK][0]&0x01)<<3) + ((ReadBlocks[CUSTOMBLOCK][1]&0xe0)>>5);
	CustomCard.ActDay = (ReadBlocks[CUSTOMBLOCK][1]&0x1f);
	CustomCard.ExpYear = (ReadBlocks[CUSTOMBLOCK][2]>>1);
	CustomCard.ExpMonth = ((ReadBlocks[CUSTOMBLOCK][2]&0x01)<<3) + ((ReadBlocks[CUSTOMBLOCK][3]&0xe0)>>5);
	CustomCard.ExpDay = (ReadBlocks[CUSTOMBLOCK][3]&0x1f);
	CustomCard.OpeCard = ReadBlocks[CUSTOMBLOCK][4]; // Operador Tarjeta
	CustomCard.EmployeeProfile = ReadBlocks[CUSTOMBLOCK][5]; // Perfil Empleado
	CustomCard.PersonalizationFlag = ((ReadBlocks[CUSTOMBLOCK][6]&0xc0)>>6); // Flag Personalizado
	CustomCard.Ruf = (ReadBlocks[CUSTOMBLOCK][6]&0x3f);
	CustomCard.TitleCode = ReadBlocks[CUSTOMBLOCK][7]; // Codigo de titulo
	CustomCard.ContractOperator1= ReadBlocks[CUSTOMBLOCK][8]; //
	CustomCard.ContractOperator2= ReadBlocks[CUSTOMBLOCK][9]; //
	CustomCard.ContractOperator3= ReadBlocks[CUSTOMBLOCK][10]; //
	CustomCard.BirthYear = (ReadBlocks[CUSTOMBLOCK][11]>>1);
	CustomCard.BirthMont = ((ReadBlocks[CUSTOMBLOCK][11]&0x01)<<3) + ((ReadBlocks[CUSTOMBLOCK][12]&0xe0)>>5);
	CustomCard.BirthDay = (ReadBlocks[CUSTOMBLOCK][12]&0x1f);
	/*CustomCard.Language = ReadBlocks[CUSTOMBLOCK][13];
        CustomCard.Language <<= 8;
        CustomCard.Language += ReadBlocks[CUSTOMBLOCK][14];*/
	CustomCard.Language[0] = ReadBlocks[CUSTOMBLOCK][13];
	CustomCard.Language[1] = ReadBlocks[CUSTOMBLOCK][14];
        
	CustomCard.Checksum5 = ReadBlocks[CUSTOMBLOCK][15];

	return 0;
}


int libMIA::PassingCustomBlock()
{
	//CUSTOMBLOCK		5
	CustomCard.ActYear = (ReadBlocks[CUSTOMBLOCK][0]>>1);
	CustomCard.ActMont = ((ReadBlocks[CUSTOMBLOCK][0]&0x01)<<3) + ((ReadBlocks[CUSTOMBLOCK][1]&0xe0)>>5);
	CustomCard.ActDay = (ReadBlocks[CUSTOMBLOCK][1]&0x1f);
	CustomCard.ExpYear = (ReadBlocks[CUSTOMBLOCK][2]>>1);
	CustomCard.ExpMonth = ((ReadBlocks[CUSTOMBLOCK][2]&0x01)<<3) + ((ReadBlocks[CUSTOMBLOCK][3]&0xe0)>>5);
	CustomCard.ExpDay = (ReadBlocks[CUSTOMBLOCK][3]&0x1f);
	CustomCard.OpeCard = ReadBlocks[CUSTOMBLOCK][4]; // Operador Tarjeta
	CustomCard.EmployeeProfile = ReadBlocks[CUSTOMBLOCK][5]; // Perfil Empleado
	CustomCard.PersonalizationFlag = ((ReadBlocks[CUSTOMBLOCK][6]&0xc0)>>6); // Flag Personalizado
	CustomCard.Ruf = (ReadBlocks[CUSTOMBLOCK][6]&0x3f);
	CustomCard.TitleCode = ReadBlocks[CUSTOMBLOCK][7]; // Codigo de titulo	
	CustomCard.ContractOperator1= ReadBlocks[CUSTOMBLOCK][8]; //
	CustomCard.ContractOperator2= ReadBlocks[CUSTOMBLOCK][9]; //
	CustomCard.ContractOperator3= ReadBlocks[CUSTOMBLOCK][10]; //
	CustomCard.BirthYear = (ReadBlocks[CUSTOMBLOCK][11]>>1);
	CustomCard.BirthMont = ((ReadBlocks[CUSTOMBLOCK][11]&0x01)<<3) + ((ReadBlocks[CUSTOMBLOCK][12]&0xe0)>>5);
	CustomCard.BirthDay = (ReadBlocks[CUSTOMBLOCK][12]&0x1f);
	CustomCard.Language[0] = ReadBlocks[CUSTOMBLOCK][13];
	CustomCard.Language[1] = ReadBlocks[CUSTOMBLOCK][14];    
	CustomCard.Checksum5 = ReadBlocks[CUSTOMBLOCK][15];

	return 0;
}

/******************************************************************************/
/* PURSESELLBLOCK    6	                                                      */
/******************************************************************************/
int libMIA::PassingPurseSellBlock()
{
	//PURSESELLBLOCK    6		
	SellCard.SellYear = (ReadBlocks[PURSESELLBLOCK][0]>>1); // 7 bits
	SellCard.SellMont = ((ReadBlocks[PURSESELLBLOCK][0]&0x01)<<3) + ((ReadBlocks[PURSESELLBLOCK][1]&0xe0)>>5); // 4 bits
	SellCard.SellDay = (ReadBlocks[PURSESELLBLOCK][1]&0x1f); // 5 bits
	SellCard.SellHour = ((ReadBlocks[PURSESELLBLOCK][2]&0xf8)>>3); // 5 bits
	SellCard.SellMinute = (((ReadBlocks[PURSESELLBLOCK][2]&0x07)<<3) + ((ReadBlocks[PURSESELLBLOCK][3]&0xe0)>>5)); // 6 bits
	SellCard.Operator = ((ReadBlocks[PURSESELLBLOCK][3]&0x1f)<<3) + ((ReadBlocks[PURSESELLBLOCK][4]&0xe0)>>5); // 8 bits
	SellCard.Line = (ReadBlocks[PURSESELLBLOCK][4]&0x1f); // 14 bits
	SellCard.Line <<= 10;
	SellCard.Line += (ReadBlocks[PURSESELLBLOCK][5]<<1);
	SellCard.Line += ((ReadBlocks[PURSESELLBLOCK][6]&0x80)>>7);
	SellCard.Station = (ReadBlocks[PURSESELLBLOCK][6]&0x7f); // 8 bits
	SellCard.Station <<= 8;
	SellCard.Station += ((ReadBlocks[PURSESELLBLOCK][7]&0xfe)>>1);
	SellCard.EquipmentSell = (ReadBlocks[PURSESELLBLOCK][7]&0x01); // 8 bits
	SellCard.EquipmentSell <<= 7;
	SellCard.EquipmentSell += ((ReadBlocks[PURSESELLBLOCK][8]&0xfe)>>1);
	SellCard.PurseType = (ReadBlocks[PURSESELLBLOCK][8]&0x01); // 3 bits
	SellCard.PurseType <<= 2;
	SellCard.PurseType += ((ReadBlocks[PURSESELLBLOCK][9]&0xc0)>>6);
	SellCard.TypeMachine = ((ReadBlocks[PURSESELLBLOCK][9]&0x3c)>>2); // 4 bits
	SellCard.Ruf = ((ReadBlocks[PURSESELLBLOCK][9]&0x03)<<1); // 3 bits
	SellCard.Ruf += ((ReadBlocks[PURSESELLBLOCK][10]&0x80)>>7);
	SellCard.ActivationFlag = ((ReadBlocks[PURSESELLBLOCK][10]&0x70)>>4);
	SellCard.AmountSell = (ReadBlocks[PURSESELLBLOCK][10]&0x0f);
	SellCard.AmountSell <<= 16;
	SellCard.AmountSell += (ReadBlocks[PURSESELLBLOCK][11]<<8);
	SellCard.AmountSell += ReadBlocks[PURSESELLBLOCK][12];
	SellCard.Deposit = ReadBlocks[PURSESELLBLOCK][13];
	SellCard.Deposit <<= 8;
	SellCard.Deposit += ReadBlocks[PURSESELLBLOCK][14];
	SellCard.Check6 = ReadBlocks[PURSESELLBLOCK][15];

	return 0;
}

/******************************************************************************/
/*  OPERATIONSTARTBLOCK	8                                                     */
/******************************************************************************/
int libMIA::PassingOperationStartBlock()
{
	//OPERATIONSTARTBLOCK	8		
	OperationStart.Check8					= ((ReadBlocks[OPERATIONSTARTBLOCK][0]&0xf0)>>4);	// 4 bits
	OperationStart.Operation				= ((ReadBlocks[OPERATIONSTARTBLOCK][0]&0x0e)>>1);	// 3 bits
	OperationStart.SubOperation				= ((ReadBlocks[OPERATIONSTARTBLOCK][0]&0x01)<<4);	// 5 bits
	OperationStart.SubOperation				+= ((ReadBlocks[OPERATIONSTARTBLOCK][1]&0xf0)>>4);  
	OperationStart.RecordCount				= (ReadBlocks[OPERATIONSTARTBLOCK][1]&0x0f);		// 20 bits
	OperationStart.RecordCount				<<= 16;
	OperationStart.RecordCount				+= (ReadBlocks[OPERATIONSTARTBLOCK][2]<<8); 
	OperationStart.RecordCount				+= ReadBlocks[OPERATIONSTARTBLOCK][3];
	OperationStart.CheckBackup				= ReadBlocks[OPERATIONSTARTBLOCK][4];				// 8 bits
	OperationStart.EndOperation				= ((ReadBlocks[OPERATIONSTARTBLOCK][5]&0xe0)>>5);	// 3 bits
	OperationStart.EndSubOperation			= (ReadBlocks[OPERATIONSTARTBLOCK][5]&0x1f);		// 5 bits
	OperationStart.EndPointers				= ReadBlocks[OPERATIONSTARTBLOCK][6];				// 18 bits
	OperationStart.EndPointers				<<= 10;
	OperationStart.EndPointers				+= (ReadBlocks[OPERATIONSTARTBLOCK][7]<<2);
	OperationStart.EndPointers				+= ((ReadBlocks[OPERATIONSTARTBLOCK][8]&0xc0)>>6);
	OperationStart.EndEspecialFlagsCard		= ((ReadBlocks[OPERATIONSTARTBLOCK][8]&0x38)>>3);	// 3 bits
	OperationStart.EndEspecialFlagsContract = (ReadBlocks[OPERATIONSTARTBLOCK][8]&0x07);		// 12 bits
	OperationStart.EndEspecialFlagsContract <<= 9;
	OperationStart.EndEspecialFlagsContract += (ReadBlocks[OPERATIONSTARTBLOCK][9]<<1);
	OperationStart.EndEspecialFlagsContract += ((ReadBlocks[OPERATIONSTARTBLOCK][10]&0x80)>>7);
	OperationStart.EndTransacionNumber		= (ReadBlocks[OPERATIONSTARTBLOCK][10]&0x7f);		// 16 bits
	OperationStart.EndTransacionNumber		<<= 9;
	OperationStart.EndTransacionNumber		+= (ReadBlocks[OPERATIONSTARTBLOCK][11]<<1);
	OperationStart.EndTransacionNumber		+= ((ReadBlocks[OPERATIONSTARTBLOCK][12]&0x80)>>7);
	OperationStart.EndPendingPayment		= (ReadBlocks[OPERATIONSTARTBLOCK][12]&0x7f);		// 20 bits
	OperationStart.EndPendingPayment		<<= 13;
	OperationStart.EndPendingPayment		+= (ReadBlocks[OPERATIONSTARTBLOCK][13]<<5);
	OperationStart.EndPendingPayment		+= ((ReadBlocks[OPERATIONSTARTBLOCK][14]&0xf8)>>3);
	OperationStart.MsgSellCardSend			= (ReadBlocks[OPERATIONSTARTBLOCK][14]&0x04>>2);
	OperationStart.Ruf						= ((ReadBlocks[OPERATIONSTARTBLOCK][14]&0x03)<<7);	// 9 bits
	OperationStart.Ruf						+= ((ReadBlocks[OPERATIONSTARTBLOCK][15]&0xfe)>>1);
	OperationStart.FlagRecordStartOperation	= (ReadBlocks[OPERATIONSTARTBLOCK][15]&0x01);		// 1 bit

	/* Backup del fin de operacion */
	OperationStart.Backup.Operation			= ((ReadBlocks[OPERATIONSTARTBLOCK][5]&0xe0)>>5);
	OperationStart.Backup.SubOperation		= (ReadBlocks[OPERATIONSTARTBLOCK][5]&0x1f);
	OperationStart.Backup.PointerPurse		= ((ReadBlocks[OPERATIONSTARTBLOCK][6]&0xc0)>>6);		// 2 bits
	OperationStart.Backup.PointerRechargePurse = ((ReadBlocks[OPERATIONSTARTBLOCK][6]&0x30)>>4);// 2 bits
	OperationStart.Backup.PointerPurseTraffic = ((ReadBlocks[OPERATIONSTARTBLOCK][6]&0x0C)>>2); // 2 bits
	OperationStart.Backup.PointerMove		=	((ReadBlocks[OPERATIONSTARTBLOCK][6]&0x03) << 2) + 
												((ReadBlocks[OPERATIONSTARTBLOCK][7]&0xc0) >> 6);	// 4 bits
	OperationStart.Backup.PointerOpec		= ((ReadBlocks[OPERATIONSTARTBLOCK][7]&0x30) >> 6); // 2 bits
	OperationStart.Backup.ContractTraffic1	= ((ReadBlocks[OPERATIONSTARTBLOCK][7]&0x08)>>3); // 1 bit
	OperationStart.Backup.ContractBalance1	= ((ReadBlocks[OPERATIONSTARTBLOCK][7]&0x04)>>2); // 1 bit
	OperationStart.Backup.ContractTraffic2	= ((ReadBlocks[OPERATIONSTARTBLOCK][7]&0x02)>>1); // 1 bit
	OperationStart.Backup.ContractBalance2	=  (ReadBlocks[OPERATIONSTARTBLOCK][7]&0x01); // 1 bit
	OperationStart.Backup.ContractTraffic3	= ((ReadBlocks[OPERATIONSTARTBLOCK][8]&0x80)>>7); // 1 bit
	OperationStart.Backup.ContractBalance3	= ((ReadBlocks[OPERATIONSTARTBLOCK][8]&0x40)>>6); // 1 bit
	OperationStart.Backup.EspecialFlagCard	= ((ReadBlocks[OPERATIONSTARTBLOCK][8]&0x38)>>3);
	OperationStart.Backup.Contract1MsgVESend = ((ReadBlocks[OPERATIONSTARTBLOCK][8]&0x04)>>2); // 1 bit;
	OperationStart.Backup.Contract1TitleState = ((ReadBlocks[OPERATIONSTARTBLOCK][8]&0x02)>>1); // 1 bit;
	OperationStart.Backup.Contract1SpecialFlag = (((ReadBlocks[OPERATIONSTARTBLOCK][8]&0x01)*2) + 
												 ((ReadBlocks[OPERATIONSTARTBLOCK][9]&0x80)>>7));	// 2 bits;
	OperationStart.Backup.Contract2MsgVESend = ((ReadBlocks[OPERATIONSTARTBLOCK][9]&0x40)>>6); // 1 bit
	OperationStart.Backup.Contract2TitleState = ((ReadBlocks[OPERATIONSTARTBLOCK][9]&0x20)>>5); // 1 bit;
	OperationStart.Backup.Contract2SpecialFlag = ((ReadBlocks[OPERATIONSTARTBLOCK][9]&0x18)>>3); // 2 bits;
	OperationStart.Backup.Contract3MsgVESend = ((ReadBlocks[OPERATIONSTARTBLOCK][9]&0x04)>>2); // 1 bit
	OperationStart.Backup.Contract3TitleState = ((ReadBlocks[OPERATIONSTARTBLOCK][9]&0x02)>>1); // 1 bit;
	OperationStart.Backup.Contract3SpecialFlag = (((ReadBlocks[OPERATIONSTARTBLOCK][9]&0x01)*2) + 
												 ((ReadBlocks[OPERATIONSTARTBLOCK][10]&0x80)>>7)); // 2 bits;
	OperationStart.Backup.TransactionNumber = (ReadBlocks[OPERATIONSTARTBLOCK][10]&0x7f); // 16 bits
	OperationStart.Backup.TransactionNumber <<= 9;
	OperationStart.Backup.TransactionNumber += (ReadBlocks[OPERATIONSTARTBLOCK][11]<<1);
	OperationStart.Backup.TransactionNumber += ((ReadBlocks[OPERATIONSTARTBLOCK][12]&0x80)>>7);
	OperationStart.Backup.AmountPendingPurse = (ReadBlocks[OPERATIONSTARTBLOCK][12]&0x7f); // 20 bits
	OperationStart.Backup.AmountPendingPurse  <<= 13;
	OperationStart.Backup.AmountPendingPurse  += (ReadBlocks[OPERATIONSTARTBLOCK][13]<<5);
	OperationStart.Backup.AmountPendingPurse += ((ReadBlocks[OPERATIONSTARTBLOCK][14]&0xf8)>>3);
	OperationStart.Backup.MsgVESend = (ReadBlocks[OPERATIONSTARTBLOCK][14]&0x04>>2);

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::PassingOperationEndBlock()
{
	//OPERATIONENDBLOCK	9
	OperationEnd.Check9				= ReadBlocks[OPERATIONENDBLOCK][0]; // 16 bits
	OperationEnd.Check9				<<= 8;
	OperationEnd.Check9				+= ReadBlocks[OPERATIONENDBLOCK][1];
	OperationEnd.Operation			= ((ReadBlocks[OPERATIONENDBLOCK][2]&0xe0)>>5); // 3 bits
	OperationEnd.SubOperation		= (ReadBlocks[OPERATIONENDBLOCK][2]&0x1f); // 5 bits
	OperationEnd.PointerPurse		= ((ReadBlocks[OPERATIONENDBLOCK][3]&0xc0)>>6); // 2 bits
	//printf("PointerPurse %02x\n",OperationEnd.PointerPurse);
	OperationEnd.PointerRechargePurse= ((ReadBlocks[OPERATIONENDBLOCK][3]&0x30)>>4); // 2 bits
	OperationEnd.PointerPurseTraffic= ((ReadBlocks[OPERATIONENDBLOCK][3]&0x0C)>>2); // 2 bits
	OperationEnd.PointerMove		= (ReadBlocks[OPERATIONENDBLOCK][3]&0x03); // 4 bits
	OperationEnd.PointerMove		<<= 2;
	OperationEnd.PointerMove		+= ((ReadBlocks[OPERATIONENDBLOCK][4]&0xc0)>>6);
	//printf("PointerMove %02x\n",OperationEnd.PointerMove);
	OperationEnd.PointerOpec		= ((ReadBlocks[OPERATIONENDBLOCK][4]&0x30)>>6); // 2 bits
	OperationEnd.ContractTraffic1	= ((ReadBlocks[OPERATIONENDBLOCK][4]&0x08)>>3); // 1 bit
	OperationEnd.ContractBalance1	= ((ReadBlocks[OPERATIONENDBLOCK][4]&0x04)>>2); // 1 bit
	OperationEnd.ContractTraffic2	= ((ReadBlocks[OPERATIONENDBLOCK][4]&0x02)>>1); // 1 bit
	OperationEnd.ContractBalance2	= (ReadBlocks[OPERATIONENDBLOCK][4]&0x01); // 1 bit
	OperationEnd.ContractTraffic3	= ((ReadBlocks[OPERATIONENDBLOCK][5]&0x80)>>7); // 1 bit
	OperationEnd.ContractBalance3	= ((ReadBlocks[OPERATIONENDBLOCK][5]&0x40)>>6); // 1 bit
	OperationEnd.EspecialFlagCard	= ((ReadBlocks[OPERATIONENDBLOCK][5]&0x38)>>3); // 3 bit
//	printf("EspecialFlagCard=%d\n", OperationEnd.EspecialFlagCard);
	OperationEnd.Contract1MsgVESend	= ((ReadBlocks[OPERATIONENDBLOCK][5]&0x04)>>2); // 1 bit
	OperationEnd.Contract1TitleState= ((ReadBlocks[OPERATIONENDBLOCK][5]&0x02)>>1); // 1 bit
	OperationEnd.Contract1SpecialFlag= (((ReadBlocks[OPERATIONENDBLOCK][5]&0x01)*2) + ((ReadBlocks[OPERATIONENDBLOCK][6]&0x80)>>7)); // 2 bits
	OperationEnd.Contract2MsgVESend	= ((ReadBlocks[OPERATIONENDBLOCK][6]&0x40)>>6); // 1 bit
	OperationEnd.Contract2TitleState= ((ReadBlocks[OPERATIONENDBLOCK][6]&0x20)>>5); // 1 bit
	OperationEnd.Contract2SpecialFlag= ((ReadBlocks[OPERATIONENDBLOCK][6]&0x18)>>3); // 2 bits
	OperationEnd.Contract3MsgVESend	= ((ReadBlocks[OPERATIONENDBLOCK][6]&0x04)>>2); // 1 bit
	OperationEnd.Contract3TitleState= ((ReadBlocks[OPERATIONENDBLOCK][6]&0x02)>>1); // 1 bit
	OperationEnd.Contract3SpecialFlag= (((ReadBlocks[OPERATIONENDBLOCK][6]&0x01)*2) + ((ReadBlocks[OPERATIONENDBLOCK][7]&0x80)>>7)); // 2 bits
	OperationEnd.TransactionNumber	= (((ReadBlocks[OPERATIONENDBLOCK][7]&0x7f)*512) + (ReadBlocks[OPERATIONENDBLOCK][8]*0x02) + ((ReadBlocks[OPERATIONENDBLOCK][9]&0x80)>>7)); // 16 bits
	OperationEnd.AmountPendingPurse	= (((ReadBlocks[OPERATIONENDBLOCK][9]&0x7f)*8192) + (ReadBlocks[OPERATIONENDBLOCK][10]*32) + ((ReadBlocks[OPERATIONENDBLOCK][11]&0xF8)>>3)); // 20 bits
	OperationEnd.MsgVESend			= ((ReadBlocks[OPERATIONENDBLOCK][11]&0x04)>>2); // 1 bit
	OperationEnd.Ruf				= ((ReadBlocks[OPERATIONENDBLOCK][11]&0x02)>>1); // 1 bit
	OperationEnd.OpeAutCode			= (ReadBlocks[OPERATIONENDBLOCK][11]&0x01); // 32 bits
	OperationEnd.OpeAutCode			<<= 31;
	OperationEnd.OpeAutCode			+= (ReadBlocks[OPERATIONENDBLOCK][12]*16777216);
	OperationEnd.OpeAutCode			+= (ReadBlocks[OPERATIONENDBLOCK][13]*32768);//0x8000
	OperationEnd.OpeAutCode			+= (ReadBlocks[OPERATIONENDBLOCK][14]*128);
	OperationEnd.OpeAutCode			+= ((ReadBlocks[OPERATIONENDBLOCK][15]&0xfe)>>1);
	OperationEnd.EndRecordOperation	= (ReadBlocks[OPERATIONENDBLOCK][15]&0x01); // 1 bit

	return 0;
}

/******************************************************************************/
/*  Purse_1 13 o Purse_2 14                                                   */
/******************************************************************************/
int libMIA::PassingPurseBlock()
{
//	printf("******************PointerPurse=%d***************\n",OperationEnd.PointerPurse);
	//Purse_1 13 o Purse_2 14
	Purse.PurseA  =   ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][0]&0xFF;
	Purse.PurseA += ((ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][1]&0xFF) << 8);
	Purse.PurseA += ((ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][2]&0xFF) << 16);
	Purse.PurseA += ((ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][3]&0xFF) << 24);

	Purse.NegPurseA  =   ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][4]&0xFF;
	Purse.NegPurseA += ((ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][5]&0xFF) << 8);
	Purse.NegPurseA += ((ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][6]&0xFF) << 16);
	Purse.NegPurseA += ((ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][7]&0xFF) << 24);

	Purse.PurseA1  =   ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][8]&0xFF;
	Purse.PurseA1 += ((ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][9]&0xFF) << 8);
	Purse.PurseA1 += ((ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][10]&0xFF) << 16);
	Purse.PurseA1 += ((ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][11]&0xFF) << 24);

	Purse.AddrA		= ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][12];
	Purse.NegAddrA	= ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][13];
	Purse.AddrA		= ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][14];
	Purse.NegAddrA	= ReadBlocks[PursesBlocks[OperationEnd.PointerPurse-1]][15];

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::PassingContract_1_ReleaseBlock()
{
	//CONTRACT_1_RELEASE	40
	ReleaseContract.Version				= ReadBlocks[CONTRACT_1_RELEASE][0]&0xC0;
	ReleaseContract.Version			  >>= 6;
	ReleaseContract.TitleCode			= ReadBlocks[CONTRACT_1_RELEASE][0]&0x3F;
	ReleaseContract.TitleCode		  <<= 2;
	ReleaseContract.TitleCode		   += ((ReadBlocks[CONTRACT_1_RELEASE][1]&0xC0) >> 6);
	ReleaseContract.TypeMachine			= ((ReadBlocks[CONTRACT_1_RELEASE][1]&0x3C) >> 2);
	ReleaseContract.Line				= ReadBlocks[CONTRACT_1_RELEASE][1]&0x03;
	ReleaseContract.Line			  <<= 3;
	ReleaseContract.Line			   += ((ReadBlocks[CONTRACT_1_RELEASE][2]&0xE0) >> 5);
	ReleaseContract.Station				= (ReadBlocks[CONTRACT_1_RELEASE][2]&0x1F);
	ReleaseContract.Station			  <<= 1;
	ReleaseContract.Station			   += ((ReadBlocks[CONTRACT_1_RELEASE][3]&0x80) >> 7);
	ReleaseContract.Ruf					= ReadBlocks[CONTRACT_1_RELEASE][3]&0x40;
	ReleaseContract.Ruf				  >>= 6;
	ReleaseContract.Machine				= ReadBlocks[CONTRACT_1_RELEASE][3]&0x3F;
	ReleaseContract.Machine			  <<= 2;
	ReleaseContract.Machine			   += ((ReadBlocks[CONTRACT_1_RELEASE][4]&0xC0) >> 6); 
	ReleaseContract.VLSN				= ReadBlocks[CONTRACT_1_RELEASE][4]&0x3F;
	ReleaseContract.VLSN			  <<= 10;
	ReleaseContract.VLSN			   += ReadBlocks[CONTRACT_1_RELEASE][5] << 2;
	ReleaseContract.VLSN			   += ((ReadBlocks[CONTRACT_1_RELEASE][6]&0xC0) >> 6);
	ReleaseContract.GeographicalCriteria= ReadBlocks[CONTRACT_1_RELEASE][6]&0x38;
	ReleaseContract.GeographicalCriteria>>= 3;	
	ReleaseContract.Destination_Line	= ReadBlocks[CONTRACT_1_RELEASE][6]&0x07;
	ReleaseContract.Destination_Line  <<= 2;
	ReleaseContract.Destination_Line   += ((ReadBlocks[CONTRACT_1_RELEASE][7]&0xc0)>>6);
	ReleaseContract.Destination_Station = (ReadBlocks[CONTRACT_1_RELEASE][7]&0x3f);
	ReleaseContract.Destination_Zone	= (ReadBlocks[CONTRACT_1_RELEASE][6]&0x07);
	ReleaseContract.Destination_Zone  <<= 8;
	ReleaseContract.Destination_Zone   += ReadBlocks[CONTRACT_1_RELEASE][7];
	ReleaseContract.DateType			= (ReadBlocks[CONTRACT_1_RELEASE][8]&0xe0);
	ReleaseContract.DateType		  >>= 5;
	ReleaseContract.Type_Year			= (ReadBlocks[CONTRACT_1_RELEASE][8]&0x1f);
	ReleaseContract.Type_Year		  <<= 2;
	ReleaseContract.Type_Year		   += ((ReadBlocks[CONTRACT_1_RELEASE][9]&0xc0)>>6);
	ReleaseContract.Type_Month			= (ReadBlocks[CONTRACT_1_RELEASE][9]&0x3c);
	ReleaseContract.Type_Month		  >>= 2;
	ReleaseContract.Type_Day			= (ReadBlocks[CONTRACT_1_RELEASE][9]&0x03);
	ReleaseContract.Type_Day		  <<= 3;
	ReleaseContract.Type_Day		   += ((ReadBlocks[CONTRACT_1_RELEASE][10]&0xe0)>>5);
	ReleaseContract.Type_Hour			= (ReadBlocks[CONTRACT_1_RELEASE][10]&0x1f);
	ReleaseContract.Type_Minute			= (ReadBlocks[CONTRACT_1_RELEASE][11]&0xfc)>>2;
	ReleaseContract.BalanceType			= (ReadBlocks[CONTRACT_1_RELEASE][11]&0x03);
	ReleaseContract.NominalSell			= ReadBlocks[CONTRACT_1_RELEASE][12];
	ReleaseContract.NominalSell		  <<= 9;
	ReleaseContract.NominalSell		   +=  (ReadBlocks[CONTRACT_1_RELEASE][13]<<1);
	ReleaseContract.NominalSell		   += ((ReadBlocks[CONTRACT_1_RELEASE][14]&0x80)>>7);		
	ReleaseContract.FacialSell			= (ReadBlocks[CONTRACT_1_RELEASE][14]&0x7f);
	ReleaseContract.FacialSell		  <<= 8;
	ReleaseContract.FacialSell		   += ReadBlocks[CONTRACT_1_RELEASE][15];

	return 0;
}

int libMIA::PassingContract_1_Transit_0_Block()
{
        //CONTRACT_1_TRANSIT_0	41
	TransitContract.Balance0 = ReadBlocks[CONTRACT_1_TRANSIT_0][0];
	TransitContract.Balance0 <<= 12;
	TransitContract.Balance0 += (ReadBlocks[CONTRACT_1_TRANSIT_0][1]<<4);
	TransitContract.Balance0 += ((ReadBlocks[CONTRACT_1_TRANSIT_0][2]&0xf0)>>4);
	TransitContract.Recharge0DateType = (ReadBlocks[CONTRACT_1_TRANSIT_0][2]&0x0c);
	TransitContract.Recharge0DateType >>= 2;		
	TransitContract.Recharge0Year = (ReadBlocks[CONTRACT_1_TRANSIT_0][2]&0x03);
	TransitContract.Recharge0Year <<= 5;
	TransitContract.Recharge0Year += ((ReadBlocks[CONTRACT_1_TRANSIT_0][3]&0xf8)>>3);
	TransitContract.Recharge0Month = ReadBlocks[CONTRACT_1_TRANSIT_0][3]&0x07;
	TransitContract.Recharge0Month <<= 1;
	TransitContract.Recharge0Month += ((ReadBlocks[CONTRACT_1_TRANSIT_0][4]&0x80)>>7);
	TransitContract.Rechar0Day = (ReadBlocks[CONTRACT_1_TRANSIT_0][4]&0x7c);
	TransitContract.Rechar0Day >>= 2;
	TransitContract.LastRecharge0Index = (ReadBlocks[CONTRACT_1_TRANSIT_0][4]&0x03);
	TransitContract.Ruf0 = ReadBlocks[CONTRACT_1_TRANSIT_0][5];
	TransitContract.Ruf0 <<= 16;
	TransitContract.Ruf0 += (ReadBlocks[CONTRACT_1_TRANSIT_0][6]*100);
	TransitContract.Ruf0  += ReadBlocks[CONTRACT_1_TRANSIT_0][7];
	TransitContract.Balance1 = ReadBlocks[CONTRACT_1_TRANSIT_0][8];
	TransitContract.Balance1 <<= 12;
	TransitContract.Balance1 += (ReadBlocks[CONTRACT_1_TRANSIT_0][9]<<4);
	TransitContract.Balance1 += ((ReadBlocks[CONTRACT_1_TRANSIT_0][10]&0xf0)>>4);
	TransitContract.Recharge1DateType = (ReadBlocks[CONTRACT_1_TRANSIT_0][10]&0x0c);
	TransitContract.Recharge1DateType >>= 2;		
	TransitContract.Recharge1Year = (ReadBlocks[CONTRACT_1_TRANSIT_0][10]&0x03);
	TransitContract.Recharge1Year <<= 5;
	TransitContract.Recharge1Year += ((ReadBlocks[CONTRACT_1_TRANSIT_0][11]&0xf8)>>3);
	TransitContract.Recharge1Month = ReadBlocks[CONTRACT_1_TRANSIT_0][11]&0x07;
	TransitContract.Recharge1Month <<= 1;
	TransitContract.Recharge1Month += ((ReadBlocks[CONTRACT_1_TRANSIT_0][12]&0x80)>>7);
	TransitContract.Rechar1Day = (ReadBlocks[CONTRACT_1_TRANSIT_0][12]&0x7c);
	TransitContract.Rechar1Day >>= 2;
	TransitContract.LastRecharge1Index = (ReadBlocks[CONTRACT_1_TRANSIT_0][4]&0x03);
	TransitContract.Ruf1 = ReadBlocks[CONTRACT_1_TRANSIT_0][13];
	TransitContract.Ruf1 <<= 16;
	TransitContract.Ruf1 += (ReadBlocks[CONTRACT_1_TRANSIT_0][14]*100);
	TransitContract.Ruf1  += ReadBlocks[CONTRACT_1_TRANSIT_0][15];
	
        return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::PassingHistoricalRecharge(HistoricalRechargebits *HistoricalRecharge, int block)
{    
	HistoricalRecharge->Year			=  (ReadBlocks[block][0]&0xFE)>>1;
	HistoricalRecharge->Mont			= ((ReadBlocks[block][0]&0x01)<<3) + ((ReadBlocks[block][1]&0xE0)>>5);
	HistoricalRecharge->Day				=  (ReadBlocks[block][1]&0x1F);
	HistoricalRecharge->Hour			=  (ReadBlocks[block][2]&0xF8)>>3;
	HistoricalRecharge->Minute			= ((ReadBlocks[block][2]&0x07)<<3)+((ReadBlocks[block][3]&0xE0)>>5);
	HistoricalRecharge->Operator		= ((ReadBlocks[block][3]&0x1F)<<3)+((ReadBlocks[block][4]&0xE0)>>5);
	HistoricalRecharge->Line			= ((ReadBlocks[block][4]&0x1F)<<9)+((ReadBlocks[block][5]&0xFF)<<1)+((ReadBlocks[block][6]&0x80)>>7);
	HistoricalRecharge->Station			= ((ReadBlocks[block][6]&0x7F)<<7)+((ReadBlocks[block][7]&0xFE)>>1);
	HistoricalRecharge->MachineType		= ((ReadBlocks[block][7]&0x01)<<3)+((ReadBlocks[block][8]&0xE0)>>5);
	HistoricalRecharge->IssuingMachine	= ((ReadBlocks[block][8]&0x1F)<<3)+((ReadBlocks[block][9]&0xE0)>>5);
	HistoricalRecharge->Ruf				=  (ReadBlocks[block][9]&0x10)>>5;
	HistoricalRecharge->ReloadValue		= ((ReadBlocks[block][9]&0x0F)<<16)+((ReadBlocks[block][10]&0xFF)<<8)+((ReadBlocks[block][11]&0xFF));
	HistoricalRecharge->PurseBalance	= ((ReadBlocks[block][12]&0x0F)<<24)+((ReadBlocks[block][13]&0xFF)<<16)+ 
										  ((ReadBlocks[block][14]&0xFF)<<8)+((ReadBlocks[block][15]&0xFF));
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::PassingHistoricalRechargeMovements()
{
	if(OperationEnd.PointerRechargePurse == 1)
	{
		PassingHistoricalRecharge(&HistoricalRecharge1,HistoricalRechargeBlocks[0]);
		PassingHistoricalRecharge(&HistoricalRecharge2,HistoricalRechargeBlocks[1]);
    }
	else
	{
		PassingHistoricalRecharge(&HistoricalRecharge1,HistoricalRechargeBlocks[0]);
		PassingHistoricalRecharge(&HistoricalRecharge2,HistoricalRechargeBlocks[1]);
	}
	return 0;
}

int libMIA::PassingHistorical(HistoricalMovementsbits *HistMovTemp, int block)
{
	HistMovTemp->DecMonth			= 	 (ReadBlocks[block][0]&0xFE) >> 1; // 7 bits
	HistMovTemp->Day				= 	((ReadBlocks[block][0]&0x01) << 4) + ((ReadBlocks[block][1]&0xF0) >> 4); // 5 bits
	HistMovTemp->Hour				= 	((ReadBlocks[block][1]&0x0F) << 1) + ((ReadBlocks[block][2]&0x80)>>7); // 5 bits
	HistMovTemp->Minute				= 	((ReadBlocks[block][2]&0x7E) >> 1); // 6 bits
	HistMovTemp->Operation			=	((ReadBlocks[block][2]&0x01) << 2) + ((ReadBlocks[block][3]&0xC0) >> 6); // 3 bits
	HistMovTemp->SubOperation		=	((ReadBlocks[block][3]&0x3E) >> 1); // 5 bits
	HistMovTemp->Operator			=	((ReadBlocks[block][3]&0x01) << 7) + ((ReadBlocks[block][4]&0xFE) >> 1); // 8 bits
	HistMovTemp->Line				=	((ReadBlocks[block][4]&0x01) << 13) + ((ReadBlocks[block][5]&0xFF) << 5) + ((ReadBlocks[block][6]&0xF8) >> 3); // 14 bits
	HistMovTemp->Station			=	((ReadBlocks[block][6]&0x07) << 11) + ((ReadBlocks[block][7]&0xFF) << 3) + ((ReadBlocks[block][8]&0xE0) >> 5); // 14 bits
	HistMovTemp->TravelPeople		=	((ReadBlocks[block][8]&0x18) >> 3); // 2 bits
	HistMovTemp->InvTravelPeople	=	((ReadBlocks[block][8]&0x06) >> 1); // 2 bits
	HistMovTemp->EquipmentSell		=	((ReadBlocks[block][8]&0x01) << 7) + ((ReadBlocks[block][9]&0xFE) >> 1); // 8 bits
	HistMovTemp->ValuedAcerCostumer	= 	((ReadBlocks[block][9]&0x01) << 1) + ((ReadBlocks[block][10]&0x80) >> 7); // 2 bits
	HistMovTemp->Way				=	((ReadBlocks[block][10]&0x7F) << 7) + ((ReadBlocks[block][11]&0xFE) >> 1); // 14 bits
	HistMovTemp->Transfer			=	((ReadBlocks[block][11]&0x01)); // 1 bit
	HistMovTemp->Balance_			=	((ReadBlocks[block][12]&0xFF) << 9) + ((ReadBlocks[block][13]&0xFF) << 1) + ((ReadBlocks[block][14]&0x80) >> 7); // 17 bits
	HistMovTemp->AmountCharge		= 	((ReadBlocks[block][14]&0x7F) << 8) + ((ReadBlocks[block][15]&0xFF)); // 15 bits

	return 0;
}

int libMIA::PassingHistoricalMovements()
{
/*  HISTORICALMOVEMENTS_1	24
    HISTORICALMOVEMENTS_2	25
    HISTORICALMOVEMENTS_3	26
    HISTORICALMOVEMENTS_4	28
    HISTORICALMOVEMENTS_5	29
    HISTORICALMOVEMENTS_6	30
    HISTORICALMOVEMENTS_7	32
    HISTORICALMOVEMENTS_8	33
    HISTORICALMOVEMENTS_9	34
    HISTORICALMOVEMENTS_10	36
    HISTORICALMOVEMENTS_11	37
    HISTORICALMOVEMENTS_12	38
 */
    int count = OperationEnd.PointerMove - 1;
    int HistMovBlocks[12];
    int block = 0;
    
    /* Esto ordena los bloques del historico de movimientos*/
    for(int i = 0; i < 12; i++){
	HistMovBlocks[i] = HistoricalMovementsBlocks[count--];
		
	if(count == -1)
            count = 11;
    }
    
    block = 0;
    PassingHistorical(&HistoricalMovements01,HistMovBlocks[block++]);
    PassingHistorical(&HistoricalMovements02,HistMovBlocks[block++]);
    PassingHistorical(&HistoricalMovements03,HistMovBlocks[block++]);
    PassingHistorical(&HistoricalMovements04,HistMovBlocks[block++]);
    PassingHistorical(&HistoricalMovements05,HistMovBlocks[block++]);
    PassingHistorical(&HistoricalMovements06,HistMovBlocks[block++]);
    PassingHistorical(&HistoricalMovements07,HistMovBlocks[block++]);
    PassingHistorical(&HistoricalMovements08,HistMovBlocks[block++]);
    PassingHistorical(&HistoricalMovements09,HistMovBlocks[block++]);
    PassingHistorical(&HistoricalMovements10,HistMovBlocks[block++]);
    PassingHistorical(&HistoricalMovements11,HistMovBlocks[block++]);
    PassingHistorical(&HistoricalMovements12,HistMovBlocks[block++]);

    return 0;
}

int libMIA::PassingLastTransit(LastTransitbits * LastTransit, int block){
    
    LastTransit->Gar             = ((ReadBlocks[block][0]&0xE0)>>5); // 3 bits
    LastTransit->DecMonth        = ((ReadBlocks[block][0]&0x1F) << 2)+((ReadBlocks[block][1]&0xC0)>>6); // 7 bits
    LastTransit->Day             = ((ReadBlocks[block][1]&0x3E)>>1); // 5 bits
    LastTransit->Hour            = ((ReadBlocks[block][1]&0x01)<<4)+((ReadBlocks[block][2]&0xF0)>>4); // 5 bits
    LastTransit->Minute          = ((ReadBlocks[block][2]&0x0F)<<2)+((ReadBlocks[block][3]&0xC0)>>6); // 6 bits
    LastTransit->Operator        = ((ReadBlocks[block][3]&0x3F)<<1)+((ReadBlocks[block][4]&0xC0)>>6); // 8 bits
    LastTransit->Line            = ((ReadBlocks[block][4]&0x3F)<<8)+(ReadBlocks[block][5]&0xFF);
    LastTransit->Station         = ((ReadBlocks[block][6]&&0xFF)<<6)+((ReadBlocks[block][7]&0xFC)>>2);
    LastTransit->EquipmentSell   = ((ReadBlocks[block][7]&0x03)<<12)+((ReadBlocks[block][8]&0xFF)<<4)+((ReadBlocks[block][9]&0xF0)>>4);
    LastTransit->Way             = ((ReadBlocks[block][9]&0x0F)<<10)+((ReadBlocks[block][10]&0xFF)<<2)+((ReadBlocks[block][11]&0xC0)>>6);
    LastTransit->Bus             = ((ReadBlocks[block][11]&0x3F)<<8)+(ReadBlocks[block][12]&0xFF);
    LastTransit->Ruf             = (ReadBlocks[block][13]&0xF0)>>4;
    LastTransit->TravelAmount    = ((ReadBlocks[block][13]&0x0F)<<16)+((ReadBlocks[block][14]&0xFF)<<8)+(ReadBlocks[block][15]&0xFF);
	return 0;    
}

int libMIA::PassingLastTransitFinal(LastTransitFinalbits * LastTransitFinal, int block){
    
    //memcpy(LastTransitFinal->Ruf,block,10);
    
    LastTransitFinal->CountTransfers = (ReadBlocks[block][10]&0xC0)>>6;
    LastTransitFinal->CountConsecutiveIn = (ReadBlocks[block][10]&0x3F);
    LastTransitFinal->CountConsecutiveExit = (ReadBlocks[block][11]&0xFC)>>2;
    LastTransitFinal->CounterTogetherTravelers = ((ReadBlocks[block][11]&0x03)<<4) + ((ReadBlocks[block][12]&0xF0)>>4);
    LastTransitFinal->PreviousTripAmount = ((ReadBlocks[block][12]&0x0F)<<16)+((ReadBlocks[block][13]&0xFF)<<8)+(ReadBlocks[block][14]&0xFF);
    LastTransitFinal->LastOperator = (ReadBlocks[block][15]&0xFF);
	return 0;
}

int libMIA::PassingHistoricalLastTransit()
{
/*  
    LASTTRANSIT_1	16
    LASTTRANSITFINAL_1	17
    LASTTRANSIT_2	18
    LASTTRANSITFINAL_2	20
    LASTTRANSIT_3	21
    LASTTRANSITFINAL_3  22
    
 */
    int count;
    int LastTransitBlocks[3];
    int LastTransitFinalBlocks[3];
    int block = 0;
    
    count = OperationEnd.PointerPurseTraffic;
    
    /* Esto ordena los bloques del historico de movimientos*/
    for(int i = 0; i < TotalTransit; i++){
	LastTransitBlocks[i] = TransitBlocks[count];
	LastTransitFinalBlocks[i] = TransitFinalBlocks[count--];
        
	if(count == -1)
            count = 2;
    }
    
    block = 0;
    PassingLastTransit(&LastTransit01,LastTransitBlocks[block]);
    PassingLastTransitFinal(&LastTransitFinal01,LastTransitFinalBlocks[block++]);
    PassingLastTransit(&LastTransit02,LastTransitBlocks[block]);
    PassingLastTransitFinal(&LastTransitFinal02,LastTransitFinalBlocks[block++]);
    PassingLastTransit(&LastTransit03,LastTransitBlocks[block]);
    PassingLastTransitFinal(&LastTransitFinal03,LastTransitFinalBlocks[block++]);
    
    return 0;
}

int libMIA::PassingCustomPrivateData(CustomPrivateDatabits *CustomPrivateDataTemp)
{
    /*
        CUSTOMPRIVATEDATA_1	56    
        CUSTOMPRIVATEDATA_2	57    
        CUSTOMPRIVATEDATA_3	58   
     */
    memcpy(CustomPrivateDataTemp->CompanyCode,ReadBlocks[CUSTOMPRIVATEDATA_1],12);
    memcpy(CustomPrivateDataTemp->EmployeePosition,ReadBlocks[CUSTOMPRIVATEDATA_1]+12,4);
    memcpy(CustomPrivateDataTemp->RUF1,ReadBlocks[CUSTOMPRIVATEDATA_2],2);
    CustomPrivateDataTemp->PwdSize = (ReadBlocks[CUSTOMPRIVATEDATA_2][2]&0xF0)>>4;
    CustomPrivateDataTemp->LoginSize = ReadBlocks[CUSTOMPRIVATEDATA_2][2]&0x0F;
    memcpy(CustomPrivateDataTemp->Login,ReadBlocks[CUSTOMPRIVATEDATA_2]+3,4);
    memcpy(CustomPrivateDataTemp->Password,ReadBlocks[CUSTOMPRIVATEDATA_2]+7,4);
    CustomPrivateDataTemp->CardType = ReadBlocks[CUSTOMPRIVATEDATA_2][11];
    memcpy(CustomPrivateDataTemp->HIWUSR,ReadBlocks[CUSTOMPRIVATEDATA_2]+12,4);
    memcpy(CustomPrivateDataTemp->Deparment,ReadBlocks[CUSTOMPRIVATEDATA_3],6);

    CustomPrivateDataTemp->EmployeeCode = 
            (ReadBlocks[CUSTOMPRIVATEDATA_3][6]<<24) +
            (ReadBlocks[CUSTOMPRIVATEDATA_3][7]<<16) +
            (ReadBlocks[CUSTOMPRIVATEDATA_3][8]<<8) +
            ReadBlocks[CUSTOMPRIVATEDATA_3][9];
    
    memcpy(CustomPrivateDataTemp->RUF2,ReadBlocks[CUSTOMPRIVATEDATA_3]+10,6);
    return 0;
}

int libMIA::PassingCustomPublicData(CustomPublicDatabits *CustomPublicDataTemp)
{
/*
    CUSTOMPUBLICDATA_1	60    
    CUSTOMPUBLICDATA_2	61    
    CUSTOMPUBLICDATA_3	62   
*/
    CustomPublicDataTemp->Gender = ReadBlocks[CUSTOMPUBLICDATA_1][0]>>4;
    CustomPublicDataTemp->NIDType = ReadBlocks[CUSTOMPUBLICDATA_1][0]&0x0F;
    
    CustomPublicDataTemp->NID[0] = ReadBlocks[CUSTOMPUBLICDATA_1][1];
    CustomPublicDataTemp->NID[1] = ReadBlocks[CUSTOMPUBLICDATA_1][2];
    CustomPublicDataTemp->NID[2] = ReadBlocks[CUSTOMPUBLICDATA_1][3];
    CustomPublicDataTemp->NID[3] = ReadBlocks[CUSTOMPUBLICDATA_1][4];
    CustomPublicDataTemp->NID[4] = ((ReadBlocks[CUSTOMPUBLICDATA_1][5]&0xF0)>>4)+0x30;
    CustomPublicDataTemp->NID[5] = ((ReadBlocks[CUSTOMPUBLICDATA_1][5]&0x0F))+0x30;
    CustomPublicDataTemp->NID[6] = ((ReadBlocks[CUSTOMPUBLICDATA_1][6]&0xF0)>>4)+0x30;
    CustomPublicDataTemp->NID[7] = ((ReadBlocks[CUSTOMPUBLICDATA_1][6]&0x0F))+0x30;
    CustomPublicDataTemp->NID[8] = ((ReadBlocks[CUSTOMPUBLICDATA_1][7]&0xF0)>>4)+0x30;
    CustomPublicDataTemp->NID[9] = ((ReadBlocks[CUSTOMPUBLICDATA_1][7]&0x0F))+0x30;
    CustomPublicDataTemp->NID[10] = ReadBlocks[CUSTOMPUBLICDATA_1][8];
    CustomPublicDataTemp->NID[11] = ReadBlocks[CUSTOMPUBLICDATA_1][9];
    CustomPublicDataTemp->NID[12] = ReadBlocks[CUSTOMPUBLICDATA_1][10];
    CustomPublicDataTemp->NID[13] = ReadBlocks[CUSTOMPUBLICDATA_1][11];
    CustomPublicDataTemp->NID[14] = ReadBlocks[CUSTOMPUBLICDATA_1][12];
    CustomPublicDataTemp->NID[15] = ReadBlocks[CUSTOMPUBLICDATA_1][13];
    CustomPublicDataTemp->NID[16] = ((ReadBlocks[CUSTOMPUBLICDATA_1][14]&0xF0)>>4)+0x30;
    CustomPublicDataTemp->NID[17] = ((ReadBlocks[CUSTOMPUBLICDATA_1][14]&0x0F))+0x30;
    CustomPublicDataTemp->NID[18] = 0x03;
    
    memcpy(CustomPublicDataTemp->Name_1,ReadBlocks[CUSTOMPUBLICDATA_2],16);
    memcpy(CustomPublicDataTemp->Name_2,ReadBlocks[CUSTOMPUBLICDATA_3],16);
    return 0;
}

unsigned char * libMIA::MakeOperationStartBlock()
{
    static unsigned char OperationStartData[16];
    memset(OperationStartData,0x00,16);	// pone en 0 todo el vector
    unsigned char *OperationStartDataReturn;
    Check   MiaCheck;
    unsigned char LowNibble = 0x0F;
    OperationStartDataReturn = &OperationStartData[0];
    //cout << "MakeOperationStart" << endl;

    OperationStartData[0] = (((OperationStart.Check8&0x0f)<<4) + ((OperationStart.Operation&0x07)<<1) + ((OperationStart.SubOperation&0x10)>>4));

    OperationStartData[1] = (((OperationStart.SubOperation&0x0f)<<4) + ((OperationStart.RecordCount&0xf0000)>>16));
    OperationStartData[2] = ((OperationStart.RecordCount&0xff00)>>8);
    OperationStartData[3] = (OperationStart.RecordCount&0xff);
    OperationStartData[4] = OperationStart.CheckBackup;
    OperationStartData[5] = (((OperationStart.EndOperation&0x07)<<5) + (OperationStart.EndSubOperation&0x1f));
    OperationStartData[6] =  ((OperationStart.EndPointers&0x3fc00)>>10);
    OperationStartData[7] =  ((OperationStart.EndPointers&0x3fc)>>2);
    OperationStartData[8] = (((OperationStart.EndPointers&0x3)<<6) + ((OperationStart.EndEspecialFlagsCard&0x7)<<3) + ((OperationStart.EndEspecialFlagsContract&0xe00)>>9));
    OperationStartData[9] = ((OperationStart.EndEspecialFlagsContract&0x1fe)>>1);
    OperationStartData[10] = (((OperationStart.EndEspecialFlagsContract&0x1)<<7) + ((OperationStart.EndTransacionNumber&0xfe00)>>9));
    OperationStartData[11] = ((OperationStart.EndTransacionNumber&0x1fe)>>1);
    OperationStartData[12] = (((OperationStart.EndTransacionNumber&0x1)<<7) + ((OperationStart.EndPendingPayment&0xfe000)>>13));
    OperationStartData[13] = ((OperationStart.EndPendingPayment&0x1fe0)>>5);
    OperationStartData[14] = (((OperationStart.EndPendingPayment&0x1f)<<3) + ((OperationStart.MsgSellCardSend&0x01)<<2) + 
                             ((OperationStart.Ruf&0x180)>>7));
    OperationStartData[15] = (((OperationStart.Ruf&0x7f)<<1) + (OperationStart.FlagRecordStartOperation&0x01));

    
    MiaCheck.Check8 = ChecksumNibble(OperationStartData,0,0x0f,3,0xff);
    MiaCheck.Check8 &= LowNibble;
    OperationStart.Check8 = MiaCheck.Check8;
    OperationStartData[0] = (((OperationStart.Check8&0x0f)<<4) + ((OperationStart.Operation&0x07)<<1) + ((OperationStart.SubOperation&0x10)>>4));

	MiaCheck.CheckBackup = ChecksumByte(OperationStartData,5,15);
    OperationStartData[4] = MiaCheck.CheckBackup;

    return OperationStartDataReturn;
}


unsigned char * libMIA::MakeOperationEndBlock()
{
	static unsigned char OperationEndData[16];
        Check   MiaCheck;
	unsigned char *OperationEndDataReturn;
        
        memset(OperationEndData,0x00,16);	// pone en 0 todo el vector
	OperationEndDataReturn = &OperationEndData[0];

	OperationEndData[0] = ((OperationEnd.Check9&0xff00)>>8);
	OperationEndData[1] = (OperationEnd.Check9&0x00ff);
	OperationEndData[2] = (((OperationEnd.Operation&0x07)<<5) + (OperationEnd.SubOperation&0x1f));
	OperationEndData[3] = (((OperationEnd.PointerPurse&0x03)<<6) + ((OperationEnd.PointerRechargePurse&0x03)<<4) +
			      ((OperationEnd.PointerPurseTraffic&0x03)<<2) + ((OperationEnd.PointerMove&0x0c)>>2));
	OperationEndData[4] = (((OperationEnd.PointerMove&0x03)<<6) + ((OperationEnd.PointerOpec&0x03)<<4) + ((OperationEnd.ContractTraffic1&0x01)<<3) +
			      ((OperationEnd.ContractBalance1&0x01)<<2) +  ((OperationEnd.ContractTraffic2&0x01)<<1) +
			      (OperationEnd.ContractBalance2&0x01));
	OperationEndData[5] = (((OperationEnd.ContractTraffic3&0x01)<<7) + ((OperationEnd.ContractBalance3&0x01)<<6) +
			      ((OperationEnd.EspecialFlagCard&0x07)<<3) +
			      ((OperationEnd.Contract1MsgVESend&0x01)<<2) + ((OperationEnd.Contract1TitleState&0x01)<<1)+
			      ((OperationEnd.Contract1SpecialFlag&0x02)>>1));
	OperationEndData[6] = (((OperationEnd.Contract1SpecialFlag&0x01)<<7) + ((OperationEnd.Contract2MsgVESend&0x01)<<6) +
                              ((OperationEnd.Contract2TitleState&0x01)<<5) +
                              ((OperationEnd.Contract2SpecialFlag&0x03)<<3) + ((OperationEnd.Contract3MsgVESend&0x01)<<2) +
                              ((OperationEnd.Contract3TitleState&0x01)<<1) + ((OperationEnd.Contract3SpecialFlag&0x02)>>1) );
	OperationEndData[7] = (((OperationEnd.Contract3SpecialFlag&0x01)<<7) + ((OperationEnd.TransactionNumber&0xfe00)>>9));
	OperationEndData[8] = ((OperationEnd.TransactionNumber&0x1fe)>>1);
	OperationEndData[9] = (((OperationEnd.TransactionNumber&0x1)<<7)  + ((OperationEnd.AmountPendingPurse & 0xfe000)>>13));
	OperationEndData[10] =((OperationEnd.AmountPendingPurse & 0x1fe0)>>5);
	OperationEndData[11] =(((OperationEnd.AmountPendingPurse & 0x1f)<<3) + ((OperationEnd.MsgVESend&0x01)<<2) + ((OperationEnd.Ruf&0x01)<<1) +
                              ((OperationEnd.OpeAutCode&0x80000000)>>31));
	OperationEndData[12] = 	((OperationEnd.OpeAutCode&0x7f800000)>>24);
	OperationEndData[13] = 	((OperationEnd.OpeAutCode&0x7f8000)>>15);
	OperationEndData[14] = 	((OperationEnd.OpeAutCode&0x7f80)>>7);
	OperationEndData[15] = 	(((OperationEnd.OpeAutCode&0x7f)<<1) + (OperationEnd.EndRecordOperation&0x01));
        
        MiaCheck.Check9 = ChecksumWord(OperationEndData,2,15);
        OperationEnd.Check9 = MiaCheck.Check9;
        
        OperationEndData[0] = ((OperationEnd.Check9&0xFF00)>>8);
	OperationEndData[1] = (OperationEnd.Check9&0x00FF);
        
	return OperationEndDataReturn;
}

unsigned char * libMIA::MakeHistoricalRecharge(){
        
    static unsigned char HistoricalRechargeBlockData[16];
    memset(HistoricalRechargeBlockData,0x00,16);	// pone en 0 todo el vector
    unsigned char *HistoricalRechargeBlockDataReturn;
    HistoricalRechargeBlockDataReturn = &HistoricalRechargeBlockData[0];

    HistoricalRechargeBlockData[0]  = ((HistoricalRecharge.Year&0x7F)<<1)+((HistoricalRecharge.Mont&0x08)>>3);
    HistoricalRechargeBlockData[1]  = ((HistoricalRecharge.Mont&0x07)<<5)+(HistoricalRecharge.Day&0x1F);
    HistoricalRechargeBlockData[2]  = ((HistoricalRecharge.Hour&0x1F)<<3)+((HistoricalRecharge.Minute&0x38)>>3);
    HistoricalRechargeBlockData[3]  = ((HistoricalRecharge.Minute&0x07)<<5)+((HistoricalRecharge.Operator&0xF8)>>3);
    HistoricalRechargeBlockData[4]  = ((HistoricalRecharge.Operator&0x07)<<5)+((HistoricalRecharge.Line&0x3E00)>>10);
    HistoricalRechargeBlockData[5]  = ((HistoricalRecharge.Line&0x01FE)>>1);
    HistoricalRechargeBlockData[6]  = ((HistoricalRecharge.Line&0x01)<<7)+((HistoricalRecharge.Station&0x3F80)>>7);
    HistoricalRechargeBlockData[7]  = ((HistoricalRecharge.Station&0x7F)<<1)+((HistoricalRecharge.MachineType&0x08)>>3);
    HistoricalRechargeBlockData[8]  = ((HistoricalRecharge.MachineType&0x07)<<5)+((HistoricalRecharge.IssuingMachine&0xF8)>>3);
    HistoricalRechargeBlockData[9]  = ((HistoricalRecharge.IssuingMachine&0x07)<<5)+((HistoricalRecharge.Ruf&0x01)<<4)+((HistoricalRecharge.ReloadValue&0xF0000)>>16);
    HistoricalRechargeBlockData[10] = ((HistoricalRecharge.ReloadValue&0x0FF00)>>8);
    HistoricalRechargeBlockData[11] =  (HistoricalRecharge.ReloadValue&0x000FF);
    HistoricalRechargeBlockData[12] = ((HistoricalRecharge.PurseBalance&0xFF000000)>>24);
    HistoricalRechargeBlockData[13] = ((HistoricalRecharge.PurseBalance&0x00FF0000)>>16);
    HistoricalRechargeBlockData[14] = ((HistoricalRecharge.PurseBalance&0x0000FF00)>>8);
    HistoricalRechargeBlockData[15] =  (HistoricalRecharge.PurseBalance&0x000000FF);
    return HistoricalRechargeBlockDataReturn;
}

unsigned char * libMIA::MakeLastTransitBlock()
{
	static unsigned char LastTransitData[16];
	memset(LastTransitData,0x00,16);	// pone en 0 todo el vector
	unsigned char *LastTransitDataReturn;
	LastTransitDataReturn = &LastTransitData[0];

	LastTransitData[0] = (((LastTransit.Gar&0x07)<<5) + ((LastTransit.DecMonth&0x7c)>>2));
	LastTransitData[1] = (((LastTransit.DecMonth&0x03)<<6) + ((LastTransit.Day&0x1f)<<1) + ((LastTransit.Hour&0x10)>>4));
	LastTransitData[2] = (((LastTransit.Hour&0x0f)<<4) + ((LastTransit.Minute&0x3c)>>2));
	LastTransitData[3] = (((LastTransit.Minute&0x03)<<6) + ((LastTransit.Operator&0xfc)>>2));
	LastTransitData[4] = (((LastTransit.Operator&0x03)<<6) + ((LastTransit.Line&0x3f00)>>8));
	LastTransitData[5] = (LastTransit.Line&0xff);
	LastTransitData[6] = ((LastTransit.Station&0x3fc0)>>6);
	LastTransitData[7] = (((LastTransit.Station&0x3f)<<2) + ((LastTransit.EquipmentSell&0x3000)>>12));
	LastTransitData[8] = ((LastTransit.EquipmentSell&0xff0)>>4);
	LastTransitData[9] = (((LastTransit.EquipmentSell&0xf)<<4) + ((LastTransit.Way&0x3c00)>>10));
	LastTransitData[10] = ((LastTransit.Way&0x3fc)>>2);
	LastTransitData[11] = (((LastTransit.Way&0x2)<<6) + (LastTransit.Bus&0x3f00));
	LastTransitData[12] = (LastTransit.Bus&0xff);
	LastTransitData[13] = (((LastTransit.Ruf&0x0F)<<4) + ((LastTransit.TravelAmount&0x0F0000)>>16));
	LastTransitData[14] = ((LastTransit.TravelAmount&0x0FF00)>>8);
	LastTransitData[15] = (LastTransit.TravelAmount&0x000FF);

	return LastTransitDataReturn;
}

unsigned char * libMIA::MakeHistoricalMovementBlock()
{
	static unsigned char HistoricalData[16];
	memset(HistoricalData,0x00,16);	// pone en 0 todo el vector
	unsigned char *HistoricalDataReturn;
	HistoricalDataReturn = &HistoricalData[0];
	//cout << "MakeHistoricalMovement" << endl;
	HistoricalData[0] = (((HistoricalMovements.DecMonth&0x7f)<<1) + 
						 ((HistoricalMovements.Day&0x10)>>4));
	HistoricalData[1] = (((HistoricalMovements.Day&0x0f)<<4) + 
						 ((HistoricalMovements.Hour&0x1e)>>1));
	HistoricalData[2] = (((HistoricalMovements.Hour&0x01)<<7) + 
						 ((HistoricalMovements.Minute&0x3f)<<1) + 
						 ((HistoricalMovements.Operation&0x04)>>2));
	HistoricalData[3] = (((HistoricalMovements.Operation&0x03)<<6) + 
						 ((HistoricalMovements.SubOperation&0x1f)<<1) + 
						 ((HistoricalMovements.Operator&0x80)>>7));
	HistoricalData[4] = (((HistoricalMovements.Operator&0x7f)<<1) + 
						 ((HistoricalMovements.Line&0x2000)>>13));
	HistoricalData[5] =  ((HistoricalMovements.Line&0x1fe0)>>5);
	HistoricalData[6] = (((HistoricalMovements.Line&0x001f)<<3) + 
						 ((HistoricalMovements.Station&0x3800)>>11));
	HistoricalData[7] =  ((HistoricalMovements.Station&0x07f8)>>3);
	HistoricalData[8] = (((HistoricalMovements.Station&0x0007)<<5) + 
						 ((HistoricalMovements.TravelPeople&0x03)<<3) + 
						 ((HistoricalMovements.InvTravelPeople&0x03)<<1) + 
		                 ((HistoricalMovements.EquipmentSell&0x80)>>7) );
	HistoricalData[9] = (((HistoricalMovements.EquipmentSell&0x7F)<<1) + 
						 ((HistoricalMovements.ValuedAcerCostumer&0x02)>>1));//??
	HistoricalData[10] =(((HistoricalMovements.ValuedAcerCostumer&0x01)<<7) + 
						 ((HistoricalMovements.Way&0x3f80)>>7));//??
	HistoricalData[11] =(((HistoricalMovements.Way&0x007f)<<1) + 
						  (HistoricalMovements.Transfer&0x01));
	HistoricalData[12] = ((HistoricalMovements.Balance_&0x1fe00)>>9);
	HistoricalData[13] = ((HistoricalMovements.Balance_&0x1fe)>>1);
	HistoricalData[14] =(((HistoricalMovements.Balance_&0x1)<<7) + 
						 ((HistoricalMovements.AmountCharge&0x7f00)>>8)) ;
	HistoricalData[15] =  (HistoricalMovements.AmountCharge&0xff);
	
	return HistoricalDataReturn;
}

unsigned char * libMIA::MakeReleaseContractBlock()
{
    static unsigned char  ReleaseContractBlockData[16];
    memset( ReleaseContractBlockData,0x00,16);	// pone en 0 todo el vector
    unsigned char * ReleaseContractBlockDataReturn;
     ReleaseContractBlockDataReturn = & ReleaseContractBlockData[0];
    
    ReleaseContractBlockData[0]  = ((ReleaseContract.Version&0x03)<<6)+((ReleaseContract.TitleCode&0xFC)>>2);
    ReleaseContractBlockData[1]  = ((ReleaseContract.TitleCode&0x03)<<6)+((ReleaseContract.TypeMachine&0x0F)<<2)+((ReleaseContract.Line&0x18)>>3);
    ReleaseContractBlockData[2]  = ((ReleaseContract.Line&0x07)<<5)+((ReleaseContract.Station&0x3E)>>1);
    ReleaseContractBlockData[3]  = ((ReleaseContract.Station&0x01)<<7)+((ReleaseContract.Ruf&0x01)<<6)+((ReleaseContract.Machine&0xFC)>>2);
    ReleaseContractBlockData[4]  = ((ReleaseContract.Machine&0x03)<<6)+((ReleaseContract.VLSN&0xFC00)>>10);
    ReleaseContractBlockData[5]  = ((ReleaseContract.VLSN&0x03FC)>>2);
    ReleaseContractBlockData[6]  = ((ReleaseContract.VLSN&0x0003)<<6)+((ReleaseContract.GeographicalCriteria&0x07) << 3)+((ReleaseContract.Destination_Line&0x1C)>>3);
    ReleaseContractBlockData[7]  = ((ReleaseContract.Destination_Line&0x03)<<6)+(ReleaseContract.Destination_Station&0x3F);
    ReleaseContractBlockData[8]  = ((ReleaseContract.DateType&0x07)<<5)+((ReleaseContract.Type_Year&0x7C)>>2);
    ReleaseContractBlockData[9]  = ((ReleaseContract.Type_Year&0x03)<<6)+((ReleaseContract.Type_Month&0x0F)<<2)+((ReleaseContract.Type_Day&0x18)>>3);
    ReleaseContractBlockData[10] = ((ReleaseContract.Type_Day&0x07)<<5)+(ReleaseContract.Type_Hour&0x1F);
    ReleaseContractBlockData[11] = ((ReleaseContract.Type_Minute&0x3F)<<2)+(ReleaseContract.BalanceType&0x03);
    ReleaseContractBlockData[12] = ((ReleaseContract.NominalSell&0x1FE00)>>9);
    ReleaseContractBlockData[13] = ((ReleaseContract.NominalSell&0x001FE)>>1);
    ReleaseContractBlockData[14] = ((ReleaseContract.NominalSell&0x00001)<<7)+((ReleaseContract.FacialSell&0x7F00)>>8);
    ReleaseContractBlockData[15] =  (ReleaseContract.FacialSell&0xFF);
    
    return  ReleaseContractBlockDataReturn;
}

unsigned char * libMIA::MakeInitializationBlock()
{
	static unsigned char InitializationData[16];
	memset(InitializationData,0x00,16);	// pone en 0 todo el vector
	unsigned char *InitializationDataReturn;
	InitializationDataReturn = &InitializationData[0];
	
	InitializationData[0] = ((Initialization.MapVersion&0x03) << 6) + ((Initialization.SamVersion&0x07) << 3) + (Initialization.CodeTitleSam1&0x07);
	InitializationData[1] = ((Initialization.CodeTitleSam2&0x07) << 5) + ((Initialization.CodeTitleSam3&0x07) << 2) + ((Initialization.CountryCode&0x0300) >> 8);
	InitializationData[2] = Initialization.CountryCode&0xFF;
	//InitializationData[3] = (Initialization.State&0xFF00) >> 8;
        InitializationData[3] = Initialization.State[0];
        
	//InitializationData[4] = Initialization.State&0xFF;
        InitializationData[4] = Initialization.State[1];
        
	//InitializationData[5] = (Initialization.City&0xFF00) >> 8;
        InitializationData[5] = Initialization.City[0];
	
        //InitializationData[6] = Initialization.City&0xFF;
        InitializationData[6] = Initialization.City[1];
	InitializationData[7] = Initialization.ApplicationCode;
	InitializationData[8] = Initialization.MachineInitializer;
	InitializationData[9] = (Initialization.InitializationYear&0xEF) << 1;
	InitializationData[9] += (Initialization.InitializationMonth&0x08) >> 3;
	InitializationData[10] = (Initialization.InitializationMonth&0x07) << 5;
	InitializationData[10] += Initialization.InitializationDay&0x1F;
	InitializationData[11] = Initialization.Category;
	InitializationData[12] = (Initialization.InitializationSerialNumber&0xFF0000) >> 16; 
	InitializationData[13] = (Initialization.InitializationSerialNumber&0x00FF00) >> 8;
	InitializationData[14] = Initialization.InitializationSerialNumber&0x0000FF;
	Initialization.Check4 = ChecksumByte(InitializationData,0,15);
	InitializationData[15] = Initialization.Check4;
	
	for(int j = 0; j < 16; j++)
		printf("%02X ",InitializationData[j]);
	printf("\n");
				
	return InitializationDataReturn;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned char *libMIA::MakePurseBlock()
{
	static unsigned char PurseData[16];
	memset(PurseData,0x00,16);	// pone en 0 todo el vector
	unsigned char *PurseDataReturn;
	PurseDataReturn = &PurseData[0];
	//cout << "MakePurseSector" << endl;

	PurseData[3]  = ((Purse.PurseA&0xFF000000)>>24);
	PurseData[2]  = ((Purse.PurseA&0xFF0000)>>16);
	PurseData[1]  = ((Purse.PurseA&0xFF00)>>8);
	PurseData[0]  = (Purse.PurseA&0xFF);
	PurseData[7]  = ((Purse.NegPurseA&0xFF000000)>>24);
	PurseData[6]  = ((Purse.NegPurseA&0xFF0000)>>16);
	PurseData[5]  = ((Purse.NegPurseA&0xFF00)>>8);
	PurseData[4]  =  (Purse.NegPurseA&0xFF);
	PurseData[11] = ((Purse.PurseA1&0xFF000000)>>24);
	PurseData[10] = ((Purse.PurseA1&0xFF0000)>>16);
	PurseData[9]  = ((Purse.PurseA1&0xFF00)>>8);
	PurseData[8]  =  (Purse.PurseA1&0xFF);
	PurseData[12] =   Purse.AddrA;
	PurseData[13] =   Purse.NegAddrA;
	PurseData[14] =   Purse.AddrB;
	PurseData[15] =   Purse.NegAddrB;

	return PurseDataReturn;
}

unsigned char * libMIA::MakeTransitContractBlock()
{	static unsigned char 	TransitContractData[16];
	unsigned char *TransitContractDataReturn;

	memset(TransitContractData,0x00,16);	// pone en 0 todo el vector
	TransitContractDataReturn = &TransitContractData[0];


	TransitContractData[0]	= ((TransitContract.Balance0&0xff000)>>12);
	TransitContractData[1]	= ((TransitContract.Balance0&0xff0)>>4);

	TransitContractData[2]	= ( ((TransitContract.Balance0&0xf)<<4)          + 
								((TransitContract.Recharge0DateType&0x3)<<2) + 
								((TransitContract.Recharge0Year&0x60)>>5) );

	TransitContractData[3]	= ( ((TransitContract.Recharge0Year&0x1f)<<3) + 
								((TransitContract.Recharge0Month&0xe)>>1) );

	TransitContractData[4]	= ( ((TransitContract.Recharge0Month&0x1)<<7) + 
								((TransitContract.Rechar0Day&0x1f)<<2)    + 
								 (TransitContract.LastRecharge0Index&0x3) );

	TransitContractData[5]	= ((TransitContract.Ruf0&0xFF0000)>>16);
	TransitContractData[6]	= ((TransitContract.Ruf0&0xFF00)>>8);
	TransitContractData[7]	= (TransitContract.Ruf0&0xFF) ;
	TransitContractData[8]	= ((TransitContract.Balance1&0xff000)>>12);
	TransitContractData[9]	= ((TransitContract.Balance1&0xff0)>>4);

	TransitContractData[10]	= ( ((TransitContract.Balance1&0xf)<<4)          + 
								((TransitContract.Recharge1DateType&0x3)<<2) + 
								((TransitContract.Recharge1Year&0x60)>>5) );

	TransitContractData[11] = ( ((TransitContract.Recharge1Year&0x1f)<<3) + 
								((TransitContract.Recharge1Month&0xe)>>1) );

	TransitContractData[12] = ( ((TransitContract.Recharge1Month&0x1)<<7) + 
								((TransitContract.Rechar1Day&0x1f)<<2) + 
								 (TransitContract.LastRecharge1Index&0x3) );

	TransitContractData[13] = ((TransitContract.Ruf1&0xFF0000)>>16) ;
	TransitContractData[14] = ((TransitContract.Ruf1&0xFF00)>>8) ;
	TransitContractData[15] = (TransitContract.Ruf1&0xFF) ;

	return TransitContractDataReturn;	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned char * libMIA::MakeCustomBlock()
{
	static unsigned char CustomBlockData[16];
	memset(CustomBlockData,0x00,16);	// pone en 0 todo el vector
	unsigned char *CustomBlockDataReturn;
	CustomBlockDataReturn = &CustomBlockData[0];
	
	CustomBlockData[0] = ((CustomCard.ActYear&0x7F) << 1) + ((CustomCard.ActMont&0x08)>>3);
	CustomBlockData[1] = ((CustomCard.ActMont&0x07) << 5) + (CustomCard.ActDay&0x1F);
	CustomBlockData[2] = ((CustomCard.ExpYear&0x7F) << 1) + ((CustomCard.ExpMonth&0x08)>>3);
	CustomBlockData[3] = ((CustomCard.ExpMonth&0x07) << 5) + ((CustomCard.ExpDay&0x1F));
	CustomBlockData[4] = CustomCard.OpeCard;
	CustomBlockData[5] = CustomCard.EmployeeProfile;
	CustomBlockData[6] = ((CustomCard.PersonalizationFlag&0x02)<<6)+((CustomCard.Ruf&0x3F));
	CustomBlockData[7] = CustomCard.TitleCode; 
	CustomBlockData[8] = CustomCard.ContractOperator1;
	CustomBlockData[9] = CustomCard.ContractOperator2;
	CustomBlockData[10] = CustomCard.ContractOperator3;
	CustomBlockData[11] = ((CustomCard.BirthYear&0x7F)<< 1) + ((CustomCard.BirthMont&0x08)>>3);
	CustomBlockData[12] = ((CustomCard.BirthMont)&0x07) + (CustomCard.BirthDay&0x1F);
	CustomBlockData[13] = CustomCard.Language[0];
	CustomBlockData[14] = CustomCard.Language[1];
	CustomCard.Checksum5 =ChecksumByte(CustomBlockData,0,14);
	CustomBlockData[15] = CustomCard.Checksum5;
	
	return CustomBlockDataReturn;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned char * libMIA::MakeSellBlock()
{
	static unsigned char SellBlockData[16];
	memset(SellBlockData,0x00,16);	// pone en 0 todo el vector
	unsigned char *SellBlockDataReturn;
	SellBlockDataReturn = &SellBlockData[0];
        
	SellBlockData[0]  = ((SellCard.SellYear&0x7F)<<1)+((SellCard.SellMont&0x08)>>3);
	SellBlockData[1]  = ((SellCard.SellMont&0x07)<<5)+(SellCard.SellDay&0x1F);
	SellBlockData[2]  = ((SellCard.SellHour&0x1F)<<3)+((SellCard.SellMinute&0x38)>>3);
	SellBlockData[3]  = ((SellCard.SellMinute&0x07)<<5)+((SellCard.Operator&0xF8)>>3);
	SellBlockData[4]  = ((SellCard.Operator&0x07)<<5)+((SellCard.Line&0x3E00)>>10);
	SellBlockData[5]  = ((SellCard.Line&0x01FE)>>1);
	SellBlockData[6]  = ((SellCard.Line&0x01)<<7)+((SellCard.Station&0x3F80)>>7);
	SellBlockData[7]  = ((SellCard.Station&0x7F)<<1)+((SellCard.EquipmentSell&0x80)>>7);
	SellBlockData[8]  = ((SellCard.EquipmentSell&0x7F)<<1)+((SellCard.PurseType&0x04)>>2);
	SellBlockData[9]  = ((SellCard.PurseType&0x03)<<6)+((SellCard.TypeMachine&0x0F)<<2)+((SellCard.Ruf&0x06)>>1);
	SellBlockData[10] = ((SellCard.Ruf&0x01)<<7)+((SellCard.ActivationFlag&0x07)<<4)+((SellCard.AmountSell&0xF0000)>>16);
	SellBlockData[11] = ((SellCard.AmountSell&0x0FF00)>>8);
	SellBlockData[12] = ((SellCard.AmountSell&0xFF));
	SellBlockData[13] = ((SellCard.Deposit&0xFF00)>>8);
	SellBlockData[14] =  (SellCard.Deposit&0xFF);
	SellCard.Check6 =ChecksumByte(SellBlockData,0,14);
	SellBlockData[15] = SellCard.Check6;

	return SellBlockDataReturn;
} 

/******************************************************************************/
/*	     Checksum for block 8 ->Check8                                        */
/******************************************************************************/
unsigned char libMIA::ChecksumNibble(	unsigned char *rData,   int bPos,
										unsigned char maskbPos, int Pos,unsigned char maskPos)
{
	unsigned char checksum_calc;
	int i;
	unsigned char swap = 0xf0;
	
	checksum_calc = (rData[bPos++]&maskbPos);
	
	for(i = bPos;i < (Pos+1);i++)
	{
		checksum_calc ^= ((rData[i]&swap)>>4);
		swap = (((swap&0x0f)<<4) | ((swap&0xf0)>>4));
		checksum_calc ^= (rData[i]&swap);
	}
	checksum_calc =~ checksum_calc;

	return checksum_calc;
}

/******************************************************************************/
/*          Checksum for block 8 ->Check8, the real checksum 8                */
/******************************************************************************/
unsigned char libMIA::ChecksumNibblechks8(	unsigned char * rData,  int bPos,
											unsigned char maskbPos, int Pos,unsigned char maskPos)
{
	unsigned char checksum_calc = (rData[bPos++]&maskbPos);
	unsigned char swap = 0xf0;
	int i;
	
	for(i = bPos;i < (Pos+1);i++)
	{
		swap = 0xf0;
		checksum_calc ^= ((rData[i]&swap)>>4);
		swap = (((swap&0x0f)<<4) | ((swap&0xf0)>>4));
		checksum_calc ^= (rData[i]&swap);
	}
	checksum_calc =~ checksum_calc;

	return checksum_calc;
}

/******************************************************************************/
/*                 Checksum for Block 9 -> Checksum Check9                    */
/******************************************************************************/
unsigned int libMIA::ChecksumWord(unsigned char * rData,int bPos,int Pos)
{	unsigned int checksum_calc = 0;
	unsigned char low = 0;
	unsigned char high = 0;
	unsigned char i;

	for(i = bPos;i < (Pos+1);i++)
	{
		if((i%2) == 0)
			high ^= rData[i];
		else
			low ^= rData[i];
	}

	low =~ low;
	high =~ high;
	checksum_calc = high;
	checksum_calc <<= 8;
	checksum_calc += low;

	return checksum_calc;
}

/******************************************************************************/
/*                Checksum for Block 8 -> Checksum Backup                     */
/******************************************************************************/
unsigned char libMIA::ChecksumByte(unsigned char * rData,int bPos,int Pos)
{	unsigned char checksum_calc = 0;
	unsigned char i;
	
	for(i = bPos;i < (Pos+1);i++)
		checksum_calc ^= rData[i];

	checksum_calc =~ checksum_calc;

	return checksum_calc;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::CopyWriteBlocks(unsigned char * Data,int BlockMemory,int BlockSize,int BlockToWrite)
{	int i = 0;

	for(i = BlockMemory; i < BlockSize; i++)
		WriteBlocks[BlockToWrite][i]=Data[i];

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
char* libMIA :: Fare2Str(long fare)
{	static char Hex[48];

	sprintf(Hex,"%08lu",fare);
	return Hex;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
char* libMIA :: Hex2Str(int count,int block)
{	static char Hex[48];
	char Aux[BLOCKSIZE];
	int i;

	strcpy(Hex,"\0");
	for (i = 0; i < count; i++)
	{ 
		sprintf(Aux,"%02x",ReadBlocks[block][i]);
		strcat(Hex,Aux);
	}
	return Hex;
}

/******************************************************************************/
/* Print All Blocks                                                           */
/******************************************************************************/
void libMIA::PrintReadData()
{
	printf("ReadBlocks\n");
	for(int i = 0; i < 64; i++)
	{	printf("B%02d=",i);
		for(int k = 0; k < 16; k++)
			printf("%02X",ReadBlocks[i][k]);
		printf("\n");
	}		
}

/******************************************************************************/
/* Print All Blocks                                                           */
/******************************************************************************/
void libMIA::PrintWriteData()
{
	printf("WriteBlocks\n");
	for(int i = 0; i < 64; i++)
	{	printf("B%02d=",i);
		for(int k = 0; k < 16; k++)
			printf("%02X",WriteBlocks[i][k]);
		printf("\n");
	}		
}

/******************************************************************************/
/* Custom Block 05    ->   Personalizacion de la Tarjeta                      */
/******************************************************************************/
unsigned char libMIA::SetCustomBlock(void)
{	time_t TimeNow = time(NULL);
	struct tm *Now = localtime(&TimeNow);  // Get Local Time

	CustomCard.ActYear				= ((Now->tm_year + OFFSETYEAR) - STARTYEAR);
	CustomCard.ActMont				=   Now->tm_mon+1;
	CustomCard.ActDay				=   Now->tm_mday;
	CustomCard.ExpYear				= 99;
	CustomCard.ExpMonth				= 12;
	CustomCard.ExpDay				= 31;
	CustomCard.OpeCard				= 0;
	CustomCard.EmployeeProfile		= 0;
	CustomCard.PersonalizationFlag	= 0;
	CustomCard.Ruf					= 0;
	CustomCard.TitleCode			= 111;
	CustomCard.ContractOperator1	= 1;
	CustomCard.ContractOperator2	= 0;
	CustomCard.ContractOperator3	= 0;
	CustomCard.BirthYear			= 0;
	CustomCard.BirthMont			= 0;
	CustomCard.BirthDay				= 0;
	CustomCard.Language[0]			= 0;
	CustomCard.Language[1]			= 0;

	return 0;
}

/******************************************************************************/
/* Purse Sell Block 06   ->     Venta o emision de la tarjeta                 */
/******************************************************************************/
unsigned char libMIA::SetSellBlock(int AmountSell, int Deposit)
{	time_t TimeNow = time(NULL);
	struct tm *Now = localtime(&TimeNow);  // Get Local TIme

	SellCard.SellYear		= ((Now->tm_year + OFFSETYEAR) - STARTYEAR);
	SellCard.SellMont		= Now->tm_mon+1;
	SellCard.SellDay		= Now->tm_mday;
	SellCard.SellHour		= Now->tm_hour;
	SellCard.SellMinute		= Now->tm_min;
	SellCard.Operator		= 0x01;
	SellCard.Line			= init.LINEA;	// Numero de la Linea
	SellCard.Station		= init.ESTACION;// Numero de Estación
	SellCard.EquipmentSell	= init.NUMDISP;	// Numero de dispositivo 71 al 80
	SellCard.PurseType		= 0x01;			// 1 Monedero Formato Mifare (Dinero)
	SellCard.TypeMachine	= 0x0b;			// 11 es la inicializadora, en el maestro dice 5 es la mera, verificar cual es el correcto
	SellCard.Ruf			= 0x00;
	SellCard.ActivationFlag	= 0x02;			// 2 Activada
	SellCard.AmountSell		= AmountSell;	// Importe de Venta, una parte paga el plastico y el resto al saldo
	SellCard.Deposit		= Deposit;		// Fianza, deposito en garantia por la tarjeta

	return 0;
}

/******************************************************************************/ 
/*  Operation Start Block 08  ->   Inicio de la Operacion                     */ 
/******************************************************************************/
unsigned char libMIA::SetOperationStartBlock(void)
{	// datos de default
	OperationStart.Check8					= 0;			// checksum, se calcula al final
	OperationStart.Operation				= 0x01;
	OperationStart.SubOperation				= RELEASE_SELL;	// primera venta, fianza y carga
	OperationStart.RecordCount				= 1;			// Primera Operacion Contador = 1
	OperationStart.CheckBackup				= 0;			// checksum, se calcula al final
	OperationStart.EndOperation				= OperationEnd.Operation;
	OperationStart.EndSubOperation			= OperationEnd.SubOperation;
	OperationStart.EndPointers				= CalculaEndPointers();			//82176
	OperationStart.EndEspecialFlagsCard		= 0;
	OperationStart.EndEspecialFlagsContract	= EndEspecialFlagsContract();	//3584
	OperationStart.EndTransacionNumber		= OperationEnd.TransactionNumber;
	OperationStart.EndPendingPayment		= OperationEnd.AmountPendingPurse;	//(TotalCantidadCargada-Fianza)*10;
	OperationStart.MsgSellCardSend			= OperationEnd.MsgVESend;
	OperationStart.Ruf						= 0;
	OperationStart.FlagRecordStartOperation	= 1;

	return 0;
}

/* 
 * Make EndPointers 
 * Agrupa los punteros de Operation End Block
 */
unsigned long libMIA::CalculaEndPointers(void)
{   
    unsigned long endp;
    endp =  (   
        ((OperationEnd.PointerPurse&0x03)*0x10000)        + 
		((OperationEnd.PointerRechargePurse&0x03)*0x4000) +
        ((OperationEnd.PointerPurseTraffic&0x03)*0x1000)  + 
		((OperationEnd.PointerMove&0xf)*0x100)            +
        ((OperationEnd.PointerOpec&0x03)*0x40)            + 
		((OperationEnd.ContractTraffic1&0x1)*0x20)        + 
        ((OperationEnd.ContractBalance1&0x1)*0x10)        + 
		((OperationEnd.ContractTraffic2&0x1)*0x8)         +
        ((OperationEnd.ContractBalance2&0x1)*0x4)         + 
		((OperationEnd.ContractTraffic3&0x1)*0x2)         + 
        (OperationEnd.ContractBalance3&0x1)
        );
//    printf("EndPointer = %lu\n",endp);
    return endp;
}

/* 
 * Make EndEspecialFlagsContract
 * Agrupa los punteros de End Especial Flags Contract
 */
unsigned long libMIA::EndEspecialFlagsContract(void)
{   
	unsigned long value;
	value =  (   
		((OperationEnd.Contract1MsgVESend&0x01)<<11)  +
		((OperationEnd.Contract1TitleState&0x01)<<10) + 
		((OperationEnd.Contract1SpecialFlag&0x03)<<8) +	
		((OperationEnd.Contract2MsgVESend&0x01)<<7)   +
		((OperationEnd.Contract2TitleState&0x01)<<6)  + 
		((OperationEnd.Contract2SpecialFlag&0x03)<<4) +
		((OperationEnd.Contract3MsgVESend&0x01)<<3)   +
		((OperationEnd.Contract3TitleState&0x01)<<2)  + 
		((OperationEnd.Contract3SpecialFlag&0x03)));

	return value;
}

/******************************************************************************/
/*           Operation End Block 09        Fin de la Operacion                */
/******************************************************************************/
unsigned char libMIA::SetOperationEndBlock(int Cantidad)
{
	// Block 9 OPERATIONENDBLOCK
	OperationEnd.Check9                 = 0;
	OperationEnd.Operation              = 1;
	OperationEnd.SubOperation           = RELEASE_SELL;
	OperationEnd.PointerPurse           = 1;
	OperationEnd.PointerRechargePurse   = 1;
	OperationEnd.PointerPurseTraffic    = 0;
	OperationEnd.PointerMove            = 1;
	OperationEnd.PointerOpec            = 0;
	OperationEnd.ContractTraffic1       = 0;
	OperationEnd.ContractBalance1       = 0;
	OperationEnd.ContractTraffic2       = 0;
	OperationEnd.ContractBalance2       = 0;
	OperationEnd.ContractTraffic3       = 0;
	OperationEnd.ContractBalance3       = 0;
	OperationEnd.EspecialFlagCard       = 2;
	OperationEnd.Contract1MsgVESend     = 1;
	OperationEnd.Contract1TitleState    = 1;
	OperationEnd.Contract1SpecialFlag   = 2;
	OperationEnd.Contract2MsgVESend     = 0;
	OperationEnd.Contract2TitleState    = 0;
	OperationEnd.Contract2SpecialFlag   = 0;
	OperationEnd.Contract3MsgVESend     = 0;
	OperationEnd.Contract3TitleState    = 0;
	OperationEnd.Contract3SpecialFlag   = 0;
	OperationEnd.TransactionNumber      = 1;
	OperationEnd.AmountPendingPurse     = 0;
	OperationEnd.MsgVESend              = 1;
	OperationEnd.Ruf                    = 0;
	OperationEnd.OpeAutCode             = 0;
	OperationEnd.EndRecordOperation     = 1;
	return 0;
}

/* 
 * Operation End Block 09 
 * Fin de la Operacion 
 */
unsigned char libMIA::SetOperationEndBlock(int pCat, int pTitle)
{
    // Block 9 OPERATIONENDBLOCK
    OperationEnd.Check9                 = 0;                // 16
    OperationEnd.Operation              = OPERACION_MONEDERO;// 3 Renombrar a OperationPlace
    OperationEnd.SubOperation           = RELEASE_SELL;     // 5 varia segun la operacion
    OperationEnd.PointerPurse           = 1;                // 2
    OperationEnd.PointerRechargePurse   = 1;                // 2
    OperationEnd.PointerPurseTraffic    = 0;                // 2
    OperationEnd.PointerMove            = 1;                // 4
    OperationEnd.PointerOpec            = 0;                // 2
    OperationEnd.ContractTraffic1       = 0;                // 1
    OperationEnd.ContractBalance1       = 0;                // 1
    OperationEnd.ContractTraffic2       = 0;                // 1
    OperationEnd.ContractBalance2       = 0;                // 1
    OperationEnd.ContractTraffic3       = 0;                // 1
    OperationEnd.ContractBalance3       = 0;                // 1

	if( (pCat == ANONIMOS) && (pTitle == 111) )
		OperationEnd.EspecialFlagCard   = RELEASE_SELL;		// 3
	else
		OperationEnd.EspecialFlagCard   = 0;				// 3

	if(init.TIPO == 0)
		OperationEnd.Contract1MsgVESend = 0;		// 0 para la inicializadora
	else
		OperationEnd.Contract1MsgVESend = 1;		// 1 para la meta

	OperationEnd.Contract1TitleState    = OPERACION_MONEDERO;                // 1

//	if((pCat == ANONIMOS)&&(pTitle == 111))
//		OperationEnd.Contract1SpecialFlag = RELEASE_SELL;	// 2
//	else
        OperationEnd.Contract1SpecialFlag = 0;				// 

    OperationEnd.Contract2MsgVESend     = 0;                // 1
    OperationEnd.Contract2TitleState    = 0;                // 1
    OperationEnd.Contract2SpecialFlag   = 0;                // 2
    OperationEnd.Contract3MsgVESend     = 0;                // 1
    OperationEnd.Contract3TitleState    = 0;                // 1
    OperationEnd.Contract3SpecialFlag   = 0;                // 2
    OperationEnd.TransactionNumber      = 1;                // 16   // primer transaccion debe de incrementar
    OperationEnd.AmountPendingPurse     = 0;                // Cantidad*10;      //20   Fianza o primera carga?

	if(init.TIPO == 0)
		OperationEnd.MsgVESend			= 0;		// 0 para la inicializadora
	else
		OperationEnd.MsgVESend			= 1;		// 1 para las maquinas de venta
    
    OperationEnd.Ruf                    = 0;                // 1
    OperationEnd.OpeAutCode             = 0;                // 32   // OPAC
    OperationEnd.EndRecordOperation     = 1;                // 1    

    return 0;
}

/******************************************************************************/
/* Historical Recharge Block 10                                               */
/* Historico de recarga 01                                                    */
/******************************************************************************/
unsigned char libMIA::SetHistoricalRecharge(int ReloadValue, int PurseBalance)
{	time_t TimeNow = time(NULL);
	struct tm *Now = localtime(&TimeNow);  // Get Local TIme

	// BLOCK 10  HISTORICAL RECHARGE
	HistoricalRecharge.Year            = ((Now->tm_year + OFFSETYEAR) - STARTYEAR);
	HistoricalRecharge.Mont            = (Now->tm_mon)+1;
	HistoricalRecharge.Day             = Now->tm_mday;
	HistoricalRecharge.Hour            = Now->tm_hour;
	HistoricalRecharge.Minute          = Now->tm_min;
	HistoricalRecharge.Operator        = 0x01;
	HistoricalRecharge.Line            = init.LINEA;//0x01;
	HistoricalRecharge.Station         = init.ESTACION; //2053;
	HistoricalRecharge.MachineType     = init.TIPO;// 7:Inicializadora ; 11: META
	HistoricalRecharge.IssuingMachine  = init.NUMDISP; // 11: inicializadora; 71,72: METAS
	HistoricalRecharge.Ruf             = 0x00;
	HistoricalRecharge.ReloadValue     = ReloadValue;
	HistoricalRecharge.PurseBalance    = PurseBalance;

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned char libMIA::SetHistoricalMovements(int ReloadValue, int Bonus)
{	time_t TimeNow = time(NULL);
	struct tm *Now = localtime(&TimeNow);  // Get Local TIme

	HistoricalMovements.DecMonth            = Date2DecadeMonth();
	HistoricalMovements.Day                 = Now->tm_mday;
	HistoricalMovements.Hour                = Now->tm_hour;
	HistoricalMovements.Minute              = Now->tm_min;
	HistoricalMovements.Operation           = TITLEPURSEOPERATION;
	HistoricalMovements.SubOperation        = RELEASE_SELL;
	HistoricalMovements.Operator            = SellCard.Operator;
	HistoricalMovements.Line                = init.LINEA;
	HistoricalMovements.Station             = init.ESTACION;
	HistoricalMovements.TravelPeople        = 0x10;		// 0x02
	HistoricalMovements.InvTravelPeople     = 0x11;		// 0x03
	HistoricalMovements.EquipmentSell       = init.NUMDISP;
	HistoricalMovements.ValuedAcerCostumer  = ADDBONO;
	HistoricalMovements.Way                 = Bonus;
	HistoricalMovements.Transfer            = NODATA;
	HistoricalMovements.Balance_            = ADDBALANCE;
	HistoricalMovements.AmountCharge        = ReloadValue;    // saldo abonado
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::SetValueBlock(int pValue,int address)
{
	Purse.PurseA = pValue;
	Purse.NegPurseA = ~Purse.PurseA;
	Purse.PurseA1 = Purse.PurseA;
    
	Purse.AddrA = address;
	Purse.NegAddrA = ~Purse.AddrA;            
	Purse.AddrB = Purse.AddrA;
	Purse.NegAddrB = ~Purse.AddrB;
    
	return 1;
}

unsigned char libMIA::SetReleaseContractBlock(int Bonus)
{   time_t TimeNow = time(NULL);
    struct tm *Now = localtime(&TimeNow);  // Get Local TIme
    BYTE *p;
    
    // BLOCK 40 RELEASECONTRACTBLOCK_1
    ReleaseContract.Version                 = 2;
    ReleaseContract.TitleCode               = 0;
    ReleaseContract.TypeMachine             = init.TIPO;
    ReleaseContract.Line                    = init.LINEA;
    ReleaseContract.Station                 = init.ESTACION;
    ReleaseContract.Ruf                     = 0;   
    ReleaseContract.Machine                 = init.NUMDISP;
    ReleaseContract.VLSN                    = 1;
    ReleaseContract.GeographicalCriteria    = 0;
    ReleaseContract.Destination_Line        = 0;
    ReleaseContract.Destination_Station     = 0;
    ReleaseContract.Destination_Zone        = 0; // esta variable no se usa
    ReleaseContract.DateType                = 0;
    ReleaseContract.Type_Year               = ((Now->tm_year + OFFSETYEAR) - STARTYEAR);
    ReleaseContract.Type_Month              = Now->tm_mon+1;
    ReleaseContract.Type_Day                = Now->tm_mday;
    ReleaseContract.Type_Hour               = Now->tm_hour;
    ReleaseContract.Type_Minute             = Now->tm_min;
    ReleaseContract.BalanceType             = RELEASE_SELL;
    ReleaseContract.NominalSell             = 0;
    ReleaseContract.FacialSell              = Bonus;   // Bono Facial o Bono de regalo

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned char libMIA::SetTransitContractBlock(	unsigned char apBal,
												long miaBono, 
												unsigned char op )
{	time_t TimeNow = time(NULL);
	struct tm *Now = localtime(&TimeNow);  // Get Local Time

	// BLOCK 41  TRANSITCONTRACTBLOCK_1    
	if(op)
	{	TransitContract.Balance0            = 0;    // 20
		TransitContract.Recharge0DateType   = 0;    // 2
		TransitContract.Recharge0Year       = 0;    // 7
		TransitContract.Recharge0Month      = 0;    // 4
		TransitContract.Rechar0Day          = 0;    // 5
		TransitContract.LastRecharge0Index  = 0;    // 2
		TransitContract.Ruf0                = 0;    // 24
		TransitContract.Balance1            = 0;    // 20
		TransitContract.Recharge1DateType   = 0;    // 2
		TransitContract.Recharge1Year       = 0;    // 7
		TransitContract.Recharge1Month      = 0;    // 4
		TransitContract.Rechar1Day          = 0;    // 5
		TransitContract.LastRecharge1Index  = 0;    // 2
		TransitContract.Ruf1                = 0;    // 24
	}

	if(apBal==0)
	{
		TransitContract.Balance0				= miaBono;
		TransitContract.Recharge0DateType		= 0;
		TransitContract.Recharge0Year			= ((Now->tm_year + OFFSETYEAR) - STARTYEAR); // +2;
		TransitContract.Recharge0Month			= Now->tm_mon+1;
		TransitContract.Rechar0Day				= Now->tm_mday;
		TransitContract.LastRecharge0Index		= 0;	// 1 22/03/2023
		TransitContract.Ruf0					= 0;
	}
	else
	{
		TransitContract.Balance1                = miaBono;
		TransitContract.Recharge1DateType       = 0;
		TransitContract.Recharge1Year           = ((Now->tm_year + OFFSETYEAR) - STARTYEAR);    // +2;;
		TransitContract.Recharge1Month          = Now->tm_mon+1;
		TransitContract.Rechar1Day              = Now->tm_mday;
		TransitContract.LastRecharge1Index      = 0;	// 1 22/03/2023
		TransitContract.Ruf1                    = 0;
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void libMIA::PrintKeys()
{
//	printf("\n Printing keys\n KeyA new       KeyB new        KeyA old        KeyB old\n");
	printf("\n Printing keys\n KeyA           KeyB\n");
	for(int k = 0; k < 16; k++)
	{
		for(int i = 0; i < 6 ; i++) printf("%02X",KeyA[k][i]);		printf("    ");
		for(int i = 0; i < 6 ; i++) printf("%02X",KeyB[k][i]);		printf("    ");
		printf("\n");
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void libMIA::PrintPassingInitialization(Initializationbits *initialization)
{
	printf("\nInitialization Block 04\n");
	printf("Map Version: %d, Sam Version %d\n",
							initialization->MapVersion,
							initialization->SamVersion);

	printf("Code Title Sam1 %d, Code Title Sam2 %d, Code Title Sam3 %d\n",
							initialization->CodeTitleSam1,
							initialization->CodeTitleSam2,
							initialization->CodeTitleSam3);

	printf("Country Code %d, State %c%c, City %c%c, App Code %d\n",
							initialization->CountryCode,
							initialization->State[0],
							initialization->State[1],
							initialization->City[0],
							initialization->City[1],
							initialization->ApplicationCode);

	printf("Initialization Machine %d, Year %d, Month %d, Day %d\n",
							initialization->MachineInitializer,
							initialization->InitializationYear,
							initialization->InitializationMonth,
							initialization->InitializationDay);

	printf("Category %d, Initialization S.N. %lu, Check4 %d\n",
							initialization->Category,
							initialization->InitializationSerialNumber,
							initialization->Check4);

	printf("\nCustom Block 05\n");
	printf("Activation Year %d, Mont %d, Day %d\n",
							CustomCard.ActYear,
							CustomCard.ActMont,
							CustomCard.ActDay);

	printf("Expedition Year %d, Month %d, Day %d\n",
							CustomCard.ExpYear,
							CustomCard.ExpMonth,
							CustomCard.ExpDay);

	printf("Operator %d, Profile %d, Personalization Type %d\n",
							CustomCard.OpeCard,
							CustomCard.EmployeeProfile,
							CustomCard.PersonalizationFlag);

	printf("Ruf %d, Title Code %d, Contract Operator 1:%d, 2:%d, 3:%d\n",
							CustomCard.Ruf,
							CustomCard.TitleCode,
							CustomCard.ContractOperator1,
							CustomCard.ContractOperator2,
							CustomCard.ContractOperator3);
    
	printf("Birth Year %d, Month %d, Day %d, Languaje %c%c, Check5 %d\n\n",
							CustomCard.BirthYear,
							CustomCard.BirthMont,
							CustomCard.BirthDay,
							CustomCard.Language[0],
							CustomCard.Language[1],
							CustomCard.Checksum5);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void libMIA::PrintPassingCard()
{
	printf("\nSell Card Block 06\n");
	printf("Sell: Year %d, Month %d, Day %d, Hour %d, Minute %d\n",
							SellCard.SellYear,
							SellCard.SellMont,
							SellCard.SellDay,
							SellCard.SellHour,
							SellCard.SellMinute);
	printf("Operator %d, Line %d, Station %d, EquipmentSell %d\n",
							SellCard.Operator,
							SellCard.Line,
							SellCard.Station,
							SellCard.EquipmentSell);
	printf("PurseType %d, TypeMachine %d, Ruf %d, ActivationFlag %d\n",
							SellCard.PurseType,
							SellCard.TypeMachine,
							SellCard.Ruf,
							SellCard.ActivationFlag);
	printf("AmountSell %lu, Deposit %d, Check6 %d\n",
							SellCard.AmountSell,
							SellCard.Deposit,
							SellCard.Check6);

	printf("\nOperation Start Block 08\n");
	printf("Check8 %d, Operation %d, Suboperation %d, RecordCount %lu\n",
							OperationStart.Check8,
							OperationStart.Operation,
							OperationStart.SubOperation,
							OperationStart.RecordCount);
	printf("CheckBackup %d, EndOperation %d, EndSubOperation %d\n",
							OperationStart.CheckBackup,
							OperationStart.EndOperation,
							OperationStart.EndSubOperation);
	printf("EndPointers %lu, EndEspecialFlags : Card %d, Contract %d\n",
							OperationStart.EndPointers,
							OperationStart.EndEspecialFlagsCard,
							OperationStart.EndEspecialFlagsContract);
	printf("EndTransactionNumber %d, EndPendingPayment %lu, MsgSellCardSend %d\n",
							OperationStart.EndTransacionNumber,
							OperationStart.EndPendingPayment,
							OperationStart.MsgSellCardSend);
	printf("Ruf %d, FlagRecordStartOperation %d\n",
							OperationStart.Ruf,
							OperationStart.FlagRecordStartOperation);

	printf("\nRelease Contract Block 09\n");
	printf("Check9 %d, Pointer Contract %d, Operation Performed %d\n",
							OperationEnd.Check9,
							OperationEnd.Operation,
							OperationEnd.SubOperation);
	printf("Pointer: Purse %d, Recharge Purse %d, Purse Traffic %d\n",
							OperationEnd.PointerPurse,
							OperationEnd.PointerRechargePurse,
							OperationEnd.PointerPurseTraffic);
	printf("Pointer: Move %d, Operation Mon %d\n",
							OperationEnd.PointerMove,
							OperationEnd.PointerOpec);
	printf("Contract 01: Traffic %d, Balance %d\n",
							OperationEnd.ContractTraffic1,
							OperationEnd.ContractBalance1);
	printf("Contract 02: Traffic %d, Balance %d\n",
							OperationEnd.ContractTraffic2,
							OperationEnd.ContractBalance2);
	printf("Contract 03: Traffic %d, Balance %d\n",
							OperationEnd.ContractTraffic3,
							OperationEnd.ContractBalance3);
	printf("Especial Flag Card %d\n",OperationEnd.EspecialFlagCard);
	printf("Contract1: MsgVESend %d, TitleState %d, SpecialFlag %d\n",
							OperationEnd.Contract1MsgVESend,
							OperationEnd.Contract1TitleState,
							OperationEnd.Contract1SpecialFlag);
	printf("Contract2: MsgVESend %d, TitleState %d, SpecialFlag %d\n",
							OperationEnd.Contract2MsgVESend,
							OperationEnd.Contract2TitleState,
							OperationEnd.Contract2SpecialFlag);
	printf("Contract3: MsgVESend %d, TitleState %d, SpecialFlag %d\n",
							OperationEnd.Contract3MsgVESend,
							OperationEnd.Contract3TitleState,
							OperationEnd.Contract3SpecialFlag);
	printf("Transaction Number %lu, AmountPendingPurse %lu, MsgVESend %d\n",
							OperationEnd.TransactionNumber,
							OperationEnd.AmountPendingPurse,
							OperationEnd.MsgVESend);
	printf("Ruf %d, OpeAutCode %lu, EndRecordOperation %d\n",
							OperationEnd.Ruf,
							OperationEnd.OpeAutCode,
							OperationEnd.EndRecordOperation);

	printf("\nPurse Block 13 ó 14\n");
	printf("PurseA %lu, NegPurseA %lu, PurseA1 %lu, AddrA %d, NegAddrA %d\n",
							Purse.PurseA,
							Purse.NegPurseA,
							Purse.PurseA1,
							Purse.AddrA,
							Purse.NegAddrA);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void libMIA::PrintHistoricalRecharge()
{
	printf("\nHistoricar Recharge 1\n");
	printf("Year             = %d\n", HistoricalRecharge1.Year);
	printf("Mont             = %d\n", HistoricalRecharge1.Mont);
	printf("Day              = %d\n", HistoricalRecharge1.Day);
	printf("Hour             = %d\n", HistoricalRecharge1.Hour);
	printf("Minute           = %d\n", HistoricalRecharge1.Minute);
	printf("Operator         = %d\n", HistoricalRecharge1.Operator);
	printf("Line             = %d\n", HistoricalRecharge1.Line);
	printf("Station          = %d\n", HistoricalRecharge1.Station);
	printf("MachineType      = %d\n", HistoricalRecharge1.MachineType);
	printf("IssuiningMachine = %d\n", HistoricalRecharge1.IssuingMachine);
	printf("Ruf              = %d\n", HistoricalRecharge1.Ruf);
	printf("ReloadValue      = %lu\n", HistoricalRecharge1.ReloadValue);
	printf("PurseBalance     = %lu\n", HistoricalRecharge1.PurseBalance);

	printf("\nHistoricar Recharge 2\n");
	printf("Year             = %d\n", HistoricalRecharge2.Year);
	printf("Mont             = %d\n", HistoricalRecharge2.Mont);
	printf("Day              = %d\n", HistoricalRecharge2.Day);
	printf("Hour             = %d\n", HistoricalRecharge2.Hour);
	printf("Minute           = %d\n", HistoricalRecharge2.Minute);
	printf("Operator         = %d\n", HistoricalRecharge2.Operator);
	printf("Line             = %d\n", HistoricalRecharge2.Line);
	printf("Station          = %d\n", HistoricalRecharge2.Station);
	printf("MachineType      = %d\n", HistoricalRecharge2.MachineType);
	printf("IssuiningMachine = %d\n", HistoricalRecharge2.IssuingMachine);
	printf("Ruf              = %d\n", HistoricalRecharge2.Ruf);
	printf("ReloadValue      = %lu\n", HistoricalRecharge2.ReloadValue);
	printf("PurseBalance     = %lu\n", HistoricalRecharge2.PurseBalance);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void libMIA::PrintLastTransit(LastTransitbits *LastTransit, LastTransitFinalbits *LastTransitFinal)
{
	printf("Gar = %d\n", LastTransit->Gar);
	printf("DecMonth = %d\n", LastTransit->DecMonth);
	printf("Day = %d\n", LastTransit->Day);
	printf("Hour = %d\n", LastTransit->Hour);
	printf("Minute = %d\n", LastTransit->Minute);
	printf("Operator = %d\n", LastTransit->Operator);
	printf("Line = %d\n", LastTransit->Line);
	printf("Station = %d\n", LastTransit->Station);
	printf("EquipmentSell = %d\n", LastTransit->EquipmentSell);
	printf("Way = %d\n", LastTransit->Way);
	printf("Bus = %d\n", LastTransit->Bus);
	printf("Ruf = %d\n",  LastTransit->Ruf);
	printf("Amount = %lu\n",  LastTransit->TravelAmount);

	printf("Ruf = %s\n", LastTransitFinal->Ruf);
	printf("CountTransfers = %d\n", LastTransitFinal->CountTransfers);
	printf("CountConsecutiveIn = %d\n", LastTransitFinal->CountConsecutiveIn);
	printf("CountConsecutiveExit = %d\n", LastTransitFinal->CountConsecutiveExit);
	printf("CounterTogetherTravelers = %d\n", LastTransitFinal->CounterTogetherTravelers);
	printf("PreviousTripAmount = %lu\n", LastTransitFinal->PreviousTripAmount);
	printf("LastOperator = %d\n", LastTransitFinal->LastOperator);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void libMIA::PrintLastTransitHist()
{	/*  
		LASTTRANSIT_1		16
		LASTTRANSITFINAL_1	17
		LASTTRANSIT_2		18
		LASTTRANSITFINAL_2	20
		LASTTRANSIT_3		21
		LASTTRANSITFINAL_3	22   
	*/

	printf("\nLast Transit 1\n");
	PrintLastTransit(&LastTransit01,&LastTransitFinal01);
	printf("\nLast Transit 2\n");
	PrintLastTransit(&LastTransit02,&LastTransitFinal02);
	printf("\nLast Transit 3\n");
	PrintLastTransit(&LastTransit03,&LastTransitFinal03);
}

/******************************************************************************/
/*	HISTORICALMOVEMENTS_1	24			HISTORICALMOVEMENTS_2	25            */
/*	HISTORICALMOVEMENTS_3	26			HISTORICALMOVEMENTS_4	28            */
/*	HISTORICALMOVEMENTS_5	29			HISTORICALMOVEMENTS_6	30            */
/*	HISTORICALMOVEMENTS_7	32			HISTORICALMOVEMENTS_8	33            */
/*	HISTORICALMOVEMENTS_9	34			HISTORICALMOVEMENTS_10	36            */
/*	HISTORICALMOVEMENTS_11	37			HISTORICALMOVEMENTS_12	38            */
/******************************************************************************/
void libMIA::PrintHistoricalMovements(void)
{	int HistMovBlocks[12];
	int count = OperationEnd.PointerMove - 1;
	HistoricalMovementsbits HistMovTemp;
 
    char fecha[10];
    char suboperacion[40];
    char estacion[40];

	// Esto ordena los bloques del historico de movimientos
	for(int i = 0; i < 12; i++)
	{
		HistMovBlocks[i] = HistoricalMovementsBlocks[count--];
		if(count == -1)
			count = 11;
	}

	for(int i = 0 ; i < 12 ; i++ )
		printf("%02d ", HistMovBlocks[i]);
	printf("\n");

	for(int i = 0; i < 12 ; i++)
	{	HistMovTemp.DecMonth 			= 	( ReadBlocks[HistMovBlocks[i]][0]&0xFE) >> 1;
		HistMovTemp.Day 				= 	((ReadBlocks[HistMovBlocks[i]][0]&0x01) << 4) + 
											(( ReadBlocks[HistMovBlocks[i]][1]&0xF0) >> 4);
		HistMovTemp.Hour 				= 	((ReadBlocks[HistMovBlocks[i]][1]&0x0F) << 1) + 
											(( ReadBlocks[HistMovBlocks[i]][2]&0x80)>>7);
		HistMovTemp.Minute				= 	((ReadBlocks[HistMovBlocks[i]][2]&0x7E) >> 1);
		HistMovTemp.Operation 			=	((ReadBlocks[HistMovBlocks[i]][2]&0x01) << 2) + 
											(( ReadBlocks[HistMovBlocks[i]][3]&0xC0) >> 6);
		HistMovTemp.SubOperation		=	((ReadBlocks[HistMovBlocks[i]][3]&0x3E) >> 1);
		HistMovTemp.Operator			=	((ReadBlocks[HistMovBlocks[i]][3]&0x01) << 7) + 
											(( ReadBlocks[HistMovBlocks[i]][4]&0xFE) >> 1);
		HistMovTemp.Line				=	((ReadBlocks[HistMovBlocks[i]][4]&0x01) << 13) + 
											((ReadBlocks[HistMovBlocks[i]][5]&0xFF) << 5) + 
											((ReadBlocks[HistMovBlocks[i]][6]&0xF8) >> 3);
		HistMovTemp.Station				=	((ReadBlocks[HistMovBlocks[i]][6]&0x07) << 11) + 
											((ReadBlocks[HistMovBlocks[i]][7]&0xFF) << 3) + 
											((ReadBlocks[HistMovBlocks[i]][8]&0xE0) >> 5);
		HistMovTemp.TravelPeople		=	((ReadBlocks[HistMovBlocks[i]][8]&0x18) >> 3);
		HistMovTemp.InvTravelPeople		=	((ReadBlocks[HistMovBlocks[i]][8]&0x05) >> 1);
		HistMovTemp.EquipmentSell		=	((ReadBlocks[HistMovBlocks[i]][8]&0x01) << 7) + 
											((ReadBlocks[HistMovBlocks[i]][9]&0xFE) >> 1);
		HistMovTemp.ValuedAcerCostumer	= 	((ReadBlocks[HistMovBlocks[i]][9]&0x01) << 1) + 
											((ReadBlocks[HistMovBlocks[i]][10]&0x80) >> 7);
		HistMovTemp.Way					=	((ReadBlocks[HistMovBlocks[i]][10]&0xFF) << 7) + 
											((ReadBlocks[HistMovBlocks[i]][11]&0xFE) >> 1);
		HistMovTemp.Transfer			=	((ReadBlocks[HistMovBlocks[i]][11]&0x01));

		HistMovTemp.Balance_			=	((ReadBlocks[HistMovBlocks[i]][12] & 0xFF) << 9) + 
											((ReadBlocks[HistMovBlocks[i]][13]&0xFF) << 1) + 
											((ReadBlocks[HistMovBlocks[i]][14]&0x80) >> 7);

		HistMovTemp.AmountCharge		= 	((ReadBlocks[HistMovBlocks[i]][14]&0x7F) << 8) + 
											((ReadBlocks[HistMovBlocks[i]][15]&0xFF));
		
		printf("Hist %02d: %03d %02d %02d %02d %02d (%02d) %02d %02d %02d (%d) %d %02d %d %04d %d %05lu %05lu\n",
				HistMovBlocks[i],
				HistMovTemp.DecMonth,
				HistMovTemp.Day,
				HistMovTemp.Hour,
				HistMovTemp.Minute,
				HistMovTemp.Operation,
				HistMovTemp.SubOperation,
				HistMovTemp.Operator,
				HistMovTemp.Line,
				HistMovTemp.Station,
				HistMovTemp.TravelPeople,
				HistMovTemp.InvTravelPeople,
				HistMovTemp.EquipmentSell,
				HistMovTemp.ValuedAcerCostumer,
				HistMovTemp.Way,
				HistMovTemp.Transfer,
				HistMovTemp.Balance_,
				HistMovTemp.AmountCharge);


        if((HistMovTemp.Operation == 0) && (HistMovTemp.SubOperation == 0))
                sprintf(suboperacion,"NINGUNO");
        
        if(HistMovTemp.Operation == 1)
		{
            if(HistMovTemp.SubOperation == 1)
                sprintf(suboperacion,"INICIALIZACION");
            if(HistMovTemp.SubOperation == 2)
                sprintf(suboperacion,"EMISION");
            if(HistMovTemp.SubOperation == 3)
                sprintf(suboperacion,"REMPLAZO");
            if(HistMovTemp.SubOperation == 4)
                sprintf(suboperacion,"RECARGA");
            if(HistMovTemp.SubOperation == 5)
                sprintf(suboperacion,"REMBOLSO");
            if(HistMovTemp.SubOperation == 6)
                sprintf(suboperacion,"ACTUALIZACION");
            if(HistMovTemp.SubOperation == 7)
                sprintf(suboperacion,"ENTRADA");
            if(HistMovTemp.SubOperation == 8)
                sprintf(suboperacion,"SALIDA");
            if(HistMovTemp.SubOperation == 9)
                sprintf(suboperacion,"ANULACION");
            if(HistMovTemp.SubOperation == 10)
                sprintf(suboperacion,"PAGO CONSUMO");
            if(HistMovTemp.SubOperation == 11)
                sprintf(suboperacion,"CANCELACION ULTIMA OPERACION");
		}
		printf("Operacion=[%s]\n",suboperacion);
	}	
}


/*
void PrintHistoricalMovements(unsigned char ReadBlocks[64][16],int PointerMove, unsigned char * HistoricalMovementsBlocks)
{
    int HistMovBlocks[12];
    HistoricalMovementsbits HistMovTemp;
    int count = PointerMove - 1;

    char fecha[10];
    char suboperacion[10];
    char estacion[40];
    
    float bono = 0, saldo = 0;

    // Esto ordena los bloques del historico de movimientos
    for(int i = 0; i < 12; i++){
        HistMovBlocks[i] = HistoricalMovementsBlocks[count--];
        if(count == -1)
            count = 11;
    }
    printf("Historicos de movimientos:\n");
    for(int i = 0; i < 12 ; i++){
        HistMovTemp.DecMonth			= 	( ReadBlocks[HistMovBlocks[i]][0]&0xFE) >> 1;
        HistMovTemp.Day					= 	((ReadBlocks[HistMovBlocks[i]][0]&0x01) << 4) + (( ReadBlocks[HistMovBlocks[i]][1]&0xF0) >> 4);
        HistMovTemp.Hour				= 	((ReadBlocks[HistMovBlocks[i]][1]&0x0F) << 1) + (( ReadBlocks[HistMovBlocks[i]][2]&0x80)>>7);
        HistMovTemp.Minute				= 	((ReadBlocks[HistMovBlocks[i]][2]&0x7E) >> 1);
        HistMovTemp.Operation			=	((ReadBlocks[HistMovBlocks[i]][2]&0x01) << 2) + (( ReadBlocks[HistMovBlocks[i]][3]&0xC0) >> 6);
        HistMovTemp.SubOperation		=	((ReadBlocks[HistMovBlocks[i]][3]&0x3E) >> 1);
        HistMovTemp.Operator			=	((ReadBlocks[HistMovBlocks[i]][3]&0x01) << 7) + (( ReadBlocks[HistMovBlocks[i]][4]&0xFE) >> 1);
        HistMovTemp.Line				=	((ReadBlocks[HistMovBlocks[i]][4]&0x01) << 13)+ (( ReadBlocks[HistMovBlocks[i]][5]&0xFF) << 5) + ((ReadBlocks[HistMovBlocks[i]][6]&0xF8) >> 3);
        HistMovTemp.Station				=	((ReadBlocks[HistMovBlocks[i]][6]&0x07) << 11)+ (( ReadBlocks[HistMovBlocks[i]][7]&0xFF) << 3) + ((ReadBlocks[HistMovBlocks[i]][8]&0xE0) >> 5);
        HistMovTemp.TravelPeople		=	((ReadBlocks[HistMovBlocks[i]][8]&0x18) >> 3);
        HistMovTemp.InvTravelPeople		=	((ReadBlocks[HistMovBlocks[i]][8]&0x05) >> 1);
        HistMovTemp.EquipmentSell		=	((ReadBlocks[HistMovBlocks[i]][8]&0x01) << 7) + (( ReadBlocks[HistMovBlocks[i]][9]&0xFE) >> 1);
        HistMovTemp.ValuedAcerCostumer	= 	((ReadBlocks[HistMovBlocks[i]][9]&0x01) << 1) + (( ReadBlocks[HistMovBlocks[i]][10]&0x80) >> 7);
        HistMovTemp.Way					=	((ReadBlocks[HistMovBlocks[i]][10]&0xFF) << 7)+ (( ReadBlocks[HistMovBlocks[i]][11]&0xFE) >> 1);
        HistMovTemp.Transfer			=	((ReadBlocks[HistMovBlocks[i]][11]&0x01));
        HistMovTemp.Balance_			=	((ReadBlocks[HistMovBlocks[i]][12]&0xFF) << 9)+ (( ReadBlocks[HistMovBlocks[i]][13]&0xFF) << 1) + ((ReadBlocks[HistMovBlocks[i]][14]&0x80) >> 7);
        HistMovTemp.AmountCharge		= 	((ReadBlocks[HistMovBlocks[i]][14]&0x7F) << 8)+ (( ReadBlocks[HistMovBlocks[i]][15]&0xFF));

        strcpy(fecha,DecadeMonth2Date(HistMovTemp.DecMonth));
 
        if((HistMovTemp.Operation == 0) && (HistMovTemp.SubOperation == 0))
                sprintf(suboperacion,"NINGUNO");

        if(HistMovTemp.Operation == 1){
            if(HistMovTemp.SubOperation == 1)
                sprintf(suboperacion,"INICIALIZACION");
            if(HistMovTemp.SubOperation == 2)
                sprintf(suboperacion,"EMISION");
            if(HistMovTemp.SubOperation == 3)
                sprintf(suboperacion,"REMPLAZO");
            if(HistMovTemp.SubOperation == 4)
                sprintf(suboperacion,"RECARGA");
            if(HistMovTemp.SubOperation == 5)
                sprintf(suboperacion,"REMBOLSO");
            if(HistMovTemp.SubOperation == 6)
                sprintf(suboperacion,"ACTUALIZACION");
            if(HistMovTemp.SubOperation == 7)
                sprintf(suboperacion,"ENTRADA");
            if(HistMovTemp.SubOperation == 8)
                sprintf(suboperacion,"SALIDA");
            if(HistMovTemp.SubOperation == 9)
                sprintf(suboperacion,"ANULACION");
            if(HistMovTemp.SubOperation == 10)
                sprintf(suboperacion,"PAGO CONSUMO");
            if(HistMovTemp.SubOperation == 11)
                sprintf(suboperacion,"CANCELACION ULTIMA OPERACION");
            
        }
            
        strcpy (estacion,get_name_station(HistMovTemp.Line,HistMovTemp.Station));
        
        bono = HistMovTemp.Way;
        bono = bono/100;
        saldo = HistMovTemp.AmountCharge;
        saldo = saldo/100;
        
        printf("%02d> Fecha: %s-%02d %02d:%02d | %s | %s | $%0.2f | %d | $%0.2f\n",
            (i+1),
            fecha,
            HistMovTemp.Day,
            HistMovTemp.Hour,
            HistMovTemp.Minute,
            suboperacion,
            estacion,
            bono, // operacion en el bono de regalo
            HistMovTemp.Transfer, // transferencia
            saldo);
    }
}
*/


void libMIA::PrintContractCard(){
    printf("\nRelease Contract Block 40 \n");
    printf("Version %d, Title Code %d, Type Machine %d\n",
	ReleaseContract.Version,
	ReleaseContract.TitleCode,
	ReleaseContract.TypeMachine);
    
    printf("Line %d, Station %d, Ruf %d, Machine %d, VLSN %d\n",
	ReleaseContract.Line,
	ReleaseContract.Station,
	ReleaseContract.Ruf,
	ReleaseContract.Machine,
	ReleaseContract.VLSN);
    printf("GeographicalCriteria %d\n",ReleaseContract.GeographicalCriteria);
    
    printf("Destination Line %d, Station %d, Zone %d\n",
	ReleaseContract.Destination_Line,
	ReleaseContract.Destination_Station,
	ReleaseContract.Destination_Zone);
    
    printf("Date Type %d, Year %d, Month %d, Day %d, Hour %d, Minute %d\n",
	ReleaseContract.DateType,
	ReleaseContract.Type_Year,
	ReleaseContract.Type_Month,
	ReleaseContract.Type_Day,
	ReleaseContract.Type_Hour,
	ReleaseContract.Type_Minute);
    
    printf("Balance Type %d, Nominal Sell %lu, Facial Sell %d\n",
	ReleaseContract.BalanceType,
	ReleaseContract.NominalSell,
	ReleaseContract.FacialSell);


    printf("\nTransit Contract Block 41\n");
    printf("Balance0 %lu, Recharge0: DateType %d, Year %d, Month %d, Day %d\n",
        TransitContract.Balance0,
        TransitContract.Recharge0DateType,
        TransitContract.Recharge0Year,
        TransitContract.Recharge0Month,
        TransitContract.Rechar0Day);
    printf("LastRecharge0Index %d, Ruf0 %lu\n",
        TransitContract.LastRecharge0Index,
        TransitContract.Ruf0);
    printf("Balance1 %lu, Recharge1: DateType %d, Year %d, Month %d, Day %d\n",
        TransitContract.Balance1,
        TransitContract.Recharge1DateType,
        TransitContract.Recharge1Year,
        TransitContract.Recharge1Month,
        TransitContract.Rechar1Day);
    printf("LastRecharge1Index %u, Ruf %lu\n",
        TransitContract.LastRecharge1Index,
        TransitContract.Ruf1);
}
void libMIA::PrintPassingCustomPrivateData(CustomPrivateDatabits *CustomPrivateDataTemp)
{
    printf("\nCustom Private Data Blocks 56, 57 & 58\n");
    printf("Company Code: %s, \t", CustomPrivateDataTemp->CompanyCode);
    printf("Employee Position: %02X%02X%02X%02X\n",
        CustomPrivateDataTemp->EmployeePosition[0],
        CustomPrivateDataTemp->EmployeePosition[1],
        CustomPrivateDataTemp->EmployeePosition[2],
        CustomPrivateDataTemp->EmployeePosition[3]);
    
    printf("RUF1: %02X%02X\n",
        CustomPrivateDataTemp->RUF1[0],
        CustomPrivateDataTemp->RUF1[1]);
    printf("Login Size: %d,\t", CustomPrivateDataTemp->LoginSize);
    printf("Password Size: %d\n", CustomPrivateDataTemp->PwdSize);
    printf("Login: %02X%02X%02X%02X, \t",
        CustomPrivateDataTemp->Login[0],
        CustomPrivateDataTemp->Login[1],
        CustomPrivateDataTemp->Login[2],
        CustomPrivateDataTemp->Login[3]);
    printf("Password: %02X%02X%02X%02X\n",
        CustomPrivateDataTemp->Password[0],
        CustomPrivateDataTemp->Password[1],
        CustomPrivateDataTemp->Password[2],
        CustomPrivateDataTemp->Password[3]);
    printf("Card Type: %d, \t",CustomPrivateDataTemp->CardType);
    
    printf("HIWUSR: %02X%02X%02X%02X\n",
        CustomPrivateDataTemp->HIWUSR[0],
        CustomPrivateDataTemp->HIWUSR[1],
        CustomPrivateDataTemp->HIWUSR[2],
        CustomPrivateDataTemp->HIWUSR[3]);
    
    printf("Department: %c%c%c%c%c%c, \t",
        CustomPrivateDataTemp->Deparment[0],
        CustomPrivateDataTemp->Deparment[1],
        CustomPrivateDataTemp->Deparment[2],
        CustomPrivateDataTemp->Deparment[3],
        CustomPrivateDataTemp->Deparment[4],
        CustomPrivateDataTemp->Deparment[5]);
    printf("Employee Code: %lu\n",CustomPrivateDataTemp->EmployeeCode);
    printf("RUF2: %02X%02X%02X%02X%02X%02X\n",
        CustomPrivateDataTemp->RUF2[0],
        CustomPrivateDataTemp->RUF2[1],
        CustomPrivateDataTemp->RUF2[2],
        CustomPrivateDataTemp->RUF2[3],
        CustomPrivateDataTemp->RUF2[4],
        CustomPrivateDataTemp->RUF2[5]);
    
}

void libMIA::PrintPassingCustomPublicData(CustomPublicDatabits *CustomPublicDataTemp)
{
    printf("\nCustom Public Data Blocks 60, 61 & 62\n");
    printf("Gender: %d, NID Type: %d\n",
            CustomPublicDataTemp->Gender,
            CustomPublicDataTemp->NIDType);

    printf("National ID: %s\n", CustomPublicDataTemp->NID);
    printf("Name: %s ", CustomPublicDataTemp->Name_1);
    printf("%s\n", CustomPublicDataTemp->Name_2);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int libMIA::ValuesTSC()
{
	ReturnTSC.TitleCode         = CustomCard.TitleCode;
	ReturnTSC.Category          = Initialization.Category;
	ReturnTSC.EmployeeProfile   = CustomCard.EmployeeProfile;
	ReturnTSC.PersonalizationFlag = CustomCard.PersonalizationFlag;

	strncpy(ReturnTSC.LogicNumber, LogicNumber,strlen(LogicNumber));

	ReturnTSC.ExpirationYear    = CustomCard.ExpYear;
	ReturnTSC.ExpirationMonth   = CustomCard.ExpMonth;
	ReturnTSC.ExpirationDay     = CustomCard.ExpDay;
	ReturnTSC.BirthYear         = CustomCard.BirthDay;
	ReturnTSC.BirthMont         = CustomCard.BirthMont;
	ReturnTSC.BirthDay          = CustomCard.BirthDay;

	strncpy(ReturnTSC.Language, CustomCard.Language, strlen(CustomCard.Language));

	ReturnTSC.balance_ewallet = MiaBalance.Purse;
	ReturnTSC.balance_ewallet = ReturnTSC.balance_ewallet / 100;

	ReturnTSC.balance_bond = MiaBalance.Bonus;
	ReturnTSC.balance_bond = ReturnTSC.balance_bond / 100;

	ReturnTSC.HistoricalMovements01 = HistoricalMovements01;
	ReturnTSC.HistoricalMovements02 = HistoricalMovements02;
	ReturnTSC.HistoricalMovements03 = HistoricalMovements03;
	ReturnTSC.HistoricalMovements04 = HistoricalMovements04;
	ReturnTSC.HistoricalMovements05 = HistoricalMovements05;
	ReturnTSC.HistoricalMovements06 = HistoricalMovements06;
	ReturnTSC.HistoricalMovements07 = HistoricalMovements07;
	ReturnTSC.HistoricalMovements08 = HistoricalMovements08;
	ReturnTSC.HistoricalMovements09 = HistoricalMovements09;
	ReturnTSC.HistoricalMovements10 = HistoricalMovements10;
	ReturnTSC.HistoricalMovements11 = HistoricalMovements11;
	ReturnTSC.HistoricalMovements12 = HistoricalMovements12;

	ReturnTSC.EmployeeCode          = CustomPrivateData.EmployeeCode;

	return true;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned char libMIA::Date2DecadeMonth(void)
{	unsigned char DecadeMonth;
	int Year ;

	time_t TimeNow = time(NULL);
	struct tm *Now = localtime(&TimeNow);

	Year = (Now->tm_year + OFFSETYEAR)%10;

	DecadeMonth = (Year * TOTALMONTHS) + (Now->tm_mon+1);

	return DecadeMonth;
}
/******************************************************************************/