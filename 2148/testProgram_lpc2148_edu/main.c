/******************************************************************************
 *
 * Copyright:
 *    (C) 2000 - 2007 Embedded Artists AB
 *
 * Description:
 *    Main program for LPC2148 Education Board test program
 *
 *****************************************************************************/


#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"
#include <printf_P.h>
#include <ea_init.h>
#include <lpc2xxx.h>
#include <consol.h>

#include "usb/lpc_usb.h"
#include "usb/lpc_hid.h"

#define PROC1_STACK_SIZE 2048
#define PROC2_STACK_SIZE 2048
#define PROC3_STACK_SIZE 2048
#define PROC4_STACK_SIZE 2048
#define PROC5_STACK_SIZE 2048
#define INIT_STACK_SIZE  400

static tU8 proc1Stack[PROC1_STACK_SIZE];
static tU8 proc2Stack[PROC2_STACK_SIZE];
static tU8 proc3Stack[PROC3_STACK_SIZE];
static tU8 proc4Stack[PROC4_STACK_SIZE];
static tU8 proc5Stack[PROC5_STACK_SIZE];
static tU8 initStack[INIT_STACK_SIZE];
static tU8 pid1;
static tU8 pid2;
static tU8 pid3;
static tU8 pid4;
static tU8 pid5;


static void proc1(void* arg);
static void proc2(void* arg);
static void proc3(void* arg);
static void proc4(void* arg);
static void proc5(void* arg);
static void initProc(void* arg);

void testLedMatrix(void);
void testLcd(void);
void testMotor(void);
void testRGB(void);
void testI2C(void);
//void testMMC(void);
void testAdc(void);
tU8  testXBee(void);

tU8 xbeePresent;
volatile tU32 msClock = 0;
extern char startupSound[];


/*****************************************************************************
 *
 * Description:
 *    The first function to execute 
 *
 ****************************************************************************/
int
main(void)
{
  tU8 error;
  tU8 pid;

  //immediately turn off buzzer (if connected)
  IODIR0 |= 0x00000080;
  IOSET0  = 0x00000080;
  
  osInit();
  osCreateProcess(initProc, initStack, INIT_STACK_SIZE, &pid, 1, NULL, &error);
  osStartProcess(pid, &error);
  
  osStart();
  return 0;
}

/*****************************************************************************
 *
 * Description:
 *    A process entry function 
 *
 * Params:
 *    [in] arg - This parameter is not used in this application. 
 *
 ****************************************************************************/
static void
proc1(void* arg)
{
	tU8  centerReleased = TRUE;
	tU8  i;
	tU32 pattern;
  tU8  error;

  {
    tU32 cnt = 0;
    tU32 i;
    
	  IODIR |= 0x00000380;
	  IOCLR  = 0x00000380;

    //
    //Initialize DAC: AOUT = P0.25
    //
    PINSEL1 &= ~0x000C0000;
    PINSEL1 |=  0x00080000;

    cnt = 0;
    while(cnt++ < 0xF890)
    {
      tS32 val;
      
      val = startupSound[cnt] - 128;
      val = val * 2;
      if (val > 127) val = 127;
      else if (val < -127) val = -127;

      DACR = ((val+128) << 8) |  //actual value to output
             (1 << 16);         //BIAS = 1, 2.5uS settling time

      //delay 125 us = 850 for 8kHz, 600 for 11 kHz
      for(i=0; i<850; i++)
        asm volatile (" nop");
    }

  }

	for(;;)
	{
    printf("\n\n\n\n\n*******************************************************\n");
    printf("*                                                     *\n");
    printf("* This is the test program for Embedded Artists'      *\n");
    printf("* LPC2148 Education Board v3.0...                     *\n");
    printf("*                                                     *\n");
    printf("* Version: 3.0                                        *\n");
    printf("* Date:    22 December 2007                           *\n");
    printf("* (C) Embedded Artists 2005-2007                      *\n");
    printf("*                                                     *\n");
    printf("*******************************************************\n");

    //
    //Start USB
    //
    printf("\n\nStarting USB Mouse test...\n");
    printf("Use the joystick switch to move the cursor on the PC screen\n");
    printf("(functions when the ADC test is visible)\n");
    if (USB_Init(0,HID_CallBack,USB_NotFast))
    {
      printf("\nERROR initializing USB!\n");
      for(;;)
        osSleep(1);
    }
    
    //Initialize HID
    HID_Init();


    //
    //Test XBee module (if present)
    //
    xbeePresent = testXBee();


    //
    //Test LEDs
    //
    PINSEL0 &= 0x0000ffff;
    if (1 == xbeePresent)
      //do not use P0.9 and P0.11
      IODIR0  |= 0x0000f500;
    else
      IODIR0  |= 0x0000ff00;
    IOSET0   = 0x0000ff00;

    for(i=0; i<3; i++)
    {
      pattern = 0x00000100;
      while(pattern <= 0x00008000)
      {
      	IOCLR = pattern;
      	osSleep(15);
      	IOSET0 = pattern;
      	
      	pattern <<= 1;
      }
    }
    

    //
    //Test EEPROM via I2C
    //
    testI2C();


    //
    //Test MMC/SD via SPI
    //
    //testMMC();
    

    //
    //Start the rest of the processes
    //
    //osCreateProcess(proc2, proc2Stack, PROC2_STACK_SIZE, &pid2, 3, NULL, &error);
    //osStartProcess(pid2, &error);
    osCreateProcess(proc3, proc3Stack, PROC3_STACK_SIZE, &pid3, 3, NULL, &error);
    osStartProcess(pid3, &error);
    //osCreateProcess(proc4, proc4Stack, PROC4_STACK_SIZE, &pid4, 3, NULL, &error);
    //osStartProcess(pid4, &error);
    //osCreateProcess(proc5, proc5Stack, PROC5_STACK_SIZE, &pid5, 3, NULL, &error);
    //osStartProcess(pid5, &error);


    //wait for a short while
    osSleep(100);
    
    IOPIN &= ~0x001f0000;
    for(;;)
    {
		  if ((IOPIN & 0x00100000) == 0)
        HID_SendReport(0,0,10);
		  else if ((IOPIN & 0x00080000) == 0)
        HID_SendReport(0,-10,0);
		  else if ((IOPIN & 0x00040000) == 0)
        HID_SendReport(0,10,0);
		  else if ((IOPIN & 0x00020000) == 0)
        HID_SendReport(0,0,-10);

      if ((IOPIN & 0x00010000) == 0)
      {
      	if (centerReleased == TRUE)
      	{
      		HID_SendReport(1,0,0);
      		centerReleased = FALSE;
      	}
      }
      else
      {
      	if (centerReleased == FALSE)
      	  HID_SendReport(0,0,0);
      	centerReleased = TRUE;
      }

      osSleep(2);
    }
  }
}

/*****************************************************************************
 *
 * Description:
 *    ledMatrix
 *
 * Params:
 *    [in] arg - This parameter is not used in this application. 
 *
 ****************************************************************************/
static void
proc2(void* arg)
{
	testLedMatrix();
}

/*****************************************************************************
 *
 * Description:
 *    Wyœwietlacz LCD
 *
 * Params:
 *    [in] arg - This parameter is not used in this application. 
 *
 ****************************************************************************/
static void
proc3(void* arg)
{
	testLcd();
}

/*****************************************************************************
 *
 * Description:
 *    RGB i Motor
 *
 * Params:
 *    [in] arg - This parameter is not used in this application. 
 *
 ****************************************************************************/
static void
proc4(void* arg)
{
	for(;;)
	{
		testRGB();
		testMotor();
	}
}

/*****************************************************************************
 *
 * Description:
 *    Acd
 *
 * Params:
 *    [in] arg - This parameter is not used in this application. 
 *
 ****************************************************************************/
static void
proc5(void* arg)
{
	testAdc();
}

/*****************************************************************************
 *
 * Description:
 *    The entry function for the initialization process. 
 *
 * Params:
 *    [in] arg - This parameter is not used in this application. 
 *
 ****************************************************************************/
static void
initProc(void* arg)
{
  tU8 error;

  eaInit();
  osCreateProcess(proc1, proc1Stack, PROC1_STACK_SIZE, &pid1, 3, NULL, &error);
  osStartProcess(pid1, &error);

  osDeleteProcess();
}

/*****************************************************************************
 *
 * Description:
 *    The timer tick entry function that is called once every timer tick
 *    interrupt in the RTOS. Observe that any processing in this
 *    function must be kept as short as possible since this function
 *    execute in interrupt context.
 *
 * Params:
 *    [in] elapsedTime - The number of elapsed milliseconds since last call.
 *
 ****************************************************************************/
void
appTick(tU32 elapsedTime)
{
  msClock += elapsedTime;
}
