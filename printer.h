/* 
 * File:   printer.h
 * Author: famisa
 *
 * Created on January 3, 2023, 4:29 PM
 */

#ifndef PRINTER_H
#define PRINTER_H


serialport printer;

const char printer_mty[] = {"\n\n***METRORREY\n"};



#define ESC			0x1B
#define GS			0x1D
#define DLE			0x10
#define EOT			0x04


const unsigned char PRINTER_ALIGN_LEFT[3]	= { ESC, 'a', 0x00};	// align left
const unsigned char PRINTER_ALIGN_CENTER[3]	= { ESC, 'a', 0x01};	// align center
const unsigned char PRINTER_ALIGN_RIGHT[3]	= { ESC, 'a', 0x02};	// align right

const unsigned char PRINTER_B_S_OFF[3]		= { ESC, '!', 0x08};	// bold size off
const unsigned char PRINTER_BOLD_ON[3]		= { ESC, '!', 0x18};	// bold on
const unsigned char PRINTER_SIZE_ON[3]		= { ESC, '!', 0x28};	// size double on
const unsigned char PRINTER_BOLD_SIZE_ON[3]	= { ESC, '!', 0x38};	// bold size on

const unsigned char PRINTER_STATUS[3]		= { DLE, EOT, 0x04};	// status
const unsigned char PRINTER_INITIALIZE[2]	= { ESC, '@'};			// Inicializa impresora
const unsigned char PRINTER_CUT[3]			= {  GS, 'V', '0'};	// cut


const unsigned char PRINTER_NORMAL[3]		= { ESC, '!', 0x03};	// Normal Size
const unsigned char PRINTER_BOLD[3]			= { ESC, '!', 0x08};	// Bold Normal
const unsigned char PRINTER_BOLD_MEDIUM[3]	= { ESC, '!', 0x20};	// Bold Medium
const unsigned char PRINTER_BOLD_LARGE[3]	= { ESC, '!', 0x10};	// Bold Large


const unsigned char FONT_A[3]	 = { ESC, '!', 0x00};	// Font A
const unsigned char FONT_B[3]	 = { ESC, '!', 0x01};	// Font A

const unsigned char MODE_CPI[3]   = { ESC, 0xC1,0x00};
const unsigned char CHAR_SIZE[3]  = { 0x1D,0x21,0x01};




unsigned const char ENCABEZADO1[]  =  {"METRORREY"};
unsigned const char ENCABEZADO2[]  =  {"Pino Suarez 1123, Centro"};
unsigned const char ENCABEZADO3[]  =  {"CP 64000, Monterrey, N.L."};

unsigned const char ENCABEZADO4[]  =  {"RECAUDO DE MONEDAS\n\n"};
unsigned const char ENCABEZADO5[]  =  {"RECAUDO DE BILLETES\n\n"};
unsigned const char ENCABEZADO6[]  =  {"RECARGA DE SALDO\n\n"};
unsigned const char ENCABEZADO7[]  =  {"VENTA Y ABONO DE SALDO\n\n"};
unsigned const char ENCABEZADO8[]  =  {"GRACIAS POR SU VISITA\n"};
unsigned const char ENCABEZADO9[]  =  {"Este ticket no es comprobante\nfiscal\n"};
unsigned const char ENCABEZADO10[] =  {"OPERACION EXITOSA\n"};
unsigned const char ENCABEZADO11[] =  {"     Concepto             Monto\n"};
unsigned const char ENCABEZADO12[] =  {"ERROR AL GRABAR TARJETA MIA\n"};
unsigned const char ENCABEZADO13[] =  {"Saldo Pendiente por Recargar: "};


typedef struct
{
	int linea;
	int	estacion;
	int equipo;
	int usuario1;
	int usuario2;
	int ficha;
	int sello;
	int alcancia;
	int folio;

	int valor[16];
	int cantidad[16];
	int total;
	int monto;

	int  saldo_inicial;
	int  bono_inicial;
	int  saldo_final;
	int  bono_final;
	int  deposito;
	
	unsigned char UID[10];
	char nlogico[30];
	char fecha[25];
	char nlinea[40];
	char nestacion[40];
	char nequipo[40];	// nombre del equipo
	

} IMP_DETALLE;

IMP_DETALLE datImprimir;

#endif /* PRINTER_H */

