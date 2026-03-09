//
//
#ifndef WINTRIPODE_H
#define WINTRIPODE_H

#define	SHOW	1
#define	HIDE	0

#define WIN_INICIO		0
#define WIN_PROCESO		1


#define MODO_TARJETA				100
#define MODO_PASSWORD				101
#define MODO_MENU					102
#define MODO_ESPERA_FIN				103
#define MENU_REGRESO				105
#define MODO_READ_CAJAS				106
#define MODO_PUERTA_ABIERTA			107
#define MODO_CIERRA_PUERTA			108

#define MODO_SDA_TARJETA			109
#define MODO_SDO_PASSWORD			110



// variables para pantenimiento
GtkWidget	*winmantto;				// ventana principal
GtkWidget	*fxMantto;				// contenedor

GtkWidget	*dateMain;				// Fecha en Ventana Principal
GtkWidget	*verMain;				// Version en Ventana Principal

GtkWidget	*msgSuperior;			// Linea de Mensajes, parte superior
GtkWidget	*msgInferior;			// Linea de Mensajes, parte inferior

GtkWidget	*msgMain;				// Mensajes en ventana principal

GtkWidget	*gifWait;				// gif de esperar

int win_menu;


#define FONT_BOLD	700


/******************************************************************************/
#include "testMonedero.h"
#include "testBilletero.h"
#include "testIO.h"
#include "testPrinter.h"
#include "testDispensador.h"
#include "testPerifericos.h"


#include "retirocajas.h"
#include "dispensador.h"

#include "libTarjeta.h"
#include "valCaja.h"


#include "winVentaRecarga.h"



// ***********************************Menu 1  **********************************
// posicion del menu
int	xmn1 = 300;
int ymn1 = 200;

#define TOTAL_OPCIONES			10	// total de Opciones

#define MENU_REIMPRIME_CORTES	0
#define MENU_CLIENTES			1
#define MENU_RETIRO_CAJAS		2
#define MENU_DISPENSADOR		3
#define MENU_ESTADO				4
#define MENU_PRUEBA				5
#define MENU_LIBERA_TARJETA		6
#define MENU_VALIDA_CAJAS		7
#define MENU_FUTURO3			8
#define MENU_FUTURO4			9

#define WIN_MENU1_EXIT			99



const  gchar *opc_menu[TOTAL_OPCIONES] = 
{
	"Reimpresion de Cortes",
	"Operaciones de Clientes",
	"Retiro de Cajas de Recaudacion",
	"Dispensador",
	"Estado del Equipo",
	"Prueba de Perifericos",
	"Libera Tarjeta",
	"Valida Caja de Valores",
	"Futuro 3",
	"Futuro 4"
};
GtkWidget	*fxMenu;				// window menu

//
// *****************  variables para el PASSWORD *******************************
//
const char pwd_exit[] = {"0196409270"};		// temp -> sale de la app
const char pwd_off[]  = {"1270919641"};		// temp -> apaga la PC

struct key
{	gint id;
	GtkWidget *label;
	GtkWidget *button;
	GtkWidget *image;	
};

#define TOTAL_TECLADO	12
const gchar *letters[] = 
{ "1", "2", "3", "4", "5", "6", "7", "8", "9", ".", "0", "B" };

const gint xkb = 50;
const gint ykb = 50;


GtkWidget	*entPass;	      // Password
GtkWidget	*fxPass;				// window password

// COLORES							         R    G    B    A
const GdkRGBA rgb_dark_red				= { 139,   0,   0, 1.0};
const GdkRGBA rgb_brown					= { 165,  42,  42, 1.0};
const GdkRGBA rgb_firebrick				= { 178,  34,  34, 1.0};
const GdkRGBA rgb_crimson				= { 220,  20,  60, 1.0};
const GdkRGBA rgb_red					= { 255,   0,   0, 1.0};
const GdkRGBA rgb_tomato				= { 255,  99,  71, 1.0};
const GdkRGBA rgb_coral					= { 255, 127,  80, 1.0};
const GdkRGBA rgb_indian_red			= { 205,  92,  92, 1.0};
const GdkRGBA rgb_light_coral			= { 240, 128, 128, 1.0};
const GdkRGBA rgb_dark_salmon			= { 233, 150, 122, 1.0};
const GdkRGBA rgb_salmon				= { 250, 128, 114, 1.0};
const GdkRGBA rgb_light_salmon			= { 255, 160, 122, 1.0};
const GdkRGBA rgb_orange_red			= { 255,  69,   0, 1.0};
const GdkRGBA rgb_dark_orange			= { 255, 140,   0, 1.0};
const GdkRGBA rgb_orange				= { 255, 165,   0, 1.0};
const GdkRGBA rgb_gold					= { 255, 215,   0, 1.0};
const GdkRGBA rgb_dark_golden_rod		= { 184, 134,  11, 1.0};
const GdkRGBA rgb_golden_rod			= { 218, 165,  32, 1.0};
const GdkRGBA rgb_pale_golden_rod		= { 238, 232, 170, 1.0};
const GdkRGBA rgb_dark_khaki			= { 189, 183, 107, 1.0};
const GdkRGBA rgb_khaki					= { 240, 230, 140, 1.0};
const GdkRGBA rgb_olive					= { 128, 128,   0, 1.0};
const GdkRGBA rgb_yellow				= { 255, 255,   0, 1.0};
const GdkRGBA rgb_yellow_green			= { 154, 205,  50, 1.0};
const GdkRGBA rgb_dark_olive_green		= {  85, 107,  47, 1.0};
const GdkRGBA rgb_olive_drab			= { 107, 142,  35, 1.0};
const GdkRGBA rgb_lawn_green			= { 124, 252,   0, 1.0};
const GdkRGBA rgb_chartreuse			= { 127, 255,   0, 1.0};
const GdkRGBA rgb_green_yellow			= { 173, 255,  47, 1.0};
const GdkRGBA rgb_dark_green			= {   0, 100,   0, 1.0};
const GdkRGBA rgb_green					= {   0, 128,   0, 1.0};
const GdkRGBA rgb_forest_green			= {  34, 139,  34, 1.0};
const GdkRGBA rgb_lime					= {   0, 255,   0, 1.0};
const GdkRGBA rgb_lime_green			= {  50, 205,  50, 1.0};
const GdkRGBA rgb_light_green			= { 144, 238, 144, 1.0};
const GdkRGBA rgb_pale_green			= { 152, 251, 152, 1.0};
const GdkRGBA rgb_dark_sea_green		= { 143, 188, 143, 1.0};
const GdkRGBA rgb_medium_spring_green	= {   0, 250, 154, 1.0};
const GdkRGBA rgb_spring_green			= {   0, 255, 127, 1.0};
const GdkRGBA rgb_sea_green				= {  46, 139,  87, 1.0};
const GdkRGBA rgb_medium_aqua_marine	= { 102, 205, 170, 1.0};
const GdkRGBA rgb_medium_sea_green		= {  60, 179, 113, 1.0};
const GdkRGBA rgb_light_sea_green		= {  32, 178, 170, 1.0};
const GdkRGBA rgb_dark_slate_gray		= {  47,  79,  79, 1.0};
const GdkRGBA rgb_teal					= {   0, 128, 128, 1.0};
const GdkRGBA rgb_dark_cyan				= {   0, 139, 139, 1.0};
const GdkRGBA rgb_aqua					= {   0, 255, 255, 1.0};
const GdkRGBA rgb_cyan					= {   0, 255, 255, 1.0};
const GdkRGBA rgb_light_cyan			= { 224, 255, 255, 1.0};
const GdkRGBA rgb_dark_turquoise		= {   0, 206, 209, 1.0};
const GdkRGBA rgb_turquoise				= {  64, 224, 208, 1.0};
const GdkRGBA rgb_medium_turquoise		= {  72, 209, 204, 1.0};
const GdkRGBA rgb_pale_turquoise		= { 175, 238, 238, 1.0};
const GdkRGBA rgb_aqua_marine			= { 127, 255, 212, 1.0};
const GdkRGBA rgb_powder_blue			= { 176, 224, 230, 1.0};
const GdkRGBA rgb_cadet_blue			= {  95, 158, 160, 1.0};
const GdkRGBA rgb_steel_blue			= {  70, 130, 180, 1.0};
const GdkRGBA rgb_corn_flower_blue		= { 100, 149, 237, 1.0};
const GdkRGBA rgb_deep_sky_blue			= {   0, 191, 255, 1.0};
const GdkRGBA rgb_dodger_blue			= {  30, 144, 255, 1.0};
const GdkRGBA rgb_light_blue			= { 173, 216, 230, 1.0};
const GdkRGBA rgb_sky_blue				= { 135, 206, 235, 1.0};
const GdkRGBA rgb_light_sky_blue		= { 135, 206, 250, 1.0};
const GdkRGBA rgb_midnight_blue			= {  25,  25, 112, 1.0};
const GdkRGBA rgb_navy					= {   0,   0, 128, 1.0};
const GdkRGBA rgb_dark_blue				= {   0,   0, 139, 1.0};
const GdkRGBA rgb_medium_blue			= {   0,   0, 205, 1.0};
const GdkRGBA rgb_blue					= {   0,   0, 255, 1.0};
const GdkRGBA rgb_royal_blue			= {  65, 105, 225, 1.0};
const GdkRGBA rgb_blue_violet			= { 138,  43, 226, 1.0};
const GdkRGBA rgb_indigo				= {  75,   0, 130, 1.0};
const GdkRGBA rgb_dark_slate_blue		= {  72,  61, 139, 1.0};
const GdkRGBA rgb_slate_blue			= { 106,  90, 205, 1.0};
const GdkRGBA rgb_medium_slate_blue		= { 123, 104, 238, 1.0};
const GdkRGBA rgb_edium_purple			= { 147, 112, 219, 1.0};
const GdkRGBA rgb_dark_magenta			= { 139,   0, 139, 1.0};
const GdkRGBA rgb_dark_violet			= { 148,   0, 211, 1.0};
const GdkRGBA rgb_dark_orchid			= { 153,  50, 204, 1.0};
const GdkRGBA rgb_medium_orchid			= { 186,  85, 211, 1.0};
const GdkRGBA rgb_purple				= { 128,   0, 128, 1.0};
const GdkRGBA rgb_thistle				= { 216, 191, 216, 1.0};
const GdkRGBA rgb_plum					= { 221, 160, 221, 1.0};
const GdkRGBA rgb_violet				= { 238, 130, 238, 1.0};
const GdkRGBA rgb_magenta_fuchsia		= { 255,   0, 255, 1.0};
const GdkRGBA rgb_orchid				= { 218, 112, 214, 1.0};
const GdkRGBA rgb_medium_violet_red		= { 199,  21, 133, 1.0};
const GdkRGBA rgb_pale_violet_red		= { 219, 112, 147, 1.0};
const GdkRGBA rgb_deep_pink				= { 255,  20, 147, 1.0};
const GdkRGBA rgb_hot_pink				= { 255, 105, 180, 1.0};
const GdkRGBA rgb_light_pink			= { 255, 182, 193, 1.0};
const GdkRGBA rgb_pink					= { 255, 192, 203, 1.0};
const GdkRGBA rgb_antique_white			= { 250, 235, 215, 1.0};
const GdkRGBA rgb_beige					= { 245, 245, 220, 1.0};
const GdkRGBA rgb_bisque				= { 255, 228, 196, 1.0};
const GdkRGBA rgb_blanched_almond		= { 255, 235, 205, 1.0};
const GdkRGBA rgb_wheat					= { 245, 222, 179, 1.0};
const GdkRGBA rgb_corn_silk				= { 255, 248, 220, 1.0};
const GdkRGBA rgb_lemon_chiffon			= { 255, 250, 205, 1.0};
const GdkRGBA rgb_light_golden_yellow	= { 250, 250, 210, 1.0};
const GdkRGBA rgb_light_yellow			= { 255, 255, 224, 1.0};
const GdkRGBA rgb_saddle_brown			= { 139,  69,  19, 1.0};
const GdkRGBA rgb_sienna				= { 160,  82,  45, 1.0};
const GdkRGBA rgb_chocolate				= { 210, 105,  30, 1.0};
const GdkRGBA rgb_peru					= { 205, 133,  63, 1.0};
const GdkRGBA rgb_sandy_brown			= { 244, 164,  96, 1.0};
const GdkRGBA rgb_burly_wood			= { 222, 184, 135, 1.0};
const GdkRGBA rgb_tan					= { 210, 180, 140, 1.0};
const GdkRGBA rgb_rosy_brown			= { 188, 143, 143, 1.0};
const GdkRGBA rgb_moccasin				= { 255, 228, 181, 1.0};
const GdkRGBA rgb_navajo_white			= { 255, 222, 173, 1.0};
const GdkRGBA rgb_peach_puff			= { 255, 218, 185, 1.0};
const GdkRGBA rgb_misty_rose			= { 255, 228, 225, 1.0};
const GdkRGBA rgb_lavender_blush		= { 255, 240, 245, 1.0};
const GdkRGBA rgb_linen					= { 250, 240, 230, 1.0};
const GdkRGBA rgb_old_lace				= { 253, 245, 230, 1.0};
const GdkRGBA rgb_papaya_whip			= { 255, 239, 213, 1.0};
const GdkRGBA rgb_sea_shell				= { 255, 245, 238, 1.0};
const GdkRGBA rgb_mint_cream			= { 245, 255, 250, 1.0};
const GdkRGBA rgb_slate_gray			= { 112, 128, 144, 1.0};
const GdkRGBA rgb_light_slate_gray		= { 119, 136, 153, 1.0};
const GdkRGBA rgb_light_steel_blue		= { 176, 196, 222, 1.0};
const GdkRGBA rgb_lavender				= { 230, 230, 250, 1.0};
const GdkRGBA rgb_floral_white			= { 255, 250, 240, 1.0};
const GdkRGBA rgb_alice_blue			= { 240, 248, 255, 1.0};
const GdkRGBA rgb_ghost_white			= { 248, 248, 255, 1.0};
const GdkRGBA rgb_honeydew				= { 240, 255, 240, 1.0};
const GdkRGBA rgb_ivory					= { 255, 255, 240, 1.0};
const GdkRGBA rgb_azure					= { 240, 255, 255, 1.0};
const GdkRGBA rgb_snow					= { 255, 250, 250, 1.0};
const GdkRGBA rgb_black					= {   0,   0,   0, 1.0};
const GdkRGBA rgb_dim_gray_grey			= { 105, 105, 105, 1.0};
const GdkRGBA rgb_gray_grey				= { 128, 128, 128, 1.0};
const GdkRGBA rgb_dark_gray_grey		= { 169, 169, 169, 1.0};
const GdkRGBA rgb_silver				= { 192, 192, 192, 1.0};
const GdkRGBA rgb_light_gray_grey		= { 211, 211, 211, 1.0};
const GdkRGBA rgb_gainsboro				= { 220, 220, 220, 1.0};
const GdkRGBA rgb_white_smoke			= { 245, 245, 245, 1.0};
const GdkRGBA rgb_white					= { 255, 255, 255, 1.0};

const GdkRGBA rgb_trans					= {   0,  0,   0, 0.0};


void gtk_changeLabel(char operacion);
void winPasswd(void);

#endif /* WINTRIPODE_H */
