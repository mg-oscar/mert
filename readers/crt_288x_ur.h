#ifndef __CRT_288K_UR__
#define __CRT_288K_UR__
#include <stdbool.h>

//communication protocol
#define ENQ  0x05
#define ACK  0x06
#define NAK  0x15
#define EOT  0x04
#define CAN  0x18
#define CSTX  0xF2
#define ETX  0x03
#define US   0x1F

//error code
#define CRT_ERRCOUNT                 -200
#define CRT_ERR_CMD                  (CRT_ERRCOUNT-1) //command  charactor error
#define CRT_ERR_CMDPARAM             (CRT_ERRCOUNT-2) //command parameter error
#define CRT_ERR_CMDDENIAL            (CRT_ERRCOUNT-3) //command can't be executed
#define CRT_ERR_DEVNOTSUP            (CRT_ERRCOUNT-4) //hardware doesn't support
#define CRT_ERR_CMDDATA              (CRT_ERRCOUNT-5) //command data error
#define CRT_ERR_LOCKCARD             (CRT_ERRCOUNT-11) //fail to operate latch hook
#define CRT_ERR_EEPROM               (CRT_ERRCOUNT-15) //EEPROM error
#define CRT_ERR_TRACKCHECK           (CRT_ERRCOUNT-20) //read Mag-card error£šparity error£©
#define CRT_ERR_READTRACK            (CRT_ERRCOUNT-21) //read Mag-card error
#define CRT_ERR_POWERDOWN            (CRT_ERRCOUNT-30) //ICRW power dump£šPower down£©
#define CRT_ERR_ICPROCESS            (CRT_ERRCOUNT-41) //fail to operate IC card module
#define CRT_ERR_ICPOWERSHORT         (CRT_ERRCOUNT-60) //IC card power short cut
#define CRT_ERR_ICACTIVATIONFAIL     (CRT_ERRCOUNT-61) //IC card fail to activate 
#define CRT_ERR_ICDENIAL             (CRT_ERRCOUNT-62) //IC card doesn't support current command
#define CRT_ERR_ICNOTRESPOND         (CRT_ERRCOUNT-63) //IC card communication error
#define CRT_ERR_ICNOTRESPOND_OTHER   (CRT_ERRCOUNT-64) //IC card communicate error (other)
#define CRT_ERR_ICNOTACTIVATION      (CRT_ERRCOUNT-65) //IC card isn't activated
#define CRT_ERR_ICNOTSUP             (CRT_ERRCOUNT-66) //ICRW doesn't support this kind IC card
#define CRT_ERR_ICEMV                (CRT_ERRCOUNT-66) //no support EMV mode
#define CRT_ERR_COMMTIMEOUT	         (CRT_ERRCOUNT-80) //communication time out, failure
#define CRT_ERR_CANCEL	             (CRT_ERRCOUNT-84) //command cancel
#define CRT_ERR_PWDPROCESS	         (CRT_ERRCOUNT-90) //fail to operate parity password
#define CRT_ERR_PWDCHECK	         (CRT_ERRCOUNT-91) //faiure of parity password
#define CRT_ERR_PWDSCRAP	         (CRT_ERRCOUNT-92) //faiure of parity password£¬the card is locked
#define CRT_ERR_PWDADDROVERFLOW	     (CRT_ERRCOUNT-93) //operation address overflow
#define CRT_ERR_PWDLENOVERFLOW	     (CRT_ERRCOUNT-94) //operation Len overflow
#define CRT_ERR_PWDPMERR	         (CRT_ERRCOUNT-95) //PM error
#define CRT_ERR_PWDCMERR	         (CRT_ERRCOUNT-96) //CM error
#define CRT_ERR_UNKNOWN              (CRT_ERRCOUNT-99) //unknown error

//open device
#define CRT_OPEN_TYPEUSB             0 //USB to open
#define CRT_OPEN_TYPERS232           1 //RS232 to open

//initialization
#define CRT_INIT_NOTUNLOCK          0 //initialization no action
#define CRT_INIT_UNLOCK             1 //initialization unlock


//card status
#define CRT_CARDST_NOCARD             1 //no card
#define CRT_CARDST_INDOOR             2 //card insertion
#define CRT_CARDST_INSIDE             3 //card inside
#define CRT_CARDST_UNKNOW             9 //device is offline, the card status unknown

//card latch status
#define CRT_LOCKCARD_NOTLOCK            1 //no latch
#define CRT_LOCKCARD_LOCKED             2 //locked
#define CRT_LOCKCARD_UNKNOW             9 //device is offline, the card latch status unknown

//reader action
#define CRT_RDACTION_NOACTION           0 //no action
#define CRT_RDACTION_UNLOCK             1 //unlock
#define CRT_RDACTION_LOCKED             2 //card latch
#define CRT_RDACTION_AUTOLOCKED         3 //auto-latch with card
#define CRT_RDACTION_NOTAUTOLOCK        4 //no auto-latch with card

//operation LED 
#define  CRT_LED_RED                    0 //operation red indicator
#define  CRT_LED_BLUE                   1 //operation blue indicator
#define  CRT_LED_ALL                    2 //operation all indicators

//LED control
#define  CRT_LED_OFF                    0 // off
#define  CRT_LED_ON                     1 // on
#define  CRT_LED_FLASHING               2 // flash

//Mag-card reading position
#define CRT_SETMAGTYPE_READASCII        0x00000000 //reading with ASCII
#define CRT_SETMAGTYPE_READHEX          0x00000001 //read with binary
#define CRT_SETMAGTYPE_AUTOUPLOAD       0x00000000 //active to upload track data
#define CRT_SETMAGTYPE_NOTUPLOAD        0x00000010 //prohibit to active to upload track data
#define CRT_SETMAGTYPE_EFFINSERT        0x00000000 //data is valid when insert the card
#define CRT_SETMAGTYPE_EFFUPPLUG        0x00000100 //data is valid when pull the card

//read Mag-track
#define CRT_ReadTracks_Track1           1 //track 1 data
#define CRT_ReadTracks_Track2           2 //track 2 data
#define CRT_ReadTracks_Track3           3 //track 3 data

// MIFARE KEY PROCESS
#define CRT_MIFARE_KEY_VERIFICATION			0x20
#define CRT_MIFARE_KEY_VERIFICATION_EEPROM	0x21
#define CRT_MIFARE_DOWNLOAD_PASS_EEPROM		0xD0
#define CRT_MIFARE_MODIFICATION_SECTOR_KEY	0xD5			


// MIFARE DATA PROCESS
#define CRT_MIFARE_READ_SECTOR				1		// 0xB0
#define CRT_MIFARE_WRITE_SECTOR				2		// 0xD1

// MIFARE VALUE PROCESS
#define CRT_MIFARE_INITIALIZATION			3		// 0xD2
#define CRT_MIFARE_READ_BALANCE				4		// 0xB1
#define CRT_MIFARE_INCREMENT				5		// 0xD3
#define CRT_MIFARE_DECREMENT				6		// 0xD4

class crt285: public usbdev, rs232
{
public:

	crt285();
	
	int m_iMode = 0;		// static
	bool g_bOpen = false;	// static

	BYTE m_CurICCommend = 0;
	BYTE m_Vcc = 0x30;
	int CRT_GetCurICCommend(int iType);

	int m_iFalshOn = 1;
	int m_iFalshOff = 1;
	bool m_bUploadData = false;
	char m_szReadData[1024] = {0};
	char m_szWriteData[1024] = {0};

	void Bcd2Asc(const unsigned char *psIn, const unsigned int uiLength, unsigned char *psOut);
	bool IsAscData(BYTE byInData[],  int iLen);
	void Asc2Bcd(const unsigned char *psIn, unsigned int uiLength, unsigned char *psOut);
	unsigned char CalXOR(unsigned char strOrder[],unsigned int strLen);
	int CRT_Write(unsigned int length,unsigned char buffer[]);
	int OrganizePacket(int iSendDataLen, BYTE bySendData[], int* iOutPacketLen, BYTE byOutPacketData[]);
	int AnalysisPacket(int *iRecvDataLen, BYTE byRecvData[], BYTE bystCode[]);
	int Recv_ACK(void);
	int CRT_RedProcess(int iFlag);
	int CRT_BlueProcess(int iFlag);
	int CheckPasswdErrCode(BYTE byData[], int iDataLen);
	int CRT_Read(unsigned int length,unsigned char buffer[]);
	int Recv_Data(int *iRecvDataLen, BYTE byRecvData[]);
	int GetErrCode(BYTE byStCode[]);
	int Recv_Data_USB(int *iRecvDataLen, BYTE byRecvData[]);
	int Recv_Data_RS232(int *iRecvDataLen, BYTE byRecvData[]);
	int CRT288x_OpenConnection(int iOpenMode, int iComPort, long lBaudRate);
	int CRT288x_CloseConnection(void);
	int CRT288x_ExeCommand(int iSendDataLen, unsigned char bySendData[] , 
							int* iRecvDataLen, unsigned char byRecvData[], 
							unsigned char byStCode[]);
	int CRT288x_InitDev(int InitMode );
	int CRT288x_GetCardStatus();
	int CRT288x_GetLockStatus();
	int CRT288x_LockedProcess(int iLockType);
	int CRT288x_LEDProcess(int iLightType, int iFlag);
	int CRT288x_SetLEDFlashTime(int iOnTime, int iOffTime);
	int CRT288x_SetReaderMagType(int iReadMode, int iDataMode, bool bAutoUpload);
	int CRT288x_ReadTrack(int iTrackNum, char szTrackData[]);
	int CRT288x_ReadAllTracks(char szTrack1Data[], char szTrack2Data[], char szTrack3Data[]);
	int CRT288x_ClearTrackData();
	int CRT288x_GetICType();
	int CRT288x_GetRFType();
	int CRT288x_PowerChip(int iType, unsigned int wChipPower, 
										unsigned char byOutChipData[], int* iOutChipDatalen);
	int CRT288x_ICPower(int iType, unsigned int wChipPower, unsigned char byOutChipData[], 
															int* iOutChipDatalen);
	int CRT288x_ChipIO(unsigned int wChipProtocol, int ulInChipDataLength, 
						unsigned char lpInbChipData[], int* ulOutChipDataLength, 
						unsigned char lpOutbChipData[]);
	int CRT288x_GetCardActiveStatus();
	int CRT288x_SAMSlotChange(int iSamNum);
	int CRT288x_SAMActivation(int type, int*datalen, unsigned char atr[]);
	int CRT288x_SAMApdu(int Type,int InDataLength, unsigned char InData[], int* OutDataLength, unsigned char OutData[]);
	int CRT288x_SetVcc(int iVcc);
	int CRT288x_SL4442CheckPasswd(int iMode, int uDataLength, unsigned char lpData[]);
	int CRT288x_SL4442Process(int iMode, int iRegion, unsigned int wStartAddr, int* uDataLength, unsigned char lpData[]);
	int CRT288x_SL4428CheckPasswd(int iMode, int uDataLength, unsigned char lpData[]);
	int CRT288x_SL4428Process(int iMode, int iRegion, unsigned int wStartAddr, int* uDataLength, unsigned char lpData[]);
	int CRT288x_24CxProcess(int iMode, unsigned int wStartAddr, int* uDataLength, unsigned char lpData[]);
	int CRT288x_MifareKeyProcess(int iMode, int iKs, int iSn, int uDataLength, unsigned char lpData[]);
	int CRT288x_MifareCardProcess(int iMode, int iSn, int iBn, int iLc, int* uDataLength, unsigned char lpData[]);
	int CRT288C_GetHidCardNums(char szHidCardNums[]);

private:


};

#endif 