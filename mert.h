/* 
 * File:   mert.h
 * Author: FAMISA
 *
 * Created on October 15, 2022, 11:26 AM
 */

#ifndef MERT_H
#define MERT_H

#define BYTE unsigned char
#define byte unsigned char
#define LWORD unsigned int
typedef unsigned short word;



// MODO DE OPERACION DE LA MERT
#define MERT_EXPENDEDORA					10
#define MERT_RECARGADORA					11

// Indica donde se encuentra el SAM para AUTENTIFICACION
unsigned char MERT_VENTA_SAM	= 0;
unsigned char MERT_RECARGA_SAM	= 0;
unsigned char MERT_COINBOX_SAM	= 0;
unsigned char MERT_BILLBOX_SAM	= 0;
unsigned char MERT_PUERTA_SAM	= 0;

int idBillBox=-1;
int idCoinBox=-1;
int idDispensador=-1;
int idPuerta=-1;

int indBillBox		=-1;
int indCoinBox		=-1;
int indDispensador	=-1;
int indPuerta		=-1;

int indPtr1222[6]{-1, -1,-1,-1,-1,-1};
int indPtr1252[6]{-1, -1,-1,-1,-1,-1};

byte tRdrCV;		// total de puertos Civintec
byte tRdrACR1222;		// total de puertos ACS
byte tRdrACR1252;		// total de puertos ACR1252

int DirModuModbus=0xFF;

char fgMenu=0;

int flagBill=0;		// bandera para modificar valores del billetero
int fgShowCoin=0;		// muestra msg de solo monedas 0=NO 1=SI
typedef struct       			// 0=NO   1=SI
{
	int  tot_reg;
	char fEfectiva[20];
	char fCaducidad[20];

	char fecha[12][20];
	char tmov[12][10];
	char estacion[12][30];
	int	 valor[12];
	int  bono[12];

	int  saldoMov[12];
	int	 bonoMov[12];
	int  equipo[12];
}HIST_MOV;

HIST_MOV movMIA;


// Civintec
#define ID_VENDOR	0x04D8
#define ID_PRODUCT	0x000C
#define USB_CVT		0x04D8000C

// CRT285
#define CRT_VENDOR	0x23d8
#define CRT_PRODUCT 0x0285
#define USB_CRT		0x23d80285

#define NO_MAX_USB				3

libusb_context *context;

struct libusb_device_handle    *lUSB[NO_MAX_USB];	// CIVINTEC
struct libusb_device_handle    *lUSBCRT = NULL;		// CHINA CREATOR

char fgCRT285 = 0;

#define SPANISH			0
#define INGLES			1

#define TARJETA_PRESENTE	0

#define MODO_ARCHIVOS		0	// Carga los archivos de configuracion
#define MODO_PERIFERICOS	1	// inicializa los diferetes perifericos
#define MODO_DECIDE			2
#define MODO_RECARGA		3
#define MODO_MANTTO			4
#define MODO_INICIAL		5
#define MODO_OPERATIVO		6
#define MODO_CAJA_LLENA		7
#define MODO_ERROR_CRT		8	// Error en lector de recarga


// MODOS DE OPERACION DE LA MERT
#define	MERT_MODO_COMPLETO				1	// 1 = Modo Completo, Vende y Recarga
#define MERT_MODO_VENTA					2	// 2 = Modo Parcial, solo vende
#define MERT_MODO_RECARGA				3	// 3 = Modo Parcial, solo recarga
#define MERT_MODO_CONSULTA				4	// 4 = Modo Consulta
#define MERT_MODO_MANTENIMIENTO			5 	// 5 = Mantenimiento
#define MERT_MODO_FUERA_DE_SERVICIO		6	// 6 = Fuera de Servicio

// ESTADOS DE LA MERT
#define MERT_OPERACION					1
#define MERT_SUSPENDIDO					2
#define MERT_DANADO						3
#define MERT_APAGADO					4
#define MERT_MANTENIMIENTO				5
#define MERT_ASIGNADO					6
#define MERT_ALMACEN					7

#define TM_PASSWORD		25000	// 25 segundos para capturar el password
#define TM_PUERTA		8000	// 8 segundos para abrir la puerta
#define TM_TERMINA		1000	// 8 segundos para abrir la puerta
#define TM_CAJAS		60000	// cada minuto lee ID de las cajas
#define TM_CAJAS_2		500		// c/segundo para cambio de caja
#define TM_CERRAR		7500	// tiempo para cerrar la puerta

#define PROC_NONE			0
#define PROC_VENTA			1
#define PROC_RECARGA		2
#define PROC_VENTA_ERR		3


#define TiempoBarridoModuloIO		200

#define PERFIL_SUPERADMIN			5
#define PERFIL_JEFE_ESTACION		3
#define PERFIL_SUPERVISOR			2
#define PERFIL_MANTENIMIENTO		6

unsigned int	tbCajas;	// tiempo entre barrido de ID de Cajas de Valores

// Estructura par la operacion general del equipo
typedef struct       			// 0=NO   1=SI
{
	unsigned char modo;			// Modo de Operacion
									// 0 = FUERA DE SERVICIO
									// 1 = Modo Completo, Vende y Recarga
									// 2 = Modo Parcial, solo vende
									// 3 = Modo Parcial, solo recarga
									// 4 = Modo Consulta
									// 5 = Mantenimiento
									// 6 = Fuera de Servicio
	unsigned char password;
	unsigned char fgMONEDERO;	// Monedero Habilitado
	unsigned char fgBILLETERO;	// Billetero Habilitado
	unsigned char fgVENTA;		// VENTA DE TARJETAS
	unsigned char fgRECARGA;	// RECARGA
	unsigned char proceso;		// 0=NADA, 1= VENTA 2= RECARGA
	unsigned char fgIdioma;		// 0=Español 1=Ingles

	// Menus
	unsigned char no_menu;		// bandera para quitar Menu
	unsigned char no_rec_venta;	// menu de venta/recarga
	char path[50];				// path base de todas las carpetas de operacion
	char ip[20];				// IP de la MERT
	char nm[20];				// NM de la MERT
}configuracion;


struct st_io
{
	char op;					// Permiso para empezar barrido del Modulo de E/S

	char status_comm;			// Status de Comm con el Modulo de E/S

	char CommName[50];
	int  Baud;

	char NoControl;				// No de Control  0=NO      1..16=SI
	char OnOFF;					// 0=OPEN    1=CLOSE     2=PULSO
	int  tiempo;				// base de tiempo de  0.1 seg

	// Cambiar de lugar estas variables
	char AccionMonedero;		// Aqui se nos indica la accion con el monedero
	char AccionBilletero;		// Aqui se nos indica la accion con el billetero
	char AccionDispensador;		// Aqui se nos indica la accion con el dispensador	
	char opVentaRecarga;		// permiso para empezar a Vender/Recargar
								// 1=Init  2=Enable  3=Mantto
	char StatusMantto;			// MERT en Mantto, no puede proceder a operar
//								// recargas ni ventas
	char StatusProceso;			// proceso de venta/recarga iniciado, no puede
								// entrar a ciertos procesos de mantenimiento

	char startCB;				// Inicia Barrido de Monedas y Billetes
	char startCajas;			// Inicia Barrido de las cajas

	char cmdBill;				// Comando para Billetero
	char cmdCoin;				// Comando para Billetero

	

};

struct st_io *stModIO = (struct st_io *)malloc(sizeof(struct st_io));

// Datos de Identificacion y Ubicacion del Equipo
typedef struct
{	int COMP;			// 1
	int LINEA;			//
	int ESTACION;		//
	int TRAY;			// 
	int TIPO;			//
	int NUMDISP;
	int TICKETVAL;
}initvalues;

configuracion mert_operacion;

initvalues mert_identidad;	// Identidad de la MERT

unsigned char GRABA_MIA		= 1;
unsigned char TipoAlarma	= 1;
unsigned char InitMIA		= 0;
unsigned char PerfilFile	= 0;
unsigned char mert_famisa	= 1;


char mnu_opcion = 0;

unsigned char BilleteroON = 1;	// Bandera para indicar si bloqueamos el billetero


#define TOTAL_USERS			2
#define NOUSER_1			0
#define NOUSER_2			1

typedef struct
{
	int nousuario;
	int noperfil;
	unsigned int noempleado;
	unsigned int nouid;
	char name[40];
	char pass[6];
}USER_DOOR;


USER_DOOR card_puerta;
USER_DOOR data_user[TOTAL_USERS];

char fgUpdate;



#define NO_INTENTOS_CAJAS			3
#define NO_REINTENTOS_CAJAS			20
unsigned int  idCajaMonedas=0;		// ID Caja de Monedas
unsigned char fgCajaMonedas=0;		// 0 = error  1	= OK
unsigned int  niCajaMonedas=0;		// numero de intentos antes de marcar error

unsigned int  idCajaBilletes=0;
unsigned char fgCajaBilletes=0;		// 0 = error  1 = OK
unsigned char niCajaBilletes=0;		// numero de intentos antes de marcar error

typedef struct
{
	int valor;
	int peso;
}PESO_MONEDA;

const PESO_MONEDA peso_moneda[10] =
{	{  50,   360},
	{ 100,   395},
	{ 200,   519},
	{ 500,   707},
	{1000,  1033},
	{2000,  1267},
	{   0,     0},
	{   0,     0},
	{   0,     0},
	{   0,     0},
};


char Led_Gris15[100];
char Led_Rojo15[100];
char Led_Verde15[100];

char Led_Gris25  [100];
char Led_Verde25 [100];
char Led_Rojo25  [100];
char Led_Amarillo25 [100];
char GifWait [100];

#define LED_GRIS		0
#define LED_ROJO		1
#define LED_VERDE		2
#define LED_AMARILLO	3


int posiciona_destrabador(void);

// Mensajes de los Eventos
const  char *descEventos[] = 
{	"Inicia Aplicacion",				// 0
	"Puerta Abierta",					// 1
	"Password OK",						// 2
	"Maquina Alarmada",					// 3
	"Caja Monedas Diferente",			// 4
	"Caja Billetes Diferente",			// 5
	"Caja Monedas No Detectada",		// 6
	"Caja Billetes No Detectada",		// 7
	"Tarjeta Rechazada",				// 8
	"Puerta Cerrada",					// 9
	"Caja Monedas Validada",			// 10
	"Caja Billetes Validada",			// 11

};

#define EVT_INICIA				0
#define EVT_PUERTA_ABIERTA		1
#define EVT_PASSWORD			2
#define EVT_ALARMADA			3
#define EVT_BOX_COIN_DIF		4
#define EVT_BOX_BILL_DIF		5
#define EVT_BOX_COIN_NO_DET		6
#define EVT_BOX_BILL_NO_DET		7
#define EVT_TARJETA_RECHAZADA	8
#define EVT_PUERTA_CERRADA		9
#define EVT_MONEDAS_VALIDADA	10
#define EVT_BILLETES_VALIDADA	11


void *mod_EntSal(void *inp);	// Modulode Entradas/Salidas
void *mod_Aceptator(void *inp);	// Barrido del Billetero y Monedero

void proc_administrativo(int re_conf);
void proceso_venta_recarga(void);
void Alarma(char status);
void EventBox(char lbl[], unsigned int  idBD, unsigned int  idRd );
void EventosMert(char msg[]);
void iniPathLed(void);
void PrintID(char msg[], unsigned char UID[]);
void billsprocesos(unsigned char posicion, unsigned char path );


#endif /* MERT_H */