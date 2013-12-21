#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"

#include <printf_P.h>
#include <ea_init.h>
#include <lpc2xxx.h>
#include <consol.h>

#include "testLcd.c"
#include "testRGB.c"

#include "i2c.h"
#include "key.h"

#include "usb/lpc_usb.h"
#include "usb/lpc_hid.h"

#include "pff2/src/diskio.h"
#include "pff2/src/pff.h"
#include "const_strings.h"
#include "functions.h"

static void ProceduraGlowna(void* arg);
#define PAMIEC_PROCEDURA_GLOWNA  400
static tU8 ProceduraGlownaPamiec[PAMIEC_PROCEDURA_GLOWNA];

static void ObslugaEkranuLcd1(void* arg);
#define ObslugaEkranuLcd1_STACK_SIZE 2048
static tU8 ObslugaEkranuLcd1Stack[ObslugaEkranuLcd1_STACK_SIZE];
static tU8 pid1;

static void ObslugaWyswietlacza128(void* arg);
#define ObslugaWyswietlacza128_STACK_SIZE 2048
static tU8 ObslugaWyswietlacza128Stack[ObslugaWyswietlacza128_STACK_SIZE];
static tU8 pid3;

FRESULT wynikInicjalizacjiSd = 0;

void PoinformujZeWczytanoSd();
void PoinformujZeNieWczytanoSd();
void OdtwarzajDzwiek();
static void WyswietlTekst(int _ktory);
static void WyswietlTekstPodany(char_t* _tekst);
FATFS fatfs;

volatile tU32 msClock = 0;

//0 - niebieska
//1 - czerwona
//2 - zielona
int kolorDiody = 0;
uint32 filesCount = 0;

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

	//	InicjalizacjaKonsoli();

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

	//InicjalizacjaKonsoli();

	WyswietlTekstPodany(C_TEXT_INIT);
	osSleep(200);
	lcdClrscr();

	// Wyœwietlacz 1.
	osCreateProcess(ObslugaEkranuLcd1, ObslugaEkranuLcd1Stack,
			ObslugaEkranuLcd1_STACK_SIZE, &pid1, 3, NULL, &error);
	osStartProcess(pid1, &error);

	if (kolorDiody != 2) {
		wynikInicjalizacjiSd = pf_mount(&fatfs);
		if (wynikInicjalizacjiSd == FR_OK) {
			kolorDiody = 2;

		} else
			kolorDiody = 1;
	}

	initKeyProc(); // inicjalizacja joystick'a

	// Wyœwietlacz 128x128
	osCreateProcess(ObslugaWyswietlacza128, ObslugaWyswietlacza128Stack,
			ObslugaWyswietlacza128_STACK_SIZE, &pid3, 3, NULL, &error);
	osStartProcess(pid3, &error);

	// Zakoñczenie procesów.
	osDeleteProcess();
}

static void ObslugaEkranuLcd1(void* arg) {
	WyswietlTekstNaLcd();
}

static void ObslugaWyswietlacza128(void *arg) {
	int tekst = 0;
	ZapalajDiode(kolorDiody, 0);

	while (1) {
		tU8 ruchJoysticka = checkKey();

		if (ruchJoysticka != KEY_NOTHING) {
			switch (ruchJoysticka) {
			case KEY_UP: {
				WyswietlTekst((++tekst) % 2);
				break;
			}
				//			case KEY_DOWN: {
				//				WyswietlTekstPodany(C_TEXT_POKAZ);
				//				break;
				//			}
			}
		} else {
			ZapalajDiode(kolorDiody, 0);
		}
	}
}

static void WyswietlTekst(int _ktory) {
	//Inicjalizacja ekranu LCD i ustawienie kolorów wyœwietlacza i tekstu.
	lcdInit();
	lcdColor(0xff, 0x00);
	lcdClrscr();
	ZapalajDiode(0, 1);
	switch (_ktory) {
	case 0:
		lcdGotoxy(0, 30);
		lcdPuts(C_TEXT_TEST1);
		break;
	case 1:
		lcdGotoxy(0, 30);
		lcdPuts(C_TEXT_TEST2);
		break;
	default:
		break;
	}
}
static void WyswietlTekstPodany(char_t* _tekst) {
	//Inicjalizacja ekranu LCD i ustawienie kolorów wyœwietlacza i tekstu.
	lcdInit();
	lcdColor(0xff, 0x00);
	lcdClrscr();
	lcdGotoxy(0, 30);
	lcdPuts(_tekst);
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
