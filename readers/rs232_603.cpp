/* 
 * File:   rs232.cpp
 * Author: Elias Gonzalez
 * 
 * Created on February 13, 2023, 10:31 PM
 */

#include "rs232_603.hpp"

rs232_603::rs232_603()
{

}

/******************************************************************************/
/*        Inicializa el Comm y guarda la configuración original               */
/******************************************************************************/
TTY_INFO *rs232_603::readyTTY(char* id)
{	TTY_INFO *ptty;

	ptty = (TTY_INFO *)malloc(sizeof(TTY_INFO));
	if(ptty == NULL)
		return NULL;
	memset(ptty,0,sizeof(TTY_INFO));
	pthread_mutex_init(&ptty->mt,NULL);
	sprintf(ptty->name,"%s",id);
//	printf("ptty->name:%s\n",ptty->name);

	// Abre y configura el puerto serie
    ptty->fd = open(ptty->name, O_RDWR | O_NOCTTY |O_NDELAY);
    if (ptty->fd <0)
    {
        free(ptty);
        return NULL;
    }

    // Obtiene y guarda la configuración original
	tcgetattr(ptty->fd,&ptty->otm);
	return ptty;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rs232_603::cleanTTY(TTY_INFO *ptty)
{

    if(ptty->fd>0)
    {
        tcsetattr(ptty->fd,TCSANOW,&ptty->otm);
        close(ptty->fd);
        ptty->fd = -1;
        free(ptty);
        ptty = NULL;
    }

    return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rs232_603::setTTYSpeed(TTY_INFO *ptty, int speed)
{
    if( tcgetattr(ptty->fd,&ptty->ntm) != 0)
    {
        printf("SetupSerial [%s]\n",ptty->name);
        return 1;
    }

    bzero(&ptty->ntm, sizeof(ptty->ntm));
    ptty->ntm.c_cflag = CS8 | CLOCAL | CREAD;
    ptty->ntm.c_iflag = IGNPAR;
    ptty->ntm.c_oflag = 0;

//	printf("*******************Baud=%d*******************\n", speed);
	ptty->ntm.c_cflag |= B9600;

    switch(speed)
    {
    case 300:
        ptty->ntm.c_cflag |= B300;
        break;
    case 1200:
        ptty->ntm.c_cflag |= B1200;
        break;
    case 2400:
        ptty->ntm.c_cflag |= B2400;
        break;
    case 4800:
        ptty->ntm.c_cflag |= B4800;
        break;
    case 9600:
        ptty->ntm.c_cflag |= B9600;
        break;
    case 19200:
        ptty->ntm.c_cflag |= B19200;
        break;
    case 38400:
        ptty->ntm.c_cflag |= B38400;
        break;
    case 115200:
        ptty->ntm.c_cflag |= B115200;
        break;
    }

    ptty->ntm.c_iflag = IGNPAR;
    ptty->ntm.c_oflag = 0;
    //

    tcflush(ptty->fd, TCIFLUSH);
    tcsetattr(ptty->fd,TCSANOW,&ptty->ntm);
    //

    return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rs232_603::setTTYParity(TTY_INFO *ptty,int databits,int parity,int stopbits)
{
    ptty->ntm.c_cflag &= ~CSIZE;
    switch (databits)
    {

    case 7:
        ptty->ntm.c_cflag |= CS7;
        break;
    case 8:
        ptty->ntm.c_cflag |= CS8;
        break;
    default:
        printf("Unsupported data size\n");
        return 5;
    }
    //


    switch (parity)
    { 

    case 'n':
    case 'N':
        ptty->ntm.c_cflag &= ~PARENB; 
        ptty->ntm.c_iflag &= ~INPCK; 
        break;
    case 'o':
    case 'O':
        ptty->ntm.c_cflag |= (PARODD|PARENB); 
        ptty->ntm.c_iflag |= INPCK; 
        break;
    case 'e':
    case 'E':
        ptty->ntm.c_cflag |= PARENB; 
        ptty->ntm.c_cflag &= ~PARODD; 
        ptty->ntm.c_iflag |= INPCK; 
        break;
    case 'S':
    case 's': /*as no parity*/
        ptty->ntm.c_cflag &= ~PARENB;
        ptty->ntm.c_cflag &= ~CSTOPB;
        break;
    default:
        printf("Unsupported parity\n");
        return 2;
    }

    switch (stopbits)
    {
    case 1:
        ptty->ntm.c_cflag &= ~CSTOPB;
        break;
    case 2:
        ptty->ntm.c_cflag |= CSTOPB;
        break;
    default:
        printf("Unsupported stop bits\n");
        return 3;
    }

    ptty->ntm.c_lflag = 0;
    ptty->ntm.c_cc[VTIME] = 0; // inter-character timer unused

    ptty->ntm.c_cc[VMIN] = 1; // blocking read until 1 chars received

    tcflush(ptty->fd, TCIFLUSH);
    if (tcsetattr(ptty->fd,TCSANOW,&ptty->ntm) != 0)
    {
        printf("SetupSerial \n");
        return 4;
    }


    return 0;
}
 
int rs232_603::recvnTTY(TTY_INFO *ptty,unsigned char *pbuf, int size)
{
    int len=0, ret = 0;  
    fd_set fs_read;  
    struct timeval tv_timeout; 
	
    int left  = size;
    unsigned char *ptmp = pbuf;  
      int alllen=0;
    
      
    while(1)    
    {
	 FD_ZERO(&fs_read);  
         FD_SET(ptty->fd, &fs_read);  
         tv_timeout.tv_sec  = 0;  
         tv_timeout.tv_usec = 300000;  
      
        ret = select(ptty->fd+1, &fs_read, NULL, NULL, &tv_timeout); 
        if(ret == 0) 
        {
            printf("recvnTTY timeout size:%d\n",size);
            return -2;
        }

        if (FD_ISSET(ptty->fd, &fs_read))
        {  
                len = read(ptty->fd, ptmp, size); 
                if(len==0)
                    return -3;
                alllen+=len;
                if(alllen < size)
                {
                    //left -= len;
                    //printf("read1 len:%d,alllen:%d,size:%d\n",len,alllen,size);
                    ptmp += len;
                }
                else
                {      
                    return size;
                }
        } 
        else 
        {  
            perror("select error");  
            return -1;  
        }  
      
    }  
    return 0;
}  

int rs232_603::sendnTTY(TTY_INFO *ptty,unsigned char *pbuf,int size)
{
    int ret,nleft;
    unsigned char *ptmp;

    ret = 0;
    nleft = size;
    ptmp = pbuf;

    while(nleft>0)
    {
        pthread_mutex_lock(&ptty->mt);
        ret = write(ptty->fd,ptmp,nleft);
        pthread_mutex_unlock(&ptty->mt);
        if(ret >0)
        {
            nleft -= ret;
            ptmp += ret;
        }
         else
        {
            return ret;
        }

    }
    return size - nleft;
}

int rs232_603::RS232_OpenWithBaudRate(char* ttysid, int baudrate)
{
	g_ptty = readyTTY(ttysid);
	if(g_ptty == NULL)
	{
		printf("readyTTY(%s) error\n",ttysid);
		return -1;
	}
	
	if(setTTYSpeed(g_ptty,baudrate)>0)
	{
		printf("setTTYSpeed(%d) error\n",baudrate);
		return -1;
	}

	if(setTTYParity(g_ptty,8,'N',1)>0)
	{
		printf("setTTYParity() error\n");
		return -1;
	}

	return 0;
}

int rs232_603::RS232_Close()
{
	cleanTTY(g_ptty);
	return 0;
}

int rs232_603::RS232_Write(unsigned int length,unsigned char buffer[])
{
	int iRet = 0; 

	int iSendLen = sendnTTY(g_ptty, buffer, length);
	if(iSendLen < length)
	{
		printf("RS232_Write() error\n");
		return -1;
	}

	return 0;

}

int rs232_603::RS232_Read( unsigned int length,unsigned char buffer[])
{
	int iRet = 0;
	
	iRet = recvnTTY(g_ptty, buffer, length);

	return iRet;
}
