/* 
 * File:   moduloio.h
 * Author: sisamo
 *
 * Created on October 10, 2022, 4:41 PM
 */

#ifndef MODULOIO_H
#define MODULOIO_H


#define TOTAL_MODULOS_IO	1
#define TOTAL_ENTRADAS		8
#define TOTAL_SALIDAS		8

#define TotalBytesEntrada	1
#define TotalBytesSalida	1

#define t_bc				0x20		// longitud del buffer de recepcion
#define TotalEntradas		0x08		// total de ent fisicas


struct t_input
{
	unsigned int time;
};


class moduloio: public modbusclass
{
public:
	int dirmodulo;
	unsigned char OnLine;
	unsigned char entradas_new[TotalBytesEntrada];
	unsigned char salidas_new[TotalBytesSalida];

	unsigned char Entradas[TOTAL_ENTRADAS];
	unsigned char EntradasOld[TOTAL_ENTRADAS];

	unsigned char Botones[TOTAL_ENTRADAS];
	unsigned int  tmBotones[TOTAL_ENTRADAS];
	unsigned char fgBotones[TOTAL_ENTRADAS];

	
	
	unsigned char Salidas[TOTAL_SALIDAS];
	unsigned char salidasOld[TOTAL_SALIDAS];

	moduloio();

	int ReadEntradas(void);		// Lee el Estado de las Entradas y los valida
	
	int relay(int  no_relay, int valor);
	int pulse(int no_relay, int valor, int tiempo);

	int relay_all(int dir_modulo, unsigned char valor);
	
	unsigned char BotonOprimido(unsigned char nboton);

	unsigned int  tmBtnLatch;
	unsigned char fgLatch;

private:
	unsigned char entradas_old[TotalBytesEntrada];	
	unsigned char salidas_old[TotalBytesEntrada];	

	int readinput(void);		// Lee las entradas de los 2 modulos de E/S
	void desgloza_entradas(void);
	void desgloza_salidas(void);
};




#define R_OPEN						0x00
#define R_CLOSE						0x01
#define R_PULSE						0x02

#define RELAY_CLOSE					0xFF00
#define RELAY_OPEN					0x0000

#define BOTON_OPRIMIDO_SI			0x81
#define BOTON_OPRIMIDO_NO			0x80

#define PUERTA_CERRADA				0x80
#define PUERTA_ABIERTA				0x81


#define INPUT_IN_0					0x80	// Estado valido = 0
#define INPUT_IN_1					0x81	// Estado valido = 1



// Modulo 1 -> ENTRADAS
#define BOTON_SUPERIOR_IZQUIERDO	0x00
#define BOTON_INFERIOR_IZQUIERDO	0x01
#define BOTON_INFERIOR_DERECHO		0x02
#define BOTON_SUPERIOR_DERECHO		0x03
#define SENSOR_PUERTA				0x04
#define SENSOR_FALLA_CORRIENTE		0x05
#define SENSOR_CAJA1				0x06
#define SENSOR_MONEDERO				0x07

// Modulo 2 -> ENTRADAS
#define INPUT8						0x08
#define INPUT9						0x09
#define INPUTA						0x0A
#define INPUTB						0x0B
#define INPUTC						0x0C
#define INPUTD						0x0D
#define INPUTE						0x0E
#define INPUTF						0x0F


// Modulo 1-> SALIDAS
#define ILUMINA_BILLETERO			0x01
#define ILUMINA_MONEDERO			0x02
#define ILUMINA_BANDEJA_CAMBIO		0x03
#define ILUMINA_LECTOR_TARJETA		0x04
#define ALARMA_AUDIBLE_VISIBLE		0x05
#define MOTOR_DESATORAR				0x06
#define SOLENOIDE_CAJA_MONEDERO		0x07
#define OUTPUT7						0x08

// Modulo 2 -> SALIDAS
#define OUTPUT8						0x09
#define OUTPUT9						0x0A
#define OUTPUTA						0x0B
#define OUTPUTB						0x0C
#define OUTPUTC						0x0D
#define OUTPUTD						0x0E
#define OUTPUTE						0x0F
#define OUTPUTF						0x10


const unsigned int RELAY_PULSO[] = { 0x0003, 0x0008, 0x000D, 0x0012, 0x0017, 0x001C, 0x0021, 0x0026 };

#endif /* MODULOIO_H */

