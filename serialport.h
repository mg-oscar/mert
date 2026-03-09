/* 
 * File:   serialport.h
 * Author: FAMISA
 *
 * Created on October 10, 2022, 1:56 PM
 */

#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>



int fd_cctalk=-1;

#define SIZE_BUFFER_RX	512

class serialport {
public:
	serialport();

	int fd;		// ID del serial port
	int apIniBufRx;
	int apFinBufRx;

	int  ReadComm (void);
	int  WriteComm( unsigned char cmd[], int lsize);
	int  OpenComm(char *namecomm, int baudrate);
	int  CloseComm(void);
	int  clear_rx (void);
	void flushtx(void);
	
	int  GetFD(void);
	void SetFD(int fde);
	
	unsigned char getBufferRx(void);

private:
	unsigned char bufRx[SIZE_BUFFER_RX];

	int  set_interface_attribs(int speed);
	void clrBufferRx(void);
	void putBufferRx(unsigned char d);

};

#endif /* SERIALPORT_H */