/* 
 * File:   rs232.h
 * Author: famisa
 *
 * Created on March 4, 2023, 5:00 PM
*/

#include <pthread.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>


#ifndef RS232_H
#define RS232_H

#define SIZE_BUFFER_RX	512


// Estructura de información del dispositivo serie
 typedef struct tty_info_t
{
	int fd;				// ID del Comm
	pthread_mutex_t mt; // mutex de sincronización de subprocesos
	char name[24];		// Nombre del Comm
	struct termios ntm; // Parametros del Comm -> Nuevos
	struct termios otm; // Parametros del Comm -> Heredaados

} TTY_INFO;


class rs232 
{
public:
	rs232();
	
	unsigned char bufRx[SIZE_BUFFER_RX];

	int RS232_OpenWithBaudRate(char* ttysid, int baudrate);
	int RS232_Write(unsigned int length,unsigned char buffer[]);
	int RS232_Read( void);
	int RS232_Close();

	unsigned char getBufferRx(void);
	int  existDat(void);

private:
	int apIniBufRx;
	int apFinBufRx;

	TTY_INFO *g_ptty;

	int recvnTTY(TTY_INFO *ptty, unsigned char *pbuf);
	int sendnTTY(TTY_INFO *ptty, unsigned char *pbuf,int size);
	int cleanTTY(TTY_INFO *ptty);
	int setTTYSpeed(TTY_INFO *ptty, int speed);
	int setTTYParity(TTY_INFO *ptty,int databits,int parity,int stopbits);

	TTY_INFO *readyTTY(char* id);

	void clrBufferRx(void);
	void putBufferRx(char d);
	int  clear_rx (void);

};

#endif /* RS232_H */