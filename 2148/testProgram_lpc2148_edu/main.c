#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"
#include <printf_P.h>
#include <ea_init.h>
#include <lpc2xxx.h>
#include <consol.h>
#include "i2c.h"
#include "key.h"

#include "usb/lpc_usb.h"
#include "usb/lpc_hid.h"

static void ProceduraGlowna(void* arg);
#define PAMIEC_PROCEDURA_GLOWNA  400
static tU8 ProceduraGlownaPamiec[PAMIEC_PROCEDURA_GLOWNA];
static tU8 ProceduraGlownaId;

static void proc1(void* arg);
#define PROC1_STACK_SIZE 2048
static tU8 proc1Stack[PROC1_STACK_SIZE];
static tU8 pid1;

static void proc2(void* arg);
#define PROC2_STACK_SIZE 750
static tU8 proc2Stack[PROC2_STACK_SIZE];
static tU8 pid2;

static tU8 contrast = 46;

void WyswietlTekstNaLcd(void);

volatile tU32 msClock = 0;

/*
 * Funkcja wejœciowa programu.
 */
int main(void) {
	tU8 error;
	tU8 pid;

	//immediately turn off buzzer (if connected)
	IODIR0 |= 0x00000080;
	IOSET0 = 0x00000080;

	osInit();

	osCreateProcess(ProceduraGlowna, ProceduraGlownaPamiec,
			PAMIEC_PROCEDURA_GLOWNA, &pid, 1, NULL, &error);
	osStartProcess(pid, &error);

	osStart();
	return 0;
}

/*
 * Procedura g³ówna programu odpalaj¹ca pozosta³e procedury.
 */
static void ProceduraGlowna(void* arg) {
	tU8 error;

	osCreateProcess(proc1, proc1Stack, PROC1_STACK_SIZE, &pid1, 3, NULL, &error);
	osStartProcess(pid1, &error);

	//eaInit(); //initialize printf
	//i2cInit(); //initialize I2C

	osCreateProcess(proc2, proc2Stack, PROC2_STACK_SIZE, &pid2, 3, NULL, &error);
	osStartProcess(pid2, &error);

	osDeleteProcess();
}

static void proc1(void* arg) {
	WyswietlTekstNaLcd();
}

static void drawMenu(void) {
	lcdColor(0, 0);
	lcdClrscr();

	lcdRect(14, 0, 102, 128, 0x6d);
	lcdRect(15, 17, 100, 110, 0);

	lcdGotoxy(24, 1); // po³o¿enie tekstu x=24, y=1
	lcdColor(0x6d, 0);
	lcdPuts("Sterowanie");

	lcdGotoxy(22, 20 + (14 * 1));
	lcdColor(0x00, 0xe0);
	//  lcdColor(0x00,0xfd);
	lcdPuts("Graj");
}

static void proc2(void* arg) {
	static tU8 i = 0;

	printf("\n\n\n\n\n*******************************************************\n");
	printf("*                                                     *\n");
	printf("* Demo program for 'Experiment Expansion Board'       *\n");
	printf("* running on LPC2103 Education Board.                 *\n");
	printf("* - Snake game                                        *\n");
	printf("*                                                     *\n");
	printf("* (C) Embedded Artists 2008                           *\n");
	printf("*                                                     *\n");
	printf("*******************************************************\n");

	IODIR |= 0x00006000; //P0.13/14
	IOSET = 0x00006000;

	lcdInit();
	initKeyProc();
	drawMenu();
	lcdContrast(contrast);
	while (1) {
		tU8 anyKey;

		anyKey = checkKey();
		if (anyKey != KEY_NOTHING) {
			//select specific function
			if (anyKey == KEY_CENTER) {
				//				playSnake();
				drawMenu();
			}

			//adjust contrast
			else if (anyKey == KEY_RIGHT) {
				contrast++;
				if (contrast > 127)
					contrast = 127;
				lcdContrast(contrast);
			} else if (anyKey == KEY_LEFT) {
				if (contrast > 0)
					contrast--;
				lcdContrast(contrast);
			}
		}
	}

	//		switch (i) {
	//		case 0:
	//			lcdIcon(15, 88, 100, 40, _fire_0_100x40c[2], _fire_0_100x40c[3],
	//					&_fire_0_100x40c[4]);
	//			i++;
	//			break;
	//		case 1:
	//			lcdIcon(15, 88, 100, 40, _fire_1_100x40c[2], _fire_1_100x40c[3],
	//					&_fire_1_100x40c[4]);
	//			i++;
	//			break;
	//		case 2:
	//			lcdIcon(15, 88, 100, 40, _fire_2_100x40c[2], _fire_2_100x40c[3],
	//					&_fire_2_100x40c[4]);
	//			i++;
	//			break;
	//		case 3:
	//			lcdIcon(15, 88, 100, 40, _fire_3_100x40c[2], _fire_3_100x40c[3],
	//					&_fire_3_100x40c[4]);
	//			i++;
	//			break;
	//		case 4:
	//			lcdIcon(15, 88, 100, 40, _fire_4_100x40c[2], _fire_4_100x40c[3],
	//					&_fire_4_100x40c[4]);
	//			i = 0;
	//			break;
	//		default:
	//			i = 0;
	//			break;
	//		}
	//		osSleep(20);
	//	}
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
void appTick(tU32 elapsedTime) {
	msClock += elapsedTime;
}
