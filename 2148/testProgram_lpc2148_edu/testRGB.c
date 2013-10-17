/******************************************************************************
 *
 * Copyright:
 *    (C) 2000 - 2005 Embedded Artists AB
 *
 *****************************************************************************/


#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"
#include <printf_P.h>
#include <lpc2xxx.h>

extern tU8 xbeePresent;

/*****************************************************************************
 *
 * Description:
 *    A process entry function. 
 *
 ****************************************************************************/
void
testRGB(void)
{
	tU8  continueTest;
	tU32 counter = 0;
	tU8  direction = 0;
	tU8  led = 0;
	
	if (1 == xbeePresent)
	{
    PINSEL0 &= 0xfff03fff;  //Enable PWM2 on P0.7, PWM4 on P0.8, and PWM6 not used
    PINSEL0 |= 0x00028000;  //Enable PWM2 on P0.7, PWM4 on P0.8, and PWM6 not used
	}
	else
	{
    PINSEL0 &= 0xfff03fff;  //Enable PWM2 on P0.7, PWM4 on P0.8, and PWM6 on P0.9
    PINSEL0 |= 0x000a8000;  //Enable PWM2 on P0.7, PWM4 on P0.8, and PWM6 on P0.9
  }

  //PULSE WIDTH MODULATION INIT*********************************************
  PWM_PR  = 0x00;    // Prescale Register
  PWM_MCR = 0x02;    // Match Control Register
  PWM_MR0 = 0x1000;    // TOTAL PERIODTID   T
  PWM_MR2 = 0x0000;    // HÖG SIGNAL        t
  PWM_MR4 = 0x0000;    // HÖG SIGNAL        t
  PWM_MR6 = 0x0000;    // HÖG SIGNAL        t
  PWM_LER = 0x55;    // Latch Enable Register
  PWM_PCR = 0x5400;  // Prescale Counter Register PWMENA2, PWMENA4, PWMENA6
  PWM_TCR = 0x09;    // Counter Enable och PWM Enable
  //************************************************************************ 

  continueTest = TRUE;
  while(continueTest == TRUE)
  {
    if (direction == 0)
    {
    	if (counter < 0x0080)
    	  counter += 0x0005;
    	else
        counter += 0x000f;

      if (counter > 0x0300)
      {
        direction = 1;
      }
    }
    else
    {
  	  if (counter == 0x0000)
  	  {
  		  direction = 0;
  		  led++;
        osSleep(40);
  		  if (led > 2)
  		  {
  		    led = 0;
continueTest = FALSE;
  		  }
  		}
      else
      {
      	if (counter < 0x0080)
     	  counter -= 0x0005;
      	else
          counter -= 0x000f;
      }
    }
    
    switch(led)
    {
    	case 0:
      PWM_MR2 = counter;    // HÖG SIGNAL
      PWM_LER = 0x04;    // Latch Enable Register
    	break;
    	case 1:
      PWM_MR4 = counter;    // HÖG SIGNAL
      PWM_LER = 0x10;    // Latch Enable Register
    	break;
    	case 2:
      PWM_MR6 = counter;    // HÖG SIGNAL
      PWM_LER = 0x40;    // Latch Enable Register
    	break;
    	default:
    	led = 0;
    	break;
    }

    osSleep(1);
  }
}
