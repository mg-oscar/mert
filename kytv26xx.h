/* 
 * File:   kytv26xx.h
 * Author: saul_
 *
 * Created on 21 de octubre de 2024, 07:54 PM
 */

#ifndef KYTV26XX_H
#define KYTV26XX_H

#define STX		0x02
#define ETX		0x03
#define EQU		0x05
#define ACK		0x06
#define NAK		0x15
#define CAN		0x18


#define KYT_STK0			0x00		// Stacker Auto
#define KYT_STK1			0x01		// Stacker 1
#define KYT_STK2			0x02		// Stacker 2

#define KYT_STACKER_EMPTY		0x01		//

#define CMD_Clear			0x30
#define CMD_StatusRequest	0x31
#define CMD_Stacker1_Out	0x40
#define CMD_Stacker2_Out	0x41
#define CMD_AutoIssue		0x42
#define CMD_Capture			0x43
#define CMD_FeedIn			0x44
#define CMD_FeedOut			0x45
#define CMD_Stop			0x46
#define CMD_Stack1Wait		0x47
#define CMD_Stack2Wait		0x48
#define CMD_AutoWait		0x49
#define CMD_FeedOutSol		0x4A		
#define CMD_B9600			0x50
#define CMD_B19200			0x51
#define CMD_Version			0x60
#define CMD_STANDBY1		0x4C
#define CMD_STANDBY2		0x4D
#define CMD_STANDBYAUTO		0x4E

#define KYT_TIMEOUT			500

typedef struct
{
	unsigned char fgDummy0;					// ST1		b7  always = 1
	unsigned char fgReady;					// ST1		b6       
	unsigned char fgStack2FinishSensor;		// ST1		b5
	unsigned char fgStack1FinishSensor;		// ST1		b4     
	unsigned char fgDummy1;					// ST1		b3  always = 0
	unsigned char fgFinishSensor;			// St1		b2
	unsigned char fgMoveSensor;				// St1		b1
	unsigned char fgBinEmpty;				// St1		b0
	unsigned char fgDummy2;					// ST2		b7  always = 1
	unsigned char fgMotor3;					// ST2		b6  Error = 1   Good = 0 
	unsigned char fgMotor2;					// ST2		b5  Error = 1   Good = 0
	unsigned char fgMotor1;					// ST2		b4  Error = 1   Good = 0
	unsigned char fgStack2Warning;			// ST2		b3  Warning = 1 Good = 0
	unsigned char fgStack1Warning;			// ST2		b2  Warning = 1 Good = 0
	unsigned char fgStack2Empty;			// ST2		b1  Empty = 1   Good = 0
	unsigned char fgStack1Empty;			// ST2		b0  Empty = 1   Good = 0
	unsigned int  fgKytW;					// condensed flag
} FLAG_KYT;

FLAG_KYT fgKyt;             // flag individuales

unsigned int fgAnt=0x0000;

class CLASS_KYT26xx: public serialport
{
public:
	CLASS_KYT26xx();

	unsigned char st0Kyt;		// status kyt
	unsigned char st1Kyt;		// status kyt
	int kytNoCard;
	
	unsigned char Stacker[2];		// Status del Stacker

	int KytStatus(void);
	int KytInit(char *namecomm, int baudrate);
	int KytClear(void);
	int KytDispensa1(void);
	int KytDispensa2(void);
	int KytDispensa(unsigned char NoStk);
	int KytCaptura(void);
	int KytEject(void);


private:
	
	unsigned short int Version;
	
	int KytRdACK(void);
	int KytrdResponse(unsigned char *buf);
	int wrCmdKyt(int cmd);
	int KytWrACK(void);
	int KytVersion(void);

	int mCMD_B9600(void);
	int mCMD_B19200(void);
	void showKyt(unsigned char b1,unsigned char b2);

};


/*_______________________________________________________________________________________
 |   BIT     |                   0                   |               1                   |
 |---------------------------------------------------------------------------------------|
 |   b15     |                                       | Always                            |
 |   b14     | Ready                                 | Busy(Inhibit)                     |
 |   b13     | StackÂ·2  Finish Sensor Non-Detectio   | Statck#2 Finish Sensor Detection |
 |   b12     | Stack#1  Finish Sensor Non-Detection  | Statck#2 Finish Sensor Detection  |
 |   b11     |               --                      |           --                      |
 |   b10     | FInish Sensor Non-Detection           | Finish Sensor Detection           |
 |   b9      | Move Sensor Non-Detection             | Move Sensor Detection             |
 |   b8      | Bin Empty                             | Error Bin Full                    |
 * ///////////////////////////////////////////////////////////////////////////////////////
 |   b7      |                                       | Alway 1                           |
 |   b6      | Motor#3 Good                          | Motor#3 Error                     |
 |   b5      | Motor#2 Good                          | Motor#2 Error                     |
 |   b4      | Motor#1 Good                          | Motor#1 Error                     |
 |   b3      | Stack#2 Good                          | Stack#2 Error                     |
 |   b2      | Stack#1 Good                          | Stack#1 Error                     |
 |   b1      | Stack#2 Good                          | Stack#2 Empty                     |
 |   b0      | Stack#1 Good                          | Stack#1 Empty                     |
 -----------------------------------------------------------------------------------------
*/
#endif /* KYTV26XX_H */

