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

/*****************************************************************************
 *
 * Description:
 *    A process entry function. 
 *
 ****************************************************************************/
void
testMotor(void)
{
  static tU32 stepmotorSteps[4] = {0x00201000, 0x00200000, 0x00000000, 0x00001000};  //P0.21 and P0.12 are used to contol the stepper motor
  static tU8 stepmotorIndex = 0;
  tU8 i;
  
  
  IODIR0 |= 0x00201000;
  IOCLR0  = stepmotorSteps[0];

  for(i=0; i<4*3; i++)
  {  
    //update to new step (forward)
    stepmotorIndex = (stepmotorIndex + 1) & 0x03;

    //output new step
    IOCLR0 = stepmotorSteps[0];
    IOSET0 = stepmotorSteps[stepmotorIndex];
    osSleep(10);
  }

  for(i=0; i<4*3; i++)
  {  
    //update to new step (backwards)
    if (stepmotorIndex == 0)
      stepmotorIndex = 3;
    else
      stepmotorIndex--;

    //output new step
    IOCLR0 = stepmotorSteps[0];
    IOSET0 = stepmotorSteps[stepmotorIndex];
    osSleep(4);
  }

}

