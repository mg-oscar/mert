/* 
 * File:   bitmap.h
 * Author: Isaac V.
 *
 * Created on 21 de enero de 2021, 03:55 PM
 * Version 3.0.4
 */




#ifndef BITMAP_H
#define BITMAP_H

//#define LowNibble		0x0F
#define NOCREDIT		0



//#define MAXMONEDERO 		999999

#define xNewKeys			0
#define xOldKeys			1   

#define Inactivated			0
#define Loaded				1
#define Activated			2
#define Canceled			3
#define BlackList			4

#define TotalRecharge		2

#define OPERACION_MONEDERO	1
#define RELEASE_SELL		2
#define RECHARGE			4

#define ADDBONO				0       // codigo +BONO         180406
#define ADDBALANCE			0       // codigo +CANTIDAD     180406

#define MaxBlocks2Write		64
#define BLOCKSIZE			16
#define NOBLOCK				0
#define OFFSETYEAR			1900
#define STARTYEAR			2000
#define TOTALMONTHS			12


/* Categoria de tarjeta */
#define SIN_CATEGORIA		0
#define ANONIMOS			1
#define EMPLEADOS			2
#define ESPECIALES			3
#define EMPRESAS_EXTERNAS	4
#define PERSONAL			5

#define TITLEPURSEOPERATION	1

#define xEntryTreatment		7
#define xSUBSTRACTBALANCE	4128
#define xLASTRANSITGAR		1
#define xLASTTRANSITRUF		160
#define NODATA				0

/*define MIA CARD*/
#define NOOPERATION			0
#define MetrorreyApp		3

/*Pointer*/
#define STARTPOINTER		1

/*Transit*/
#define TotalTransit		3

/*Purses*/
#define TotalPurses			2
#define PURSES_SECTOR		3
#define PURSE1_BLOCK		1
#define PURSE2_BLOCK		2

/*Historicals*/
#define TotalMovements		12

/* ****************************************************************************
 * 
 *      MIA Aplication 
 *      Mifare Classic Blocks Names
 *      This definitions are used with the ReadBlock[64][16] 
 *      and WriteBlocks[64][16] arrays
 * 
 * ****************************************************************************/
//#define NoBlock                     0
//#define PURSECURITY_1               1
//#define PURSECURITY_2               2

#define UIDSAMBLOCK                 1

#define InitializationBlock         4
#define CUSTOMBLOCK                 5
#define PURSESELLBLOCK              6
#define OPERATIONSTARTBLOCK         8
#define OPERATIONENDBLOCK           9
    
#define HISTORICALPURSERECHARGE_1   10
#define HISTORICALPURSERECHARGE_2   12
    
#define Purse_1                     13
#define Purse_2                     14

#define LASTTRANSIT_1               16
#define LASTTRANSITFINAL_1          17
#define LASTTRANSIT_2               18
#define LASTTRANSITFINAL_2          20
#define LASTTRANSIT_3               21
#define LASTTRANSITFINAL_3          22

#define HISTORICALMOVEMENTS_1       24
#define HISTORICALMOVEMENTS_2       25
#define HISTORICALMOVEMENTS_3       26
#define HISTORICALMOVEMENTS_4       28
#define HISTORICALMOVEMENTS_5       29
#define HISTORICALMOVEMENTS_6       30
#define HISTORICALMOVEMENTS_7       32
#define HISTORICALMOVEMENTS_8       33
#define HISTORICALMOVEMENTS_9       34
#define HISTORICALMOVEMENTS_10      36
#define HISTORICALMOVEMENTS_11      37
#define HISTORICALMOVEMENTS_12      38
			
#define CONTRACT_1_RELEASE          40
#define CONTRACT_1_TRANSIT_0        41
#define CONTRACT_1_TRANSIT_1        42
    
#define CONTRACT_2_RELEASE          44
#define CONTRACT_2_TRANSIT_0        45
#define CONTRACT_2_TRANSIT_1        46

#define CONTRACT_3_RELEASE          48
#define CONTRACT_3_TRANSIT_0        49
#define CONTRACT_3_TRANSIT_1        50

#define CONTRACT_1_BONUS            52
#define CONTRACT_2_BONUS            53
#define CONTRACT_3_BONUS            54
    
#define CUSTOMPRIVATEDATA_1         56
#define CUSTOMPRIVATEDATA_2         57
#define CUSTOMPRIVATEDATA_3         58

#define CUSTOMPUBLICDATA_1          60
#define CUSTOMPUBLICDATA_2          61
#define CUSTOMPUBLICDATA_3          62


/* *****************************************************************************
 *          Bit Mapping 
 *          Structures Variables 
 * ****************************************************************************/
    
//Initialization Variables, block 04
typedef	struct
{
    unsigned char MapVersion;
    unsigned char SamVersion;
    unsigned char CodeTitleSam1;
    unsigned char CodeTitleSam2;
    unsigned char CodeTitleSam3;
    unsigned int  CountryCode;
    char          State[2];        //unsigned int  State;
    char          City[2];         //unsigned int  City;
    unsigned char ApplicationCode;
    unsigned char MachineInitializer;
    unsigned char InitializationYear;
    unsigned char InitializationMonth;
    unsigned char InitializationDay;
    unsigned char Category;
    unsigned long InitializationSerialNumber;
    unsigned char Check4;
}Initializationbits;

// Custom Card Variables, block 05  
typedef	struct 
{
	unsigned char ActYear;
	unsigned char ActMont;
	unsigned char ActDay;
	unsigned char ExpYear;
	unsigned char ExpMonth;
	unsigned char ExpDay;
	unsigned char OpeCard;
	unsigned char EmployeeProfile;
	unsigned char PersonalizationFlag;
	unsigned char Ruf;
	unsigned char TitleCode;
	unsigned char ContractOperator1;
	unsigned char ContractOperator2;
	unsigned char ContractOperator3;
	unsigned char BirthYear;
	unsigned char BirthMont;
	unsigned char BirthDay;
	char          Language[2];      //unsigned int Language;
	unsigned char Checksum5;
}CustomCardbits;

// Sell Card Variables, block 06
typedef	struct
{
	unsigned char SellYear;
	unsigned char SellMont;
	unsigned char SellDay;
	unsigned char SellHour;
	unsigned char SellMinute;
	unsigned char Operator;
	unsigned int  Line;
	unsigned int  Station;
	unsigned char EquipmentSell;
	unsigned char PurseType;
	unsigned char TypeMachine;
	unsigned char Ruf;
	unsigned char ActivationFlag;
	unsigned long AmountSell;
	unsigned int  Deposit;
	unsigned char Check6;
}SellCardbits;

/*
 * Operation End Backup, block 09
 */

typedef struct
{
    unsigned char Operation;
    unsigned char SubOperation;
    unsigned char PointerPurse;	
    unsigned char PointerRechargePurse;
    unsigned char PointerPurseTraffic;
    unsigned char PointerMove;
    unsigned char PointerOpec;
    unsigned char ContractTraffic1;
    unsigned char ContractBalance1;
    unsigned char ContractTraffic2;
    unsigned char ContractBalance2;
    unsigned char ContractTraffic3;
    unsigned char ContractBalance3;
    unsigned char EspecialFlagCard;
    unsigned char Contract1MsgVESend;
    unsigned char Contract1TitleState;
    unsigned char Contract1SpecialFlag;
    unsigned char Contract2MsgVESend;
    unsigned char Contract2TitleState;
    unsigned char Contract2SpecialFlag;
    unsigned char Contract3MsgVESend;
    unsigned char Contract3TitleState;
    unsigned char Contract3SpecialFlag;
    unsigned long TransactionNumber;
    unsigned long AmountPendingPurse;
    unsigned char MsgVESend;
}BackupOperationEndbits;


/*
 * Operation End variables, block 09
 */
        
typedef struct
{
    unsigned int  Check9;
    unsigned char Operation;
    unsigned char SubOperation;
    unsigned char PointerPurse;	
    unsigned char PointerRechargePurse;
    unsigned char PointerPurseTraffic;
    unsigned char PointerMove;
    unsigned char PointerOpec;
    unsigned char ContractTraffic1;
    unsigned char ContractBalance1;
    unsigned char ContractTraffic2;
    unsigned char ContractBalance2;
    unsigned char ContractTraffic3;
    unsigned char ContractBalance3;
    unsigned char EspecialFlagCard;
    unsigned char Contract1MsgVESend;
    unsigned char Contract1TitleState;
    unsigned char Contract1SpecialFlag;
    unsigned char Contract2MsgVESend;
    unsigned char Contract2TitleState;
    unsigned char Contract2SpecialFlag;
    unsigned char Contract3MsgVESend;
    unsigned char Contract3TitleState;
    unsigned char Contract3SpecialFlag;
    unsigned long TransactionNumber;
    unsigned long AmountPendingPurse;
    unsigned char MsgVESend;
    unsigned char Ruf;
    unsigned long OpeAutCode;
    unsigned char EndRecordOperation;
}OperationEndbits;


/*
 * Operation Start variables, block 08
 */
 
typedef struct
{
    unsigned char Check8;
    unsigned char Operation;
    unsigned char SubOperation;
    unsigned long RecordCount;
    unsigned char CheckBackup;
    unsigned char EndOperation;
    unsigned char EndSubOperation;
    unsigned long EndPointers;
    unsigned char EndEspecialFlagsCard;
    unsigned int  EndEspecialFlagsContract;
    unsigned int  EndTransacionNumber;
    unsigned long EndPendingPayment;
    unsigned char MsgSellCardSend;
    unsigned int  Ruf;
    unsigned char FlagRecordStartOperation;
    OperationEndbits Backup;
}OperationStartbits;        

/* 
 * Historical Recharge block 10 & 12
 */
typedef struct 
{
    unsigned char Year;
    unsigned char Mont;
    unsigned char Day;
    unsigned char Hour;
    unsigned char Minute;
    unsigned char Operator;
    unsigned int  Line;
    unsigned int  Station;
    unsigned char MachineType;
    unsigned char IssuingMachine;
    unsigned char Ruf;
    unsigned long ReloadValue;
    unsigned long PurseBalance;
}HistoricalRechargebits;

// Purse, block 13 & 14    
typedef	struct 
{
	long PurseA;
	long NegPurseA;
	long PurseA1;
	unsigned char AddrA;
	unsigned char NegAddrA;
	unsigned char AddrB;
	unsigned char NegAddrB;  
}Pursebits;

/*
 * Last Transit_0, block 16, 18 & 21
 */
typedef struct
{
	unsigned char Gar;
	unsigned char DecMonth;
	unsigned char Day;
	unsigned char Hour;
	unsigned char Minute;
	unsigned char Operator;
	unsigned int  Line;
	unsigned int  Station;
	unsigned int  EquipmentSell;
	unsigned int  Way;
	unsigned int  Bus;
	unsigned char Ruf;
	long TravelAmount;
}LastTransitbits;

/*
 * Last Transit_1, blocks 17, 20 & 22
 */        
        
typedef struct
{
	unsigned char Ruf[10];
	unsigned char CountTransfers;
	unsigned char CountConsecutiveIn;
	unsigned char CountConsecutiveExit;
	unsigned char CounterTogetherTravelers;
	unsigned long PreviousTripAmount;
	unsigned char LastOperator;
}LastTransitFinalbits;
        
/*
 * Historical Movements 01 - 12, 
 * blocks 24, 25, 26, 28, 29, 30, 32, 33, 34, 36, 37 & 38
 */

typedef struct
{
    unsigned char DecMonth;
    unsigned char Day;
    unsigned char Hour;
    unsigned char Minute;
    unsigned char Operation;
    unsigned char SubOperation;
    unsigned char Operator;
    unsigned int  Line;
    unsigned int  Station;
    unsigned char TravelPeople;
    unsigned char InvTravelPeople;
    unsigned char EquipmentSell;
    unsigned char ValuedAcerCostumer;
    unsigned int  Way;
    unsigned char Transfer;
    unsigned long Balance_;
    unsigned long AmountCharge;
}HistoricalMovementsbits;

/*
 * Release bits by Contracts 01 - 03, 
 * blocks 40, 44 & 48
 */
        
typedef struct
{
    unsigned char Version;
    unsigned char TitleCode;
    unsigned char TypeMachine;
    unsigned char Line;
    unsigned char Station;
    unsigned char Ruf;
    unsigned char Machine;
    unsigned int  VLSN;
    unsigned char GeographicalCriteria;
    unsigned char Destination_Line;
    unsigned char Destination_Station;
    unsigned char Destination_Zone;
    unsigned char DateType;
    unsigned char Type_Year;
    unsigned char Type_Month;
    unsigned char Type_Day;
    unsigned char Type_Hour;
    unsigned char Type_Minute;
    unsigned char BalanceType;
    unsigned long NominalSell;
    unsigned int  FacialSell;

}ReleaseContractbits;         
        
/*
 * Transit bits by contract 01 - 03, 
 * blocks 41, 45 & 49
 */        
        
typedef struct
{
    long Balance0;
    unsigned char Recharge0DateType;
    unsigned char Recharge0Year;
    unsigned char Recharge0Month;
    unsigned char Rechar0Day;
    unsigned char LastRecharge0Index;
    unsigned long Ruf0;
    long Balance1;
    unsigned char Recharge1DateType;
    unsigned char Recharge1Year;
    unsigned char Recharge1Month;
    unsigned char Rechar1Day;
    unsigned char LastRecharge1Index;
    unsigned long Ruf1;
}TransitContractbits;

/*
 * Custom Private Data bits, 
 * blocks 56, 57 & 58
 */               

typedef struct{
    char CompanyCode[12];				// 12 bytes
    unsigned char EmployeePosition[4];	// 32 bits
    unsigned char RUF1[2];				// 2 bytes
    int  LoginSize;						// 4 bits
    int  PwdSize;						// 4 bits
    unsigned char Login[4];				// 4 bytes  
    unsigned char Password[4];			// 4 bytes  
    unsigned char CardType;				// 8 bits
    unsigned char HIWUSR[4];			// 32 bits
    char Deparment[6];					// 6 bytes
    unsigned long EmployeeCode;			// 32 bits
    unsigned char RUF2[6];				// 6 bytes
}CustomPrivateDatabits;

/*
 * Custom Public Data bits, 
 * blocks 60, 61 & 62
 */               

typedef struct{
    int  Gender;			// 4 bits
    int  NIDType;			// 4 bits, Type of Number of personal identification
    char NID[20];			// 15 bytes, Number of personal identification
    char  Name_1[16];		// 16 bytes   
    char  Name_2[16];		// 16 bytes
}CustomPublicDatabits;

typedef struct
{
    unsigned char Check8;
    unsigned char CheckBackup;
    unsigned int Check9;

}Check;    

/*Credit */
typedef struct
{
     long Fare;
     long Bonus;
     long Purse;
     long RemBonus;
     long RemPurse;
     long ToPay;

}Balance;
/*
 * Operations Card variables
 */
              
typedef struct 
{
    int         NumberBlocks2Write;
    int /*u8*/  Blocks[MaxBlocks2Write];
    int /*u8*/  SourceCreditBlock;
    int /*u32*/ Decrement;
    int /*u32*/ Increment;
    int /*u32*/ Cantidad;
    long        Purse;
    int         Msg;
    Balance     Credits;
    unsigned char Category;
    unsigned char TitleCode;
    char        CURP[20];
    int         Line;
    int         Station;
}OperationCard; 

typedef struct{
    unsigned char   type_card;
    char            uid_card[16];
    char            LogicNumber[64];            // Numero Logico o ItemID, identificador
                                                // unico de la tarjeta.
                                                // Fecha de expiración de la tarjeta
    char            uid_sam_1[16];
    char            uid_sam_2[16];
    char            uid_sam_3[16];
    char            uid_sam_4[16];
    double          fare;
    unsigned long   payment_seq;
    double          balance_ewallet;
    double          final_balance_ewallet;
    double          balance_bond;
    double          final_balance_bond;
    char            created_at[32];
    int             id_station;
    char            id_equipment[16];                                           //NodeCode: Company,Line,Station,Tray,NumDisp
}TransactionInfo;

#endif /* BITMAP_H */