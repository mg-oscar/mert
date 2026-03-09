/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   configuracion.h
 * Author: sisamo
 *
 * Created on November 5, 2022, 7:02 PM
 */

#ifndef CONFIGURACION_H
#define CONFIGURACION_H

typedef struct
{
	char Code[50];
	int  Format;
	int  Version;
	int  SourceVersion;
	char CreationDate[20];
	char ActivationDate[20];
	char Author[50];
} header;

header hdr_initval;
header hdr_config;
header hdr_perifericos;
header hdr_operacion;


#endif /* CONFIGURACION_H */

