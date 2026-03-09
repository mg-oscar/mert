
/******************************************************************************/
const int xout = 400;
const int yout = 100;

const int xinp = 800;
const int yinp = 50;

char fontIO[]  = {"Noto Mono"};
char colorIO[] = {"gainsboro"};

char relay_accion=0;	// default open

#define TOTAL_OUTPUT			8		// total de salidas
const  gchar *mod_output[] = 
{	"   LUZ   \nBILLETERO", 
	"   LUZ  \nMONEDERO",
	"  LUZ  \nBANDEJA",
	" LUZ \nLECTOR",
	"ALARMA",
	"    NO \nDISPONIBLE",
	"SOLENOIDE\n  CAJA  ",
	"SALIDA 8" };


#define TOTAL_INPUT			8		// total de entradas
const  gchar *mod_input[] = 
{
	"0 Btn Sup Izq  ",
	"1 Btn Inf Izq  ",
	"2 Btn Inf Der  ",
	"3 Btn Sup Der  ",
	"4 Puerta       ",
	"5 Falla Energia",
	"6 Sensor Caja  ",
	"7 Hall Sensor  ",
};
GArray *arrInput;

GtkWidget	*fxModOutput;		// window ModES


#define ILUMINA_BILLETERO			0x01
#define ILUMINA_MONEDERO			0x02
#define ILUMINA_BANDEJA_CAMBIO		0x03
#define ILUMINA_LECTOR_TARJETA		0x04
#define ALARMA_AUDIBLE_VISIBLE		0x05
#define MOTOR_DESATORAR				0x06
#define SOLENOIDE_CAJA_MONEDERO		0x07
#define OUTPUT7						0x08
