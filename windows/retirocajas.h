/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   retirocajas.h
 * Author: famisa
 *
 * Created on December 26, 2022, 11:44 AM
 */

#ifndef RETIROCAJAS_H
#define RETIROCAJAS_H


const int altura = 420;
const int ancho  = 200;


typedef struct
{
	int usuario;
	int	ficha;
	int sello;
	int caja;
}CORTE_DATOS;

GtkWidget *frmRC, *frmCorte;
GtkWidget *entUser, *entFicha, *entSello;
GtkWidget *ledCaja, *btnCaja, *btnLed;

int total_peso;
int total_monedas;

int total_billetes;

int RetCajaMonedas;
int RetCajaBilletes;

char fgCorte = 0;
int selEntry = 1;

CORTE_DATOS corte_datos;

void winCorteCaja(char *caja);

int rdIDCajaMonedas(void);
int rdIDCajaBilletes(void);
//void waitNewBox(void);



#endif /* RETIROCAJAS_H */

