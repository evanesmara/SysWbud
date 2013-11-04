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

	//wyœwietlacz 1
	osCreateProcess(proc1, proc1Stack, PROC1_STACK_SIZE, &pid1, 3, NULL, &error);
	osStartProcess(pid1, &error);

	//eaInit(); //initialize printf
	//i2cInit(); //initialize I2C

	//wyœwietlacz 2
	osCreateProcess(proc2, proc2Stack, PROC2_STACK_SIZE, &pid2, 3, NULL, &error);
	osStartProcess(pid2, &error);

	osDeleteProcess();
}

static void proc1(void* arg) {
	WyswietlTekstNaLcd();
}

/*
 * 0 - Graj
 * 1 - Zatrzymaj
 * 2 - Nastepny
 */
static void drawMenu(int _akcja) {
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
	switch (_akcja) {
	case 0: {
		lcdPuts("Graj");
		break;
	}
	case 1: {
		lcdPuts("Zatrzymaj");
		break;
	}
	case 2: {
		lcdPuts("Nastepny");
		break;
	}
	}
}

static void proc2(void* arg) {
	static tU8 i = 0;

	IODIR |= 0x00006000; //P0.13/14
	IOSET = 0x00006000;

	lcdInit();
	initKeyProc();
	int akcja = 0;
	drawMenu(akcja);
	lcdContrast(contrast);
	while (1) {
		tU8 anyKey; // ruch joistick'a

		anyKey = checkKey();
		if (anyKey != KEY_NOTHING) {
			if (anyKey == KEY_CENTER) {
				drawMenu(0);
			} else if (anyKey == KEY_RIGHT) {
				drawMenu((++akcja) % 3);
			} else if (anyKey == KEY_LEFT) {
				drawMenu((--akcja) % 3);
			} else if (anyKey == KEY_DOWN) {
				drawMenu(akcja);
			} else if (anyKey == KEY_UP) {
				drawMenu(akcja);
			}
		}

//		//rysowanie ognia na dole
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
	}
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
