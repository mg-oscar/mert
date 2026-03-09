/* 
 * File:   rs232.hpp
 * Author: Elias Gonzalez
 *
 * Created on February 13, 2023, 10:31 PM
 */

#ifndef RS232_603_HPP
#define RS232_603_HPP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/ioctl.h>

#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

//typedef struct tty_info_t
//{
//    int fd; 
//    pthread_mutex_t mt;
//    char name[24]; 
//    struct termios ntm; 
//    struct termios otm; 
//} TTY_INFO;

//TTY_INFO *readyTTY(char* id);

//static TTY_INFO *g_ptty = NULL;

class rs232_603 {
public:
    rs232_603();

    int RS232_OpenWithBaudRate(char* ttysid, int baudrate);
    int RS232_Write(unsigned int length,unsigned char buffer[]);
    int RS232_Read( unsigned int length,unsigned char buffer[]);  
    int RS232_Close();
    
private:

	int recvnTTY(TTY_INFO *ptty,unsigned char *pbuf, int size);
	int sendnTTY(TTY_INFO *ptty,unsigned char *pbuf,int size);
	int cleanTTY(TTY_INFO *ptty);
	int setTTYSpeed(TTY_INFO *ptty, int speed);
	int setTTYParity(TTY_INFO *ptty,int databits,int parity,int stopbits);

	TTY_INFO *g_ptty;
	TTY_INFO *readyTTY(char* id);

	
};

#endif /* RS232_603_HPP */

