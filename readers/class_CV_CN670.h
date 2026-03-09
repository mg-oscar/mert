/*
 * File:   class_civintec.h
 * Author: FAMISA
 *
 * Created on October 3, 2022, 4:16 PM
 */

// MIFARE

#ifndef CLASS_CV_CN670_H
#define CLASS_CV_CN670_H

#define CMD_SetAddress			0x06
#define CMD_GetSerialNum		0x09
#define CMD_GetVerNum			0x0A
#define CMD_SetUserInfo			0x0B
#define CMD_GetUserInfo			0x0C
#define CMD_SetAutoPowerSave	0x0D
#define CMD_SetWiegandModeLB	0x12
#define CMD_SetWiegandMode		0x18
#define CMD_GetWiegandMode		0x1d
#define CMD_SetLED				0x24
#define CMD_ActiveLED			0x25
#define CMD_ActiveBuzzer		0x26
#define CMD_PN_InListPassive	0x3D
#define CMD_PN_SelectTarget		0x3E
#define CMD_MF_Auth             0x40
#define CMD_MF_Read				0x41
#define CMD_MF_Write			0x42
#define CMD_MF_Transfer			0x43
#define CMD_MF_Value			0x44
#define CMD_MF_LoadKey			0x45
#define CMD_MF_LoadKeyFromEE	0x46
#define CMD_MF_StoreKeyToEE		0x47
#define CMD_ISO7816_APDU		0xE6



#define STX				0x02
#define ETX				0x03

#define BuzzerOff		0x00
#define BuzzerOn		0x01

#define LED_RED			0x01
#define LED_GREEN		0x02
#define LED_ALL_ON		0x03
#define LED_ALL_OFF		0x00


class CLASS_CV_CN670: public usbdev
{
public:
	unsigned char tot_reader;		// total de lectores encontrados
	unsigned char num_reader[10];	// numero de lector 0..9

	CLASS_CV_CN670();

	//**************************************************************************
	int CV_OpenUSB(unsigned char ndrv);
	int CV_CloseUSB(void);

	int GetSerialNum(unsigned char addr, int *Address, char *SerialNUM);
	int CardPresent( unsigned char *MFUID );

	int	ActiveBuzzer(unsigned char addr, unsigned char modo, unsigned char *buf);

	int GetUserInfo(int addr, char *name);
	int SetUserInfo(int addr, char *name);

	int SetLED(int addr, unsigned char modo);


	int SetAutoPowerSave(unsigned char addr, unsigned char modo);

	int SendSetWiegandMode(unsigned char addr);
	int SendGetWiegandMode(unsigned char addr);
	int SendSetWiegandModeLB(unsigned char addr, unsigned char op);

//	int ReadComm(void);
	
	int	CloseComm(void);
	int PN_InListPassiveTarget(int addr, unsigned char CardType, unsigned char MaxTg, 
				unsigned char BaudRate, unsigned char IniDataLen, 
				unsigned char *IniData, unsigned char *NbTg, 
				unsigned char *RDataLen,unsigned char *RData);

	int ISO7816_ATR(BYTE addr, BYTE var1, BYTE *byRecvData , BYTE iRecvDataLen, int nmax);
	int ISO7816_APDU_Exchange(BYTE addr, BYTE var1, BYTE *apdu, BYTE var2, 
			BYTE *byRecvData, BYTE iRecvDataLen, int nmax);

	int MF_Read (BYTE addr, BYTE nblock, BYTE tblock, BYTE *buffer);
	int MF_Write(BYTE addr, BYTE nblock, BYTE tblock, BYTE *buffer);
	int PN_SelectTarget(BYTE addr, BYTE ncard);

	int MF_Decrement(BYTE addr, BYTE PurseBlock, int valor);
	int MF_Transfer(BYTE addr, BYTE PurseBlock);
	int MF_LoadKey(BYTE addr, BYTE *Key);
	int MF_Auth(BYTE addr, BYTE tkey, BYTE *MFUID, BYTE nblk);
	//**************************************************************************/

private:

	int	SendCmd(unsigned char addr, unsigned char *datos, int len, unsigned char *dRx);
	int rdResponse(unsigned char *bufRx, unsigned char seq);

};

#endif /* CLASS_CV_CN670_H */
