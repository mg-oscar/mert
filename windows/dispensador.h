/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dispensador.h
 * Author: famisa
 *
 * Created on Marzo 14, 2023, 11:44 AM
 */

#ifndef DISPENSADOR_H
#define DISPENSADOR_H


const int d_altura = 320;
const int d_ancho  = 200;


GtkWidget *frmDispensador;
GtkWidget *frmM1, *frmM2;

GtkWidget *entDisp[4], *cmbDisp;

int dispEntry=0;

char dispActive=0;	// default NO

#define DISP_CARGA			0x01
#define DISP_RETIRA			0x02
#define DISP_VENTA			0x03
#define DISP_HOLD			0x04

#define DISP_DUMMY				0x00
#define DISP_OK					0x01
#define DISP_ERR_STACKER		0x01
#define DISP_ERR_GETCARD		0x02
#define DISP_ERR_NOCARD			0x03
#define DISP_ERR_READCARD		0x04
#define DISP_ERR_MIA			0x05
#define DISP_ERR_WRITECARD		0x06
#define DISP_ERR_BINBOX			0x07
#define DISP_ERR_VENTA_CANCEL	0x08

typedef struct
{
	char msg[60];
} ERROR_DISP;

ERROR_DISP disp_err[10] =
{
	"************    OK  **********",		// 0
	"        ERROR EN STACKER      ",       // 1
	"  ERROR AL TOMAR LA TARJETA   ",		// 2
	" ERROR NO DETECTO LA TARJETA  ",		// 3
	"** Error al Leer la Tarjeta **",		// 4
	"Error, Tarjeta No Inicializada",		// 5
	"  Error al Grabar la Tarjeta  ",		// 6
	"Error al arrojar la Tarjeta   ",		// 7
	"Saldo Incompleto para compra  "		// 8
};

/*ERROR_DISP xdisp_err[10] =
{	"******************************",		// 0
	"************    OK  **********",		// 1
	"        ERROR EN STACKER      ",       // 2
	"  ERROR AL TOMAR LA TARJETA   ",		// 3
	" ERROR NO DETECTO LA TARJETA  ",		// 4
	"** Error al Leer la Tarjeta **",		// 5
	"Error, Tarjeta No Inicializada",		// 6
	"  Error al Grabar la Tarjeta  ",		// 7
	"Error al arrojar la Tarjeta   ",		// 8
	"Venta Cancelada por fondos    "		// 9
};
*/
#endif /* DISPENSADOR_H */

