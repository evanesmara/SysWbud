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
#include <consol.h>
#include <config.h>

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD

#define  SPI_CS   0x00008000  //<= new board, old board = 0x00800000


const tU8 eaText[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01,
	0x02,0x04,0x08,0x10,0x20,0x40,0x80,
	0x40,0x20,0x10,0x08,0x04,0x02,0x01,
	0x02,0x04,0x08,0x10,0x20,0x40,0x80,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00,
  0x7f,0x49,0x49,0x41,0x00,      //E
  0x7f,0x02,0x04,0x02,0x7f,0x00, //M
  0x7f,0x49,0x49,0x36,0x00,      //B
  0x7f,0x49,0x49,0x41,0x00,      //E
  0x7f,0x41,0x41,0x3e,0x00,      //D
  0x7f,0x41,0x41,0x3e,0x00,      //D
  0x7f,0x49,0x49,0x41,0x00,      //E
  0x7f,0x41,0x41,0x3e,0x00,      //D
  0x00,0x00,0x00,0x00,0x00,
  0x7e,0x09,0x09,0x7e,0x00,      //A
  0x7f,0x19,0x29,0x46,0x00,      //R
  0x01,0x01,0x7f,0x01,0x01,0x00, //T
  0x7f,0x00,                     //I
  0x26,0x49,0x49,0x32,0x00,      //S
  0x01,0x01,0x7f,0x01,0x01,0x00, //T
  0x26,0x49,0x49,0x32,0x00,      //S
	0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00};

tU8 pattern[8] = {0,0,0,0,0,0,0,0};

void ledMatrix(void);


/*****************************************************************************
 *
 * Description:
 *
 ****************************************************************************/
static void
startTimer1(tU16 delayInMs)
{
  //initialize VIC for Timer1 interrupts
  VICIntSelect &= ~0x20;           //Timer1 interrupt is assigned to IRQ (not FIQ)
  VICVectAddr5  = (tU32)ledMatrix; //register ISR address
  VICVectCntl5  = 0x25;            //enable vector interrupt for timer1
  VICIntEnable  = 0x20;            //enable timer1 interrupt
  
  //initialize and start Timer #0
  T1TCR = 0x00000002;                           //disable and reset Timer1
  T1PC  = 0x00000000;                           //no prescale of clock
  T1MR0 = delayInMs *                           //calculate no of timer ticks
         ((CRYSTAL_FREQUENCY * PLL_FACTOR) / (1000 * VPBDIV_FACTOR));
  T1IR  = 0x000000ff;                           //reset all flags before enable IRQs
  T1MCR = 0x00000003;                           //reset counter and generate IRQ on MR0 match
  T1TCR = 0x00000001;                           //start Timer1
}


/*****************************************************************************
 *
 * Description:
 *    A process entry function 
 *
 ****************************************************************************/
void
testLedMatrix(void)
{
 	tU8 cntA = 0;

  PINSEL0 |= 0x00001500 ;  //Initiering av SPI
  SPI_SPCCR = 0x08;    
  SPI_SPCR  = 0x60;
  IODIR0 |= SPI_CS;
  
  startTimer1(2);

  for(;;)
  {
  	osSleep(16);

    cntA++;
    if (cntA > sizeof(eaText)-8)
      cntA = 0;

#if 1
pattern[0] = eaText[cntA+0];
pattern[1] = eaText[cntA+1];
pattern[2] = eaText[cntA+2];
pattern[3] = eaText[cntA+3];
pattern[4] = eaText[cntA+4];
pattern[5] = eaText[cntA+5];
pattern[6] = eaText[cntA+6];
pattern[7] = eaText[cntA+7];
#else
pattern[7] = ((eaText[cntA]   & 0x01)   ) | ((eaText[cntA+1] & 0x01)<<1) | ((eaText[cntA+2] & 0x01)<<2) | ((eaText[cntA+3] & 0x01)<<3) |
             ((eaText[cntA+4] & 0x01)<<4) | ((eaText[cntA+5] & 0x01)<<5) | ((eaText[cntA+6] & 0x01)<<6) | ((eaText[cntA+7] & 0x01)<<7);
pattern[6] = ((eaText[cntA]   & 0x02)>>1) | ((eaText[cntA+1] & 0x02))    | ((eaText[cntA+2] & 0x02)<<1) | ((eaText[cntA+3] & 0x02)<<2) |
             ((eaText[cntA+4] & 0x02)<<3) | ((eaText[cntA+5] & 0x02)<<4) | ((eaText[cntA+6] & 0x02)<<5) | ((eaText[cntA+7] & 0x02)<<6);
pattern[5] = ((eaText[cntA]   & 0x04)>>2) | ((eaText[cntA+1] & 0x04)>>1) | ((eaText[cntA+2] & 0x04)   ) | ((eaText[cntA+3] & 0x04)<<1) |
             ((eaText[cntA+4] & 0x04)<<2) | ((eaText[cntA+5] & 0x04)<<3) | ((eaText[cntA+6] & 0x04)<<4) | ((eaText[cntA+7] & 0x04)<<5);
pattern[4] = ((eaText[cntA]   & 0x08)>>3) | ((eaText[cntA+1] & 0x08)>>2) | ((eaText[cntA+2] & 0x08)>>1) | ((eaText[cntA+3] & 0x08)   ) |
             ((eaText[cntA+4] & 0x08)<<1) | ((eaText[cntA+5] & 0x08)<<2) | ((eaText[cntA+6] & 0x08)<<3) | ((eaText[cntA+7] & 0x08)<<4);
pattern[3] = ((eaText[cntA]   & 0x10)>>4) | ((eaText[cntA+1] & 0x10)>>3) | ((eaText[cntA+2] & 0x10)>>2) | ((eaText[cntA+3] & 0x10)>>1) |
             ((eaText[cntA+4] & 0x10)   ) | ((eaText[cntA+5] & 0x10)<<1) | ((eaText[cntA+6] & 0x10)<<2) | ((eaText[cntA+7] & 0x10)<<3);
pattern[2] = ((eaText[cntA]   & 0x20)>>5) | ((eaText[cntA+1] & 0x20)>>4) | ((eaText[cntA+2] & 0x20)>>3) | ((eaText[cntA+3] & 0x20)>>2) |
             ((eaText[cntA+4] & 0x20)>>1) | ((eaText[cntA+5] & 0x20)   ) | ((eaText[cntA+6] & 0x20)<<1) | ((eaText[cntA+7] & 0x20)<<2);
pattern[1] = ((eaText[cntA]   & 0x40)>>6) | ((eaText[cntA+1] & 0x40)>>5) | ((eaText[cntA+2] & 0x40)>>4) | ((eaText[cntA+3] & 0x40)>>3) |
             ((eaText[cntA+4] & 0x40)>>2) | ((eaText[cntA+5] & 0x40)>>1) | ((eaText[cntA+6] & 0x40)   ) | ((eaText[cntA+7] & 0x40)<<1);
pattern[0] = ((eaText[cntA]   & 0x80)>>7) | ((eaText[cntA+1] & 0x80)>>6) | ((eaText[cntA+2] & 0x80)>>5) | ((eaText[cntA+3] & 0x80)>>4) |
             ((eaText[cntA+4] & 0x80)>>3) | ((eaText[cntA+5] & 0x80)>>2) | ((eaText[cntA+6] & 0x80)>>1) | ((eaText[cntA+7] & 0x80)   );
#endif

#if 0
  			if (++cntA >= 10)
  			  cntA = 0;
  			switch(cntA)
  			{
  				case 0:
  				pattern[0] = 0xff;
  				pattern[1] = 0x01;
  				pattern[2] = 0x01;
  				pattern[3] = 0x01;
  				pattern[4] = 0x01;
  				pattern[5] = 0x01;
  				pattern[6] = 0x01;
  				pattern[7] = 0x01;
  				break;
  				case 1:
  				pattern[0] = 0x02;
  				pattern[1] = 0xff;
  				pattern[2] = 0x02;
  				pattern[3] = 0x02;
  				pattern[4] = 0x02;
  				pattern[5] = 0x02;
  				pattern[6] = 0x02;
  				pattern[7] = 0x02;
  				break;
  				case 2:
  				pattern[0] = 0x04;
  				pattern[1] = 0x04;
  				pattern[2] = 0xff;
  				pattern[3] = 0x04;
  				pattern[4] = 0x04;
  				pattern[5] = 0x04;
  				pattern[6] = 0x04;
  				pattern[7] = 0x04;
  				break;
  				case 3:
  				pattern[0] = 0x08;
  				pattern[1] = 0x08;
  				pattern[2] = 0x08;
  				pattern[3] = 0xff;
  				pattern[4] = 0x08;
  				pattern[5] = 0x08;
  				pattern[6] = 0x08;
  				pattern[7] = 0x08;
  				break;
  				case 4:
  				pattern[0] = 0x10;
  				pattern[1] = 0x10;
  				pattern[2] = 0x10;
  				pattern[3] = 0x10;
  				pattern[4] = 0xff;
  				pattern[5] = 0x10;
  				pattern[6] = 0x10;
  				pattern[7] = 0x10;
  				break;
  				case 5:
  				pattern[0] = 0x20;
  				pattern[1] = 0x20;
  				pattern[2] = 0x20;
  				pattern[3] = 0x20;
  				pattern[4] = 0x20;
  				pattern[5] = 0xff;
  				pattern[6] = 0x20;
  				pattern[7] = 0x20;
  				break;
  				case 6:
  				pattern[0] = 0x40;
  				pattern[1] = 0x40;
  				pattern[2] = 0x40;
  				pattern[3] = 0x40;
  				pattern[4] = 0x40;
  				pattern[5] = 0x40;
  				pattern[6] = 0xff;
  				pattern[7] = 0x40;
  				break;
  				case 7:
  				pattern[0] = 0x80;
  				pattern[1] = 0x80;
  				pattern[2] = 0x80;
  				pattern[3] = 0x80;
  				pattern[4] = 0x80;
  				pattern[5] = 0x80;
  				pattern[6] = 0x80;
  				pattern[7] = 0xff;
  				break;
  				case 8:
  				case 10:
  				case 12:
  				pattern[0] = 0x00;
  				pattern[1] = 0x00;
  				pattern[2] = 0x00;
  				pattern[3] = 0x00;
  				pattern[4] = 0x00;
  				pattern[5] = 0x00;
  				pattern[6] = 0x00;
  				pattern[7] = 0x00;
  				break;
  				case 9:
  				case 11:
  				pattern[0] = 0xff;
  				pattern[1] = 0xff;
  				pattern[2] = 0xff;
  				pattern[3] = 0xff;
  				pattern[4] = 0xff;
  				pattern[5] = 0xff;
  				pattern[6] = 0xff;
  				pattern[7] = 0xff;
  				break;


  				case 13:
  				case 14:
  				case 15:
  				pattern[0] = 0x01;
  				pattern[1] = 0x01;
  				pattern[2] = 0x01;
  				pattern[3] = 0x01;
  				pattern[4] = 0x01;
  				pattern[5] = 0x01;
  				pattern[6] = 0x01;
  				pattern[7] = 0x01;
  				break;
  				case 16:
  				pattern[0] = 0x01;
  				pattern[1] = 0x02;
  				pattern[2] = 0x02;
  				pattern[3] = 0x02;
  				pattern[4] = 0x02;
  				pattern[5] = 0x02;
  				pattern[6] = 0x02;
  				pattern[7] = 0x01;
  				break;
  				case 17:
  				pattern[0] = 0x01;
  				pattern[1] = 0x02;
  				pattern[2] = 0x04;
  				pattern[3] = 0x04;
  				pattern[4] = 0x04;
  				pattern[5] = 0x04;
  				pattern[6] = 0x02;
  				pattern[7] = 0x01;
  				break;
  				case 18:
  				pattern[0] = 0x01;
  				pattern[1] = 0x02;
  				pattern[2] = 0x04;
  				pattern[3] = 0x08;
  				pattern[4] = 0x08;
  				pattern[5] = 0x04;
  				pattern[6] = 0x02;
  				pattern[7] = 0x01;
  				break;
  				case 19:
  				pattern[0] = 0x01;
  				pattern[1] = 0x02;
  				pattern[2] = 0x04;
  				pattern[3] = 0x04;
  				pattern[4] = 0x04;
  				pattern[5] = 0x04;
  				pattern[6] = 0x02;
  				pattern[7] = 0x01;
  				break;
  				case 20:
  				pattern[0] = 0x01;
  				pattern[1] = 0x02;
  				pattern[2] = 0x02;
  				pattern[3] = 0x02;
  				pattern[4] = 0x02;
  				pattern[5] = 0x02;
  				pattern[6] = 0x02;
  				pattern[7] = 0x01;
  				break;
  				case 21:
  				pattern[0] = 0x01;
  				pattern[1] = 0x01;
  				pattern[2] = 0x01;
  				pattern[3] = 0x01;
  				pattern[4] = 0x01;
  				pattern[5] = 0x01;
  				pattern[6] = 0x01;
  				pattern[7] = 0x01;
  				break;


 				default:
 				cntA = 0;
 				break;
 			}
#endif
	}
}
