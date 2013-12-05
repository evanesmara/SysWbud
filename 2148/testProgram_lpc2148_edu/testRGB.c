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

/*
 * 0x04 - czerwona
 * 0x10 - niebieska
 * 0x40 - zielona
 */
void ZaplajDiode(int kolor) {

	tU8 continueTest;
	//Zmienne s³u¿¹ce do sterowania moc¹ diody.
	//Powolne zapalanie i gaszenie.
	tU32 counter = 0;

	int direction = 0;
	int led = 0;

	PINSEL0 &= 0xfff03fff; //Enable PWM2 on P0.7, PWM4 on P0.8, and PWM6 on P0.9
	PINSEL0 |= 0x000a8000; //Enable PWM2 on P0.7, PWM4 on P0.8, and PWM6 on P0.9

	//PULSE WIDTH MODULATION INIT*********************************************
	PWM_PR = 0x00; // Prescale Register
	PWM_MCR = 0x02; // Match Control Register
	PWM_MR0 = 0x1000; // TOTAL PERIODTID   T
	PWM_MR2 = 0x0000; // HÖG SIGNAL        t
	PWM_MR4 = 0x0000; // HÖG SIGNAL        t
	PWM_MR6 = 0x0000; // HÖG SIGNAL        t
	PWM_LER = 0x55; // Latch Enable Register
	PWM_PCR = 0x5400; // Prescale Counter Register PWMENA2, PWMENA4, PWMENA6
	PWM_TCR = 0x09; // Counter Enable och PWM Enable
	//************************************************************************

	continueTest = TRUE;
	while (1) {

		if (direction == 0) {
			if (counter < 0x0080)
				counter += 0x0005;
			else
				counter += 0x000f;

			if (counter > 0x0300) {
				direction = 1;
			}
		} else {
			if (counter == 0x0000) {
				direction = 0;
				led++;
				osSleep(40);
				if (led > 2) {
					led = 0;
					continueTest = FALSE;
				}
			} else {
				if (counter < 0x0080)
					counter -= 0x0005;
				else
					counter -= 0x000f;
			}
		}

		switch (kolor) {
		case 1: //czerwony
			PWM_MR2 = counter;
			PWM_LER = 0x10;
			break;

		case 2: //zielony
			PWM_MR6 = counter;
			PWM_LER = 0x10;
			break;

		default:
			PWM_MR4 = counter;
			PWM_LER = 0x10;
		}

		osSleep(1);
	}
}
