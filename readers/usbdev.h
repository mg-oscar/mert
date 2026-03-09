
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//#include <libusb.h>
#include <libusb-1.0/libusb.h>


#ifndef __USBDEV__
#define __USBDEV__


//#define VID_THIS 0x04D8
//#define PID_THIS 0x000C

//#define ID_VENDOR	0x04D8
//#define ID_PRODUCT	0x000C

class usbdev 
{

public:
	usbdev();

	struct libusb_device_handle    *nUSB;

//	int USB_Open(void);

	int USB_CloseAll(void);
	int USB_Close( void );
	int USB_Close(struct libusb_device_handle  *nONE );
	int USB_Write(unsigned int length,unsigned char buffer[]);
	int USB_Read( unsigned int length,unsigned char buffer[]);
	int USB_Read2( unsigned int length,unsigned char buffer[], int smax);
	
private:
	int find_dpfp_device(void);

};

#endif //__USB_DEV__
