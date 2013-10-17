/******************************************************************************
 *
 * Copyright:
 *    (C) 2000 - 2007 Embedded Artists AB
 *
 *****************************************************************************/


#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"
#include <printf_P.h>
#include <lpc2xxx.h>
#include <consol.h>
#include "uart.h"

extern volatile tU32 msClock;

/*****************************************************************************
 *
 * Description:
 *    
 *
 ****************************************************************************/
tU8
testXBee(void)
{
  tU32 rxChars = 0;
  tU32 timeStamp;

  printf("\n\n*******************************************************\n");
  printf("*                                                     *\n");
  printf("* Communication with the XBee ZigBee module is tested *\n");
  printf("*                                                     *\n");
  printf("*******************************************************\n");
  printf("*  Communication with XBee module below:              *\n\n");

  //initialize uart #1: 9600 bps, 8N1, no FIFO
  initUart1(B9600((FOSC * PLL_MUL) / PBSD), UART_8N1, UART_FIFO_4);

  IODIR0 |= 0x00008000;
  IOSET0  = 0x00008000;
  IOCLR0  = 0x00008000;
  osSleep(1);
  IOSET0  = 0x00008000;

  IODIR0 |= 0x00000400;  //P0.10-RTS output
  IOCLR0  = 0x00000400;
  IODIR0 |= 0x00002000;  //P0.13-DTR output
  IOSET0  = 0x00002000;
  
  osSleep(20);
  uart1SendCh('+');
  uart1SendCh('+');
  uart1SendCh('+');
  osSleep(110);
  uart1SendCh('A');
  uart1SendCh('T');
  uart1SendCh('V');
  uart1SendCh('L');
  uart1SendCh('\n');
  rxChars = 0;

  //get current time
  timeStamp = msClock;

  while((msClock - timeStamp) < 1500)
  {
    tU8 rxChar;
    
    //check if any character has been received
    if (TRUE == uart1GetChar(&rxChar))
    {
      rxChars++;
      if (rxChar == '\r')
        printf("\n%c", rxChar);
      else
        printf("%c", rxChar);
    }
  }
  
  if (rxChars > 75)
  {
    printf("\n*******************************************************");
    printf("\n*  XBee module detected!!!                            *");
    printf("\n*******************************************************\n\n\n");
    return 1; //= XBee module present
  }
  else
  {
    printf("\n*******************************************************");
    printf("\n*  No XBee module detected...                         *");
    printf("\n*******************************************************\n\n\n");
    return 0; //= no XBee module present
  }
}
