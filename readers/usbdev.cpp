#include "usbdev.h"

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
usbdev::usbdev()
{
	nUSB = NULL;
}


#define VID_CRT 0x23d8
#define PID_CRT 0x0285

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int usbdev::USB_CloseAll(void)
{	int i;

	for(i=0; i<NO_MAX_USB; i++)
	{
//		if(hUSB[i] == NULL)
//			 continue;
//		libusb_release_interface(hUSB[i], 0);
//		libusb_attach_kernel_driver(hUSB[i], 0);  			
//		libusb_close(hUSB[i]);
//		hUSB[i] = NULL;
	}
	libusb_exit(NULL);
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int usbdev::USB_Close(struct libusb_device_handle  *nONE )
{
	if(nONE == NULL)
		return 0x01;
	libusb_release_interface(nONE, 0);
	libusb_attach_kernel_driver(nONE, 0);  			
	libusb_close(nONE);
	nONE = NULL;
	libusb_exit(NULL);
	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int usbdev::USB_Close(void)
{
	if(nUSB == NULL)
		return 0;

	libusb_release_interface(nUSB, 0);
	libusb_attach_kernel_driver(nUSB, 0);  			
	libusb_close(nUSB);
	nUSB = NULL;
	libusb_exit(NULL);

	return 0;
}

/******************************************************************************/
/*                                                                            */
/******************************************************************************/
int usbdev::USB_Write(unsigned int length,unsigned char buffer[])
{	int r, size;

	r = libusb_bulk_transfer(nUSB,
					0x01,
					buffer,
					length,
					&size,
					1000);
	if (r < 0)
	{
	    fprintf(stderr, "USB_Write libusb_interrupt_transfer  %d\n", r);
	    return r;
	}
	return 0;
}

/******************************************************************************/
/*                      Read para Lectores Civintec                           */
/******************************************************************************/
int usbdev::USB_Read2( unsigned int length, unsigned char buffer[], int smax)
{	int r = 0;
	int cRecvLen = 64;
	unsigned int i = 0;
	int size;

	r = libusb_bulk_transfer(nUSB,  
					0x81,  
					buffer,  
					cRecvLen,  
					&size,  
					smax);	// timeout= 40 mseg  
	if (r < 0)
	{
//		fprintf(stderr, "USB_Read libusb_interrupt_transfer  %d\n",r);
		return 0;
	}

	return size;
}


/******************************************************************************/
/*  Read para Lector de CRT                                                   */
/******************************************************************************/
int usbdev::USB_Read( unsigned int length,unsigned char buffer[])
{	int r = 0;
	int cRecvLen = 64;
	unsigned int i = 0;
	int size;

	r = libusb_interrupt_transfer(	nUSB,  
									0x81,  
									buffer,
									cRecvLen,
									&size,
									3000);

	if (r < 0) 
	{
		r = libusb_interrupt_transfer(	nUSB,  
									0x81,  
									buffer,
									cRecvLen,
									&size,
									1000);
//	    fprintf(stderr, "USB_Read libusb_interrupt_transfer  %d\n",r);
	}
	else
		r = 0;
	return r;
}
////////////////////////////////////////////////////////////////////////////////