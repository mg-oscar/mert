/*
 * File:   serialport.cpp
 * Author: famisa
 *
 * Created on October 10, 2022, 1:56 PM
 */

#include "serialport.h"

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
serialport::serialport()
{
	apIniBufRx = 0;
	apFinBufRx = 0;
	fd = -1;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int  serialport::GetFD(void)
{
	return fd;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void serialport::SetFD(int fde)
{
	fd = fde;	
}



/******************************************************************************/
/* Lee los datos recibidos                                                    */
/******************************************************************************/
int serialport::clear_rx (void)
{	int rdlen, nread;
	unsigned char rx[5];

	ioctl(fd, FIONREAD, &rdlen);	// lee cuantos datos se recibieron
	while(rdlen > 0)
	{	//lee y elimina datos en uart
		nread = read(fd, rx, 1);
		if(nread > 0)
			;
		else
			break;
		ioctl(fd, FIONREAD, &rdlen);
	}

	clrBufferRx();
	return 0x00;
}

/******************************************************************************/
/* Lee los datos recibidos                                                    */
/******************************************************************************/
int serialport::ReadComm (void)
{	int rdlen, nread;
	unsigned char rx[5];

	ioctl(fd, FIONREAD, &rdlen);	// lee cuantos datos se recibieron
	if(rdlen > 0)
	{	// almacena los datos en el buffer circular
		for(int i=0; i<rdlen; i++)
		{	//nread = read(fp_control, bufRxIRQ, 1);
			nread = read(fd, rx, 1);
			if(nread > 0)
			{
				putBufferRx(rx[0]);
//				printf("%02X,", rx[0]);
			}
			else
				break;
		}
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int  serialport::WriteComm( unsigned char cmd[], int lsize)
{
	if( write( fd, cmd, lsize) != lsize)
	{
		printf("err data send\n");
		return 0x01;
	}
	return 0x00;
}

/******************************************************************************/
/*          PONE LOS ATRIBUTOS DE COMUNICACION EN UN PUERTO SERIAL            */
/******************************************************************************/
int serialport::set_interface_attribs( int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("---> Error from tcgetattr: %d %s\n", fd, strerror(errno));
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    // ignore modem controls
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         // 8-bit characters
    tty.c_cflag &= ~PARENB;     // no parity bit
    tty.c_cflag &= ~CSTOPB;     // only need 1 stop bit
    tty.c_cflag &= ~CRTSCTS;    // no hardware flowcontrol

    // setup for non-canonical mode
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

    // fetch bytes as they become available
    tty.c_cc[VMIN] =  1;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        printf("Error from tcsetattr: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
void serialport::flushtx(void)
{
	sleep(2); //required to make flush work, for some reason
	tcflush(fd,TCIOFLUSH);
}

/******************************************************************************/
/*                	Abre y configura un puerto serial                         */
/******************************************************************************/
int serialport::OpenComm(char *namecomm, int baudrate)
{
	fd = open(namecomm, O_RDWR | O_NOCTTY);
	if(fd<0)
	{
//		printf("Error open comm: %s\n", namecomm);
		return 0x01;
	}
		printf("OK open comm: %d -> %s\n", fd, namecomm);

	fcntl(fd, F_SETFL, FNDELAY);

	// Configure the serial port attributes: 
	set_interface_attribs(baudrate);

	clear_rx();
	return 0x00;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int  serialport::CloseComm(void)
{
	close(fd);
	return 0;
}

/******************************************************************************/
/* borra el buffer circular                                                   */
/******************************************************************************/
void serialport::clrBufferRx(void)
{
	apIniBufRx = apFinBufRx;
}

/******************************************************************************/
/* toma un dato del buffer circular                                           */
/******************************************************************************/
unsigned char serialport::getBufferRx(void)
{
	if(apIniBufRx >= SIZE_BUFFER_RX)
		apIniBufRx = 0;
	return bufRx[apIniBufRx++];
}

/******************************************************************************/
/* inserta un dato al buffer circular                                         */
/******************************************************************************/
void serialport::putBufferRx(unsigned char d)
{
	if(apFinBufRx>= SIZE_BUFFER_RX)
		apFinBufRx = 0;
	bufRx[apFinBufRx++] = d;
}
/******************************************************************************/