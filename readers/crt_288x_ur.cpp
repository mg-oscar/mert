//
//
//
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
crt285::crt285()
{
	m_iMode = 0;
	g_bOpen = false;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
bool crt285::IsAscData(BYTE byInData[],  int iLen)
{
	int i=0;
	for ( ;i<iLen;i++)
	{
		if(	(byInData[i] >= '0' && byInData[i] <= '9') || 
			(byInData[i] >= 'a' && byInData[i] <= 'f') || 
			(byInData[i] >= 'A' && byInData[i] <= 'F')	)
		{

		}
		else
		{
			return false;
		}
	}
	return true;
}

/******************************************************************************/
/* Convierte una cdeda hexadecimal a ASCII                                    */
/******************************************************************************/
void crt285::Bcd2Asc(const unsigned char *psIn, const unsigned int uiLength, unsigned char *psOut)
{
	static const unsigned char ucHexToChar[20] = {"0123456789ABCDEF"};
	unsigned int uiCnt;

	if ((psOut==NULL) || (psIn==NULL))
	{
		return;
	}

	for (uiCnt=0; uiCnt<uiLength; uiCnt++)
	{
		psOut[2*uiCnt] = ucHexToChar[psIn[uiCnt]>>4];
		psOut[2*uiCnt+1] = ucHexToChar[psIn[uiCnt] & 0x0F];
	}
}


/*void crt285::Bcd2Asc(const unsigned char *psIn, const unsigned int uiLength, unsigned char *psOut)
{
	static const unsigned char ucHexToChar[20] = {"0123456789ABCDEF"};
	unsigned int uiCnt;

	if ((psOut==NULL) || (psIn==NULL))
	{
		return;
	}

	for (uiCnt=0; uiCnt<uiLength; uiCnt++)
	{
		psOut[2*uiCnt] = ucHexToChar[psIn[uiCnt]>>4];
		psOut[2*uiCnt+1] = ucHexToChar[psIn[uiCnt] & 0x0F];
	}
}
*/
void crt285::Asc2Bcd(const unsigned char *psIn, unsigned int uiLength, unsigned char *psOut)
{
	unsigned char ucTemp;
	unsigned int i;

	if ((psOut==NULL) || (psIn==NULL))
	{
		return;
	}
	

	for (i=0; i<uiLength; i+=2)
	{
		ucTemp = psIn[i];
		if (ucTemp > '9' )
		{
			ucTemp = (unsigned char)toupper((int)ucTemp) -'A'+0x0A;
		}
		else
		{
			ucTemp &= 0x0F;
		}
		psOut[i/2] = (ucTemp<<4);

		ucTemp = psIn[i+1];
		if (ucTemp > '9' )
		{
			ucTemp = (unsigned char)toupper((int)ucTemp) -'A'+0x0A;
		}
		else
		{
			ucTemp &= 0x0F;
		}
		psOut[i/2] |= ucTemp;

	}

}


unsigned char crt285::CalXOR(unsigned char strOrder[],unsigned int strLen)
{
	unsigned char XORValue=0;
	unsigned int ii=0;	
	for(;ii<strLen;ii++)
	{
		XORValue^=strOrder[ii];
	}
	return XORValue;
}

int crt285::CRT_Write(unsigned int length,unsigned char buffer[])
{   char hexa[5];

	int iRet = 0;

	if (m_iMode != 1 && m_iMode != 0)
	{
		return CRT_ERR_CMDPARAM;
	}

	memset(m_szWriteData, 0, sizeof(m_szWriteData));
	int i=0;	
	for (i=0;i<length;i++)
	{	sprintf(hexa, "%02X ", buffer[i]);
		strcat(m_szWriteData, hexa);
	}

	m_bUploadData = false;

	if(m_iMode == 1)
	{
		iRet = RS232_Write( length, buffer);
	}
	else if (m_iMode == 0)
	{
		int iMultiple = (length+63)/64;
		int i= 0;
		BYTE byTempSend[64] = {0};

		memset(byTempSend, 0xFF, sizeof(byTempSend));

		for (;i<iMultiple-1;i++)
		{
			memset(byTempSend, 0xFF, sizeof(byTempSend));
			memcpy(byTempSend, buffer+i*64, 64);
			iRet = USB_Write(64, byTempSend);
			if (iRet != 0)
			{
				return iRet;
			}
		}
		memset(byTempSend, 0xFF, sizeof(byTempSend));
		memcpy(byTempSend, buffer+i*64, length-i*64);
		iRet = USB_Write(length-i*64, byTempSend);
	}

	return iRet;

}


int  crt285::CRT288x_OpenConnection(int iMode, int iComPort, long BaudRate)
{

	int iRet = 0;
	m_iMode = iMode;
	if (m_iMode == 1)
	{
//		iRet = RS232_OpenWithBaudRate(iComPort, BaudRate);
	}
	else if (m_iMode == 0)
	{
//		iRet = USB_Open();
	}
	else
	{
		return CRT_ERR_CMDPARAM;
	}

	if (iRet == 0)
	{
		iRet = CRT288x_InitDev(0);
	}
	return iRet;
}

int  crt285::CRT288x_CloseConnection()
{
	if (m_iMode == 1)
	{
		RS232_Close();
	}
	else if (m_iMode == 0)
	{
		 USB_Close();
	}
	else
	{
		return CRT_ERR_CMDPARAM;
	}

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int  crt285::CRT288x_ExeCommand(	int iSendDataLen, 
									BYTE bySendData[] , 
									int* iRecvDataLen, 
									BYTE byRecvData[],
									BYTE byStCode[]	)
{	int iRet = 0;

	BYTE TempSendBuf[1024] = {0};
	int iTempSendLen = 0;

	iRet = OrganizePacket(iSendDataLen, bySendData, &iTempSendLen, TempSendBuf);
	if(iRet != 0)
	{
		printf("CRT288x_ExeCommand OrganizePacket failed! \n");
		return iRet;
	}

	//DWORD dwStartTime = GetTickCount();
	iRet = CRT_Write(iTempSendLen, TempSendBuf);
	if(iRet != 0)
	{
		printf("CRT288x_ExeCommand Send Command failed! \n");
		return iRet;
	}

	iRet = AnalysisPacket(iRecvDataLen, byRecvData, byStCode);

	return iRet;
}


int crt285::CRT288x_InitDev(int InitMode )
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x30;
	switch(InitMode)
	{
	case CRT_INIT_UNLOCK:
		{
			cmdData[2] = 0x30;
		}
		break;
	case CRT_INIT_NOTUNLOCK:
		{
			cmdData[2] = 0x31;
		}
		break;
	default:
		{
			return CRT_ERR_CMDPARAM;
		}
		break;
	}

	cmdDataLen = 3;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return 0;
}

/******************************************************************************/
int  crt285::CRT288x_GetCardStatus()
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x31;
	cmdData[2] = 0x30;
	cmdDataLen = 3;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet => %d\n",iRet);
		return CRT_CARDST_UNKNOW;
	}

	if(stCode[0] != 0x50)
	{
		return CRT_CARDST_UNKNOW;
	}

	switch(stCode[2])
	{
	case 0x30:
		return CRT_CARDST_NOCARD;
		break;
	case 0x31:
		return CRT_CARDST_INDOOR ;
		break;
	case 0x32:
		return CRT_CARDST_INSIDE;
		break;
	default:
		break;

	}

	return CRT_CARDST_UNKNOW;
}


int  crt285::CRT288x_GetLockStatus()
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x31;
	cmdData[2] = 0x30;
	cmdDataLen = 3;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return CRT_LOCKCARD_UNKNOW;
	}

	if(stCode[0] != 0x50)
	{
		return CRT_LOCKCARD_UNKNOW;
	}
	switch(stCode[1])
	{
	case 0x30:
		return CRT_LOCKCARD_LOCKED;
		break;
	case 0x31:
		return CRT_LOCKCARD_NOTLOCK;
		break;
	default:
		break;

	}

	return CRT_LOCKCARD_UNKNOW ;
}

int  crt285::CRT288x_LockedProcess(int iType)
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0xB0;
	switch(iType)
	{
	case CRT_RDACTION_NOACTION:
		{
			return 0;
		}
		break;
	case CRT_RDACTION_UNLOCK:
		{
			cmdData[2] = 0x31;
		}
		break;
	case CRT_RDACTION_LOCKED:
		{
			cmdData[2] = 0x30;
		}
		break;
	case CRT_RDACTION_AUTOLOCKED:
		{
			cmdData[2] = 0x32;
		}
		break;
	case CRT_RDACTION_NOTAUTOLOCK:
		{
			cmdData[2] = 0x33;
		}
		break;
	default:
		{
			return CRT_ERR_CMDPARAM;
		}
		break;
	}
	cmdDataLen = 3;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return 0;
}

int crt285::CRT_RedProcess(int iFlag)
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x80;
	cmdData[2] = 0x30;
	cmdData[3] = 0x30;  //PM1

	cmdDataLen = 5;
	switch(iFlag)
	{
	case CRT_LED_OFF:
		cmdData[4] = 0x30;
		break;
	case CRT_LED_ON:
		cmdData[4] = 0x31;
		break;
	case CRT_LED_FLASHING:
		cmdData[2] = 0x31;
		cmdData[4] = m_iFalshOn;
		cmdData[5] = m_iFalshOff;
		cmdDataLen = 6;
		break;
	}

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return 0;
}

int crt285::CRT_BlueProcess(int iFlag)
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x80;
	cmdData[2] = 0x30;
	cmdData[3] = 0x31;  //PM1

	cmdDataLen = 5;
	switch(iFlag)
	{
	case CRT_LED_OFF:
		cmdData[4] = 0x30;
		break;
	case CRT_LED_ON:
		cmdData[4] = 0x31;
		break;
	case CRT_LED_FLASHING:
		cmdData[2] = 0x31;
		cmdData[4] = m_iFalshOn;
		cmdData[5] = m_iFalshOff;
		cmdDataLen = 6;
		break;
	}

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return 0;
}



int  crt285::CRT288x_LEDProcess(int iType, int iFlag)
{
	int iRet = 0;
	switch(iType)
	{
	case CRT_LED_RED:
		iRet = CRT_RedProcess(iFlag);
		break;
	case CRT_LED_BLUE:
		iRet = CRT_BlueProcess(iFlag);
		break;
	case CRT_LED_ALL:
		iRet = CRT_RedProcess(iFlag);
		iRet = CRT_BlueProcess(iFlag);
		break;
	default:
		return CRT_ERR_CMDPARAM;
	}

	return iRet;
}



int  crt285::CRT288x_SetLEDFlashTime(int iOnTime, int iOffTime)
{
	if (iOnTime<0 || iOnTime>0xFF || iOffTime<0 || iOffTime>0xFF)
	{
		return CRT_ERR_CMDPARAM;
	}
	m_iFalshOn = iOnTime;
	m_iFalshOff = iOffTime;

	return 0;
}

int  crt285::CRT288x_SetReaderMagType(int iReadMode, int iDataMode, bool bAutoUpload)
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x36;
	cmdData[2] = 0x30;

	if (iReadMode == 0)
	{
		cmdData[3] = 0x30;
	}
	else if (iReadMode == 1)
	{
		cmdData[3] = 0x31;
	}
	else
	{
		return CRT_ERR_CMDPARAM;
	}

	if (bAutoUpload)
	{
		cmdData[4] = 0x30;
	}
	else
	{
		cmdData[4] = 0x31;
	}

	cmdData[5] = 0x37;

	if (iDataMode == 0)
	{
		cmdData[6] = 0x30;
	}
	else if (iDataMode == 1)
	{
		cmdData[6] = 0x31;
	}
	else
	{
		return CRT_ERR_CMDPARAM;
	}

	cmdDataLen = 7;
	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return 0;

}

int  crt285::CRT288x_ReadAllTracks(char szTrack1Data[], char szTrack2Data[], char szTrack3Data[])
{
	CRT288x_ReadTrack(1,szTrack1Data);
	CRT288x_ReadTrack(2,szTrack2Data);
	CRT288x_ReadTrack(3,szTrack3Data);
	//ŽŠÀíŽÅµÀÐÅÏ¢
	if( strlen(szTrack1Data) == 0 && strlen(szTrack2Data)==0 && strlen(szTrack3Data) == 0)
		return CRT_ERR_READTRACK;
	return 0;
}

int  crt285::CRT288x_ReadTrack(int iTrackNum, char szTrackData[])
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x36;
	cmdData[2] = 0x31;
	cmdData[3] = 0x30;
	if(iTrackNum == 1)
		cmdData[4] = 0x31;
	else if(iTrackNum == 2)
		cmdData[4] = 0x32;
	else if(iTrackNum == 3)
		cmdData[4] = 0x33;
	else
		return CRT_ERR_CMDPARAM;

	cmdDataLen = 5;
	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}
	if (RecvDataLen <= 1 || RecvData[0] != 'P')
	{
		return CRT_ERR_READTRACK;
	}

	memcpy(szTrackData, RecvData+1, RecvDataLen-1); //È¥µôP
	return 0;
}


int  crt285::CRT288x_ClearTrackData()
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x36;
	cmdData[2] = 0x39;

	cmdDataLen = 3;
	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return 0;
}


int  crt285::CRT288x_GetICType()
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x50;
	cmdData[2] = 0x30;

	cmdDataLen = 3;
	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return (RecvData[0]-'0')*10+(RecvData[1]-'0');
}


int crt285::CRT288x_GetRFType()
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x50;
	cmdData[2] = 0x31;

	cmdDataLen = 3;
	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return 100+(RecvData[0]-'0')*10+(RecvData[1]-'0');
}


int  crt285::CRT288x_PowerChip(int iType, LWORD wChipPower, BYTE byOutChipData[], int* iOutChipDatalen)
{	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 3;

	cmdData[0] = 0x43;

	iRet = CRT_GetCurICCommend(iType);
	if (iRet != 0)
	{
		return iRet;
	}
	cmdData[1] = m_CurICCommend;

	bool bColdRetset = false;
	if (wChipPower & 0x02) //ÀäžŽÎ»
	{
		bColdRetset = true;
		cmdData[2] = 0x30;

		if (iType>100 && iType<200) //RF¿š
		{
			cmdData[3] = 'A';
			cmdData[4] = 'B';
			cmdDataLen = 5;
		}
		if (iType == 10 || iType == 11 || iType == 210) //T0, T1 CPU¿š, SAM¿š
		{
			cmdData[3] = m_Vcc;
			cmdDataLen = 4;
		}
	}
	else if (wChipPower & 0x04) //ÈÈžŽÎ»
	{
		cmdData[2] = 0x38;
	}
	else if (wChipPower & 0x08) //ÏÂµç
	{
		cmdData[2] = 0x31;
	}
	else
	{
		return CRT_ERR_CMDPARAM;
	}

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	if (bColdRetset)
	{
		if (iType == 10 || iType == 11 || iType == 210) //T0, T1 CPU¿š, SAM¿š µÚÒ»žöÐ­Òé×ŽÌ¬ÂëÈ¥µô ×îºó×ŽÌ¬Âë
		{
			*iOutChipDatalen = RecvDataLen-1;
			memcpy(byOutChipData, RecvData+1, RecvDataLen-1);
		}
		else
		{
			*iOutChipDatalen = RecvDataLen;
			memcpy(byOutChipData, RecvData, RecvDataLen);
		}

	}

	return 0;
}


int  crt285::CRT288x_ChipIO(LWORD wChipProtocol, int ulInChipDataLength , BYTE lpInbChipData[], int* ulOutChipDataLength , BYTE lpOutbChipData[])
{

	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = m_CurICCommend;
	if (wChipProtocol == 1)
	{
		cmdData[2] = 0x33;  //T0
	}
	else if (wChipProtocol == 2)
	{
		cmdData[2] = 0x34; //T1
	}
	else
	{
		if ( m_CurICCommend==0x60) //m_CurICCommend==0x52 ||
		{
			cmdData[2] = 0x33;
		}
		else
		{
			cmdData[2] = 0x39; //×Ô¶¯Ñ¡Ôñ
		}
		
	}

	if (m_CurICCommend ==0x60 )
	{
		cmdData[2] += 1;
	}

	int i;

	BYTE chTempSend[1024]= {0};
	int chTempSendLen = 0;
	if (IsAscData(lpInbChipData,ulInChipDataLength))
	{
		Asc2Bcd(lpInbChipData, ulInChipDataLength, chTempSend);
		chTempSendLen = ulInChipDataLength/2;	
	}
	else
	{
		chTempSendLen = ulInChipDataLength;
		memcpy(chTempSend, lpInbChipData, ulInChipDataLength);	
	}

	memcpy(cmdData+3, chTempSend, chTempSendLen);
	cmdDataLen = chTempSendLen+3;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	*ulOutChipDataLength = RecvDataLen;
	memcpy(lpOutbChipData, RecvData, RecvDataLen);

	return 0;
}

int  crt285::CRT288x_GetCardActiveStatus()
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 3;

	cmdData[0] = 0x43;
	cmdData[1] = m_CurICCommend;
	cmdData[2] = 0x32;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	if (RecvData[0] == 0x30) //¿šÆ¬ÎŽŒ€»î
	{
		return 3;
	}
	else if (RecvData[0] == 0x31)  //¿šÆ¬ÒÑŒ€»î£¬µ±Ç°CPU ¿š¹€×÷Ê±ÖÓÆµÂÊÎª3.57 MHZ
	{
		return 1;
	}
	else if (RecvData[0] == 0x32) //¿šÆ¬ÒÑŒ€»î£¬µ±Ç°CPU ¿š¹€×÷Ê±ÖÓÆµÂÊÎª7.16 MHZ
	{
		return 2;
	}
	else
	{
		return 9;
	}
}

int  crt285::CRT288x_SetVcc(int iVcc)
{
	if (iVcc <1 || iVcc >3)
	{
		return CRT_ERR_CMDPARAM;
	}
	if (iVcc == 1)  //5V EMV
	{
		m_Vcc = 0x30;
	}
	else if (iVcc == 2) //5V ISO7816
	{
		m_Vcc = 0x33;
	}
	else
	{
		m_Vcc = 0x35;
	}
	return 0;
}

int crt285::CRT288x_SAMSlotChange(int iSamNum)
{
	if (iSamNum <1 || iSamNum >10)
	{
		return CRT_ERR_CMDPARAM;
	}

	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 4;

	cmdData[0] = 0x43;
	cmdData[1] = 0x52;
	cmdData[2] = 0x40;
	cmdData[3] = 0x30+iSamNum-1;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return 0;

}

int crt285::CRT288x_SAMActivation(int type, int* datalen, BYTE atr[])
{	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};
	int RecvDataLen = 0;
	int cmdDataLen = 3;
	BYTE btype=0;
	
	if(type==0)
		btype=0x30;
	else if(type==1)
		btype=0x33;
	else
		btype=0x35;

	cmdData[0] = 0x43;
	cmdData[1] = 0x52;
	cmdData[2] = 0x30;
	cmdData[3] = btype;
	cmdDataLen = 4;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		//LOGE("SAMActivation ExeCommand error :%d", iRet);
		return iRet;
	}

	memcpy(atr, RecvData, RecvDataLen);
	*datalen = RecvDataLen;
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int crt285::CRT288x_SAMApdu(	int Type,
								int InDataLength , 
								BYTE InData[], 
								int* OutDataLength , 
								BYTE OutData[]		)
{
//	printf("enter   \n");
	BYTE btype=0;
	if(Type==0)
		btype=0x33;
	else if(Type==1)
		btype=0x34;
	else
		btype=0x39;
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;		// "C"
	cmdData[1] = 0x52;
	cmdData[2] = btype;

	BYTE chTempSend[1024]= {0};
	int chTempSendLen = 0;

	if (IsAscData(InData,InDataLength))
	{
		Asc2Bcd(InData, InDataLength, chTempSend);
		chTempSendLen = InDataLength/2;	
	}
	else
	{
		chTempSendLen = InDataLength;
		memcpy(chTempSend, InData, InDataLength);	
	}
	
	memcpy(cmdData+3, chTempSend, chTempSendLen);
	cmdDataLen = chTempSendLen+3;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);

	if(iRet != 0)
	{
		//LOGE("SAMApdu ExeCommand error :%d", iRet);
		return iRet;
	}

	*OutDataLength = RecvDataLen;
	memcpy(OutData, RecvData, RecvDataLen);

	return 0;
}


int crt285::CheckPasswdErrCode(BYTE byData[], int iDataLen)
{	BYTE byFrist = byData[iDataLen-5];
	BYTE bySecond = byData[iDataLen-4];
	
	if (byFrist==0x90 && bySecond==0x00)
	{
		return 0;
	}
	else if (byFrist==0x6F && bySecond==0x00)
	{
		return CRT_ERR_PWDPROCESS;
	}
	else if (byFrist==0x6F && bySecond==0x01)
	{
		return CRT_ERR_PWDCHECK;
	}
	else if (byFrist==0x6F && bySecond==0x02)
	{
		return CRT_ERR_PWDSCRAP;
	}
	else if (byFrist==0x6B && bySecond==0x00)
	{
		return CRT_ERR_PWDADDROVERFLOW;
	}
	else if (byFrist==0x67 && bySecond==0x00)
	{
		return CRT_ERR_PWDLENOVERFLOW;
	}
	else if (byFrist==0x6D && bySecond==0x00)
	{
		return CRT_ERR_PWDPMERR;
	}
	else if (byFrist==0x6E && bySecond==0x00)
	{
		return CRT_ERR_PWDCMERR;
	}
	else 
	{
		return CRT_ERR_UNKNOWN;
	}
}


int  crt285::CRT288x_SL4442CheckPasswd(int iMode, int uDataLength, BYTE lpData[])
{
	if (iMode <1 || iMode>2)
	{
		return CRT_ERR_CMDPARAM;
	}

	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x53;
	cmdData[2] = 0x33;
	cmdData[3] = 0x00;
	if (iMode == 1)
	{
		cmdData[4] = 0x20;
		cmdData[5] = 0x03;
	}
	else
	{
		cmdData[4] = 0xD0;
		cmdData[5] = 0x02;
	}
	
	cmdData[6] = 0x01;
	cmdData[7] = 0x03;

	int tmpDatalen = uDataLength;
	unsigned char szTempData[1024] = {0};
	if(IsAscData(lpData, tmpDatalen))
	{
		Asc2Bcd(lpData, tmpDatalen, szTempData);
		tmpDatalen = tmpDatalen/2;	
	}
	else
	{
		memcpy(szTempData, lpData, tmpDatalen);
	}
	memcpy(cmdData+8, szTempData, tmpDatalen);
	cmdDataLen = 8+tmpDatalen;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return  CheckPasswdErrCode(RecvData, RecvDataLen);


}

int  crt285::CRT288x_SL4442Process(int iMode, int iRegion ,LWORD wStartAddr, int* uDataLength, BYTE lpData[])
{
	if (*uDataLength <= 0)
	{
		return CRT_ERR_CMDPARAM;
	}

	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;
	int tmpDatalen = *uDataLength;

	cmdData[0] = 0x43;
	cmdData[1] = 0x53; 
	cmdData[2] = 0x33;
	cmdData[3] = 0x00;
	//¶ÁÐŽ
	if (iMode == 1) // ¶Á
	{
		cmdData[4] = 0xB0;
		cmdDataLen = 8;
	}
	else if (iMode == 2) //ÐŽ
	{
		cmdData[4] = 0xD0;
	
		unsigned char szTempData[1024] = {0};

		if(IsAscData(lpData, tmpDatalen))
		{
			Asc2Bcd(lpData, tmpDatalen, szTempData);
			tmpDatalen = tmpDatalen/2;		
		}
		else
		{
			memcpy(szTempData, lpData, tmpDatalen);
		}
		memcpy(cmdData+8, szTempData, tmpDatalen);
		cmdDataLen = 8+tmpDatalen;
	}
	else
	{
		return CRT_ERR_CMDPARAM;
	}

	//Î»ÖÃ
	if (iRegion == 1) //Ö÷ŽæŽ¢
	{
		cmdData[5] = 0x00;
	}
	else if (iRegion == 2) //±£»€Çø
	{
		cmdData[5] = 0x01;
	}
	else if (iRegion == 3) //°²È«Çø
	{
		cmdData[5] = 0x02;
	}
	else
	{
		return CRT_ERR_CMDPARAM;
	}


	cmdData[6] = wStartAddr&0xFF;
	cmdData[7] = tmpDatalen;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	if (iMode == 1) //¶Á
	{
		*uDataLength = RecvDataLen;
		memcpy(lpData, RecvData, RecvDataLen);
	}

	return CheckPasswdErrCode(RecvData, RecvDataLen);

}

int  crt285::CRT288x_SL4428CheckPasswd(int iMode, int uDataLength, BYTE lpData[])
{
	if (iMode <1 || iMode>2)
	{
		return CRT_ERR_CMDPARAM;
	}

	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0x53;
	cmdData[2] = 0x34;
	cmdData[3] = 0x00;
	if (iMode == 1)
	{
		cmdData[4] = 0x20;
	}
	else
	{
		cmdData[4] = 0x24;
	}
	cmdData[5] = 0x00;
	cmdData[6] = 0x00;
	cmdData[7] = 0x02;

	int tmpDatalen = uDataLength;
	unsigned char szTempData[1024] = {0};
	if(IsAscData(lpData, tmpDatalen))
	{
		Asc2Bcd(lpData, tmpDatalen, szTempData);
		tmpDatalen = tmpDatalen/2;
	}
	else
	{
		memcpy(szTempData, lpData, tmpDatalen);
	}
	memcpy(cmdData+8, szTempData, tmpDatalen);
	cmdDataLen = 8+tmpDatalen;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return CheckPasswdErrCode(RecvData, RecvDataLen);


}

int  crt285::CRT288x_SL4428Process(int iMode, int iRegion ,LWORD wStartAddr, int* uDataLength, BYTE lpData[])
{
	if (*uDataLength <= 0)
	{
		return CRT_ERR_CMDPARAM;
	}

	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;
	int tmpDatalen = *uDataLength;

	cmdData[0] = 0x43;
	cmdData[1] = 0x53; 
	cmdData[2] = 0x34;
	cmdData[3] = 0x00;

	//¶ÁÐŽ
	if (iMode == 1) // ¶Á
	{
		cmdData[4] = 0xB0;
		cmdDataLen = 8;
	}
	else if (iMode == 2) //ÐŽ
	{
		cmdData[4] = 0xD0;

		unsigned char szTempData[1024] = {0};

		if(IsAscData(lpData, tmpDatalen))
		{
			Asc2Bcd(lpData, tmpDatalen, szTempData);
			tmpDatalen = tmpDatalen/2;
		}
		else
		{
			memcpy(szTempData, lpData, tmpDatalen);
		}
		memcpy(cmdData+8, szTempData, tmpDatalen);
		cmdDataLen = 8+tmpDatalen;
	}
	else
	{
		return CRT_ERR_CMDPARAM;
	}

	//Î»ÖÃ
	if (iRegion == 1) //Ö÷ŽæŽ¢
	{
		int temp = wStartAddr&0x0F00;
		cmdData[5] = temp>>8;
	}
	else if (iRegion == 2) //±£»€Çø
	{

		if(iMode == 2)
		{
			int temp = wStartAddr&0x0F00;
			cmdData[5] =0x20 | (0xFF&(temp>>8));
		}
		else
		{
			cmdData[5] = 0x10;
		}
	}
	else
	{
		return CRT_ERR_CMDPARAM;
	}

	cmdData[6] = wStartAddr&0xFF;
	cmdData[7] = tmpDatalen;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	if (iMode == 1) //¶Á
	{
		*uDataLength = RecvDataLen;
		memcpy(lpData, RecvData, RecvDataLen);
	}

	return CheckPasswdErrCode(RecvData, RecvDataLen);

}


int  crt285::CRT288x_24CxProcess(int iMode ,LWORD wStartAddr, int* uDataLength, BYTE lpData[])
{

	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;
	int tmpDatalen = *uDataLength;

	cmdData[0] = 0x43;
	cmdData[1] = 0x54; 
	cmdData[3] = 0x00;

	if (iMode == 1) //¶Á
	{
		cmdData[2] = 0x33;
		cmdData[4] = 0xB0;
		cmdDataLen = 8;
	}
	else if (iMode == 2) //ÐŽ
	{
		cmdData[2] = 0x34;
		cmdData[4] = 0xD0;
		unsigned char szTempData[1024] = {0};
		if(IsAscData(lpData, tmpDatalen))
		{
			Asc2Bcd(lpData, tmpDatalen, szTempData);
			tmpDatalen = tmpDatalen/2;
		}
		else
		{
			memcpy(szTempData, lpData, tmpDatalen);
		}
		memcpy(cmdData+8, szTempData, tmpDatalen);
		cmdDataLen = 8+tmpDatalen;
	}
	else
	{
		return CRT_ERR_CMDPARAM;
	}

	cmdData[6] = wStartAddr&0xFF;
	cmdData[7] = tmpDatalen;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	if (iMode == 1) //¶Á
	{
		*uDataLength = RecvDataLen;
		memcpy(lpData, RecvData, RecvDataLen);
	}

	return CheckPasswdErrCode(RecvData, RecvDataLen);


}


int  crt285::CRT288x_MifareKeyProcess(int iPar, int iKs, int iSn, int uDataLength, BYTE lpData[])
{	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	if (iKs<0 ||iKs>1 ||iSn<0 || iSn>0x0F)
	{
		return CRT_ERR_CMDPARAM;
	}

	// validar iPar
	// 0x20 0x21 0xD0 0xD1

	cmdData[0] = 0x43;
	cmdData[1] = 0x60; 
	cmdData[2] = 0x33; 
	cmdData[3] = 0x00; 
	cmdData[4] = iPar;
	cmdData[5] = iKs; 
	cmdData[6] = iSn; 
	cmdData[7] = 0x06; 

	int tmpDatalen = uDataLength;
	unsigned char szTempData[1024] = {0};
	if(IsAscData(lpData, tmpDatalen))
	{
		Asc2Bcd(lpData, tmpDatalen, szTempData);
		tmpDatalen = tmpDatalen/2;	
	}
	else
	{
		memcpy(szTempData, lpData, tmpDatalen);
	}
	memcpy(cmdData+8, szTempData, tmpDatalen);
	cmdDataLen = 8+tmpDatalen;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	return CheckPasswdErrCode(RecvData, RecvDataLen);
}

int crt285::CRT288x_MifareCardProcess(int iMode, int iSn, int iBn, int iLc, int* uDataLength, BYTE lpData[])
{	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	if (iBn<0 ||iBn>0x0F ||iSn<0 || iSn>0x27 || iLc<0 || iLc>0x10)
	{
		return CRT_ERR_CMDPARAM;
	}

	cmdData[0] = 0x43;
	cmdData[1] = 0x60; 
	cmdData[2] = 0x33; 
	cmdData[3] = 0x00; 
	if (iMode == 1) //¶ÁÉÈÇø¿éÊýŸÝ
	{
		cmdData[4] = 0xB0; 
		cmdData[5] = iSn;
		cmdData[6] = iBn;
		cmdData[7] = iLc;
		cmdDataLen = 8;
	}
	else if (iMode == 2) //ÐŽÉÈÇø¿éÊýŸÝ
	{
		cmdData[4] = 0xD1; 
		cmdData[5] = iSn;
		cmdData[6] = iBn;
		cmdData[7] = iLc;
		int tmpDatalen = *uDataLength;
		unsigned char szTempData[1024] = {0};
		if(IsAscData(lpData, tmpDatalen))
		{
			Asc2Bcd(lpData, tmpDatalen, szTempData);
			tmpDatalen = tmpDatalen/2;		
		}
		else
		{
			memcpy(szTempData, lpData, tmpDatalen);
		}
		memcpy(cmdData+8, szTempData, tmpDatalen);
		cmdDataLen = 8+tmpDatalen;
	}
	else if (iMode == 3) //S50 S70 ³õÊŒ»¯
	{
		cmdData[4] = 0xD2; 
		cmdData[5] = iSn;
		cmdData[6] = iBn;
		cmdData[7] = 0x04;
		int iValue = *uDataLength;
		int tmpDatalen = 4;
		cmdData[8] = iValue%256;
		cmdData[9] = iValue/256%256;
		cmdData[10] = iValue/256/256%256;
		cmdData[11] = iValue/256/256/256%256;
// 		unsigned char szTempData[1024] = {0};
// 		if(IsHexString(lpData, *uDataLength))
// 		{
// 			//tmpDatalen = tmpDatalen/2;
// 			HexString2Bcd(lpData, szTempData, *uDataLength/2);
// 		}
// 		else
// 		{
// 			memcpy(szTempData, lpData, *uDataLength);
// 		}
// 		memcpy(cmdData+8, szTempData, tmpDatalen);
		cmdDataLen = 8+tmpDatalen;
	}
	else if (iMode == 4) //S50 S70 ¶ÁÓà¶î
	{
		cmdData[4] = 0xB1; 
		cmdData[5] = iSn;
		cmdData[6] = iBn;
		
		cmdDataLen = 7;
	}
	else if (iMode == 5) //S50 S70 ÔöÖµ
	{
		cmdData[4] = 0xD3; 
		cmdData[5] = iSn;
		cmdData[6] = iBn;
		cmdData[7] = 0x04;
		int iValue = *uDataLength;
		int tmpDatalen = 4;
		cmdData[8] = iValue%256;
		cmdData[9] = iValue/256%256;
		cmdData[10] = iValue/256/256%256;
		cmdData[11] = iValue/256/256/256%256;
// 		unsigned char szTempData[1024] = {0};
// 		if(IsHexString(lpData, tmpDatalen))
// 		{
// 			tmpDatalen = tmpDatalen/2;
// 			HexString2Bcd(lpData, szTempData, tmpDatalen);
// 		}
// 		else
// 		{
// 			memcpy(szTempData, lpData, tmpDatalen);
// 		}
// 		memcpy(cmdData+8, szTempData, tmpDatalen);
		cmdDataLen = 8+tmpDatalen;
	}
	else if (iMode == 6) //S50 S70 ŒõÖµ
	{
		cmdData[4] = 0xD4; 
		cmdData[5] = iSn;
		cmdData[6] = iBn;
		cmdData[7] = 0x04;
		int iValue = *uDataLength;
		int tmpDatalen = 4;
		cmdData[8] = iValue%256;
		cmdData[9] = iValue/256%256;
		cmdData[10] = iValue/256/256%256;
		cmdData[11] = iValue/256/256/256%256;
		cmdDataLen = 8+tmpDatalen;
	}
	else
	{
		return CRT_ERR_CMDPARAM;
	}

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	iRet = CheckPasswdErrCode(RecvData, RecvDataLen);
	if (iRet != 0)
	{
		return iRet;
	}

	if (RecvDataLen > 2)
	{
		*uDataLength = RecvDataLen -2;
		memcpy(lpData, RecvData, RecvDataLen -2);
	}
	return 0;
}


int crt285::CRT_GetCurICCommend(int iType)
{
	m_CurICCommend = 0x00;

	switch(iType)
	{
	case 10: //T0 CPU
		m_CurICCommend = 0x51;
		break;

	case 11: //T1 CPU
		m_CurICCommend = 0x51;
		break;

	case 20: //SL4442
		m_CurICCommend = 0x53;
		break;

	case 21: // SL4428
		m_CurICCommend = 0x53;
		break;

	case 30: //AT24C01
		m_CurICCommend = 0x54;
		break;

	case 31: //AT24C02
		m_CurICCommend = 0x54;
		break;

	case 32: //AT24C04
		m_CurICCommend = 0x54;
		break;

	case 33: //AT24C08
		m_CurICCommend = 0x54;
		break;

	case 34: //AT24C16
		m_CurICCommend = 0x54;
		break;

	case 35: //AT24C32
		m_CurICCommend = 0x54;
		break;

	case 36: //AT24C64
		m_CurICCommend = 0x54;
		break;

	case 37: //AT24C128
		m_CurICCommend = 0x54;
		break;

	case 38: //AT24C256
		m_CurICCommend = 0x54;
		break;

	case 110: //S50
		m_CurICCommend = 0x60;
		break;

	case 111: //S70
		m_CurICCommend = 0x60;
		break;

	case 112: //UL
		m_CurICCommend = 0x60;
		break;

	case 120: //TYPE A CPU
		m_CurICCommend = 0x60;
		break;

	case 130: //TYPE B CPU
		m_CurICCommend = 0x60;
		break;

	case 210: //SAM ¿š
		m_CurICCommend = 0x52;
		break;

	default:
		return CRT_ERR_CMDPARAM;
	}

	return 0;
}


int  crt285::CRT288C_GetHidCardNums(char szHidCardNums[])
{
	int iRet = 0;
	BYTE cmdData[1024] = {0};
	BYTE RecvData[1024] = {0};
	BYTE stCode[10] = {0};

	int RecvDataLen = 0;
	int cmdDataLen = 0;

	cmdData[0] = 0x43;
	cmdData[1] = 0xE0;
	cmdData[2] = 0x30;
	cmdDataLen = 3;

	iRet = CRT288x_ExeCommand(cmdDataLen, cmdData, &RecvDataLen, RecvData, stCode);
	if(iRet != 0)
	{
		printf("iRet = %d\n",iRet);
		return iRet;
	}

	memcpy(szHidCardNums, RecvData, RecvDataLen);

	return 0;
}

/******************************************************************************/
int crt285::CRT_Read(unsigned int length,unsigned char buffer[])
{	char hexa[5];

	int iRet = 0;
	if (m_iMode != 1 && m_iMode != 0)
	{
		return CRT_ERR_CMDPARAM;
	}


	if(m_iMode == 1)
	{
//		iRet = RS232_Read(length, buffer);
	}
	else if (m_iMode == 0)
	{
		iRet = USB_Read(length, buffer);
	}

	memset(m_szReadData, 0, sizeof(m_szReadData));
	int i=0;
	for (; i<length;i++)
	{	sprintf(hexa, "%02X ", buffer[i]);
		strcat(m_szReadData, hexa);
	}
	return iRet;

}



int crt285::OrganizePacket(int iSendDataLen, BYTE bySendData[], int* iOutPacketLen, BYTE byOutPacketData[])
{
	if(bySendData == NULL )
	{
		return CRT_ERR_EEPROM;
	}


	int tmpDatalen = iSendDataLen;
	unsigned char szTempData[1024] = {0};
	if(bySendData[0] != 0x43 && IsAscData(bySendData, tmpDatalen))
	{
		Asc2Bcd(bySendData, tmpDatalen, szTempData);
		tmpDatalen = tmpDatalen/2;
	}
	else
	{
		memcpy(szTempData, bySendData, tmpDatalen);
	}

	int i = 0;
	byOutPacketData[0] = CSTX;
	byOutPacketData[1] = (tmpDatalen)/256;
	byOutPacketData[2] = (tmpDatalen)%256;

	for (i=0;i<tmpDatalen;i++)
	{
		byOutPacketData[i+3]=szTempData[i];
	}
	byOutPacketData[i+3] = ETX;
	byOutPacketData[i+4] = CalXOR(byOutPacketData,tmpDatalen+4);
	*iOutPacketLen = tmpDatalen+5;

	return 0; 
}

int crt285::AnalysisPacket(int *iRecvDataLen, BYTE byRecvData[], BYTE bystCode[])
{
	if(byRecvData == NULL || bystCode == NULL)
	{
		return CRT_ERR_EEPROM;
	}

	BYTE TempRecvBuf[1024] = {0};
	int iTempRecvLen = 0;

	int iRet = 0;
	int i = 0;

	iRet = Recv_ACK() ;
	if (iRet != 0)
	{
		return iRet;
	}

	iRet = Recv_Data(&iTempRecvLen, TempRecvBuf);
	if (iRet != 0)
	{
		return iRet;
	}

	bystCode[0]=TempRecvBuf[0];
	bystCode[1]=TempRecvBuf[3];
	bystCode[2]=TempRecvBuf[4];
	*iRecvDataLen=0;

	if (iTempRecvLen>7 )
	{
		*iRecvDataLen = iTempRecvLen-7;
		for (i=0;i<iTempRecvLen-7;i++)
			byRecvData[i]=TempRecvBuf[5+i];
	}

	BYTE SendAckBuf[3] = {0};
	SendAckBuf[0] = 0x06;

	iRet = CRT_Write(1, SendAckBuf);  
	if(iRet != 0)
	{
		printf("Send_ACK write ack err  \n");
		return iRet;
	}

	return GetErrCode(bystCode);
}

/******************************************************************************/
int crt285::Recv_ACK(void)
{
	BYTE TempBuf[1024] = {0};
	int iTempLen = 0;

	int iRet = 0;

	iTempLen = 1;
	iRet = CRT_Read(iTempLen, TempBuf);
	if(iRet != 0)
	{
		printf("Recv_ACK Read ack err  \n");
		return iRet;
	}

	if(TempBuf[0] != ACK)
	{
		printf("CRT_Read ack : %02x  \n",TempBuf[0]);
		printf("CRT_Read ack err  \n");
		return CRT_ERR_ICNOTRESPOND;
	}

	return 0;
}

int crt285::Recv_Data(int *iRecvDataLen, BYTE byRecvData[])
{
	if(m_iMode == 1)
	{
//		return Recv_Data_RS232(iRecvDataLen, byRecvData);
	}
	else if (m_iMode == 0)
	{
		return Recv_Data_USB(iRecvDataLen, byRecvData);
	}


	return CRT_ERR_CMDPARAM;
}

int crt285::Recv_Data_USB(int *iRecvDataLen, BYTE byRecvData[])
{
	BYTE TempRecvBuf[1024] = {0};
	int iTempRecvLen = 64;

	int iRet = 0;

	iRet = CRT_Read(iTempRecvLen, TempRecvBuf);

	if(iRet < 0)
	{
		printf("Recv_Data_USB Read head err  \n");
		return iRet;
	}
	if(TempRecvBuf[0] != CSTX)
	{
		printf("Recv_Data_USB CRT_Read head err  \n");
		return CRT_ERR_ICNOTRESPOND;
	}

	int iGetLen = TempRecvBuf[1] * 256 + TempRecvBuf[2]+2+3;

	int ii = 1;
	while(1)
	{
		if(iGetLen < iTempRecvLen*ii)
		{
			break;
		}

		iRet = CRT_Read(iTempRecvLen, TempRecvBuf+iTempRecvLen*ii);
		if(iRet < 0)
		{
			printf("Recv_Data_USB Read head err  \n");
			return iRet;
		}

		ii++;
	}

	if(TempRecvBuf[3] != 0x50 && TempRecvBuf[3] != 0x4e)
	{
		printf("Recv_Data_USB unkown err!  \n");
		return CRT_ERR_ICNOTRESPOND_OTHER;
	}

	*iRecvDataLen = iGetLen;
	memcpy(byRecvData, TempRecvBuf+3, iGetLen);
	return 0;
}

int crt285::Recv_Data_RS232(int *iRecvDataLen, BYTE byRecvData[])
{
	BYTE TempRecvBuf[1024] = {0};
	int iTempRecvLen = 0;

	int iRet = 0;

	iTempRecvLen = 3;
	iRet = CRT_Read(iTempRecvLen, TempRecvBuf);
	if(iRet < 0)
	{
		printf("Recv_Data_RS232 Read head err  \n");
		return iRet;
	}

	if(TempRecvBuf[0] != CSTX)
	{
		printf("Recv_Data_RS232 CRT_Read head err  \n");
		return CRT_ERR_ICNOTRESPOND;
	}


	iTempRecvLen = TempRecvBuf[1] * 256 + TempRecvBuf[2]+2;
	memset(TempRecvBuf, 0x00, sizeof(TempRecvBuf));
	iRet = CRT_Read(iTempRecvLen, TempRecvBuf);  //recv packet data
	if(iRet < 0)
	{
		printf("Recv_Data_RS232 recv packet data err  \n");
		return iRet;
	}


	if(TempRecvBuf[0] != 0x50 && TempRecvBuf[0] != 0x4e)
	{
		printf("Recv_Data_RS232 unkown err!  \n");
		return CRT_ERR_ICNOTRESPOND_OTHER;
	}

	*iRecvDataLen = iTempRecvLen;
	memcpy(byRecvData, TempRecvBuf, iTempRecvLen);

	return 0;
}



int crt285::GetErrCode(BYTE byStCode[])
{
	if(byStCode[0] == 0x50)
	{
		return 0;
	}

	char szCode[3] = {0};
	memcpy(szCode, byStCode+1, 2);
	int iCode = atoi(szCode);

	switch(iCode) {
	case 0:
		printf("CMD err£¡\n");
		return CRT_ERR_CMD;
		break;
	case 1:
		printf("CMDPARAM err£¡\n");
		return CRT_ERR_CMDPARAM ;
		break;
	case 2:
		printf("CMDDENIAL err£¡\n");
		return CRT_ERR_CMDDENIAL ;
		break;
	case 3:
		printf("DEVNOTSUP err£¡");
		return CRT_ERR_DEVNOTSUP ;
		break;
	case 4:
		printf("CMDDATA err£¡\n");
		return CRT_ERR_CMDDATA ;
		break;
	case 11:
		printf("LOCKCARD err£¡\n");
		return CRT_ERR_LOCKCARD;
		break;
	case 15:
		printf("EEPROM err£¡\n");
		return CRT_ERR_EEPROM;
		break;
	case 20:
		printf("TRACKCHECK err£¡\n");
		return CRT_ERR_TRACKCHECK;
		break;
	case 21:
		printf("READTRACK  err£¡\n");
		return CRT_ERR_READTRACK ;
		break;
	case 30:
		printf("POWERDOWN err£¡");
		return CRT_ERR_POWERDOWN;
		break;
	case 41:
		printf("ICPROCESS err£¡\n");
		return CRT_ERR_ICPROCESS;
		break;
	case 60:
		printf("ICPOWERSHORT err£¡\n");
		return CRT_ERR_ICPOWERSHORT;
		break;
	case 61:
		printf("ICACTIVATIONFAIL err£¡\n");
		return CRT_ERR_ICACTIVATIONFAIL;
		break;
	case 62:
		printf("ICDENIAL err£¡\n");
		return CRT_ERR_ICDENIAL;
		break;
	case 63:
		printf("ICNOTRESPOND err£¡\n");
		return CRT_ERR_ICNOTRESPOND;
		break;
	case 64:
		printf("ICNOTRESPOND_OTHER err£¡\n");
		return CRT_ERR_ICNOTRESPOND_OTHER;
		break;
	case 65:
		printf("ICNOTACTIVATION err£¡\n");
		return CRT_ERR_ICNOTACTIVATION;
		break;
	case 66:
		printf("ICNOTSUP err£¡\n");
		return CRT_ERR_ICNOTSUP;
		break;
	case 69:
		printf("ICEMV err£¡\n");
		return CRT_ERR_ICEMV;
		break;
	default:
		printf("UNKNOWN err£¡\n");
		return CRT_ERR_UNKNOWN;
		break;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////

