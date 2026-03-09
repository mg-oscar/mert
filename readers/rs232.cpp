/* 
 * File:   rs232.cpp
 * Author: famisa
 * 
 * Created on March 4, 2023, 5:00 PM
*/

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
rs232::rs232()
{
	g_ptty = NULL;
	apIniBufRx = 0;
	apFinBufRx = 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rs232::RS232_OpenWithBaudRate(char* ttysid, int baudrate)
{
	g_ptty = readyTTY(ttysid);
	if(g_ptty == NULL)
	{
		printf("readyTTY(%s) error\n",ttysid);
		return 1;
	}

	if(setTTYSpeed(g_ptty,baudrate)>0)
	{
		printf("setTTYSpeed(%d) error\n",baudrate);
		return -1;
	}

	if(setTTYParity(g_ptty,8,'N',1)>0)
	{
		printf("setTTYParity() error\n");
		return -2;
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rs232::RS232_Close()
{
	cleanTTY(g_ptty);
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rs232::RS232_Write(unsigned int length, unsigned char buffer[])
{
	int iSendLen = sendnTTY(g_ptty, buffer, length);
	if(iSendLen < length)
	{
		printf("RS232_Write() error\n");
		return -1;
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rs232::RS232_Read( void )
{	int iRet, i;
	unsigned char buffer[100];
		
	iRet = recvnTTY(g_ptty, buffer);
	if(iRet<0)
		return iRet;
//	printf("iRet=%d\n", iRet);
	for(i=0; i < iRet; i++)
	{
		putBufferRx(buffer[i]);
//		printf("%02X-",buffer[i]);
	}

	return iRet;
}

/******************************************************************************/
/*        Inicializa el Comm y guarda la configuración original               */
/******************************************************************************/
TTY_INFO *rs232::readyTTY(char* id)
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
/*     Limpia los recursos del dispositivo serie                              */
/******************************************************************************/
int rs232::cleanTTY(TTY_INFO *ptty)
{
	// cerrar/abrir dispositivo serie
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
int rs232::setTTYSpeed(TTY_INFO *ptty, int speed)
{
	// Get serial port settings
	if( tcgetattr(ptty->fd,&ptty->ntm) != 0)
	{
		printf("SetupSerial [%s]\n",ptty->name);
		return 1;
	}

	bzero(&ptty->ntm, sizeof(ptty->ntm));
	ptty->ntm.c_cflag = CS8 | CLOCAL | CREAD;
	ptty->ntm.c_iflag = IGNPAR;
	ptty->ntm.c_oflag = 0;

	switch(speed)
	{	case 300:
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

	tcflush(ptty->fd, TCIFLUSH);
	tcsetattr(ptty->fd,TCSANOW,&ptty->ntm);

	return 0;
}

/******************************************************************************/
/* Bits de Datos, Paridad, Stop Bit                                           */
/******************************************************************************/
int rs232::setTTYParity(TTY_INFO *ptty,int databits,int parity,int stopbits)
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
    
	switch (parity)
	{	
		case 'n':
		case 'N':
			ptty->ntm.c_cflag &= ~PARENB; /* Clear parity enable */
			ptty->ntm.c_iflag &= ~INPCK; /* Enable parity checking */
			break;
		case 'o':
		case 'O':
			ptty->ntm.c_cflag |= (PARODD|PARENB); /* 设置为奇效验*/
			ptty->ntm.c_iflag |= INPCK; /* Disnable parity checking */
			break;
		case 'e':
		case 'E':
			ptty->ntm.c_cflag |= PARENB; /* Enable parity */
			ptty->ntm.c_cflag &= ~PARODD; /* 转换为偶效验*/
			ptty->ntm.c_iflag |= INPCK; /* Disnable parity checking */
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
	
//	ptty->ntm.c_lflag = 0;
	
	    // setup for non-canonical mode
	ptty->ntm.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	ptty->ntm.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	ptty->ntm.c_oflag &= ~OPOST;

	
	ptty->ntm.c_cc[VTIME] = 0; // inter-character timer unused
	ptty->ntm.c_cc[VMIN] = 0; // blocking read until 1 chars received

	tcflush(ptty->fd, TCIFLUSH);
	if (tcsetattr(ptty->fd,TCSANOW,&ptty->ntm) != 0)
	{
		printf("Error SetupSerial \n");
		return 4;
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rs232::recvnTTY(TTY_INFO *ptty, unsigned char *pbuf)  
{	int len=0, ret = 0;  
	fd_set fs_read;  
	struct timeval tv_timeout; 
	
	FD_ZERO(&fs_read);  
	FD_SET(ptty->fd, &fs_read);  
	tv_timeout.tv_sec  = 1;
	tv_timeout.tv_usec = 0;	//0;

	ret = select(ptty->fd+1, &fs_read, NULL, NULL, &tv_timeout); 
	
	if(ret == 0) 
	{
//		printf("recvnTTY timeout\n");
		return -2;
	}

	if (FD_ISSET(ptty->fd, &fs_read))
	{
		ioctl(ptty->fd, FIONREAD, &ret);	// lee cuantos datos se recibieron
		if(ret <= 0)
			return -3;		// no hay datos

		len = read(ptty->fd, pbuf, ret);
		if(len==0)
			return -3;	// no se recibieron datos
		return len;
	}
	else 
	{  
		perror("select error");  
		return -1;  
	}
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rs232::sendnTTY(TTY_INFO *ptty, unsigned char *pbuf, int size)
{	int ret,nleft;
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
/******************************************************************************/
/* borra el buffer circular                                                   */
/******************************************************************************/
void rs232::clrBufferRx(void)
{
	apIniBufRx = apFinBufRx;
}

/******************************************************************************/
/* toma un dato del buffer circular                                           */
/******************************************************************************/
unsigned char rs232::getBufferRx(void)
{
	if(apIniBufRx >= SIZE_BUFFER_RX)
		apIniBufRx = 0;
	return bufRx[apIniBufRx++];
}

/******************************************************************************/
/* inserta un dato al buffer circular                                         */
/******************************************************************************/
void rs232::putBufferRx(char d)
{
	if(apFinBufRx>= SIZE_BUFFER_RX)
		apFinBufRx = 0;
	bufRx[apFinBufRx++] = d;
}
/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int rs232::existDat(void)
{
	if(apIniBufRx==apFinBufRx)
		return 0x00;
	return 0x01;
	
}
////////////////////////////////////////////////////////////////////////////////


