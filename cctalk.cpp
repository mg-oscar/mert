/******************************************************************************/
/*                  Implementacion protocolo CC-TALK                          */
/******************************************************************************/

#include "cctalk.h"
#include "serialport.h"


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
cctalk::cctalk( )
{
	status = 0;
	error = 0xFF;
	fg_master_inhibit = 0;
//	bill_reset = 0;
	num_intentos = 0;

	TotalDenominaciones=0;
	memset(EnableChannel, 0x00, sizeof(EnableChannel));

	ClearDepositado();
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int cctalk::calc_cvf( char cvf[], int fac)
{	char cvfs[4] = {'\0', '\0', '\0', '\0'};
	int factor;
	unsigned char tp;

	if(strlen(cvf)!=0x03)
		return 0.00;

	// numericos
	if( isdigit(cvf[0]) && isdigit(cvf[1]) && isdigit(cvf[2]) )
		return atof(cvf) * fac;

	// revisar formato valido, total de caracteres alfanumericos
	tp = 0 ;
	if(isdigit(cvf[0])==0) tp++;
	if(isdigit(cvf[1])==0) tp++;
	if(isdigit(cvf[2])==0) tp++;
	if(tp>1)
		return 0.0;		// Error en formato, se permite solo 1 caracter Alfanumerico [. m M K G]

	if( (cvf[0]=='.') || (cvf[1]=='.') || (cvf[2]=='.') )
		return atof(cvf) * 100;

	// ALFANUMERICOS
	factor = 0.0;
	strcpy(cvfs, cvf);
	if( isdigit(cvfs[0])==0)
	{	if(cvfs[0]=='M')	
			factor = 1000000;
		else if(cvfs[0]=='G')
			factor = 1000000000;
		cvfs[0] = '.';
	}
	else if( isdigit(cvfs[1])==0)
	{	if(cvfs[1]=='m')	
			factor = .001;
		else if(cvfs[1]=='K')
			factor = 1000;
		else if(cvfs[1]=='M')
			factor = 1000000;
		cvfs[1] = '.';
	} else if( isdigit(cvfs[2])==0)
	{	if(cvfs[2]=='m')	
			factor = .001;
		else if(cvf[2]=='K')
			factor = 1000;
		else if(cvf[2]=='M')
			factor = 1000000;
		cvfs[2] = '.';
	}

	return atoi(cvfs) * factor;
}

/******************************************************************************/
/*  Decodifica los datos de la respuesta con CHECKSUM                         */
/******************************************************************************/
char cctalk::analizaRx( unsigned char *tx, char txlen)
{	char op;
	unsigned char ap, td, i,k, chk,tchk;

	op = 0;
	for(i=0; i< cctalk_apuntador;)
	{
		switch(op)
		{	case 0:	// eco
					for(k=0; k<txlen;k++)
					{	if(cctalk_buffer[k] == tx[k])
						{	
							//printf("igual %d\n",k);
						}
						else
						{	// eliminar un byte
							break;
						}
					}
					op = 1;
					i += txlen-1;
//					printf("eco ok %d\n",i);
					break;
			case 1:	// master address
					if(cctalk_buffer[i] == MASTER_ADDRESS)
					{	op=2;
//						printf("address 1 %d\n",i);
						chk = cctalk_buffer[i];
					}
					break;
			case 2:	//totl de datos a recibir
					td = cctalk_buffer[i];
//					printf("tot dat %d %d\n",td,i);
					chk += cctalk_buffer[i];
					op = 3;
					break;
			case 3:	// slave address
					if(cctalk_buffer[i] == cctalk_slave_address)
					{	op=4;
						chk += cctalk_buffer[i];
					}
					break;
			case 4:	// dummy
					op=5;
					chk += cctalk_buffer[i];
					tchk = 0;
					if(td==0)	// no hay datos
					{	
						op = 6;
					}
					break;
			case 5:	// datos de la respuesta
					chk = chk + cctalk_buffer[i];
					tchk++;
					if(tchk==td)
					{	
						op = 6;
					}
					break;
			case 6:	// checksum
					chk= 0 - chk;
					if(cctalk_buffer[i] == chk)
					{
						return td+1;
					}
					break;
		}
		i++;
	}
	op = 0;
	return FALSE;
}

/******************************************************************************/
/*  Decodifica los datos de la respuesta con CRC CCITT                        */
/******************************************************************************/
char cctalk::analizaRxCrc(unsigned char *tx, char txlen)
{	char op;
	unsigned char td, i,k;
	unsigned short crc;

	if(cctalk_echo==CCTALK_CON_ECO)
		op = 0;
	else
		op = 1;

	for(i=0; i< cctalk_apuntador;)
	{
		switch(op)
		{	case 0:
					for(k=0; k<txlen;k++)
					{	if(cctalk_buffer[k] != tx[k])
						{	// error en ECO, eliminar un byte
							break;
						}
					}
					op = 1;
					i += txlen-1;
					break;

			case 1:	// master address
					if(cctalk_buffer[i] == MASTER_ADDRESS)
						op=2;
					else
					{	
						// elimina primer dato
					}
					break;
			case 2:	//total de datos a recibir
					td = cctalk_buffer[i];	// total de datos a recibir
					td += 3;	 // 00 y CRC
					op = 3;
					break;
			case 3:	// resto de los datos
					td--;
					if(td==0)
					{
						crc = crc_CCITT_A( i-txlen, &cctalk_buffer[txlen], 0x000 );
						if( ((crc & 0xFF) == cctalk_buffer[txlen+2]) && ( ((crc>>8)&0xFF) == cctalk_buffer[i]) )
						{
							for(k=0; k<txlen; k++)
								cctalk_buffer[k] = cctalk_buffer[k+txlen];
							return cctalk_buffer[1]+1;		// total de datos recibidos

						}
//						else
//							;
					}
					break;
		}
		i++;
	}
	op = 0;
	return FALSE;
}

/******************************************************************************/
/*           trasmite respetando un tiempo minimo de barrido                  */
/******************************************************************************/
void cctalk::tx_cctalk(unsigned char tx[], int len)
{	static int tmcctalk = milisegundos();

//	while( (milisegundos()-tmcctalk) < 50) ;
//	tmcctalk = milisegundos();
	write(fd, tx, len);	//TRIPODE_LEN_CMD);
}

/******************************************************************************/
/*  Envia comando x cctalk                                                    */
/******************************************************************************/
int cctalk::SendCmdCCTALK(unsigned char tx[], int len, int timeout)
{	char chk,i;
	unsigned char da;
	int	tpdTimeOut;
	unsigned short crc;

	if(cctalk_verifica == CCTALK_CHK)
	{
		chk = checksum( len, tx, 0x00 );
		tx[len] =  chk;
		len++;
	}
	else if(cctalk_verifica == CCTALK_CRC)
	{	crc = crc_CCITT_A( len, tx, 0x000 );
		tx[0x02] =  crc & 0xFF;;	
		tx[len++] =  crc>>8;;
	}
	else return  0x00;		// ERROR EN VERIFICACION

	clear_rx();	// borra datos pendientes de leer
	apIniBufRx = apFinBufRx = cctalk_apuntador = 0;

	memset( cctalk_buffer, 0x00, sizeof(cctalk_buffer) );
	tx_cctalk(tx, len);

	tpdTimeOut = milisegundos();
	while( (milisegundos()- tpdTimeOut)  < timeout)
	{
		ReadComm ();
		if(apIniBufRx != apFinBufRx)
		{	da = getBufferRx();

			cctalk_buffer[cctalk_apuntador++]= da;

			if(cctalk_verifica == CCTALK_CHK)
				i = analizaRx(tx, len);
			else
				i = analizaRxCrc(tx, len);
			if(i != FALSE)
			{
				return i;
			}
		}
	}
	return FALSE;
}

/******************************************************************************/
/*                    Obtiene Coin ID                                         */
/******************************************************************************/
void cctalk::obtiene_coinid(int re)
{	int x;

	memset(cc_talk_cvf, '\0', sizeof(cc_talk_cvf) );

	x = (cctalk_echo==CCTALK_CON_ECO)?10:4;

	if(re==6)	// total de datos en respuesta de CoinID
	{
		if( (cctalk_buffer[x]=='M') && (cctalk_buffer[x+1]=='X') )
			memcpy(cc_talk_cvf, &cctalk_buffer[x+2], 3);
	}
}

/******************************************************************************/
/*                    Obtiene Bill ID                                         */
/******************************************************************************/
void cctalk::obtiene_billid(int re)
{	int i,x;

	memset(cc_talk_cvf, '\0', sizeof(cc_talk_cvf) );

	x = (cctalk_echo==CCTALK_CON_ECO)?10:4;
	if(re==7)	// long respuesta bill ID
	{
		if( (cctalk_buffer[x]=='M') && (cctalk_buffer[x+1]=='X') )
			memcpy(cc_talk_cvf, &cctalk_buffer[x+2], 4);
	}
}

/******************************************************************************/
/*                      Obtiene Polling Priority                              */
/******************************************************************************/
int cctalk::obtiene_pp(int re)
{	int x;

	x = (cctalk_echo== CCTALK_CON_ECO)?9:4;

	switch(cctalk_buffer[x])
	{
		case 1:	// milisegundos
			return cctalk_buffer[x+1];
			break;
		case 2:	// milisegundos*10
			return cctalk_buffer[x+1]*10;
			break;
		case 3:	// segundos
			return cctalk_buffer[x+1]*1000;
			break;
	}
	return 500;
}

/******************************************************************************/
/*                      Obtiene Inhibit Status, total de Canales              */
/******************************************************************************/
int cctalk::obtiene_is(int re)
{	unsigned char x,k;

	TotalDenominaciones=0;
	memset(EnableChannel, 0x00, sizeof(EnableChannel));

	x = (cctalk_echo== CCTALK_CON_ECO)?9:4;

	
	for(k=0; k<re; k++ )
		EnableChannel[k] = cctalk_buffer[x+k];

	TotalDenominaciones = re *8;

	return 0;
}

/******************************************************************************/
/*                      Obtiene Buffer Credits & Error                        */
/******************************************************************************/
void cctalk::obtiene_rbce(int re)
{	int k, x;

	x = (cctalk_echo == CCTALK_CON_ECO)?9:4;

	cctalkevents.events = cctalk_buffer[x];

	for(k=0; k<10;k++)
		cctalkevents.datos[k] = cctalk_buffer[x+1+k];
}

/******************************************************************************/
/*  Obtiene datos texto                                                       */
/******************************************************************************/
void cctalk::ObtieneDatosStr(int re)
{	int x;

	memset(cctalk_datos, '\0', sizeof(cctalk_datos) );

	x = (cctalk_echo== CCTALK_CON_ECO)?9:4;
	if(re!=0)
		memcpy(cctalk_datos, &cctalk_buffer[x], re-1);

	return;
}

/******************************************************************************/
/* ontiene datos numericos                                                    */
/******************************************************************************/
long cctalk::ObtieneDatosNum(int re)
{	int x;
	x = (cctalk_echo== CCTALK_CON_ECO)?9:4;

	return  (long)(cctalk_buffer[x] + cctalk_buffer[x+1]*256 + cctalk_buffer[x+2]*65536);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned char cctalk::ObtieneDato1B(int re)
{	int x;

	x = (cctalk_echo== CCTALK_CON_ECO)?9:4;

	return  cctalk_buffer[x];
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned int cctalk::ObtieneDato2B(int re)
{	int x;

	x = (cctalk_echo== CCTALK_CON_ECO)?9:4;

	return  int(cctalk_buffer[x+1]*256) + cctalk_buffer[x];
}

/******************************************************************************/
/*                  Trasfiere los contadores al EEPROM                        */
/******************************************************************************/
int cctalk::CountersToEeprom(void)
{
	if( SendCmdGral(CCTALK_COUNTERS_TO_EEPROM) != FALSE)
	{
		// ACK
		return 0x00;
	} 
	return 0x100;
}

/******************************************************************************/
/* Request Coin ID  -> 1 al TOTAL_DENOMINACION                                                 */
/******************************************************************************/
int	cctalk::RequestCoinID(void)
{	int re,i;
	int cvf;
	unsigned char txcmd[20];
	int lcmd;

	memset(&depositado.valor, 0, sizeof(depositado.valor) ); 

	txcmd[0]	= cctalk_slave_address;	
	txcmd[1]	= 0x01;
	txcmd[2]	= MASTER_ADDRESS;
	txcmd[3]	= CCTALK_REQUEST_COIN_ID;
	txcmd[4]	= 0x01;
	lcmd		= 5; 

	// Request Coin ID
	printf("Request Coin ID\n");	
	for(i=0; i< TotalDenominaciones; i++)
	{	txcmd[4] = i+1;	// no de posicion

		if( (re=SendCmdCCTALK(txcmd, lcmd, TIMEOUT_CCTALK)) != FALSE)
		{	// obtiene CoindID
			obtiene_coinid(--re);
			if(strlen(cc_talk_cvf)>0)
			{
				printf("CVF<%02d=%s\n",i, cc_talk_cvf );
				strcpy( depositado.cvf[i], cc_talk_cvf);
			}
			else
			{
				strcpy( depositado.cvf[i], "000");
			}
			cvf = calc_cvf( cc_talk_cvf, cctalk_factor);
			if(cvf>COIN_MAYOR) cvf = 0;
			depositado.valor[i] = cvf;
 
		} else return 0x100;
	}

	printf("Denominacion Monedas\n");
	for(i=0;i<TotalDenominaciones;i++)
		printf("CVF<%02d=%s    ---    Pos=%2d= %d\n",i, 
									depositado.cvf[i], i, depositado.valor[i] );
	printf("\n");

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	cctalk::Divisor(unsigned char d)
{	int k,div=1;

	for(k=0; k<d; k++)
		div *= 10;
	return div;
}

/******************************************************************************/
/* Request Scaling/Factor                                                     */
/******************************************************************************/
int	cctalk::RequestScaling(void)
{	int re,i;
	int cvf, scala, div;		// para calcular valor real del billete
	unsigned char txcmd[20];
	int lcmd;

	// Requesr Scaling Factor
	txcmd[0]	= cctalk_slave_address;	
	txcmd[1]	= 0x02;
	txcmd[2]	= MASTER_ADDRESS;
	txcmd[3]	= CCTALK_REQUEST_SCALING;
	txcmd[4]	= 'M';
	txcmd[5]	= 'X';
	lcmd		= 6; 
	if( (re=SendCmdCCTALK(txcmd, lcmd, TIMEOUT_CCTALK)) != FALSE)
	{	// obtiene Scaling
		cctalk_scala = (int)cctalk_buffer[5]*256 +  cctalk_buffer[4];
		cctalk_divisor = Divisor( cctalk_buffer[6] ); 

		printf("scaling=[%d] divisor=[%d]\n", cctalk_scala, cctalk_divisor);

	} else return 0x100;
	return 0;
}


/******************************************************************************/
/* Request Bill ID  -> 1 al 8                                                 */
/******************************************************************************/
int	cctalk::RequestBillID(void)
{	int re,i;
	int cvf;		// para calcular valor real del billete
	unsigned char txcmd[20];
	int lcmd;

	memset(&depositado.valor, 0, sizeof(depositado.valor) ); 
	
	// Requesr Scaling Factor
	txcmd[0]	= cctalk_slave_address;	
	txcmd[1]	= 0x02;
	txcmd[2]	= MASTER_ADDRESS;
	txcmd[3]	= CCTALK_REQUEST_SCALING;
	txcmd[4]	= 'M';
	txcmd[5]	= 'X';
	lcmd		= 6; 
	if( (re=SendCmdCCTALK(txcmd, lcmd, TIMEOUT_CCTALK)) != FALSE)
	{	// obtiene Scaling
		cctalk_scala = (int)cctalk_buffer[5]*256 +  cctalk_buffer[4];
		cctalk_divisor = Divisor( cctalk_buffer[6] ); 

		printf("scaling=[%d] divisor=[%d]\n", cctalk_scala, cctalk_divisor);

	} else return 0x100;

	txcmd[0]	= cctalk_slave_address;	
	txcmd[1]	= 0x01;
	txcmd[2]	= MASTER_ADDRESS;
	txcmd[3]	= CCTALK_BILL_ID;
	lcmd		= 5; 

	// Request Bill ID
	for(i=0; i<TotalDenominaciones; i++)
	{
		txcmd[4] = i+1;	// no de posicion
		if( (re=SendCmdCCTALK(txcmd, lcmd, TIMEOUT_CCTALK)) != FALSE)
		{	// obtiene BillID
			obtiene_billid(--re);
			if(strlen(cc_talk_cvf)>0)
			{
				strcpy( depositado.cvf[i], cc_talk_cvf);
			}
			else
			{
				strcpy( depositado.cvf[i], "000");
			}

			cvf = atoi(cc_talk_cvf);
			depositado.valor[i] = (cvf * cctalk_scala / cctalk_divisor) * 100;
			if(depositado.valor[i]==1800)
				depositado.valor[i] = 0;
		} else return 0x100;
	}

//	printf("Denominacion Billetes\n");
//	for(i=0;i<TotalDenominaciones;i++)
//		printf("Pos=%2d= %d\n", i, depositado.valor[i] );
//	printf("\n");
	
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	cctalk::SendMasterInhibitStatus(unsigned char status)
{	int re,i;
	unsigned char txcmd[20];
	int lcmd;

	// Master Inhibit
	txcmd[0]	= cctalk_slave_address;	
	txcmd[1]	= 0x01;
	txcmd[2]	= MASTER_ADDRESS;
	txcmd[3]	= CCTALK_MODIFY_MASTER_INHIBIT_STATUS;
	txcmd[4]	= status;
	lcmd		= 5;

	if( (re=SendCmdCCTALK(txcmd, lcmd, TIMEOUT_CCTALK)) != FALSE)
		return 0x00;

	return 0x100;
}

/******************************************************************************/
/* Comando con 1 Byte de Informacion                                          */
/******************************************************************************/
int	cctalk::SendCmdGral(unsigned char cmd)
{	unsigned char txcmd[20];
	int lcmd;

	// Master Inhibit
	txcmd[0]	= cctalk_slave_address;	
	txcmd[1]	= 0x00;
	txcmd[2]	= MASTER_ADDRESS;
	txcmd[3]	= cmd;
	lcmd		= 4;

	return SendCmdCCTALK(txcmd, lcmd, TIMEOUT_CCTALK);
}

/******************************************************************************/
/* Comando con 1 Byte de Informacion                                          */
/******************************************************************************/
int	cctalk::SendCmd1B(unsigned char cmd, unsigned char dato)
{	int re;
	unsigned char txcmd[20];
	int lcmd;

	// Master Inhibit
	txcmd[0]	= cctalk_slave_address;
	txcmd[1]	= 0x01;
	txcmd[2]	= MASTER_ADDRESS;
	txcmd[3]	= cmd;
	txcmd[4]	= dato;
	lcmd		= 5;

	if( (re=SendCmdCCTALK(txcmd, lcmd, TIMEOUT_CCTALK)) == FALSE)
		return 0x00;

	// ACK
	return re;	// OK -> total de datos
}

/******************************************************************************/
/* Disable Chanels                                                            */
/******************************************************************************/
int cctalk::SendDisable(void)
{	unsigned char dis[4];

	dis[0]=dis[1]=dis[2]=dis[3]=0x00;

	SendModifyInhibitStatus(dis, TotalDenominaciones/8);

	return 0x00;
}

/******************************************************************************/
/* Disable Chanels                                                            */
/******************************************************************************/
int cctalk::SendEnable(void)
{

	// Enable
	EnableChannel[0] = EnableChannel[1] =EnableChannel[2] =EnableChannel[3] = 0xff;
	
	SendModifyInhibitStatus(EnableChannel, (unsigned char)TotalDenominaciones/8);

	return 0x00;
}

/******************************************************************************/
/* Request Status Sensors                                                     */
/******************************************************************************/
int	cctalk::SendRequestSelfTest(void)
{	int re;
	unsigned char txcmd[20];
	unsigned char no_err;
	int lcmd;

	// Master Inhibit
	txcmd[0]	= cctalk_slave_address;
	txcmd[1]	= 0x00;
	txcmd[2]	= MASTER_ADDRESS;
	txcmd[3]	= CMD_TEST;
	lcmd		= 4;

	if( (re=SendCmdCCTALK(txcmd, lcmd, TIMEOUT_CCTALK)) == FALSE)
	{
		printf("Error  Self Test\n");
		return 0;
	}
	else
	{
		// ACK
		no_err = ObtieneDato1B(--re);
	}
	no_err++;
	return no_err;	// OK -> total de datos
}

/******************************************************************************/
/* Send Reset Device                                                          */
/******************************************************************************/
int	cctalk::SendResetDevice(void)
{	int re;
	unsigned char txcmd[20];
	int lcmd;

	// Master Inhibit
	txcmd[0]	= cctalk_slave_address;
	txcmd[1]	= 0x00;
	txcmd[2]	= MASTER_ADDRESS;
	txcmd[3]	= CCTALK_RESET_DEVICE;
	lcmd		= 4;

	if( (re=SendCmdCCTALK(txcmd, lcmd, TIMEOUT_CCTALK)) == FALSE)
	{
		printf("Error Reset Device\n");
		return 0x100;
	}
	// ACK
	return 0x00;
}

/******************************************************************************/
/* Modify Inhibit Status Channels                                             */
/******************************************************************************/
int cctalk::SendModifyInhibitStatus( unsigned char *dat, unsigned char tdat )
{	unsigned char txcmd[20];
	int lcmd,k;

//	tdat = 4;		// //
	// Modify Inhibit Status
	txcmd[0]	= cctalk_slave_address;	
	txcmd[1]	= 0x02;
	txcmd[2]	= MASTER_ADDRESS;
	txcmd[3]	= CCTALK_MODIFY_INHIBIT_STATUS;

	for(k=0; k<tdat; k++)
	txcmd[4+k]	= dat[k];		//canales;
	lcmd		= 4 + tdat;

	return SendCmdCCTALK(txcmd, lcmd, TIMEOUT_CCTALK);
}

/******************************************************************************/
/* Modify Inhibit Status Channels                                             */
/******************************************************************************/
int cctalk::SendModifyInhibitBillStatus(unsigned char *dat, unsigned char tdat )
{	unsigned char txcmd[20];
	int lcmd,k;
	
	tdat = 4;
	// Modify Inhibit Status
	txcmd[0]	= cctalk_slave_address;	
	txcmd[1]	= 0x04;
	txcmd[2]	= MASTER_ADDRESS;
	txcmd[3]	= CCTALK_MODIFY_INHIBIT_STATUS;

	for(k=0; k<tdat; k++)
	txcmd[4+k]	= 0xff;//dat[k];		//canales;
	lcmd		= 4 + tdat;

	return SendCmdCCTALK(txcmd, lcmd, TIMEOUT_CCTALK);
}

/******************************************************************************/
/*    Real All Counters                                                       */
/******************************************************************************/
int	cctalk::ReadAllCounters(void)
{	int re;

	// Request Accept Counter
	if( (re=SendCmdGral(CCTALK_REQUEST_ACCEPT_COUNTER)) != FALSE)
	{
		counters.cctalk_ac = ObtieneDatosNum(re);

	} else return 0x100;

	// Request Insertion Counter
	if( (re=SendCmdGral(CCTALK_REQUEST_INSERTION_COUNTER)) != FALSE)
	{
		counters.cctalk_ic = ObtieneDatosNum(re);
	} 
	else 
		return 0x100;


	// Request Reject Counter
	if( (re=SendCmdGral(CCTALK_REQUEST_REJECT_COUNTER)) == FALSE)
		return 0x100;

	counters.cctalk_rc = ObtieneDatosNum(re);
	return 0x00;	// ALL OK
}

/******************************************************************************/
/* Obtiene los eventos recibidos                                              */
/******************************************************************************/
int cctalk::ReadEvents(char first)
{	int re;
	unsigned char noevent,j, rbe;

	if (cctalk_slave_address == COIN_ADDRESS)
		rbe = CCTALK_COIN_READ_BUFFER_EVENTOS;
	else
		rbe = CCTALK_BILL_READ_BUFFER_EVENTOS;

	// Read Buffer Credits & Errors
	if( (re=SendCmdGral(rbe)) != FALSE)
	{
		// OK
		if(re != 12)
		{
			return -1;	// error en recepcion de eventos
		}

		obtiene_rbce(--re);

		if(first)
		{
			cctalk_event_old = cctalkevents.events;
			return 0x00;
		}

		noevent = cctalkevents.events - cctalk_event_old;

		if( cctalk_event_old >  cctalkevents.events)
		{
			noevent--;
		}

		if(noevent>5)
		{
//			if (cctalk_slave_address == COIN_ADDRESS)
			printf("Perdida Eventos=[%d][%d][%d]\n",
									noevent, cctalk_event_old, cctalkevents.events);
			cctalk_event_old = cctalkevents.events;
			noevent = 0;
		}
		return noevent;
	}
	return -1;	// Error
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void cctalk::ClearDepositado(void)
{
//	memset(&depositado, 0, sizeof(depositado) );
	
	memset(&depositado.counter, 0, sizeof(depositado.counter) );
	depositado.cantidad = 0;
}

/******************************************************************************/
/*         Set Master Inhibit Status OFF                                      */
/******************************************************************************/
int cctalk::cctalk_OnOff(char status)
{
	// Set Master Inhibit Status ON
	if( SendMasterInhibitStatus(status) == 0)
	{
		return 0x00;
	}
	return 0x100;
}

/******************************************************************************/
/* Envia y Espera confirmacion del Master Inhibit                             */
/******************************************************************************/
int cctalk::SendWaitMasterInhibit(unsigned char status)
{	int re,st,i ;

	for(i=0; i<5; i++ )
	{
		if( cctalk_OnOff(status) == 0)
		{	// Enviado y Contestado OK, verifica Status
			if( (re=SendCmdGral(CCTALK_REQUEST_MASTER_INHIBIT_STATUS)) != FALSE)
			{
				st = ObtieneDato1B(re);
				fg_master_inhibit = st;

//				printf("Master Inhibit = %d %d %d \n", fg_master_inhibit, st, status);

				if(st==status)
				{
					// Master Inhibit puesto al valor solicitado
//					printf("Master Inhibit puesto al valor solicitado = [%d]\n", st);
					return 0;
				}
			}
		}
		usleep(100000);
	}
	return 0x01;	// ok
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int	cctalk::obtiene_flag(int re)
{	int x;

	memset(cctalk_datos, '\0', sizeof(cctalk_datos) );

	x = (cctalk_echo== CCTALK_CON_ECO)?9:4;

	return cctalk_buffer[x];
}

/******************************************************************************/
/* Inicializa y configura al monedero /BIlletero                              */
/******************************************************************************/
int	cctalk::Inicializa(unsigned char addr, int verif, int eco)
{	int re;

	cctalk_slave_address	= addr;		// direccion del dispositivo
	cctalk_verifica			= verif;
	cctalk_echo				= eco;

	memset( &depositado.valor, 0x00, sizeof(depositado.valor) );

	printf("Inicializa\n");

//	if(cctalk_slave_address == BILL_ADDRESS)
//	{
//		SendResetDevice();
//		sleep(1);
//	}

	// Simple Poll, verifica comunicacion
	if( (re=SendCmdGral(CCTALK_SIMPLE_POLL)) == FALSE)
		return 0x100;

	// ACK
	printf("ACK -> Set Inhibit Status Channel\n"); 

//	printf("Request Equipment ID\n");
	// Request Equipment ID
	if( (re=SendCmdGral(CCTALK_REQUEST_EQUIPMENT_ID)) != FALSE)
	{	// obtiene ID
		ObtieneDatosStr(re);
		strcpy(cctalkID.cctalk_id, cctalk_datos);
		printf("EQUIPMENT ID = [%d][%s]\n", re, cctalkID.cctalk_id);
	} else return 0x100;

	printf("Request Manufacturer ID\n");
	// Request Manufacturer ID
	if( (re=SendCmdGral(CCTALK_REQUEST_MANUFACTURER_ID)) != FALSE)
	{	// Manufacturer ID
		ObtieneDatosStr(re);
		strcpy(cctalkID.cctalk_mid, cctalk_datos);
		printf("MANUFACTURER ID = [%d][%s]\n", re, cctalkID.cctalk_mid);

		cctalk_factor=1;
		if( strcmp(cctalkID.cctalk_mid, "AZK") == 0)
			cctalk_factor=10;

	} else return 0x100;

	printf("Request Product Code\n");
	// Request Product Code  -> NO CONTESTA
	if( (re=SendCmdGral(CCTALK_REQUEST_PRODUCT_CODE)) != FALSE)
	{	// obtiene PC
		ObtieneDatosStr(re);
		strcpy(cctalkID.cctalk_pc, cctalk_datos);
		printf("PC = [%d][%s]\n", re, cctalkID.cctalk_pc);
	} 
	else return 0x100;

	printf("Request Build Code\n");
	// Request Build Code
	if( (re=SendCmdGral(CCTALK_REQUEST_BUILD_CODE)) != FALSE)
	{	// obtiene Build Code
		ObtieneDatosStr(re);
		strcpy(cctalkID.cctalk_bc, cctalk_datos);
		printf("BUILD CODE = [%d][%s]\n", re, cctalkID.cctalk_bc);
	} else return 0x100;

	printf("Request Serial Number\n");
	// Request Serial Number
	if( (re=SendCmdGral(CCTALK_REQUEST_SERIAL_NUMBER)) != FALSE)
	{	// Serial Number
		cctalkID.cctalk_sn = ObtieneDatosNum(re);
		printf("SERIAL NUMBER = [%ld]\n", cctalkID.cctalk_sn);
	} else return 0x100;

	printf("Request Software REvision------\n");
	// Request Software Revision
	if( (re=SendCmdGral(CCTALK_REQUEST_SOFTWARE_REVISION)) != FALSE)
	{	// Software Revision
		ObtieneDatosStr(re);
		strcpy(cctalkID.cctalk_sr, cctalk_datos);
		printf("SOFTWARE REVISION = [%s]\n", cctalkID.cctalk_sr);
	} else return 0x100;

//	// Set Inhibit Status Channels

	unsigned char dat[4];
	dat[0]=dat[1]=dat[2]= dat[3]=0xff;

	if ( SendModifyInhibitStatus(dat, 4) != FALSE)
//	{	// ACK
		printf("********ACK -> Set Inhibit Status Channel\n");
//	} else return 0x100;

	if( SendMasterInhibitStatus(0x01) == 0)
	{
		printf("Master Inhibit OFF\n");
	}
	else return 0x100;

	// Request Master Inhibit Status
	if( (re=SendCmdGral(CCTALK_REQUEST_MASTER_INHIBIT_STATUS)) != FALSE)
	{	// Request Master Inhibit Status
		inhibit = 	ObtieneDato1B(re);
		printf("Master Inhibit Status = [%d]\n", inhibit);
	} 
	else 
	{
		printf("ERROR NO CONTESTA Request Master Inhibit\n");
		return 0x100;
	}

	if( ReadEvents(0x01) == 0x00 )
	{
		printf("Read Buffer OK\n");
	}
	else return 0x100;

	// Request Polling Priority
	if( (re=SendCmdGral(CCTALK_REQUEST_POLLING_PRIORITY)) != FALSE)
	{	// Polling Priority
		cctalkID.cctalk_pp =  obtiene_pp(--re);
		printf("POLLING PRIORITY = [%ld]\n", cctalkID.cctalk_pp);
	} else return 0x100;

		if( (re=SendCmdGral(CCTALK_REQUEST_INHIBIT_STATUS)) != FALSE)
		{	// Request Inhibit Status
			cctalkID.cctalk_pp =  obtiene_is(--re);
			printf("Inhibit Status = \n");
		} else return 0x100;

	if(cctalk_slave_address == COIN_ADDRESS)
	{
		// Request Coin ID
		if( RequestCoinID() == FALSE)
			printf("Coin ID OK\n");
		else
			return 0x100;
	}
	else
	{
		// Request Bill ID
		if( RequestBillID() == FALSE)
			printf("Bill ID OK\n");
		else
			return 0x100;

		re=SendRequestSelfTest();
		printf("BILL ERR =%d\n",re);
		if(re==0)
		{
			printf("Billetero No Contesta\n");
			return 0x100;
		}
		re--;
		if(re==0)
		{
			fg_master_inhibit=1;	// OK
			printf("Billetero EN LINEA\n");
		}
		else
		{
			fg_master_inhibit=0;	//ERROR
			printf("Billetero FUERA DE SERVICIO\n");
		}

		// Expulsa cualquier billete que pudiera estar en el STACKER
		if( RouteBill(BILL_RETURN) == FALSE)
		{	// ACK
			printf("***************ACK -> Bill Expulsa OK\n");
		} else return 0x100;



	}


//	RouteBill(BILL_RETURN);

	if( SendMasterInhibitStatus(0x01) == 0)
	{
		printf("Master Inhibit OFF\n");
	}
	else
		return 0x100;

	status = 0x01;		// Comunicacion Establecida

	return 0x00;
}

/******************************************************************************/
/*                Comando para aceptar o regresar el billete                  */
/******************************************************************************/
int cctalk::RouteBill(unsigned char op)
{	int re;

	if( (re=SendCmd1B(CCTALK_ROUTE_BILL, op)) != FALSE)
	{	// ACK
		if(re==1)
		; //	printf("ACK -> RouteBill=%d OK\n",op);
		else
		; //	printf("Regreso datos=[%d]\n",re);
	} 
	else 
		return 0x100;	//Error

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void cctalk::EraseEvents(void)
{
	cctalk_event_old = cctalkevents.events;	// elimina buffer de eventos 
}

/******************************************************************************/
/* Regresa el MSB = ResultA y LSB= ResultB                                                         */
/******************************************************************************/
unsigned int cctalk::RdEvent(unsigned char ne)
{	int evento;

	cctalk_event_old++;
	if(cctalk_event_old==0)
		cctalk_event_old++;

	evento  = cctalkevents.datos[ne*2] * 0x100;
	evento += cctalkevents.datos[ne*2 + 1];

	return evento;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int cctalk::Value(unsigned char position)
{
	if( position > TotalDenominaciones )
	{	
		// error en informacion
		return -1;
	}
	return depositado.valor[position];
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int cctalk::Value_orgx(unsigned char position)
{
	if( position > TotalDenominaciones )
	{
		// error en informacion
		return -1;
	}
	return depositado.valor[position];
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned short int cctalk::crc_CCITT_A( unsigned short l, unsigned char *p, unsigned short seed )
{	int i;
	unsigned short crc = seed;

	
	for ( i = 0; i < l; ++i )		
		if( i!=2 )
		{
			crc = ( crc << 8 ) ^ crc_ccitt_lookup[ ( crc >> 8 ) ^ p[ i ] ];
		}
	return crc;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned char cctalk::checksum( unsigned char l, unsigned char *p, unsigned char seed )
{	unsigned char chk, i;

	chk=seed;
	for(i=0;i < l; i++)
		chk += *p++;
	chk = 0x00 - chk;
	return chk;
}
////////////////////////////////////////////////////////////////////////////////