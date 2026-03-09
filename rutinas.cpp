//
//
//
#include <pwd.h>
void datetime(char *tfecha);
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void argumentos(int argc, char** argv)
{	char tmptxt[100];

// -h = help
// -p = path
// -n = no graba la MIA
// -a = cambia la alarma al buzzer
// -x = toma perfil del archivo
// -d = DTE MERT

	for(int k=0; k<argc; k++)
	{
		strncpy(tmptxt, argv[k], 100);

		if(strncmp(tmptxt,"-h",2)==0)
		{	// mostrar ayuda
			printf("\n\n      Opciones de ejecucion \n\n");
			printf("-p = cambiar path  [ejemplo: -p/home/debian/bin/]\n");
			printf("-m = Imprime Tramas de comm Modbus\n");
			printf("-n = No graba en la MIA\n");
			printf("-i = Inicializa MIA\n");
			printf("-a = Cambia la Alarma al buzzer\n");
			printf("-x = Toma perfil del archivo\n");
			printf("\n\n");
			exit(-1);
		}

		if(strncmp(tmptxt,"-p",2)==0)
		{	memmove(mert_operacion.path, tmptxt+2, strlen(tmptxt)-1);
			printf("path=[%s]\n", mert_operacion.path);
		}

		if(strncmp(tmptxt,"-d",2)==0)
		{
			mert_famisa = 0;
			printf("DTE MERT\n");
		}

		if(strncmp(tmptxt,"-n",2)==0)
		{
			GRABA_MIA = 0;
			printf("NO GRABA MIA\n");
		}

		if(strncmp(tmptxt,"-i",2)==0)
		{
			InitMIA = 1;
			printf("Inicializa MIA\n");
		}
		if(strncmp(tmptxt,"-a",2)==0)
		{
			TipoAlarma = 0;
			printf("Alarma->Buzzer\n");
		}
		if(strncmp(tmptxt,"-x",2)==0)
		{
			PerfilFile = 1;
			printf("Perfil -> File\n");
		}
	}
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int close_usb(libusb_device_handle *devh)
{
	if(devh == NULL)
		return 0;
	libusb_release_interface(devh, 0);
	libusb_attach_kernel_driver(devh, 0);                               
    libusb_close(devh);
    devh = NULL;

//	revisar si cierra todas las conexiones de USB de la app
//	libusb_exit(NULL);
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
libusb_device_handle *find_usb_device(unsigned int idvendor, unsigned int idproduct)
{	libusb_device_handle *devh;
	int r;

	devh = libusb_open_device_with_vid_pid(NULL, idvendor, idproduct);
	if(devh==NULL)
		return NULL;
	r = libusb_detach_kernel_driver(devh, 0);

	if (r < 0)
	{
		printf("libusb_detach_kernel_driver %d\n", r);
		close_usb(devh);
		return NULL;
	}

	r = libusb_claim_interface(devh, 0);
	if (r < 0)
	{
		printf("usb_claim_interface error %d\n", r);
		return NULL;
	}
	return devh;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int SearchAndOpenUSBPorts(void)
{	int tusb=0;
//	libusb_context *context;
	libusb_device **list;
	ssize_t			count;
	size_t			index;
	unsigned long USB_ID;

	struct libusb_device           *device;
	struct libusb_device_descriptor descriptor;
	char                            buffer[256];
	int                             result;

	tusb=0;
	count = libusb_get_device_list(context, &list);
	for (index = 0; index < count; ++index)
	{
		device = list[index];
		if ((result = libusb_get_device_descriptor(device, &descriptor)) != 0)
		{
			fprintf(stderr, "error(%d): reading descriptor\n", result);
			continue;
		}

		USB_ID = (descriptor.idVendor<<16) + (descriptor.idProduct);
//		if( (USB_ID == USB_CRT) || (USB_ID == USB_CVT) )
		if( (USB_ID == USB_CVT) )
		{
			if ((result = libusb_open(device, &lUSB[tusb])) != 0)
			{
				fprintf(stderr, "error(%d): openning device 0x%04X:0x%04X\n", 
				result, descriptor.idVendor, descriptor.idProduct);
				continue;
			}

			result = libusb_detach_kernel_driver(lUSB[tusb], 0);
			if (result < 0)
			{
//				fprintf(stderr, "libusb_detach_kernel_driver %d\n", result);
				return -2;
			}

			result = libusb_claim_interface(lUSB[tusb], 0);
			if (result < 0)
			{
//				fprintf(stderr, "usb_claim_interface error %d\n", result);
				return -2;
			}

//			if( USB_CRT	== USB_ID)
//			{	printf("USB-CRT=%d\n", tusb);
//				lUSBCRT = lUSB[tusb];
//				NoUsbCRT = tusb;
//			}
			tusb++;
		}
	}
	printf("Total de USB=%d\n", tusb);

	for(index=0; index<tusb; index++)
	{
		printf("\n\tdevice #: %zu \n", index);

		result = libusb_get_string_descriptor_ascii(
														lUSB[index],
														descriptor.iProduct,
														(unsigned char *)buffer,
														sizeof(buffer)	);
		if (result != 0)
			fprintf(stdout, "\tproduct     : %s\n", buffer);

		result = libusb_get_string_descriptor_ascii(
														lUSB[index],
														descriptor.iManufacturer,
														(unsigned char *)buffer,
														sizeof(buffer)	);
		if (result != 0)
			fprintf(stdout, "\tmanufacturer: %s\n\n", buffer);
	}

	libusb_free_device_list(list, 1);

	return tusb;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int USB_Close(void)
{	int i;

	for(i=0; i<NO_MAX_USB; i++)
	{
		if(lUSB[i] != NULL)
		{
			libusb_release_interface(lUSB[i], 0);
			libusb_attach_kernel_driver(lUSB[i], 0);
			libusb_close(lUSB[i]);
			lUSB[i] = NULL;
		}
	}
//	libusb_exit(NULL);
	return 0;
}

/******************************************************************************/
/* Busca todos los lectores CIVINTEC, deben ser 3                             */
/******************************************************************************/
int SearchAllUSB(void)
{	int k, tusb=0;

	for(k=0; k<NO_MAX_USB; k++)
		lUSB[k] = NULL ;

	k=0;
	while(1)
	{
		if( (tusb=SearchAndOpenUSBPorts()) <= 0)
		{	k++;
			if(k > 10)
			{	//printf("Error al abrir el USB\n");
				USB_Close();
				return 0x00;
			}
			USB_Close();
		}
		else
			break;
	}
	printf("tusb=%d\n", tusb);
	return tusb;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void iniPathLed(void)
{
	sprintf(Led_Gris15, "%simagenes/led_gris15.png",mert_operacion.path);
	sprintf(Led_Rojo15, "%simagenes/led_rojo15.png",mert_operacion.path);
	sprintf(Led_Verde15, "%simagenes/led_verde15.png",mert_operacion.path);

	
	sprintf(Led_Gris25,		"%simagenes/led_gris25.png",mert_operacion.path);
	sprintf(Led_Rojo25,		"%simagenes/led_rojo25.png",mert_operacion.path);
	sprintf(Led_Verde25,	"%simagenes/led_verde25.png",mert_operacion.path);
	sprintf(Led_Amarillo25, "%simagenes/led_amarillo25.png",mert_operacion.path);
	sprintf(GifWait,		"%simagenes/chrono.gif",mert_operacion.path);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void PrintID(char msg[], unsigned char UID[])
{	int i;

	printf("\n Caja %s-> ", msg);
	printf("ID.=[");
	for(i=0; i<4; i++)
		printf("%02X", UID[i]);
	printf("]\n\n");
}

/******************************************************************************/
/*  puertos: enp2s0  enp3s0                                                    */
/******************************************************************************/
char obtieneIP_MASK(char name[], char ip[], char nm[])
{	int fd;
	struct ifreq ifr;

// enp2s0  enp3s0

	strcpy(ip, "0.0.0.0");
	strcpy(nm, "0.0.0.0");

	if( (fd = socket(AF_INET, SOCK_DGRAM, 0)) != -1)
	{
		ifr.ifr_addr.sa_family = AF_INET;
		strncpy(ifr.ifr_name, name, IFNAMSIZ-1);

		// lee la IP ADDDRES
		ioctl(fd, SIOCGIFADDR, &ifr);
		sprintf(ip, "%s",  inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

		//lee la MASK
		ioctl(fd, SIOCGIFNETMASK, &ifr);
		sprintf(nm, "%s",  inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

		close(fd);		
		return 0x00;

	}
	printf("no se encontro IP\n");
	return 0x01;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned int milisegundos(void)
{	struct timeval lapsed;
	unsigned int milisec;

	gettimeofday (&lapsed, NULL);
	milisec = (lapsed.tv_sec * 1000000 + lapsed.tv_usec) / 1000;

	return milisec;
}

/******************************************************************************/
// Imprime el numero en binario
/******************************************************************************/
void binary(unsigned char n) 
{	unsigned char i;
	// Reverse loop
	for (i = 1 << 7; i > 0; i >>= 1)
	{	if(i==0x08)
			printf(" ");
		printf("%u", !!(n & i));
	}
}

/******************************************************************************/
/* left trim spaces & $                                                                 */
/******************************************************************************/
char *ltrimspaces(char *s)
{
	while( isspace(*s) || (*s=='$') ) s++;
//	while( *s=='$' ) s++;
//	while( isspace(*s) ) s++;
//	while(*s == ' ') s++;	
    return s;
}

/******************************************************************************/
/* left trim                                                                  */
/******************************************************************************/
char *ltrim(char *s)
{
	while(isspace(*s)) s++;
//	while(*s==' ') s++;	
    return s;
}

/******************************************************************************/
/* right trim                                                                 */
/******************************************************************************/
char *rtrim(char *s)
{
    char* back = s + strlen(s);
    while(isspace(*--back));
//	while((*--back)==' ');
    *(back+1) = '\0';
    return s;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
char *trim(char *s)
{
    return rtrim(ltrim(s)); 
}

/******************************************************************************/
/* scr=origen, devuelte el substring entre m->inicio l=total de chars         */
/******************************************************************************/
char* substr(const char *src, int m, int l)
{	int i;

	char *dest = (char*)malloc(sizeof(char) * (l + 1));
	for ( i = 0; i < l && (*(src + i) != '\0'); i++)
	{
		*dest = *(src + m + i);
		dest++;
	}
	*dest = '\0';
	return dest - l;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void datetime(char *tfecha)
{	time_t TimeNow;
	struct tm ticket_time;

	time(&TimeNow);  // time actual
	ticket_time = *localtime(&TimeNow);
	strftime(tfecha, 50,"%Y-%m-%d %H:%M:%S", &ticket_time);
}

/******************************************************************************/
/* formato de regreso:   18/01/2023 11:10:00                                  */
/******************************************************************************/
char* dtime(time_t tim)
{	struct tm ticket_time;

	char *tfecha = (char*)malloc(24);

	ticket_time = *localtime(&tim);
	strftime(tfecha, 22,"%d/%m/%Y %H:%M:%S", &ticket_time);
	return tfecha;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
char *datet(char opc)
{	time_t TimeNow;
	struct tm ticket_time;
	char *tfecha = (char*)malloc(26);

	time(&TimeNow);  // time actual
	ticket_time = *localtime(&TimeNow);
	if(opc)
		strftime(tfecha, 26,"%d/%m/%Y %H:%M:%S", &ticket_time);
	else
		strftime(tfecha, 26,"%Y-%m-%d %H:%M:%S", &ticket_time);

	return tfecha;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
unsigned char Date2DecadeMonth(void)
{	unsigned char DecadeMonth;
	int Year ;

	time_t TimeNow = time(NULL);
	struct tm *Now = localtime(&TimeNow);

	Year = (Now->tm_year + 1900)%10;

	DecadeMonth = (Year * 12) + (Now->tm_mon+1);

	return DecadeMonth;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
char *DecadeMonth2Date(int DecMes )
{	
	int Year, Month, DecMon;
	char Date[8];
	char *DateReturn;

	DateReturn = &Date[0];
	time_t TimeNow = time(NULL);
	struct tm *Now = localtime(&TimeNow);
	
	Year = (Now->tm_year + 1900)/10;
	Year = Year*10;
	
	if( (DecMes%12) == 0)
		Year = Year + (DecMes/12) -1;
	else
		Year = Year + (DecMes/12);

	if( (DecMes%12)==0)
		Month = 12;
	else
		Month = DecMes%12;
	
	snprintf(Date, 8, "%02d/%02d", Month, Year);
	
	return DateReturn;
}


/******************************************************************************/
/*                                                                            */
/******************************************************************************/
char *DecadeMonth2Date_OLD(unsigned char dmonth )
{	time_t TimeNow;
	struct tm ticket_time;
	int year;
	unsigned char ye, mo;
	char *tfecha = (char*)malloc(26);

	time(&TimeNow);  // time actual
	ticket_time = *localtime(&TimeNow);

	ye = dmonth / 12;				// años despues de la decada
	mo = dmonth - (12 * ye);	// mes del año

	strftime(tfecha, 26,"%Y", &ticket_time);
	tfecha[0] = tfecha[2];
	tfecha[1] = tfecha[3];
	tfecha[2] = 0x00;

	year =  atoi(tfecha);

	year =  (year / 10) * 10;
	year = year + ye;

	sprintf(tfecha, "%02d/%02d", mo, year);

	return tfecha;
}

/******************************************************************************/
/* convierte char UID a unsigned int                                          */
/******************************************************************************/
unsigned int cnvuidhex( unsigned char uid[] )
{	char uidstr[10];

	sprintf(uidstr,"%02X%02X%02X%02X", uid[0], uid[1], uid[2], uid[3]);
//	printf("UID STR=[%s]\n", uidstr);
	return strtoul( uidstr, NULL, 16);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void interchange(long long *a, long long *b)
{	long long temporal = *a;

	*a = *b;
	*b = temporal;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void OrdenaBlackList(long long lista[], int totdat)
{
	for (int i = 0; i < totdat - 1; i++)
	{	for (int j = i + 1; j < totdat; j++)
			if (lista[i] > lista[j]) 
				interchange(&lista[i], &lista[j]);
	}
}

/******************************************************************************/
/*  busqueda binaria: lista, treg=tot reg, clave = numero a buscar            */
/******************************************************************************/
int BusquedaBinaria(long long lista[], int treg, long long clave)
{	short central, bajo, alto;
	long long valorCentral;
	char ciclos = 0;

	bajo = 0;
	alto = treg-1;
	while(bajo <= alto)
	{	ciclos++;
		central = (bajo+alto)/2;
		valorCentral = lista[central];
		if(clave==valorCentral)
			return central; // encontrado, regresa la posicion
		else if(clave<valorCentral)
			alto = central - 1;	// ir a mitad inferior
		else
			bajo = central + 1; // ir a mitad superior
	}
	return -1;  // no se encontro el valor
}

/******************************************************************************/
/*  busqueda binaria en USERS                                                 */
/******************************************************************************/
int SearchUser(USERS list, unsigned int UID)
{	short central, bajo, alto;
	unsigned int valorCentral;
	char ciclos = 0;
	int treg;

	treg = list.tot_reg;

	bajo = 0;
	alto = treg-1;
	while(bajo <= alto)
	{	ciclos++;
		central = (bajo+alto)/2;
		valorCentral = list.UID[central];
		if(UID==valorCentral)
			return central; // encontrado, regresa la posicion
		else if(UID<valorCentral)
			alto = central - 1;	// ir a mitad inferior
		else
			bajo = central + 1; // ir a mitad superior
	}
	return -1;  // no se encontro el valor
}

/******************************************************************************/
/* Busca y regresa el peso de una moneda                                      */
/******************************************************************************/
int pmoneda( int valor, int tot)
{	int i;

	for(i=0; i<10; i++)
		if(peso_moneda[i].valor == valor)
			return peso_moneda[i].peso * tot;
	return 0;
}

/******************************************************************************/
/* AbortHandler:                                                              */
/*                                                                            */
/* This ensures the AESAccess Shared library destructor function is called.   */
/* If the application exits normaly the destructor is always called, however  */
/* if the application receives a signal (SIGINT...) the destructor is not     */
/* called, that is why we need this signal handler (force exit() call).       */
/*                                                                            */
/******************************************************************************/
void AbortHandler(int signum)
{	char strfec[80];

	sprintf ( strfec, "Externo, Evento de Salida = [%d]\n",signum);
	printf ( "\n[%s]\n",strfec);
//	crealog(strfec);

	exit(-1);
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void iniSignal(void)
{
    //-- Catch signals so we exit cleanly
    (void)signal(SIGHUP,      AbortHandler);
    (void)signal(SIGINT,      AbortHandler);
    (void)signal(SIGQUIT,     AbortHandler);
    (void)signal(SIGILL,      AbortHandler);
    (void)signal(SIGTRAP,     AbortHandler);
    (void)signal(SIGABRT,     AbortHandler);
    (void)signal(SIGBUS,      AbortHandler);
    (void)signal(SIGFPE,      AbortHandler);
    (void)signal(SIGKILL,     AbortHandler);
    (void)signal(SIGUSR1,     AbortHandler);
    (void)signal(SIGSEGV,     AbortHandler);
    (void)signal(SIGUSR2,     AbortHandler);
    (void)signal(SIGPIPE,     AbortHandler);
    (void)signal(SIGALRM,     AbortHandler);
    (void)signal(SIGTERM,     AbortHandler);
//    (void)signal(SIGCHLD,     AbortHandler);
    (void)signal(SIGCONT,     AbortHandler);
    (void)signal(SIGSTOP,     AbortHandler);
    (void)signal(SIGTSTP,     AbortHandler);
    (void)signal(SIGTTIN,     AbortHandler);
    (void)signal(SIGTTOU,     AbortHandler);
    (void)signal(SIGURG,      AbortHandler);
    (void)signal(SIGXCPU,     AbortHandler);
    (void)signal(SIGXFSZ,     AbortHandler);
    (void)signal(SIGVTALRM,   AbortHandler);
    (void)signal(SIGPROF,     AbortHandler);
    (void)signal(SIGWINCH,    SIG_IGN);
    (void)signal(SIGIO,       AbortHandler);
    (void)signal(SIGPWR,      AbortHandler);
    (void)signal(SIGSYS,      AbortHandler);
}



/******************************************************************************/


