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

//extern tU8 xbeePresent;

/*****************************************************************************
 *
 * Description:
 *    A process entry function. 
 *
 ****************************************************************************/

static tU8 inicjalizacjaOdbyta = 0;
/*
 * 0x04 - czerwona
 * 0x10 - niebieska
 * 0x40 - zielona
 */
static void ZapalajDiode (tU8 kolor, int ifInit)
{
	if (ifInit == 0)
	{
		PINSEL0 &= 0xfff03fff; //Enable PWM2 on P0.7, PWM4 on P0.8, and PWM6 on P0.9
		PINSEL0 |= 0x000a8000; //Enable PWM2 on P0.7, PWM4 on P0.8, and PWM6 on P0.9

		//PULSE WIDTH MODULATION INIT*********************************************
		PWM_PR = 0x00; // Prescale Register
		PWM_MCR = 0x02; // Match Control Register
		PWM_MR0 = 0x1000; // TOTAL PERIODTID   T
		PWM_MR2 = 0x0000; // H�G SIGNAL        t
		PWM_MR4 = 0x0000; // H�G SIGNAL        t
		PWM_MR6 = 0x0000; // H�G SIGNAL        t
		PWM_LER = 0x55; // Latch Enable Register
		PWM_PCR = 0x5400; // Prescale Counter Register PWMENA2, PWMENA4, PWMENA6
		PWM_TCR = 0x09; // Counter Enable och PWM Enable
		//************************************************************************

		inicjalizacjaOdbyta = 1;
	}

	switch (kolor)
	{
	case 1: //czerwony
		PWM_MR2 = 100;
		PWM_MR4 = 0;
		PWM_MR6 = 0;
		PWM_LER = 0x01;
		break;

	case 2: //zielony
		PWM_MR2 = 0;
		PWM_MR4 = 0;
		PWM_MR6 = 100;
		PWM_LER = 0x01;
		break;

	default:
		PWM_MR2 = 0;
		PWM_MR4 = 100;
		PWM_MR6 = 0;
		PWM_LER = 0x01;
	}

	osSleep (10);
}
