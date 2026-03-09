/*
 * File:   libMIA.hpp
 * Author: ISAAC Vasconcelos
 *
 * Created on 21 de enero de 2021, 09:33 AM
 * Modificada el 3 de octubre del 2022
 * Version 1.0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <stddef.h>
#include <stdlib.h>
#include <memory.h>


#ifndef LIBMIA_HPP
#define LIBMIA_HPP

#define MAXMONEDERO		999999

#define TarjetaViajesValida			1
#define TarjetaMonederoValida		2
#define SaldoRestante				3
#define TarjetaActiva				4

#define ReaderInitError				-1
#define ReaderConnError				-2

#define SamErrorSlot1				-3
#define SamErrorSlot2				-4
#define SamErrorSlot3				-5
#define SamErrorSlot4				-6

#define ErrorLectura				-7
#define LlavesInvalidas				-8
#define AppCodeInvalido				-9
#define TituloCaduco				-10

#define TarjetaInactiva				-11
#define TarjetaPrevaluada			-12
#define TarjetaCancelada			-13
#define TarjetaListaNegra			-14
#define TarjetaDesactivada			-15
#define TarjetaSinCategoria			-16

#define ErrorChks8					-17
#define ErrorChks9					-18

#define SaldoInsuficiente			-19
#define MonederoInvalido			-20

#define ErrorEscritura				-21

#define BonoInvalido				-22

#define TarjetaEnCeros				-23
#define InitError					-24



#include "bitmap.h"

// Constantes agregadas x famisa
#define KEYA		0
#define KEYB		1
#define KEYUID		2
#define KEYDEF		3


typedef struct
{
	unsigned char TitleCode;			// Codigo de Titulo de la tarjeta
	unsigned char Category;				// Categoria del Titulo
	unsigned char EmployeeProfile;		// Perfil del empleado
	unsigned char PersonalizationFlag;	// Flag de personalizacion
   
	char   LogicNumber[64];				// Numero Logico o ItemID, identificador
										// unico de la tarjeta.
										// Fecha de expiración de la tarjeta
	unsigned char ExpirationYear;		// Año
	unsigned char ExpirationMonth;		// Mes
	unsigned char ExpirationDay;		// Día
    
										// Fecha de cumpleaños
	unsigned char BirthYear;			// Año
	unsigned char BirthMont;			// Mes
	unsigned char BirthDay;				// Día
	char          Language[2];			// Lenguaje materno del usuario

	double        balance_ewallet;		// Importe del Saldo de la tarjeta
	double        balance_bond;			// Importe del Bono de la tarjeta

	// Blocks 24, 25, 26, 28, 29, 30, 32, 33, 34, 36, 37 & 38
	HistoricalMovementsbits  HistoricalMovements01;   
	HistoricalMovementsbits  HistoricalMovements02;  
	HistoricalMovementsbits  HistoricalMovements03;
	HistoricalMovementsbits  HistoricalMovements04;
	HistoricalMovementsbits  HistoricalMovements05;
	HistoricalMovementsbits  HistoricalMovements06;
	HistoricalMovementsbits  HistoricalMovements07;
	HistoricalMovementsbits  HistoricalMovements08;
	HistoricalMovementsbits  HistoricalMovements09;
	HistoricalMovementsbits  HistoricalMovements10;
	HistoricalMovementsbits  HistoricalMovements11;
	HistoricalMovementsbits  HistoricalMovements12;
	unsigned long EmployeeCode;			// 32 bits
}ReturnValues;

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
class libMIA
{
public:
	libMIA();
	int Lib_Version;
	int Lib_Type;

	//  Mifare Classic 1K EV1.
	//  Variables used with the negotiations with the Mifare Classic Cards.
	unsigned char	MifareUID[8];
	unsigned char	SamUID[8];

	unsigned char	ReadBlocks[64][16];		// Blocks readed from card
	unsigned char	WriteBlocks[64][16];	// Blocks to write on the card
	unsigned char	KeyA[16][6];			// PSAM generate the KeyA, production keys
	unsigned char	KeyB[16][6];			// ISAM generate the KeyB, production keys

	char			LogicNumber[64];

//	unsigned char	LastMifareUID[8];
//	unsigned char	OldKeyA[16][6];			// PSAM generate the KeyA, 
//	unsigned char	OldKeyB[16][6];			// ISAM generate the KeyB

	int sector;
	int block;
	
	// The following structures are used in the operations with the card of 
	//	the MIA App
	OperationCard			OperationCardIn;
	Balance					MiaBalance;

	// The following structures are used in the prototype of the query terminal
	initvalues				init;
	TransactionInfo			Trans;
	ReturnValues			ReturnTSC;

	int TransferVerifica(initvalues id);
	int UpdateBlocksPointers(long miaSaldo, long miaBono);
	int ReadBalanceCard(void);
	int ValuesTSC();
	int InitValues(initvalues ID);
	int BlockInZero(unsigned char blk);
	int SetBlockInZero(void);

	int VentaMIA(initvalues id, long tCarga, long  totBono,long totFianza);


	// Functions for debug purpose.
	void PrintReadData();
	void PrintWriteData();
	void PrintKeys();

	
	// para tener acceso al historico
	OperationEndbits         OperationEnd;				// Block 09
	unsigned char HistoricalMovementsBlocks[TotalMovements];
	int PassingHistorical(HistoricalMovementsbits *HistoricalMovementsTemp, int block);

	int VerEstadoInicial(void);


private:

//	char LogicNumber[64];

	int LastTransitBlock;
	int HistoricalRechargeBlock;
	int HistoricalMovementsBlock;
	int PurseBlock;
	int TransferFlag;
	
	unsigned char PursesBlocks[TotalPurses];
	unsigned char TransitBlocks[TotalTransit];
	unsigned char TransitFinalBlocks[TotalTransit];
	unsigned char HistoricalRechargeBlocks[TotalPurses];
//	unsigned char HistoricalMovementsBlocks[TotalMovements];


	// The following structures are part of the bit maps of MIA app.
	// The following statement is used in the function MiaCard()
	Initializationbits       Initialization;			// Block 04
	CustomCardbits           CustomCard;				// Block 05
	SellCardbits             SellCard;					// Block 06
	OperationStartbits       OperationStart;			// Block 08
//	OperationEndbits         OperationEnd;				// Block 09
	OperationEndbits         OperationEndAux;
	HistoricalRechargebits   HistoricalRecharge;     
	HistoricalRechargebits   HistoricalRecharge1;		// Block 10
	HistoricalRechargebits   HistoricalRecharge2;		// Block 12   
	Pursebits                Purse;						// Blocks 13 & 14
	LastTransitbits          LastTransit;
	LastTransitFinalbits     LastTransitFinal;
	LastTransitbits          LastTransit01;				// Blocks 16, 18 & 21
	LastTransitFinalbits     LastTransitFinal01;		// Blocks 17, 20 & 22	
	LastTransitbits          LastTransit02;            
	LastTransitFinalbits     LastTransitFinal02;        
	LastTransitbits          LastTransit03;            
	LastTransitFinalbits     LastTransitFinal03;
	HistoricalMovementsbits  HistoricalMovements;		// Blocks 24, 25, 26, 28,
	HistoricalMovementsbits  HistoricalMovements01;		// 29, 30, 32, 33, 34, 36,
	HistoricalMovementsbits  HistoricalMovements02;		// 37 & 38
	HistoricalMovementsbits  HistoricalMovements03;
	HistoricalMovementsbits  HistoricalMovements04;
	HistoricalMovementsbits  HistoricalMovements05;
	HistoricalMovementsbits  HistoricalMovements06;
	HistoricalMovementsbits  HistoricalMovements07;
	HistoricalMovementsbits  HistoricalMovements08;
	HistoricalMovementsbits  HistoricalMovements09;
	HistoricalMovementsbits  HistoricalMovements10;
	HistoricalMovementsbits  HistoricalMovements11;
	HistoricalMovementsbits  HistoricalMovements12;
	ReleaseContractbits      ReleaseContract;			// Blocks 40, 44 & 48
	TransitContractbits      TransitContract;			// Blocks 41, 45 & 49

	// TransitContractsbits 42, 46 and 50 are documented by telvent but are out 
	//	use in the production app MIA

	// The following statements are used in the functions BasicMiaCard() and
	//	EmployeeMiaCard().

	CustomPrivateDatabits	CustomPrivateData;      // Blocks 56, 57 & 58
	CustomPublicDatabits	CustomPublicData;       // Blocks 60, 61 & 62
	
	int CreateLogicNumber(void);



	// trasfiere los datos de ReadBlock a los diferentes varriables de la MIA
	int PassingInitializationBlock(); 
	int PassingCustomBlock();
	int PassingPurseSellBlock(); 
	int PassingOperationStartBlock();
	int PassingOperationEndBlock();
	int PassingPurseBlock();
	int PassingContract_1_ReleaseBlock();
	int PassingContract_1_Transit_0_Block();
	int PassingHistoricalRechargeMovements();
	int PassingHistoricalRecharge(HistoricalRechargebits *HistoricalRecharge, int block);
//	int PassingHistorical(HistoricalMovementsbits *HistoricalMovementsTemp, int block);
	int PassingHistoricalMovements();
	int PassingLastTransit(LastTransitbits * LastTransit, int block);
	int PassingLastTransitFinal(LastTransitFinalbits * LastTransitFinal, int block);
	int PassingHistoricalLastTransit();
	int PassingCustomPrivateData(CustomPrivateDatabits    *CustomPrivateDataTemp);
	int PassingCustomPublicData(CustomPublicDatabits     *CustomPublicDataTemp);

	
	// trasfiere las variables de la MIA a los blocks de datos
	unsigned char * MakeInitializationBlock();			// 4
	unsigned char * MakeCustomBlock();					// 5
	unsigned char * MakeSellBlock();					// 6
	unsigned char * MakeOperationStartBlock();			// 8
	unsigned char * MakeOperationEndBlock();			// 9
	unsigned char * MakeHistoricalRecharge();			// 10
	unsigned char * MakePurseBlock();					// 13 y 14
	unsigned char * MakeLastTransitBlock();				// 16, 18 y 21
	unsigned char * MakeHistoricalMovementBlock();		// 24, 25, 26, 28, 29, 30, 32, 33, 34, 36, 37, 38
	unsigned char * MakeReleaseContractBlock();			// 40
	unsigned char * MakeTransitContractBlock();			// 41

	unsigned char SetCustomBlock(void);
	unsigned char SetSellBlock(int,int);
	unsigned char SetOperationStartBlock(void);
	unsigned char SetOperationEndBlock(int);
	unsigned char SetOperationEndBlock(int pCat, int pTitle);

	unsigned char SetHistoricalRecharge(int ReloadValue, int PurseBalance);
	unsigned char SetHistoricalMovements(int,int);
	unsigned char SetReleaseContractBlock(int);
	unsigned char SetTransitContractBlock(unsigned char apBal,long miaBono, unsigned char op);
	int           SetValueBlock(int pValue,int address);


	// Small functions used by the principal functions and operations with the MIA card.
	unsigned char ChecksumNibble(unsigned char *rData,int bPos,unsigned char maskbPos,int Pos,unsigned char maskPos);
	unsigned int  ChecksumWord(unsigned char * rData,int bPos,int Pos);
	unsigned char ChecksumByte(unsigned char * rData,int bPos,int Pos);
	unsigned char ChecksumNibblechks8(unsigned char * rData,int bPos,unsigned char maskbPos,int Pos,unsigned char maskPos);     

	unsigned char Date2DecadeMonth(void);

	int   CopyWriteBlocks(unsigned char *,int ,int,int);
	char* Fare2Str(long fare);
	char* Hex2Str(int count,int block);

	unsigned long CalculaEndPointers(void);
	unsigned long EndEspecialFlagsContract(void);

//	int VerEstadoInicial(void);

	void PrintPassingInitialization(Initializationbits *initialization);
	void PrintPassingCard();

	void PrintHistoricalRecharge();
	void PrintHistoricalMovements();
//	void PrintHistorical();

	void PrintLastTransit(LastTransitbits *LastTransit, LastTransitFinalbits *LastTransitFinal);
	void PrintLastTransitHist();
	void PrintContractCard();
	void PrintPassingCustomPrivateData(CustomPrivateDatabits *CustomPrivateDataTemp);
	void PrintPassingCustomPublicData(CustomPublicDatabits *CustomPublicDataTemp);
};


// Clave de SubOperaciones en la MIA
#define OPER_INICIALIZA				1		// INICIALIZACION
#define OPER_EMISION				2		// EMISION
#define OPER_REEMPLAZO				3		// REMPLAZO
#define OPER_RECARGA				4		// RECARGA
#define OPER_REEMBOLSO				5		// REMBOLSO
#define OPER_ACTUALIZACION			6		// ACTUALIZACION
#define OPER_ENTRADA				7		// ENTRADA
#define OPER_SALIDA					8		// SALIDA
#define OPER_ANULACION				9		// ANULACION
#define OPER_PAGO_CONSUMO			10		// PAGO CONSUMO
#define OPER_CANCEL_ULTIMA_OPER		11		// CANCELACION ULTIMA OPERACION


const  char *nameOperaciones[] = 
{	"NO OPERACION",
	"INICIALIZA",
	"EMISION",
	"REEMPLAZO",
	"RECARGA",
	"REEMBOLSO",
	"ACTUALIZACION",
	"ENTRADA",
	"SALIDA ",
	"ANULACION "
	"PAGO CONSUMO",
	"CANCEL ULT OPER"
};


#endif	/* LIBMIA_HPP */
