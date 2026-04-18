/*
 * File:   xmls.cpp
 * Author: FAMISA
 * 
 */
/******************************************************************************/
/* Rutinas para el manejo de los XML, lectura y actualizacion                 */
/******************************************************************************/
//
#include "xmls.h"
#include "mert.h"
#include "cctalk.h"
//
//
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void xmlBank(xmlNode * node, int indent_len, int *xl, int *xc)
{	char aux[40];
	xmlAttr *prop;
	xmlChar *value;

	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "billcoin-list")==0)
			{	// sacar sus valores
				(*xl)++;
				*xc = 0;
			}
			else if( strcmp((const char *)node->name, "billcoin")==0)
			{	// sacar sus valores
				prop = node->properties;
				while (prop!=NULL)
				{
					value=xmlGetProp(node, prop->name);
					if(value != NULL)
					{
						if(strcmp((const char *)prop->name,"id") == 0)
						{	sprintf(aux,"%s",value);
							BankNotes[*xl].id[*xc] = atoi(aux);
						}
						else if (strcmp((const char *)prop->name,"denominacion") == 0)
						{
							BankNotes[*xl].denominacion[*xc] = 
													atof((const char *)value) * 100;
//							atof((const char *)xmlNodeGetContent(node)) * 100;
						}
						else if (strcmp((const char *)prop->name,"name") == 0)
						{	sprintf(aux,"%s",value);
							strcpy( BankNotes[*xl].name[*xc], aux);
						}
						else if (strcmp((const char *)prop->name,"enable") == 0)
						{	sprintf(aux,"%s",value);
							BankNotes[*xl].enable[*xc] = atoi(aux);
							(*xc)++;
							BankNotes[*xl].tog_reg = (*xc);
							printf("Registros=%3d\n", BankNotes[*xl].tog_reg );
						}
						prop=prop->next;
					}				
				}
			}
        }
        xmlBank(node->children, indent_len + 1, xl, xc);
		node = node->next;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlBank(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;
	int xlin, xcol;

	char fname[100];
	snprintf(fname,100,"%sconfig/notes.xml",mert_operacion.path);

	memset( &BankXmlHdr,  0, sizeof(BankXmlHdr) );
	memset( &BankNotes,    0, sizeof(BankNotes) );

	node = GetXmlHeaderTDE(docXml, fname, &MenuXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xlin=-1; xcol=0;
	xmlBank(node, 1, &xlin, &xcol);

	int i,j;
	printf("Bank Notes\n");
	printf("Fecha de Creacion=%s\n", dtime(BankXmlHdr.time_creacion));

	printf("Header=[%s] Version=%d\n", BankXmlHdr.tipo, BankXmlHdr.version);

	printf("BILLETES\nTotal de Registros=%3d\n", BankNotes[0].tog_reg );
	for(i=0; i< BankNotes[0].tog_reg; i++)
	{
		printf("ID=%2d %6d %d %s\n", BankNotes[0].id[i],     BankNotes[0].denominacion[i],
									 BankNotes[0].enable[i], BankNotes[0].name[i]);

	}

	printf("\nMONEDAS\nTotal de Registros=%3d\n", BankNotes[1].tog_reg );
	for(i=0; i< BankNotes[1].tog_reg; i++)
	{
		printf("ID=%2d %6d %d %s\n", BankNotes[1].id[i],     BankNotes[1].denominacion[i],
									 BankNotes[1].enable[i], BankNotes[1].name[i]);

	}

	
	xmlFreeDoc(docXml);
	xmlCleanupParser();

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void xmlMenus(xmlNode * node, int indent_len, int *xl, int *xc)
{	char aux[40];
	xmlAttr *prop;
	xmlChar *value;

	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "opciones-list")==0)
			{	// sacar sus valores
				(*xl)++;
				*xc = 0;
//				printf("opciones list\n");
				prop = node->properties;
				while (prop!=NULL)
				{
					value=xmlGetProp(node, prop->name);
					if(value != NULL)
					{
						if(strcmp((const char *)prop->name,"code") == 0)
						{	sprintf(aux,"%s",value);
							MenuName.code[*xl] = atoi(aux);
						}
						else if(strcmp((const char *)prop->name,"name") == 0)
						{
							strcpy(MenuName.name[*xl], (const char *)value);
							MenuName.tot_reg = (*xl) + 1;
						}
					}
					prop=prop->next;
				}
			}
			else if( strcmp((const char *)node->name, "opcion")==0)
			{	// sacar sus valores
				prop = node->properties;
				while (prop!=NULL)
				{
					value=xmlGetProp(node, prop->name);
					if(value != NULL)
					{
						if(strcmp((const char *)prop->name,"opcion") == 0)
						{	sprintf(aux,"%s",value);
							MenuType[*xl].opcion[*xc] = atoi(aux);
							MenuType[*xl].tot_reg = (*xc)+1;
							(*xc)++;
						}
						prop=prop->next;
					}				
				}
			}else if( strcmp((const char *)node->name, "profile-list")==0)
			{	// sacar sus valores
				(*xl)=0;
//				printf("profile list\n");
			}
			else if( strcmp((const char *)node->name, "type-profile")==0)
			{	// sacar sus valores
				//profile-code menu  name
				prop = node->properties;
				while (prop!=NULL)
				{
					value=xmlGetProp(node, prop->name);
					if(value != NULL)
					{
						if(strcmp((const char *)prop->name,"profile-code") == 0)
						{	//printf("profile=%s\n", value);
							sprintf(aux,"%s",value);
							MenuProfile.profile[*xl] = atoi(aux);
						}
						else if(strcmp((const char *)prop->name,"menu") == 0)
						{	//printf("menu=%s\n", value);
							sprintf(aux,"%s",value);
							MenuProfile.menu[*xl] = atoi(aux);
						}
						else if(strcmp((const char *)prop->name,"name") == 0)
						{	//printf("name=%s\n", value);
							strcpy(MenuProfile.name[*xl], (const char *)value);
							(*xl)++;
							 MenuProfile.tot_reg = *xl;
						}
						prop=prop->next;
					}				
				}
			}			
        }
        xmlMenus(node->children, indent_len + 1, xl, xc);
		node = node->next;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlMenus(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;
	int xlin, xcol;

	char fname[100];
	snprintf(fname,100,"%sconfig/opciones.xml",mert_operacion.path);
	
	memset( &MenuXmlHdr,  0, sizeof(MenuXmlHdr) );
	memset( &MenuType,    0, sizeof(MenuType) );
	memset( &MenuProfile, 0, sizeof(MenuProfile) );
	memset( &MenuName,    0, sizeof(MenuName) );

	node = GetXmlHeaderTDE(docXml, fname, &MenuXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xlin=-1; xcol=0;
	xmlMenus(node, 1, &xlin, &xcol);

	int i,j;
	printf("MENU's\n");
	printf("Fecha de Creacion=%s\n", dtime(MenuXmlHdr.time_creacion));

	printf("Header=[%s] Version=%d\n", MenuXmlHdr.tipo, MenuXmlHdr.version);
	
	printf("Total de Registros=%3d\n", MenuName.tot_reg );
	for(i=0; i<10;i++)
	{	printf("[i=%2d]ID=%02d->%s\n", i, MenuName.code[i], MenuName.name[i]);
		printf("Total de Registros=%d\n", MenuType[i].tot_reg);
		for(j=0;j<10;j++)
			printf("Menu=[i=%2d][%2d]\n",i, MenuType[i].opcion[j]);
	}
	printf("Perfiles\n");
	printf("Total de Registros=%d\n", MenuProfile.tot_reg);
	for(i=0; i<MenuProfile.tot_reg; i++)
		printf("Perfil=[i=%2d][%4d][%4d][%s]\n", i, MenuProfile.profile[i],
													MenuProfile.menu[i],
													MenuProfile.name[i]);

	xmlFreeDoc(docXml);
	xmlCleanupParser();

	return 0;
}

/******************************************************************************/
/*  Obtiene el valor de los limites                                           */
/******************************************************************************/
void xmlConfig(xmlNode * node, int indent_len)
{
	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "dispensador")==0)
			{
				Config.Dispensador = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "billetero")==0)
			{
				Config.Billetero = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "monedero")==0)
			{
				Config.Monedero = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "stacker")==0)
			{
				//Config.Stacker = atoi((const char *)xmlNodeGetContent(node)); //Un solo Stacker ignorar configuracion
			}
			else if( strcmp((const char *)node->name, "NoMov")==0)
			{
				Config.NoMov = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "binbox")==0)
			{
				Config.Captura = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "binboxmax")==0)
			{
				Config.BinBoxMax = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "NoMovEvent")==0)
			{
				Config.NoMovEvent = atoi((const char *)xmlNodeGetContent(node));
			}
        }
        xmlConfig(node->children, indent_len + 1);
		node = node->next;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlConfig(char fname[])
{	xmlDoc *docXml = NULL;
	xmlNode *node;

	memset( &ConfigXmlHdr, 0, sizeof(ConfigXmlHdr) );

	node = GetXmlHeaderTDE(docXml, fname, &ConfigXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xmlConfig(node, 1);

	printf("CONFIG\n");
	printf("Fecha de Creacion=%s\n", dtime(ConfigXmlHdr.time_creacion));
	printf("Header=[%s] Version=[%d]\n", ConfigXmlHdr.tipo, ConfigXmlHdr.version);

	printf("Dispensador      =%d\n", Config.Dispensador);
	printf("Monedero         =%d\n", Config.Monedero);
	printf("Billetero        =%d\n", Config.Billetero);
	printf("Stacker Activo   =%d\n", Config.Stacker);
	printf("NoMov            =%d\n", Config.NoMov);
	printf("Capturadas       =%d\n", Config.Captura);
	printf("BinBox Maximo    =%d\n", Config.BinBoxMax);
	printf("NoMov Eventos    =%d\n", Config.NoMovEvent);

	xmlFreeDoc(docXml);
	xmlCleanupParser();

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void xmlPuertos(xmlNode * node, int indent_len)
{	xmlAttr *prop;

	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{	printf("******Node=%s\n", (const char *)node->name );
			if( strcmp((const char *)node->name, "EXPENDEDORA")==0)
			{	prop = node->properties;
				while (prop!=NULL)
				{	printf("******Node=%s\n", (const char *)xmlGetProp(node, prop->name) );
//					if(strcmp((const char *)prop->name,"code") == 0)
//					{
//						Lineas.code[*xl] = atoi((const char *)xmlGetProp(node, prop->name));
//					}
					prop=prop->next;
				}
			}
        }
        xmlPuertos(node->children, indent_len + 1);
		node = node->next;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlPuertos(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;
	int xlin;

	char filename[100];
	snprintf(filename,100,"%sconfig/puertos.xml",mert_operacion.path);

	memset( &MertPortXmlHdr, 0, sizeof(MertPortXmlHdr) );
	memset( &MertPort     ,  0, sizeof(MertPort) );


	node = GetXmlHeaderTDE(docXml, filename, &MertPortXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xmlPuertos(node, 1);

	int i;
	printf("Puertos\n");
	printf("Fecha de Creacion=%s\n", dtime(MertPortXmlHdr.time_creacion));
	printf("Header=[%s] Version=%d\n", MertPortXmlHdr.tipo, MertPortXmlHdr.version);

	printf("Expendedora\n");
	printf("Marca     = %d\n", MertPort.EXP_Marca);
	printf("Tipo      = %d\n", MertPort.EXP_TipoPuerto);
	printf("SLOTS's   = [%d][%d][%d][%d]\n",	MertPort.EXP_SAM_SLOT0,
												MertPort.EXP_SAM_SLOT1,
												MertPort.EXP_SAM_SLOT2,
												MertPort.EXP_SAM_SLOT3);
	printf("COMM      = [%s][%d]\n", MertPort.EXP_SerialName,
									 MertPort.EXP_SerialBaud);

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

	xmlFreeDoc(docXml);
	xmlCleanupParser();
	
	return 0;
}



/******************************************************************************/
/*  Obtiene el valor de los campos BlackList SAM                              */
/******************************************************************************/
void XmlBLSAMs(xmlNode * node, int indent_len, int *xl)
{	char *eptr;

	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "sam")==0)
			{
				(*xl)++;
			}
			if( strcmp((const char *)node->name, "c1")==0)
			{
				BlackListSAM.bl_sam[*xl] = strtoll((char *)xmlNodeGetContent(node), &eptr, 16);
				BlackListSAM.tog_reg = (*xl) + 1;
			}
        }
        XmlBLSAMs(node->children, indent_len + 1, xl);
		node = node->next;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlBLSAMs(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;
	int xlin;

	char filename[100];
	snprintf(filename,100,"%sblacklist/black_list_sam.ln",mert_operacion.path);

	memset( &BlackListSAMXmlHdr, 0, sizeof(BlackListSAMXmlHdr) );
	memset( &BlackListSAM,       0, sizeof(BlackListSAM) );


	node = GetXmlHeaderTDE(docXml, filename, &BlackListSAMXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xlin=-1;
	XmlBLSAMs(node, 1, &xlin);

	OrdenaBlackList(BlackListSAM.bl_sam, BlackListSAM.tog_reg);

	printf("ListaNegra SAM\n");
	printf("Fecha de Creacion=%s\n", dtime(BlackListSAMXmlHdr.time_creacion));

	printf("Header=[%s] Version=[%d]\n", BlackListSAMXmlHdr.tipo, BlackListSAMXmlHdr.version);
	printf("Total de Registros=%d\n", BlackListSAM.tog_reg);

	xmlFreeDoc(docXml);
	xmlCleanupParser();
	
	return 0;
}

/******************************************************************************/
/*  Obtiene el valor de los campos Black List MIA's                           */
/******************************************************************************/
void XmlBLMIAs(xmlNode * node, int indent_len, int *xl)
{	char *eptr;
	char aux[40], strnum[30];
	long long val;

	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "tarjeta")==0)
			{
				(*xl)++;
			}
			else if( strcmp((const char *)node->name, "c1")==0)
			{	// de momento no hacemos nada
				// (char *)xmlNodeGetContent(node)
			}
			else if( strcmp((const char *)node->name, "c2")==0)
			{	
				strcpy( aux, substr( (char *)xmlNodeGetContent(node),8,19 ) );
				BlackListMIA.bl_mia[*xl] = strtoll(aux, &eptr, 10);
//				BlackListMIA.bl_mia[*xl] = val;

//				strcpy( aux, (const char *)xmlNodeGetContent(node) );
//				memcpy(strnum, &aux[8], 19);
//				strnum[19] = '\0';
//				val = strtoll(strnum, &eptr, 10);
//				BlackListMIA.bl_mia[*xl] = val;

//				printf("C2=[xl=%4d][%s][%s]\n", *xl, (char *)xmlNodeGetContent(node), aux);
//				printf("MIA[i=%4d][%019llud]\n",*xl, BlackListMIA.bl_mia[*xl] );

				BlackListMIA.tog_reg = (*xl) + 1;
			}
        }
        XmlBLMIAs(node->children, indent_len + 1, xl);
		node = node->next;
	}
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlBLMIAs(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;
	int xlin;
	
//	long long clave = 210509022600000036;
	long long clave = 110107092500000001;

	char filename[100];
	snprintf(filename,100,"%sblacklist/black_list_card.ln",mert_operacion.path);

	memset( &BlackListMIAXmlHdr, 0, sizeof(BlackListMIAXmlHdr) );
	memset( &BlackListMIA,       0, sizeof(BlackListMIA) );


	node = GetXmlHeaderTDE(docXml, filename, &BlackListMIAXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xlin=-1;
	XmlBLMIAs(node, 1, &xlin);

	OrdenaBlackList(BlackListMIA.bl_mia, BlackListMIA.tog_reg);
	
	xlin = BusquedaBinaria(BlackListMIA.bl_mia, BlackListMIA.tog_reg, clave);
	printf("Indice=%d\n", xlin);

	printf("ListaNegra MIA\n");
	printf("Fecha de Creacion=%s\n", dtime(BlackListMIAXmlHdr.time_creacion));

	printf("Header=[%s] Version=[%d]\n", BlackListMIAXmlHdr.tipo, BlackListMIAXmlHdr.version);
	printf("Total de Registros=%d\n", BlackListMIA.tog_reg);

	xmlFreeDoc(docXml);
	xmlCleanupParser();
	
	return 0;
}

/******************************************************************************/
/*  Obtiene el valor de los campos                                            */
/******************************************************************************/
void xmlReaders(xmlNode * node, int indent_len, int *xl)
{
	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "reader")==0)
			{	// sacar sus valores
				(*xl)++;
			}
			else if( strcmp((const char *)node->name, "c1")==0)
			{
				strcpy(Readers.posicion[*xl], (char *)xmlNodeGetContent(node));
			}
			else if(strcmp((const char *)node->name,"c2") == 0)
			{
				Readers.slot[*xl] = atoi((char *)xmlNodeGetContent(node));
			}
			else if(strcmp((const char *)node->name,"c3") == 0)
			{
				Readers.UID[*xl] = strtol((char *)xmlNodeGetContent(node), NULL, 16);
				Readers.tot_reg = (*xl) + 1;
			}
        }
        xmlReaders(node->children, indent_len + 1, xl);
		node = node->next;
	}
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlReaders(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;
	int xlin;

	char filename[100];
	snprintf(filename,100,"%sconfig/ksvr.rdrs",mert_operacion.path);

	memset( &ReadersXmlHdr, 0, sizeof(ReadersXmlHdr) );
	memset( &Readers,       0, sizeof(Readers) );


	node = GetXmlHeaderTDE(docXml, filename, &ReadersXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xlin=-1;
	xmlReaders(node, 1, &xlin);
			
	int i;
	printf("Readers\n");
	printf("Fecha de Creacion=%s\n", dtime(ReadersXmlHdr.time_creacion));

	printf("Header=[%s] Version=[%d]\n", ReadersXmlHdr.tipo, ReadersXmlHdr.version);
	printf("Total de Registros=%d\n", Readers.tot_reg);
	for(i=0; i<Readers.tot_reg;i++)
		printf("Readers[i=%2d][%-20s][%2d][%016lx]\n",i, Readers.posicion[i],
						Readers.slot[i], Readers.UID[i]);
	xmlFreeDoc(docXml);
	xmlCleanupParser();
	
	return 0;
}

/******************************************************************************/
/*  Obtiene el valor de los limites                                           */
/******************************************************************************/
void xmlMertLmt(xmlNode * node, int indent_len)
{
	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "type")==0)
			{
				MertLmt.type = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "cost_travel")==0)
			{
				MertLmt.cost_travel = atof((const char *)xmlNodeGetContent(node)) * 100;
			}
			else if( strcmp((const char *)node->name, "cost_card")==0)
			{
				MertLmt.cost_card = atof((const char *)xmlNodeGetContent(node)) * 100;
			}
			else if( strcmp((const char *)node->name, "max_balance")==0)
			{
				MertLmt.max_balance = atof((const char *)xmlNodeGetContent(node)) * 100;
			}
			else if( strcmp((const char *)node->name, "coin_capacity_stored")==0)
			{
				MertLmt.coin_capacity_stored = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "threshold_stored")==0)
			{
				MertLmt.threshold_stored = atof((const char *)xmlNodeGetContent(node)) * 100;
			}
			else if( strcmp((const char *)node->name, "capacity_validator")==0)
			{
				MertLmt.capacity_validator = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "capacity_dispenser")==0)
			{
				MertLmt.capacity_dispenser = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "threshold_dispenser")==0)
			{
				MertLmt.threshold_dispenser = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "bill_capacity_stored")==0)
			{
				MertLmt.bill_capacity_stored = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "time_ope_refill")==0)
			{
				MertLmt.time_ope_refill = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "time_ope_purchase")==0)
			{
				MertLmt.time_ope_purchase = atoi((const char *)xmlNodeGetContent(node));
			}
			else if( strcmp((const char *)node->name, "url")==0)
			{
				strcpy(MertLmt.url, (char *)xmlNodeGetContent(node));
			}
        }
        xmlMertLmt(node->children, indent_len + 1);
		node = node->next;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlMertLmt(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;

	char filename[100];
	snprintf(filename,100,"%sconfig/ksvr.prms",mert_operacion.path);

	memset( &MertLmtXmlHdr, 0, sizeof(MertLmtXmlHdr) );
	memset( &MertLmt,       0, sizeof(MertLmt) );

	node = GetXmlHeaderTDE(docXml, filename, &MertLmtXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xmlMertLmt(node, 1);

	MertLmt.coin_threshold_stored = 
					( MertLmt.coin_capacity_stored * MertLmt.threshold_stored)/100;
	MertLmt.bill_threshold_stored = 
					( MertLmt.bill_capacity_stored * MertLmt.threshold_stored)/100;
	
	
	
	mert_identidad.TIPO = MertLmt.type;	// tipo de maquina

	printf("MERT LIMITES\n");
	printf("Fecha de Creacion=%s\n", dtime(MertLmtXmlHdr.time_creacion));
	printf("Header=[%s] Version=[%d]\n", MertLmtXmlHdr.tipo, MertLmtXmlHdr.version);

	printf("type                  =%d\n", MertLmt.type);
	printf("cost_travel           =%d\n", MertLmt.cost_travel);
	printf("cost_card             =%d\n", MertLmt.cost_card);
	printf("max_balance           =%d\n", MertLmt.max_balance);
	printf("coin_capacity_stored  =%d\n", MertLmt.coin_capacity_stored);	
	printf("threshold_stored      =%d\n", MertLmt.threshold_stored);
	printf("capacity_validator    =%d\n", MertLmt.capacity_validator);
	printf("capacity_dispenser    =%d\n", MertLmt.capacity_dispenser);
	printf("threshold_dispenser   =%d\n", MertLmt.threshold_dispenser);
	printf("url                   =%s\n", MertLmt.url);
	printf("bill_capacity_stored  =%d\n", MertLmt.bill_capacity_stored);
	printf("time_ope_refill       =%d\n", MertLmt.time_ope_refill);
	printf("time_ope_purchase     =%d\n", MertLmt.time_ope_purchase);

	printf("Monedero Casi Lleno   =%d\n", MertLmt.coin_threshold_stored);
	printf("Billetero Casi Lleno  =%d\n", MertLmt.bill_threshold_stored);
	
	xmlFreeDoc(docXml);
	xmlCleanupParser();
	
	return 0;
}

/******************************************************************************/
/* // Convierte Lin/Est de TDE a Telvent                                      */
/******************************************************************************/
void cnvTdeTV(void)		
{	int i, j;
	
	for(i=0; i< Lineas.tot_reg; i++)
	{
		if( Lineas.codeTDE[i]==MertID.lineaTDE)
		{
			MertID.lineaTV = Lineas.code[i];

			for(j=0; j<Lineas.estaciones[i].tot_reg; j++)
			{	
				if(Lineas.estaciones[i].codeTDE[j] == MertID.estacionTDE)
				{
					MertID.estacionTV = Lineas.estaciones[i].code[j];
					strcpy(MertID.name, Lineas.estaciones[i].name[j] );
					break;	
				}
			}
		}
	}
}

/******************************************************************************/
/*  Obtiene el valor de los campos                                            */
/******************************************************************************/
void xmlMertID(xmlNode * node, int indent_len)
{
	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "code")==0)
			{
				strcpy(MertID.code, (char *)xmlNodeGetContent(node));
			}
			else if(strcmp((const char *)node->name,"state") == 0)
			{
				MertID.state = atoi((const char *)xmlNodeGetContent(node));
			}
        }
        xmlMertID(node->children, indent_len + 1);
		node = node->next;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlMertID(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;

	char filename[100];
	snprintf(filename,100,"%sconfig/ksvr.conf",mert_operacion.path);

	memset( &MertIDXmlHdr, 0, sizeof(MertIDXmlHdr) );
	memset( &MertID,       0, sizeof(MertID) );


	node = GetXmlHeaderTDE(docXml, filename, &MertIDXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xmlMertID(node, 1);

	printf("MERT ID\n");
	printf("Fecha de Creacion=%s\n", dtime(MertIDXmlHdr.time_creacion));
	printf("Header=[%s] Version=[%d]\n", MertIDXmlHdr.tipo, MertIDXmlHdr.version);
	// META030360073
	MertID.lineaTDE    = atoi( substr(MertID.code, strlen(MertID.code)- 9, 2));
	MertID.estacionTDE = atoi( substr(MertID.code, strlen(MertID.code)- 7, 3));
	MertID.NumDisp     = atoi( substr(MertID.code, strlen(MertID.code)- 4, 5));

	cnvTdeTV();		// Convierte Lin/Est de TDE a Telvent

	printf("ID           =%s\n", MertID.code);
	printf("Nombre       =%s\n", MertID.name);
	printf("Estado       =%d\n", MertID.state);
	printf("Linea TDE    =%d\n", MertID.lineaTDE);
	printf("Estacion TDE =%d\n", MertID.estacionTDE);
	printf("Linea TV     =%d\n", MertID.lineaTV);
	printf("Estacion TV  =%d\n", MertID.estacionTV);
	printf("No.Disp ID   =%d\n", MertID.NumDisp);

	mert_identidad.COMP = 1;	// COMPAÑIA
	mert_identidad.TRAY = 0;
	mert_identidad.TICKETVAL = 0;

	mert_identidad.LINEA    = MertID.lineaTV;
	mert_identidad.ESTACION = MertID.estacionTV;
	mert_identidad.NUMDISP  = MertID.NumDisp;

	xmlFreeDoc(docXml);
	xmlCleanupParser();
	
	return 0;
}

/******************************************************************************/
/*  Obtiene el valor de los campos de Usuarios                                */
/******************************************************************************/
void xmlLineas(xmlNode * node, int indent_len, int *xl, int *xc)
{	xmlAttr *prop;

	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "TransportMean")==0)
			{
				(*xl)++;
				prop = node->properties;
				while (prop!=NULL)
				{
					if(strcmp((const char *)prop->name,"code") == 0)
					{
						Lineas.code[*xl] = atoi((const char *)xmlGetProp(node, prop->name));
					}
					if(strcmp((const char *)prop->name,"codeTDE") == 0)
					{
						Lineas.codeTDE[*xl] = atoi((const char *)xmlGetProp(node, prop->name));
					}
					else if(strcmp((const char *)prop->name,"name") == 0)
					{	
						strcpy(Lineas.name[*xl], (const char *)xmlGetProp(node, prop->name));
						Lineas.tot_reg = (*xl) + 1;
					}
					prop=prop->next;
				}
			}
			else if( strcmp((const char *)node->name, "AreaList")==0)
			{	
				*xc = 0;
			}
			else if( strcmp((const char *)node->name, "Area")==0)
			{
				prop = node->properties;
				while (prop!=NULL)
				{
					if(strcmp((const char *)prop->name,"code") == 0)
					{
						Lineas.estaciones[*xl].code[*xc] = atoi((const char *)xmlGetProp(node, prop->name));
					}
					if(strcmp((const char *)prop->name,"codeTDE") == 0)
					{
						Lineas.estaciones[*xl].codeTDE[*xc] = atoi((const char *)xmlGetProp(node, prop->name));
					}
					else if(strcmp((const char *)prop->name,"name") == 0)
					{	
						strcpy(Lineas.estaciones[*xl].name[*xc], (const char *)xmlGetProp(node, prop->name));
						(*xc)++;
						Lineas.estaciones[*xl].tot_reg = (*xc);
					}
					prop=prop->next;
				}
			}
        }
        xmlLineas(node->children, indent_len + 1, xl, xc);
		node = node->next;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlLineas(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;
	int xlin, xcol;

	char filename[100];
	snprintf(filename,100,"%sconfig/geomty.xml",mert_operacion.path);

	memset( &LineasXmlHdr, 0, sizeof(LineasXmlHdr) );
	memset( &Lineas,       0, sizeof(Lineas) );

	node = GetXmlHeaderTDE(docXml, filename, &LineasXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xlin=-1;
	xcol = 0;
	xmlLineas(node, 1, &xlin, &xcol);
			
	int i,j;
	printf("LINEAS\n");
	printf("Fecha de Creacion=%s\n", dtime(LineasXmlHdr.time_creacion));
	printf("Header=[%s] Version=[%d]\n", LineasXmlHdr.tipo, LineasXmlHdr.version);
	printf("Total de Lineas=%d\n", Lineas.tot_reg);

	for(i=0; i<Lineas.tot_reg;i++)
	{	printf("Lineas[i=%2d][%2d][%2d][%-50s]\n",i,
					Lineas.code[i], Lineas.codeTDE[i], Lineas.name[i]);
		printf("Estaciones\n");
		printf("Total de Estaciones=%d\n", Lineas.estaciones[i].tot_reg);
		for(j=0; j<Lineas.estaciones[i].tot_reg; j++)
		{	printf("    Estaciones[i=%2d][%2d][%2d][%-50s]\n",j, Lineas.estaciones[i].code[j],
							Lineas.estaciones[i].codeTDE[j], Lineas.estaciones[i].name[j]);
		}
	}

	xmlFreeDoc(docXml);
	xmlCleanupParser();
	
	return 0;
}

/******************************************************************************/
/*  Obtiene el valor de los campos de Usuarios                                */
/******************************************************************************/
void xmlUsers(xmlNode * node, int indent_len, int *xl)
{
	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "tarjeta")==0)
			{	// sacar sus valores
				(*xl)++;
			}
			else if( strcmp((const char *)node->name, "c1")==0)
			{
				Usuarios.UID[*xl] = strtol((char *)xmlNodeGetContent(node), NULL, 16);
			}
			else if(strcmp((const char *)node->name,"c2") == 0)
			{
				strcpy(Usuarios.NumLog[*xl], (char *)xmlNodeGetContent(node));
			}
			else if(strcmp((const char *)node->name,"c3") == 0)
			{
				Usuarios.profile[*xl] = atoi((char *)xmlNodeGetContent(node));
			}
			else if(strcmp((const char *)node->name,"c4") == 0)
			{
				strcpy(Usuarios.name[*xl], (char *)xmlNodeGetContent(node));
			}
			else if(strcmp((const char *)node->name,"c5") == 0)
			{
				strcpy(Usuarios.pwd[*xl], (char *)xmlNodeGetContent(node));
				Usuarios.tot_reg = (*xl) + 1;
			}
        }
        xmlUsers(node->children, indent_len + 1, xl);
		node = node->next;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void OrdenaWhiteList(USERS *lista, int totdat)
{	unsigned int UID;
	char NumLog[30];
	char profile;
	char name[40];
	char pwd[6]; 

	for (int i = 0; i < totdat - 1; i++)
	{	for (int j = i + 1; j < totdat; j++)
			if ( lista->UID[i] > lista->UID[j])
			{	// intercambia posiciones
				UID          = lista->UID[i];
				profile      = lista->profile[i];
				strcpy(NumLog, lista->NumLog[i]);
				strcpy(name,   lista->name[i]);
				strcpy(pwd,    lista->pwd[i]);

				lista->UID[i]          = lista->UID[j];
				lista->profile[i]      = lista->profile[j];	
				strcpy(lista->NumLog[i], lista->NumLog[j]);
				strcpy(lista->name[i],   lista->name[j]);
				strcpy(lista->pwd[i],    lista->pwd[j]);

				lista->UID[j]          = UID;
				lista->profile[j]      = profile;	
				strcpy(lista->NumLog[j], NumLog);
				strcpy(lista->name[j],   name);
				strcpy(lista->pwd[j],    pwd);
			}
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlUsers(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;
	int xlin;

	char filename[100];
	snprintf(filename,100,"%sblacklist/white_list_card.lw",mert_operacion.path);

	memset( &UsuariosXmlHdr, 0, sizeof(UsuariosXmlHdr) );
	memset( &Usuarios,       0, sizeof(Usuarios) );


	node = GetXmlHeaderTDE(docXml, filename, &UsuariosXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xlin=-1;
	xmlUsers(node, 1, &xlin);
	xmlFreeDoc(docXml);
	xmlCleanupParser();
			
	int i;
	printf("USUARIOS\n");
	printf("Fecha de Creacion=%s\n", dtime(UsuariosXmlHdr.time_creacion));
	
	printf("Header=[%s] Version=[%d]\n", UsuariosXmlHdr.tipo, UsuariosXmlHdr.version);
	printf("Total de Registros=%d\n", Usuarios.tot_reg);
//	for(i=0; i<Usuarios.tot_reg;i++)
//		printf("Usuarios[i=%2d][%08x][%-30s][%2d][%-50s][%10s]\n",i, Usuarios.UID[i], Usuarios.NumLog[i],
//						Usuarios.profile[i],Usuarios.name[i], Usuarios.pwd[i]);

	// ordenar x UID
	OrdenaWhiteList(&Usuarios, Usuarios.tot_reg);
	
	for(i=0; i<Usuarios.tot_reg;i++)
		printf("Usuarios[i=%2d][%08x][%-30s][%2d][%-50s][%10s]\n",i, Usuarios.UID[i], Usuarios.NumLog[i],
						Usuarios.profile[i],Usuarios.name[i], Usuarios.pwd[i]);
	
	return 0;
}

/******************************************************************************/
/*  Obtiene el valor de los campos del archivo de Balance                     */
/******************************************************************************/
void xmlBalance(xmlNode * node, int indent_len, int *xl)
{
	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "title")==0)
			{	// sacar sus valores
				(*xl)++;
			}
			else if( strcmp((const char *)node->name, "code")==0)
			{
				BalanceDatos.profile[*xl] = atoi((char *)xmlNodeGetContent(node));
			}
			else if(strcmp((const char *)node->name,"max-balance") == 0)
			{
				BalanceDatos.max_balance[*xl] = atoi((char *)xmlNodeGetContent(node));
			}
			else if(strcmp((const char *)node->name,"expiration") == 0)
			{
				BalanceDatos.expiration[*xl] = atoi((char *)xmlNodeGetContent(node));
				BalanceDatos.tot_reg = (*xl) + 1;
			}
        }
        xmlBalance(node->children, indent_len + 1, xl);
		node = node->next;
	}
}



/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlBalance(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;
	int xlin;

	char filename[100];
	snprintf(filename,100,"%sconfig/balance_scheme.tbs",mert_operacion.path);

	memset( &BalanceXmlHdr, 0, sizeof(BalanceXmlHdr) );
	memset( &BalanceDatos,  0, sizeof(BalanceDatos) );


	node = GetXmlHeaderTDE(docXml, filename, &BalanceXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xlin=-1;
	xmlBalance(node, 1, &xlin);

	int i;
	printf("BALANCE\n");
	printf("Fecha de Creacion=%s\n", dtime(BalanceXmlHdr.time_creacion));
	
	printf("Header=[%s] Version=%d\n", BalanceXmlHdr.tipo, BalanceXmlHdr.version);
	printf("Total de Registros=%d\n", BalanceDatos.tot_reg);
	for(i=0; i<BalanceDatos.tot_reg; i++)
		printf("Balance[i=%2d][%6d][%6d][%6d]\n",i, BalanceDatos.profile[i],
						BalanceDatos.max_balance[i],BalanceDatos.expiration[i]);
	xmlFreeDoc(docXml);
	xmlCleanupParser();
	
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void xmlBonus(xmlNode * node, int indent_len, int *xl, int *xc)
{	char aux[40];
	xmlAttr *prop;
	xmlChar *value;

	while(node)
	{	if(node->type == XML_ELEMENT_NODE)
		{
			if( strcmp((const char *)node->name, "balance-list")==0)
			{	// sacar sus valores
				(*xl)++;
				*xc = 0;
//				printf("balance list\n");
				prop = node->properties;
				while (prop!=NULL)
				{
					value=xmlGetProp(node, prop->name);
					if(value != NULL)
					{
						if(strcmp((const char *)prop->name,"code") == 0)
						{	sprintf(aux,"%s",value);
							BonosName.code[*xl] = atoi(aux);
						}
						else if(strcmp((const char *)prop->name,"name") == 0)
						{	
							strcpy(BonosName.name[*xl], (const char *)value);
							BonosName.tot_reg = (*xl) + 1;
						}
					}
					prop=prop->next;
				}
			}
			else if( strcmp((const char *)node->name, "balance")==0)
			{	// sacar sus valores
				prop = node->properties;
				while (prop!=NULL)
				{
					value=xmlGetProp(node, prop->name);
					if(value != NULL)
					{
						if(strcmp((const char *)prop->name,"code") == 0)
						{	sprintf(aux,"%s",value);
							BonosRango[*xl].code[*xc] = atoi(aux);
						}
						else if(strcmp((const char *)prop->name,"gift-value") == 0)
						{	sprintf(aux,"%s",value);
							BonosRango[*xl].bono[*xc] = atoi(aux);
						}
						else if(strcmp((const char *)prop->name,"name") == 0)
						{	
							strcpy(BonosRango[*xl].name[*xc], (const char *)value);
						}
						else if(strcmp((const char *)prop->name,"value") == 0)
						{	sprintf(aux,"%s",value);
							BonosRango[*xl].value[*xc] = atoi(aux);
							BonosRango[*xl].tot_reg = (*xc)+1;
							(*xc)++;
						}
						prop=prop->next;
					}				
				}
			}else if( strcmp((const char *)node->name, "title-list")==0)
			{	// sacar sus valores
				(*xl)=0;
				printf("title list\n");
			}
			else if( strcmp((const char *)node->name, "type-title")==0)
			{	// sacar sus valores
				prop = node->properties;
				while (prop!=NULL)
				{
					value=xmlGetProp(node, prop->name);
					if(value != NULL)
					{
						if(strcmp((const char *)prop->name,"balance-list-code") == 0)
						{	sprintf(aux,"%s",value);
							BonosProfile.code[*xl] = atoi(aux);
						}
						else if(strcmp((const char *)prop->name,"code") == 0)
						{	sprintf(aux,"%s",value);
							BonosProfile.profile[*xl] = atoi(aux);
							(*xl)++;
							 BonosProfile.tot_reg = *xl;
						}
						prop=prop->next;
					}				
				}
			}			
        }
        xmlBonus(node->children, indent_len + 1, xl, xc);
		node = node->next;
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rdXmlBonus(void)
{	xmlDoc *docXml = NULL;
	xmlNode *node;
	int xlin, xcol;

	char fname[100];
	snprintf(fname,100,"%sconfig/bond_scheme.bs",mert_operacion.path);

	memset( &BonosXmlHdr, 0, sizeof(BonosXmlHdr)  );
	memset( &BonosRango,   0, sizeof(BonosRango)   );
	memset( &BonosProfile,0, sizeof(BonosProfile) );
	memset( &BonosName,    0, sizeof(BonosName)    );

	node = GetXmlHeaderTDE(docXml, fname, &BonosXmlHdr);
	if( node==NULL)
	{
		printf("Error XML\n");
		return 0x01;	// error
	}

	xlin=-1; xcol=0;
	xmlBonus(node, 1, &xlin, &xcol);
			
	int i,j;
	printf("BONOS\n");
	printf("Fecha de Creacion=%s\n", dtime(BonosXmlHdr.time_creacion));

	printf("Header=[%s] Version=%d\n", BonosXmlHdr.tipo, BonosXmlHdr.version);
	
	printf("Total de Registros=%3d\n", BonosName.tot_reg );
	for(i=0; i<5;i++)
	{	printf("[i=%2d]ID=%02d->%s\n", i, BonosName.code[i], BonosName.name[i]);
		printf("Total de Registros=%d\n", BonosRango[i].tot_reg);
		for(j=0;j<20;j++)
			printf("Bono=[i=%2d][%2d][%-20s][%6d][%6d]\n",i, 
					BonosRango[i].code[j],BonosRango[i].name[j],
					BonosRango[i].value[j],BonosRango[i].bono[j]);
	}
	printf("Perfiles\n");
	printf("Total de Registros=%d\n", BonosProfile.tot_reg);
	for(i=0; i<BonosProfile.tot_reg; i++)
		printf("Perfil=[i=%2d][%4d][%4d]\n", i, BonosProfile.code[i], BonosProfile.profile[i]);

	xmlFreeDoc(docXml);
	xmlCleanupParser();

	return 0;
}

/******************************************************************************/
/* Lee encabezado XML formato TDE                                             */
/******************************************************************************/
xmlNode *GetXmlHeaderTDE(xmlDoc *docXML, char* path, XML_HDR_TDE *xmlhdr)
{	xmlChar *value;
	xmlAttr *prop;
	xmlNode *root;
	struct stat attrib;
//	struct tm ticket_time;

	char tmptxt[40], aux[40];

	if(access(path, F_OK)==0)
	{
		stat(path, &attrib);
		xmlhdr->time_creacion = attrib.st_mtime;
	}

	
	docXML = xmlReadFile(path, NULL, 0);
	if (docXML == NULL)
	{
		printf("Could not parse the XML file");
		return NULL;
	}
	root = xmlDocGetRootElement(docXML);

	value=xmlNodeGetContent(root);
	if (root->properties!=NULL)
	{	prop=root->properties;
		while (prop!=NULL)
		{	value=xmlGetProp(root, prop->name);
			if (value != NULL)
			{
				sprintf(tmptxt,"%s",prop->name);
				if(strcmp("tipo",tmptxt) == 0)
				{
					sprintf(xmlhdr->tipo,"%s", value);
				}
				else if(strcmp("version",tmptxt) == 0)
				{
					sprintf(aux,"%s", value);
					xmlhdr->version=atoi(aux);					
				}
			}
			prop=prop->next;
		}
	}
	return root->children;
}

/******************************************************************************/
/*  carga todos los xml                                                       */
/******************************************************************************/
void ldXMLs(void)
{
	rdXmlBonus();
	rdXmlMenus();
	rdXmlBalance();
	rdXmlUsers();
	rdXmlLineas();
	rdXmlBLMIAs();
	rdXmlBLSAMs();
	rdXmlMertID();
	rdXmlMertLmt();
	rdXmlReaders();
	rdXmlBank();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void UpdateConfig(void)
{	FILE *fb;
	struct stat attrib;

	char dxml[2048] = {};
	char ln[70], fname[100];

	strcpy (dxml, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n<archivo tipo=\"mert\" version=\"1\">\n");

	sprintf (ln, "  <dispensador>%d</dispensador>\n", Config.Dispensador);
	strcat(dxml, ln);
	sprintf (ln, "  <billetero>%d</billetero>\n", Config.Billetero);
	strcat(dxml, ln);
	sprintf (ln, "  <monedero>%d</monedero>\n", Config.Monedero);
	strcat(dxml, ln);
	sprintf (ln, "  <stacker>%d</stacker>\n", Config.Stacker);
	strcat(dxml, ln);
	sprintf (ln, "  <NoMov>%d</NoMov>\n", Config.NoMov);
	strcat(dxml, ln);
	sprintf (ln, "  <binbox>%d</binbox>\n", Config.Captura);
	strcat(dxml, ln);
	sprintf (ln, "  <binboxmax>%d</binboxmax>\n", Config.BinBoxMax);
	strcat(dxml, ln);
	sprintf (ln, "  <NoMovEvent>%d</NoMovEvent>\n", Config.NoMovEvent);
	strcat(dxml, ln);
	strcat (dxml, "</archivo>\n");

//	snprintf(fname,100,"%sconfig/mert.cfg", mert_operacion.path);
	snprintf(fname,100,"%sconfig/mert001.cfg",mert_operacion.path);
	if( (fb=fopen(fname,"w")) != NULL)
	{
		fprintf ( fb,"%s", dxml);
		fclose(fb);
	}
	else
	{	// error al crear archivo del Dispensador
		sprintf(dxml,"Error al crear archivo del Dispensador [%s]", fname);
		printf("%s\n",dxml);
	}

	if(access(fname, F_OK)==0)
	{
		stat(fname, &attrib);
		ConfigXmlHdr.time_creacion = attrib.st_mtime;
	}

	return ;
}

/******************************************************************************/
/* Carga Config XML                                                           */
/******************************************************************************/
int ReadConfigXML(void)
{	char fname[100];

	snprintf(fname,100,"%sconfig/mert001.cfg",mert_operacion.path);

	// Default
	Config.Dispensador	= 1;
	Config.Monedero		= 1;
	Config.Billetero	= 1;
	Config.Stacker		= 1; /* Stacker 1 */
	Config.NoMov		= 1;
	Config.Captura		= 0;
	Config.BinBoxMax	= 25;
	Config.NoMovEvent	= 0;

	if(access(fname, F_OK)!=0)
	{	// no existe, lo crea, valores x default;
		Config.NoMovEvent	= 1;
		printf("\n*******NO EXISTE CONFIG ************\n");
		UpdateConfig();
	}
	//printf("Existe XML, lo carga \n");
	rdXmlConfig(fname);
	if(	Config.NoMovEvent == 0)
	{
		Config.NoMovEvent	= 1;
		printf("No existe el campo\n");
		UpdateConfig();
	}
	return 0;
}

/******************************************************************************/
/* revisa si hubo cambios en config.xml                                       */
/******************************************************************************/
/*int revXmlBonus(void)
{	static char tmstart=0;
	static time_t init_time;
	struct stat attrib;

	char fname[100];
	snprintf(fname,100,"%sconfig/bond_scheme.bs",mert_operacion.path);

	if(access(fname, F_OK)==0)
	{
		stat(fname, &attrib);

		if(tmstart==0)
		{	if( rdXmlBonus( fname ) != 0)
			{	printf("1-Error en INITVAL\n");
				return 0x0400;
			}
			tmstart = 1;
			init_time = attrib.st_mtime;
			return 0x00;	// OK
		}

		if( init_time != attrib.st_mtime)
		{	printf("diferente tiempo\n");
			// leer archivo
			if( rdXmlBonus( fname ) != 0)
			{	printf("2-Error en INITVAL\n");
				return 0x0400;
			}
			init_time = attrib.st_mtime;
			return 0x00;	// OK
		}
	}
//	printf("No existe initval.xml\n");
	return 0x0401;
}*/

//
//	********** Crea los XML de las operaciones de la MERT  *********************
//
/******************************************************************************/
/* Calcula la cantidad y el peso de las monedas depositadas en la caja        */
/******************************************************************************/
void CalculaPesoTotalParcial(int *tmonedas, int *pmonedas)
{	int i;

	*pmonedas = *tmonedas = 0;
	for(i=0;i<TOTAL_DENOM_BM; i++)
	{	if(detMonedas.Valor[i] != 0)
		{	*tmonedas += detMonedas.Total[i];
			*pmonedas += pmoneda(detMonedas.Valor[i], detMonedas.Total[i]);
		}
	}
}

/******************************************************************************/
/* Calcula la cantidad de billetes en el stacker                              */
/******************************************************************************/
void CalculaBilletesParcial(int *tbilletes)
{	int i;

	*tbilletes = 0;
	for(i=0;i<TOTAL_DENOM_BM; i++)
	{
		if(detBilletes.Valor[i] != 0)
			*tbilletes += detBilletes.Total[i];
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void xmlCorteParcial( void )
{	FILE *fb;
	char dxml[2048] = {};
	char date[30];
	char ln[70], fname[100];
	int tcoin, tpeso, tbill, total;

	CalculaPesoTotalParcial(&tcoin, &tpeso);
	CalculaBilletesParcial(&tbill);
	total = detMonedas.CantidadTotal + detBilletes.CantidadTotal;
	datetime(date);

	strcpy (dxml,	"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>\n"
					"<archivo tipo=\"parcial\">\n");

	sprintf (ln, "  <cmonedas>%d</cmonedas>\n", tcoin);
	strcat(dxml, ln);
	sprintf (ln, "  <tmonedas>%.2f</tmonedas>\n", (double)detMonedas.CantidadTotal/100);
	strcat(dxml, ln);
	sprintf (ln, "  <pmonedas>%.3f</pmonedas>\n", (double)tpeso/100000);
	strcat(dxml, ln);	
	sprintf (ln, "  <cbilletes>%d</cbilletes>\n", tbill);
	strcat(dxml, ln);
	sprintf (ln, "  <tbilletes>%.2f</tbilletes>\n", (double)detBilletes.CantidadTotal/100);
	strcat(dxml, ln);
	sprintf (ln, "  <total>%.2f</total>\n", (double)total/100);
	strcat(dxml, ln);
	sprintf (ln, "  <Stacker1>%d</Stacker1>\n", detStackers[0].TotalTarjetas);
	strcat(dxml, ln);
	sprintf (ln, "  <Stacker2>%d</Stacker2>\n", detStackers[1].TotalTarjetas);
	strcat(dxml, ln);
	sprintf (ln, "  <binbox>%d</binbox>\n", Config.Captura);
	strcat(dxml, ln);
	sprintf (ln, "  <ultimomovimiento>%s</ultimomovimiento>\n", date);
	strcat(dxml, ln);

	
	strcat (dxml, "</archivo>\n");
	snprintf(fname,100,"%stransacciones/parcial.dat",mert_operacion.path);

//	printf("[bytes=%d]\n%s\n", (int)strlen(dxml), fname);
//	printf("[%s]\n", dxml);

	if( (fb=fopen(fname,"w")) != NULL)
	{
		fprintf ( fb,"%s", dxml);
		fclose(fb);
	}
	else
	{
		// error al crear archivo de transaccion
		printf("Error al crear archivo de transaccion [%s]\n", fname);
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void xmlTransaccion(DATA_TRANS dtrans)
{	FILE *fb;
	char dxml[2048] = {};
	char ln[70], fname[100];
	int i;

	strcpy (dxml,	"<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n"
					"<archivo tipo=\"trans\" version=\"2\">\n");

	sprintf (ln, "  <c1>%d</c1>\n", detTransaccion.NoMov);
	strcat(dxml, ln);
	sprintf (ln, "  <c2>%s</c2>\n", detTransaccion.fecha);
	strcat(dxml, ln);
	sprintf (ln, "  <c3>%s</c3>\n", detTransaccion.ID_Name);
	strcat(dxml, ln);
	sprintf (ln, "  <c4>%d</c4>\n", detTransaccion.Linea_TDE);
	strcat(dxml, ln);
	sprintf (ln, "  <c5>%d</c5>\n", detTransaccion.Estacion_TDE);
	strcat(dxml, ln);
	sprintf (ln, "  <c6>%d</c6>\n", detTransaccion.Linea_TVT);
	strcat(dxml, ln);
	sprintf (ln, "  <c7>%d</c7>\n", detTransaccion.Estacion_TVT);
	strcat(dxml, ln);
	sprintf (ln, "  <c8>%d</c8>\n", detTransaccion.NoDisp);
	strcat(dxml, ln);
	sprintf (ln, "  <c9>%d</c9>\n", detTransaccion.Status);
	strcat(dxml, ln);
	sprintf (ln, "  <c10>%s</c10>\n", detTransaccion.UID_SAM);
	strcat(dxml, ln);
	sprintf (ln, "  <c11>%s</c11>\n", detTransaccion.UID_MIA);
	strcat(dxml, ln);
	sprintf (ln, "  <c12>%s</c12>\n", detTransaccion.NoLogico);
	strcat(dxml, ln);
	sprintf (ln, "  <c13>%.2f</c13>\n", (double)detTransaccion.SaldoInicial/100);
	strcat(dxml, ln);
	sprintf (ln, "  <c14>%.2f</c14>\n", (double)detTransaccion.Deposito/100);
	strcat(dxml, ln);
	sprintf (ln, "  <c15>%.2f</c15>\n", (double)detTransaccion.SaldoFinal/100);
	strcat(dxml, ln);
	sprintf (ln, "  <c16>%.2f</c16>\n", (double)detTransaccion.BonoInicial/100);
	strcat(dxml, ln);
	sprintf (ln, "  <c17>%.2f</c17>\n", (double)detTransaccion.BonoDepositado/100);
	strcat(dxml, ln);
	sprintf (ln, "  <c18>%.2f</c18>\n", (double)detTransaccion.BonoFinal/100);
	strcat(dxml, ln);
	sprintf (ln, "  <c19>%d</c19>\n", detTransaccion.VentaRecarga);
	strcat(dxml, ln);
	sprintf (ln, "  <c20>%.2f</c20>\n", (double)detTransaccion.ValorMIA/100);
	strcat(dxml, ln);
	sprintf (ln, "  <c21>%u</c21>\n", detTransaccion.NoCajaBilletes);
	strcat(dxml, ln);
	sprintf (ln, "  <c22>%u</c22>\n", detTransaccion.NoCajaMonedas);
	strcat(dxml, ln);
	strcat (dxml, "</archivo>\n");

//	printf("[bytes=%d]\n", (int)strlen(dxml));
//	printf("[%s]\n", dxml);

	snprintf(fname,100,"%stransacciones/",mert_operacion.path);

	snprintf(ln,70, "%02d%02d%03d%04d%08d.tra", MertLmt.type, MertID.lineaTDE,
												MertID.estacionTDE, MertID.NumDisp,
												detTransaccion.NoMov);
 	strcat(fname,ln);
	if( (fb=fopen(fname,"w")) != NULL)
	{	
		fprintf ( fb,"%s", dxml);
		fclose(fb);
	}
	else
	{
		// error al crear archivo de transaccion
		sprintf(dxml,"Error al crear archivo de transaccion [%s]", fname);
		printf("%s\n",dxml);
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void xmlStacker(int nstk, DET_STACKERS detalle)
{	FILE *fb;
	char dxml[2048] = {};
	char ln[70], fname[100];
	int i;

	strcpy (dxml, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n<archivo tipo=\"dispensador\">\n");

	sprintf (ln, "  <c1>%d</c1>\n", nstk+1);
	strcat(dxml, ln);
	sprintf (ln, "  <c2>%d</c2>\n", detalle.NoMov);
	strcat(dxml, ln);
	sprintf (ln, "  <c3>%s</c3>\n", detalle.Fecha);
	strcat(dxml, ln);
	sprintf (ln, "  <c4>%d</c4>\n", detalle.TipoOperacion);
	strcat(dxml, ln);
	sprintf (ln, "  <c5>%d</c5>\n", detalle.TotalOperacion);
	strcat(dxml, ln);
	sprintf (ln, "  <c6>%d</c6>\n", detalle.TotalTarjetas);
	strcat(dxml, ln);
	sprintf (ln, "  <c7>%d</c7>\n", Config.Captura);
	strcat(dxml, ln);
	sprintf (ln, "  <c8>%u</c8>\n", detalle.NoEmpleado);
	strcat(dxml, ln);
	sprintf (ln, "  <c9>%s</c9>\n", detalle.UID_MIA);
	strcat(dxml, ln);
	strcat (dxml, "</archivo>\n");

	snprintf(fname,100,"%stransacciones/%d%02d%02d%03d%04d%08d.stk",
									mert_operacion.path,
									nstk,
									MertLmt.type, 
									MertID.lineaTDE,
									MertID.estacionTDE, 
									MertID.NumDisp,
									detalle.NoMov);

	if( (fb=fopen(fname,"w")) != NULL)
	{
		fprintf ( fb,"%s", dxml);
		fclose(fb);
	}
	else
	{	// error al crear archivo de transaccion
		sprintf(dxml,"Error al crear archivo del Dispensador [%s]", fname);
		printf("%s\n",dxml);
	}
	return ;
}
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void xmlCaptura(void)
{	FILE *fb;
	char dxml[2048] = {};
	char ln[70], fname[100], fecha[30];

	datetime(fecha);

	
	strcpy (dxml, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n<archivo tipo=\"dispensador\">\n");

	sprintf (ln, "  <NoMov>%d</NoMov>\n", Config.NoMov);
	strcat(dxml, ln);
	sprintf (ln, "  <Fecha>%s</Fecha>\n", fecha);
	strcat(dxml, ln);
	sprintf (ln, "  <Tarjetas>%d</Tarjetas>\n", Config.Captura);
	strcat(dxml, ln);

	sprintf (ln, "  <NoEmp>%d</NoEmp>\n", data_user[NOUSER_1].noempleado);
	strcat(dxml, ln);
	sprintf (ln, "  <NoEmp2>%d</NoEmp2>\n", data_user[NOUSER_2].noempleado);
	strcat(dxml, ln);

	strcat (dxml, "</archivo>\n");

	snprintf(fname,100,"%stransacciones/%s%08d.cap",
									mert_operacion.path, 
									MertID.code,
									Config.NoMov);

	if( (fb=fopen(fname,"w")) != NULL)
	{
		fprintf ( fb,"%s", dxml);
		fclose(fb);
	}
	else
	{	// error al crear archivo de transaccion
		sprintf(dxml,"Error al crear archivo de Bandeja de Captura [%s]", fname);
		printf("%s\n",dxml);
	}
	return ;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void creaXmlEventos(int nemp, int noevento,  int d1, int d2)
{	FILE *fb;
	char dxml[2048] = {};
	char ln[70], fname[100], fecha[30];

	datetime(fecha);

	strcpy (dxml, "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?>\n<archivo tipo=\"eventos\" version=\"1\">\n");

	sprintf (ln, "  <NoMov>%d</NoMov>\n", Config.NoMovEvent);
	strcat(dxml, ln);
	sprintf (ln, "  <Fecha>%s</Fecha>\n", fecha);
	strcat(dxml, ln);
	sprintf (ln, "  <NoEmp>%d</NoEmp>\n", nemp);
	strcat(dxml, ln);	
	sprintf (ln, "  <NoEvento>%d</NoEvento>\n", noevento);
	strcat(dxml, ln);
	sprintf (ln, "  <Dato1>%08X</Dato1>\n", d1);
	strcat(dxml, ln);
	sprintf (ln, "  <Dato2>%08X</Dato2>\n", d2);
	strcat(dxml, ln);
	sprintf (ln, "  <Descripcion>%s</Descripcion>\n", descEventos[noevento]);
	strcat(dxml, ln);
	strcat (dxml, "</archivo>\n");

	snprintf(fname,100,"%stransacciones/%08d.evt",mert_operacion.path, Config.NoMovEvent);
	if( (fb=fopen(fname,"w")) != NULL)
	{
		fprintf ( fb,"%s", dxml);
		fclose(fb);
	}
	else
	{	// error al crear archivo del Dispensador
		sprintf(dxml,"Error al crear archivo del Dispensador [%s]", fname);
		printf("%s\n",dxml);
	}

	Config.NoMovEvent++;
	UpdateConfig();

	return ;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void EventosMert(char msg[])
{
	char fname[] ={"/home/user/mert/mert_events.txt"};
	char date[100], da[30];

	FILE *fptr=NULL; 
	fptr = fopen(fname, "a");

	datetime(da);
	sprintf(date,"%s -> %s\n", da, msg);
	fprintf ( fptr,"%s", date);
	fclose(fptr);	
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void EventBox(char lbl[], unsigned int  idBD, unsigned int  idRd )
{	char msg[100];

	sprintf(msg,"Caja de %s, NO COINCIDE BD=%08X, RD=%08X", lbl, idBD, idRd);
	EventosMert(msg);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void EventCntBill(void)
{	char fname[] ={"/home/user/mert/bill_events.txt"};
	char date[100], da[30];
	int k;

	if( WRITE_EVENTS == 0)
		return;

	FILE *fptr=NULL;
	fptr = fopen(fname, "a");

	datetime(da);
	sprintf(date,"%s -> [$ 20=%3d] [$ 50=%3d] [$100=%3d] [$200=%3d]\n", da,
				detBilletes.Total[2], detBilletes.Total[3], 
				detBilletes.Total[4], detBilletes.Total[5]);
	fprintf ( fptr,"%s", date);

	fclose(fptr);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void EventDetBill(cctalk bills)
{	char fname[] ={"/home/user/mert/bill_events.txt"};
	char date[100], da[30];
	int k;

	if( WRITE_EVENTS == 0)
		return;
	
	FILE *fptr=NULL;
	fptr = fopen(fname, "a");

	datetime(da);
	sprintf(date,"%s -> [$ 20=%3d] [$ 50=%3d] [$100=%3d] [$200=%3d]\n", da,
				bills.depositado.counter[2], bills.depositado.counter[3], 
				bills.depositado.counter[4], bills.depositado.counter[5]);
	fprintf ( fptr,"%s", date);

	fclose(fptr);
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void EventBill(char msg[])
{	char fname[] ={"/home/user/mert/bill_events.txt"};
	char date[100], da[30];

	
	if( WRITE_EVENTS == 0)
		return;

	
	FILE *fptr=NULL; 
	fptr = fopen(fname, "a");

	datetime(da);
	sprintf(date,"%s -> %s\n", da, msg);
	fprintf ( fptr,"%s", date);

	fclose(fptr);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void EventCoin(char msg[])
{	char fname[] ={"/home/user/mert/coin_events.txt"};
	char date[100], da[30];
	int k;

	if( WRITE_EVENTS == 0)
		return;

	FILE *fptr=NULL; 
	fptr = fopen(fname, "a");

	datetime(da);
	sprintf(date,"%s -> %s\n", da, msg);
	fprintf ( fptr,"%s", date);

	fclose(fptr);
}
////////////////////////////////////////////////////////////////////////////////