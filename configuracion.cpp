
#include "mert.h"
#include "configuracion.h"

//
//
//
/******************************************************************************/
/* BUsca el sig tag y devuelve su valor                                       */
/******************************************************************************/
int GetXmlTagValue(char *RespBuffer, char *Tag, char *TagValue, int *start, int len)
{	int pos;
	char FirstTag[50] = {0}; //First Tag
	char SecondTag[50] = {0};//Second Tag
	int PosFirstTag=-1, PosSecondTag=-1;

	
	//verifica la longitud del buffer de entrada
	if (len<=0)
		return -1;

	// clear resultado
	memset(TagValue, 0, sizeof(TagValue));

	//Crea etiqueta inicial
	memset(FirstTag, 0, sizeof(FirstTag));
	strcpy(FirstTag, "<");
	strcat(FirstTag, Tag);
	strcat(FirstTag, ">");

	//Create etiqueta final
	memset(SecondTag, 0, sizeof(SecondTag));
	strcpy(SecondTag, "</");
	strcat(SecondTag, Tag);
	strcat(SecondTag, ">");
	
	//Busca la posicion de la primer etiqueta
	for (pos= *start; pos<len; pos++)
	{
		if ( !memcmp(FirstTag, RespBuffer+pos, strlen(FirstTag)) )
		{
			PosFirstTag = pos;
			break;
		}
	}

	//Busca la posicion de la etiqueta final
	for (pos=*start; pos<len; pos++)
	{
		if ( !memcmp(SecondTag, RespBuffer+pos, strlen(SecondTag)) )
		{
			PosSecondTag = pos;
			break;
		}
	}

	if ( (PosFirstTag !=-1) && (PosSecondTag !=-1) )
	{
		if (PosSecondTag-PosFirstTag-strlen(FirstTag))
		{
			//Get tag value
			memcpy( TagValue, RespBuffer+PosFirstTag+strlen(FirstTag), 
								PosSecondTag-PosFirstTag-strlen(FirstTag) );
			TagValue[PosSecondTag-PosFirstTag-strlen(FirstTag)] = '\0';
			*start = PosSecondTag;
			if (strlen(TagValue))
				return 1;
		}
	}
	return -1;
}

/******************************************************************************/
/* BUsca el sig tag y devuelve su valor                                       */
/******************************************************************************/
int GetXmlTagValueHeader(char *RespBuffer, int len, char *Tag, int *start, char *TagValue)
{	int pos;
	char FirstTag[50] = {0};	//First Tag
	char SecondTag[50] = {0};	//Second Tag
	int PosFirstTag=-1;
	int PosSecondTag=-1;

	//verifica la longitud del buffer de entrada
	if(len<=0)
		return -1;

	// clear resultado
	memset(TagValue, 0, sizeof(TagValue));

	//Crea etiqueta inicial
	memset(FirstTag, 0, sizeof(FirstTag));
	strcpy(FirstTag, Tag);
	strcat(FirstTag, "=\"");

	//Create etiqueta final
	memset(SecondTag, 0, sizeof(SecondTag));
	strcpy(SecondTag, "\"");

	//Busca la posicion de la primer etiqueta
	for(pos= *start; pos<len; pos++)
	{
		if( !memcmp(FirstTag, RespBuffer+pos, strlen(FirstTag)) )
		{
			PosFirstTag = pos;
			break;
		}
	}

	//Busca la posicion de la etiqueta final
	for( pos = PosFirstTag + strlen(FirstTag) ; pos<len; pos++)
	{
		if( memcmp(SecondTag, RespBuffer+pos, strlen(SecondTag)) ==0 )
		{	//printf("igual[%d][%d]\n",strlen(SecondTag),pos);
			PosSecondTag = pos;
			break;
		}
	}

	if( (PosFirstTag !=-1) && (PosSecondTag !=-1) )
	{
		if(PosSecondTag-PosFirstTag-strlen(FirstTag))
		{
			//Get tag value
			memcpy( TagValue, RespBuffer+PosFirstTag+strlen(FirstTag),
								PosSecondTag-PosFirstTag-strlen(FirstTag) );
			TagValue[PosSecondTag-PosFirstTag-strlen(FirstTag)] = '\0';
			*start = PosSecondTag;
			if (strlen(TagValue))
				return 1;
		}
	}
	return -1;
}

/******************************************************************************/
/* BUsca el sig tag y devuelve su valor                                       */
/******************************************************************************/
int GetXmlTagStart(char *RespBuffer, char *Tag, int *start, int len)
{	int pos;
	char FirstTag[50] = {0}; //First Tag
	int PosFirstTag=-1;

	//verifica la longitud del buffer de entrada
	if (len<=0)
		return -1;

	//Crea etiqueta inicial
	memset(FirstTag, 0, sizeof(FirstTag));
	strcpy(FirstTag, "<");
	strcat(FirstTag, Tag);
	strcat(FirstTag, ">");

	//Busca la posicion de la primer etiqueta
	for (pos= *start; pos<len; pos++)
	{
		if ( !memcmp(FirstTag, RespBuffer + pos, strlen(FirstTag)) )
		{
			PosFirstTag = pos;
			*start = pos + strlen(FirstTag);
			return 0x00;
		}
	}
	return -1;
}

/******************************************************************************/
/* BUsca el sig tag y devuelve su valor                                       */
/******************************************************************************/
int GetXmlTagStartLabel(char *RespBuffer, int len, char *Tag, int *start)
{	int pos;
	char FirstTag[50] = {0}; //First Tag
	int PosFirstTag=-1;

	//verifica la longitud del buffer de entrada
	if (len<=0)
		return -1;

	//Crea etiqueta inicial
	memset(FirstTag, 0, sizeof(FirstTag));
	strcpy(FirstTag, Tag);
	strcat(FirstTag, "=\"");

	//Busca la posicion de la primer etiqueta
	for (pos= *start; pos<len; pos++)
	{
		if ( !memcmp(FirstTag, RespBuffer + pos, strlen(FirstTag)) )
		{
			PosFirstTag = pos;
			*start = pos + strlen(FirstTag);
			return 0x00;
		}
	}
	return -1;
}

/******************************************************************************/
/* BUsca el sig tag y devuelve su valor                                       */
/******************************************************************************/
int GetHeaderXml(header *hdr, char Response[], int tc, int *start)
{	char Tag[20];
	char datex[20];
	int  st = *start;

	strcpy(Tag,"Code");
	GetXmlTagValueHeader(Response, tc, Tag, &st, hdr->Code);

	strcpy(Tag,"Format");
	GetXmlTagValueHeader(Response, tc, Tag, &st, datex);
	hdr->Format = atoi(datex);

	strcpy(Tag,"Version");
	GetXmlTagValueHeader(Response, tc, Tag, &st, datex);
	hdr->Version = atoi(datex);

	strcpy(Tag,"SourceVersion");
	GetXmlTagValueHeader(Response, tc, Tag, &st, datex);
	hdr->SourceVersion = atoi(datex);

	strcpy(Tag,"CreationDate");
	GetXmlTagValueHeader(Response, tc, Tag, &st, hdr->CreationDate);

	strcpy(Tag,"ActivationDate");
	GetXmlTagValueHeader(Response, tc, Tag, &st, hdr->ActivationDate);

	strcpy(Tag,"Author");
	GetXmlTagValueHeader(Response, tc, Tag, &st, hdr->Author);
	return 0;
}

/******************************************************************************/
/*  lee el archivo de configuracion del TRIPODE                               */
/******************************************************************************/
int rdInitVal(char fname[])
{	FILE *fptr = NULL; 
	char Response[1000];
	char Tag[20]={};	// Tag a buscar
	char datex[30]={};	// dato leido en texto

	int tc, start=0;

	// LEE ARCHIVO DE CONFIGURACION
	if(access(fname, F_OK)==0)
	{	printf("FILE=[%s]\n",fname);
		fptr = fopen(fname, "r");
		if( (tc=fread(Response, 1,1000, fptr)) > 0)
		{	start = 0;
			GetHeaderXml(&hdr_initval, Response, tc, &start);

			strcpy(Tag,"COMP");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			mert_identidad.COMP = atoi(datex);

			strcpy(Tag,"LINEA");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			mert_identidad.LINEA = atoi(datex);

			strcpy(Tag,"ESTACION");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			mert_identidad.ESTACION = atoi(datex);

			strcpy(Tag,"TRAY");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			mert_identidad.TRAY = atoi(datex);

			strcpy(Tag,"TIPO");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			mert_identidad.TIPO = atoi(datex);

			strcpy(Tag,"NUMDISP");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			mert_identidad.NUMDISP = atoi(datex);

			strcpy(Tag,"TICKETVAL");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			mert_identidad.TICKETVAL = atoi(datex);


			printf("\nHEADER\n");
			printf("CODE:           %s\n", hdr_initval.Code);
			printf("FORMAT:         %d\n", hdr_initval.Format);
			printf("VERSION:        %d\n", hdr_initval.Version);
			printf("SOURCEVERSION:  %d\n", hdr_initval.SourceVersion);
			printf("CREATEDATE:     %s\n", hdr_initval.CreationDate);
			printf("ACTIVATIONDATE: %s\n", hdr_initval.ActivationDate);
			printf("AUTHOR:         %s\n", hdr_initval.Author);
			printf("------------------------------------------\n");			
			printf("COMP:        %d\n", mert_identidad.COMP);
			printf("LINEA:       %d\n", mert_identidad.LINEA);
			printf("ESTACION:    %d\n", mert_identidad.ESTACION);
			printf("TRAY:        %d\n", mert_identidad.TRAY);
			printf("TIPO:        %d\n", mert_identidad.TIPO);
			printf("NUMDISP:     %d\n", mert_identidad.NUMDISP);
			printf("TICKETVAL:   %d\n", mert_identidad.TICKETVAL);
			printf("\n");

			fclose(fptr);
			return 0;
		}
	}
	printf("NO existe [%s]\n",fname);
	return 1;
}

/******************************************************************************/
/*  lee el archivo de configuracion del TRIPODE                               */
/******************************************************************************/
int rdPuertos(char fname[])
{	FILE *fptr = NULL; 
	char Response[2000];
	char Tag[20]={};	// Tag a buscar
	char datex[30]={};	// dato leido en texto
	int start = 0;
	int tc;

	char marca[10];

	memset(marca, 0x00, sizeof(marca));


	// LEE ARCHIVO DE PUERTOS
	if(access(fname, F_OK)==0)
	{	printf("\nSI existe[%s]\n",fname);
		fptr = fopen(fname, "r");
		if((tc=fread(Response, 1,2000, fptr)) > 0)
		{	start = 0;
			GetHeaderXml(&hdr_config, Response, tc, &start);

			strcpy(Tag,"EXPENDEDORA");
			GetXmlTagStart(Response,Tag, &start, tc);
			strcpy(Tag,"MARCA");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.EXP_Marca = atoi(datex);
			marca[0] = MertPort.EXP_Marca;

			strcpy(Tag,"SLOT0");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.EXP_SAM_SLOT0 = atoi(datex);
			strcpy(Tag,"SLOT1");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.EXP_SAM_SLOT1 = atoi(datex);
			strcpy(Tag,"SLOT2");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.EXP_SAM_SLOT2 = atoi(datex);
			strcpy(Tag,"SLOT3");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.EXP_SAM_SLOT3 = atoi(datex);
			strcpy(Tag,"TIPO_PUERTO");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.EXP_TipoPuerto= atoi(datex);
			strcpy(Tag,"BAUD");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.EXP_SerialBaud= atoi(datex);
			strcpy(Tag,"COMM_NAME");
			GetXmlTagValue(Response,Tag, MertPort.EXP_SerialName, &start, tc);

			strcpy(Tag,"USB_NAME");
			GetXmlTagValue(Response,Tag, MertPort.EXP_USBName, &start, tc);
			strcpy(Tag,"USB_MARCA");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.EXP_USBMarca = atoi(datex);
			marca[9] = MertPort.EXP_USBMarca;

			start = 0;
			strcpy(Tag,"RECARGA");
			GetXmlTagStart(Response,Tag, &start, tc);
			strcpy(Tag,"MARCA");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.REC_Marca = atoi(datex);
			marca[1] = MertPort.REC_Marca;
			strcpy(Tag,"SLOT0");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.REC_SAM_SLOT0 = atoi(datex);
			strcpy(Tag,"SLOT1");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.REC_SAM_SLOT1 = atoi(datex);
			strcpy(Tag,"SLOT2");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.REC_SAM_SLOT2 = atoi(datex);
			strcpy(Tag,"SLOT3");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.REC_SAM_SLOT3 = atoi(datex);
			strcpy(Tag,"TIPO_PUERTO");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.REC_TipoPuerto= atoi(datex);
			strcpy(Tag,"BAUD");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.REC_SerialBaud= atoi(datex);
			strcpy(Tag,"COMM_NAME");
			GetXmlTagValue(Response,Tag, MertPort.REC_SerialName, &start, tc);

			start = 0;
			strcpy(Tag,"CAJA_MONEDAS");
			GetXmlTagStart(Response,Tag, &start, tc);
			strcpy(Tag,"MARCA");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.COINBOX_Marca = atoi(datex);
			marca[2] = MertPort.COINBOX_Marca;
			strcpy(Tag,"SLOT0");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.COINBOX_SAM_SLOT0 = atoi(datex);
			strcpy(Tag,"SLOT1");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.COINBOX_SAM_SLOT1 = atoi(datex);
			strcpy(Tag,"SLOT2");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.COINBOX_SAM_SLOT2 = atoi(datex);
			strcpy(Tag,"SLOT3");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.COINBOX_SAM_SLOT3 = atoi(datex);
			strcpy(Tag,"TIPO_PUERTO");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.COINBOX_TipoPuerto= atoi(datex);
			strcpy(Tag,"BAUD");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.COINBOX_SerialBaud= atoi(datex);
			strcpy(Tag,"COMM_NAME");
			GetXmlTagValue(Response,Tag, MertPort.COINBOX_SerialName, &start, tc);

			start = 0;
			strcpy(Tag,"CAJA_BILLETES");
			GetXmlTagStart(Response,Tag, &start, tc);
			strcpy(Tag,"MARCA");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.BILLBOX_Marca = atoi(datex);
			marca[3] = MertPort.BILLBOX_Marca;
			strcpy(Tag,"SLOT0");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.BILLBOX_SAM_SLOT0 = atoi(datex);
			strcpy(Tag,"SLOT1");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.BILLBOX_SAM_SLOT1 = atoi(datex);
			strcpy(Tag,"SLOT2");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.BILLBOX_SAM_SLOT2 = atoi(datex);
			strcpy(Tag,"SLOT3");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.BILLBOX_SAM_SLOT3 = atoi(datex);
			strcpy(Tag,"TIPO_PUERTO");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.BILLBOX_TipoPuerto= atoi(datex);
			strcpy(Tag,"BAUD");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.BILLBOX_SerialBaud= atoi(datex);
			strcpy(Tag,"COMM_NAME");
			GetXmlTagValue(Response,Tag, MertPort.BILLBOX_SerialName, &start, tc);

			start = 0;
			strcpy(Tag,"PUERTA");
			GetXmlTagStart(Response,Tag, &start, tc);
			strcpy(Tag,"MARCA");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.PUERTA_Marca = atoi(datex);
			marca[4] = MertPort.PUERTA_Marca;
			strcpy(Tag,"SLOT0");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.PUERTA_SAM_SLOT0 = atoi(datex);
			strcpy(Tag,"SLOT1");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.PUERTA_SAM_SLOT1 = atoi(datex);
			strcpy(Tag,"SLOT2");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.PUERTA_SAM_SLOT2 = atoi(datex);
			strcpy(Tag,"SLOT3");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.PUERTA_SAM_SLOT3 = atoi(datex);
			strcpy(Tag,"TIPO_PUERTO");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.PUERTA_TipoPuerto= atoi(datex);
			strcpy(Tag,"BAUD");
			GetXmlTagValue(Response,Tag,datex, &start, tc);
			MertPort.PUERTA_SerialBaud= atoi(datex);
			strcpy(Tag,"COMM_NAME");
			GetXmlTagValue(Response,Tag, MertPort.PUERTA_SerialName, &start, tc);

			start = 0;
			strcpy(Tag,"MODULO_ES");
			GetXmlTagStart(Response,Tag, &start, tc);
			strcpy(Tag,"BAUD");
			GetXmlTagValue(Response,Tag, datex, &start, tc);
			MertPort.MODIO_BAUD = atoi(datex);
			strcpy(Tag,"COMM_NAME");
			GetXmlTagValue(Response,Tag, MertPort.MODIO_COMM, &start, tc);
			strcpy(Tag,"NoMODULOS");
			GetXmlTagValue(Response,Tag, datex, &start, tc);
			MertPort.MODIO_TP = atoi(datex);
	
			start=0;
			strcpy(Tag,"MONEDERO");
			GetXmlTagStart(Response,Tag, &start, tc);
			strcpy(Tag,"PROTOCOLO");
			GetXmlTagValue(Response,Tag, datex, &start, tc);
			MertPort.COIN_PROTOCOL = atoi(datex);
			strcpy(Tag,"BAUD");
			GetXmlTagValue(Response,Tag, datex, &start, tc);
			MertPort.COIN_BAUD = atoi(datex);
			strcpy(Tag,"COMM_NAME");
			GetXmlTagValue(Response,Tag, MertPort.COIN_COMM, &start, tc);
			strcpy(Tag,"VERIFICA");
			GetXmlTagValue(Response,Tag, datex, &start, tc);
			MertPort.COIN_VERIFICACION = atoi(datex);
			strcpy(Tag,"ECO");
			GetXmlTagValue(Response,Tag, datex, &start, tc);
			MertPort.COIN_ECHO = atoi(datex);
			strcpy(Tag,"COIN_MARCA");
			GetXmlTagValue(Response,Tag, MertPort.COIN_MARCA, &start, tc);
			
			start=0;
			strcpy(Tag,"BILLETERO");
			GetXmlTagStart(Response,Tag, &start, tc);
			strcpy(Tag,"PROTOCOLO");
			GetXmlTagValue(Response,Tag, datex, &start, tc);
			MertPort.BILL_PROTOCOL = atoi(datex);
			strcpy(Tag,"BAUD");
			GetXmlTagValue(Response,Tag, datex, &start, tc);
			MertPort.BILL_BAUD = atoi(datex);
			strcpy(Tag,"COMM_NAME");
			GetXmlTagValue(Response,Tag, MertPort.BILL_COMM, &start, tc);
			strcpy(Tag,"VERIFICA");
			GetXmlTagValue(Response,Tag, datex, &start, tc);
			MertPort.BILL_VERIFICACION = atoi(datex);
			strcpy(Tag,"ECO");
			GetXmlTagValue(Response,Tag, datex, &start, tc);
			MertPort.BILL_ECHO = atoi(datex);
			strcpy(Tag,"BILL_MARCA");
			GetXmlTagValue(Response,Tag, MertPort.BILL_MARCA, &start, tc);

			fclose(fptr);

			printf("\nCONFIG\n");
			printf("CODE:           %s\n", hdr_config.Code);
			printf("FORMAT:         %d\n", hdr_config.Format);
			printf("VERSION:        %d\n", hdr_config.Version);
			printf("SOURCEVERSION:  %d\n", hdr_config.SourceVersion);
			printf("CREATEDATE:     %s\n", hdr_config.CreationDate);
			printf("ACTIVATIONDATE: %s\n", hdr_config.ActivationDate);
			printf("AUTHOR:         %s\n", hdr_config.Author);
			printf("------------------------------------------\n");
			printf("Expendedora\n");
			printf("Marca     = %d\n", MertPort.EXP_Marca);
			printf("Tipo      = %d\n", MertPort.EXP_TipoPuerto);
			printf("SLOTS's   = [%d][%d][%d][%d]\n",	MertPort.EXP_SAM_SLOT0,
														MertPort.EXP_SAM_SLOT1,
														MertPort.EXP_SAM_SLOT2,
														MertPort.EXP_SAM_SLOT3);
			printf("COMM      = [%s][%d]\n", MertPort.EXP_SerialName,
											 MertPort.EXP_SerialBaud);
			
			
			printf("USB_NAME      = [%s][%d]\n", MertPort.EXP_USBName,
											 MertPort.EXP_USBMarca);

			printf("RECARGA\n");
			printf("Marca     = %d\n", MertPort.REC_Marca);
			printf("Tipo      = %d\n", MertPort.REC_TipoPuerto);
			printf("SLOTS's   = [%d][%d][%d][%d]\n",	MertPort.REC_SAM_SLOT0,
														MertPort.REC_SAM_SLOT1,
														MertPort.REC_SAM_SLOT2,
														MertPort.REC_SAM_SLOT3);
			printf("COMM      = [%s][%d]\n", MertPort.REC_SerialName,
											 MertPort.REC_SerialBaud);

			printf("CAJA MONEDAS\n");
			printf("Marca     = %d\n", MertPort.COINBOX_Marca);
			printf("Tipo      = %d\n", MertPort.COINBOX_TipoPuerto);
			printf("SLOTS's   = [%d][%d][%d][%d]\n",	MertPort.COINBOX_SAM_SLOT0,
														MertPort.COINBOX_SAM_SLOT1,
														MertPort.COINBOX_SAM_SLOT2,
														MertPort.COINBOX_SAM_SLOT3);
			printf("COMM      = [%s][%d]\n", MertPort.COINBOX_SerialName,
											 MertPort.COINBOX_SerialBaud);

			printf("CAJA BILLETES\n");
			printf("Marca     = %d\n", MertPort.BILLBOX_Marca);
			printf("Tipo      = %d\n", MertPort.BILLBOX_TipoPuerto);
			printf("SLOTS's   = [%d][%d][%d][%d]\n",	MertPort.BILLBOX_SAM_SLOT0,
														MertPort.BILLBOX_SAM_SLOT1,
														MertPort.BILLBOX_SAM_SLOT2,
														MertPort.BILLBOX_SAM_SLOT3);
			printf("COMM      = [%s][%d]\n", MertPort.BILLBOX_SerialName,
											 MertPort.BILLBOX_SerialBaud);

			printf("puerta\n");
			printf("Marca     = %d\n", MertPort.PUERTA_Marca);
			printf("Tipo      = %d\n", MertPort.PUERTA_TipoPuerto);
			printf("SLOTS's   = [%d][%d][%d][%d]\n",	MertPort.PUERTA_SAM_SLOT0,
														MertPort.PUERTA_SAM_SLOT1,
														MertPort.PUERTA_SAM_SLOT2,
														MertPort.PUERTA_SAM_SLOT3);
			printf("COMM      = [%s][%d]\n", MertPort.PUERTA_SerialName,
											 MertPort.PUERTA_SerialBaud);

			printf("SERIALES\n");
			printf("MODIO:     [%s][%d][%d]\n", MertPort.MODIO_COMM, MertPort.MODIO_BAUD, MertPort.MODIO_TP);
			printf("COIN:      [%d][%s][%d][%d][%d]\n", MertPort.COIN_PROTOCOL, MertPort.COIN_COMM,  
														MertPort.COIN_BAUD, MertPort.COIN_VERIFICACION, MertPort.COIN_ECHO);
			printf("BILL:      [%d][%s][%d][%d][%d]\n", MertPort.BILL_PROTOCOL, MertPort.BILL_COMM,  
														MertPort.BILL_BAUD, MertPort.BILL_VERIFICACION, MertPort.BILL_ECHO);
			printf("\n");

			tRdrCV = tRdrACR1222 = tRdrACR1222 = 0;

			// Contar total de lectores de ACS y Civintec
			printf("TOTAL DE LECTORES\n");
			for(byte k=0; k<10; k++)
			{
				if(marca[k]==RDR_CN670) 
					tRdrCV++;
				else if (marca[k]==RDR_ACR1222L )
 					tRdrACR1222++;
				else if ( marca[k]==RDR_ACR1252L )
 					tRdrACR1252++;
			}			
			printf("LECTORES CV_CN670	= %d\n", tRdrCV);
			printf("LECTORES ACR1222L	= %d\n", tRdrACR1222);
			printf("LECTORES ACR1252L	= %d\n", tRdrACR1252);

			return 0;
		}
	}
	printf("NO existe [%s]\n",fname);
	return 1;
}

/******************************************************************************/
/* revisa si hubo cambios en puertos.xml                                      */
/******************************************************************************/
int revPuertos(void)
{	static char tmstart=0;
	static time_t port_time;
	struct stat attrib;

	char fname[100];
	
	if(mert_famisa)
		snprintf(fname,100,"%sconfig/puertos.xml",mert_operacion.path);
	else
		snprintf(fname,100,"%sconfig/puertos_dte.xml",mert_operacion.path);
		

	if(access(fname, F_OK)==0)
	{
		stat(fname, &attrib);
		if(tmstart==0)
		{	//printf("Lee Ports Config\n");
			if( rdPuertos( fname) != 0)
			{	printf("Error al abrir configuracion de puertos\n");
				return 0x0400;
			}
			tmstart = 1;
			port_time = attrib.st_mtime;
			return 0;
		}

		if( port_time != attrib.st_mtime)
		{	printf("diferente tiempo\n");
			// leer archivo
			if( rdPuertos( fname ) != 0)
			{	printf("Error al abrir configuracion de puertos\n");
				return 0x0400;
			}
			port_time = attrib.st_mtime;
			return 0;
		}
	}
//	printf("No existe puertos.xml\n");
	return 0x0401;
}
////////////////////////////////////////////////////////////////////////////////

