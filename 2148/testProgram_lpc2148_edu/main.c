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

#include "pff2/src/diskio.h"

static void ProceduraGlowna (void* arg);
#define PAMIEC_PROCEDURA_GLOWNA  400
static tU8 ProceduraGlownaPamiec[PAMIEC_PROCEDURA_GLOWNA];

static void proc1 (void* arg);
#define PROC1_STACK_SIZE 2048
static tU8 proc1Stack[PROC1_STACK_SIZE];
static tU8 pid1;

//static void proc2 (void* arg);
//#define PROC2_STACK_SIZE 750
//static tU8 proc2Stack[PROC2_STACK_SIZE];
//static tU8 pid2;

static void proc3 (void* arg);
#define PROC3_STACK_SIZE 2048
static tU8 proc3Stack[PROC3_STACK_SIZE];
static tU8 pid3;

static void ZapalajDiodeProces (void* arg);
#define ZAPALAJDIODE_STACK_SIZE 256
static tU8 zapalajDiode[ZAPALAJDIODE_STACK_SIZE];
static tU8 pidZapalajDiode;

static tU8 contrast = 46;

//static void drawMenu (int);
static void PoinformujZeWczytanoSd ();
static void PoinformujZeNieWczytanoSd (tU8 blad);

volatile tU32 msClock = 0;

tU8 kolorDiody = 0;
tU8 wariantTekstu = 0;

/*
 * Funkcja wejœciowa programu.
 */
int main (void)
{
	tU8 error;
	tU8 pid;

	//immediately turn off buzzer (if connected)
	IODIR0 |= 0x00000080;
	IOSET0 = 0x00000080;

	osInit ();

	osCreateProcess (ProceduraGlowna, ProceduraGlownaPamiec, PAMIEC_PROCEDURA_GLOWNA, &pid, 1, NULL, &error);
	osStartProcess (pid, &error);

	osStart ();
	return 0;
}

/*
 * Procedura g³ówna programu odpalaj¹ca pozosta³e procedury.
 */
static void ProceduraGlowna (void* arg)
{
	tU8 error;

	InicjalizacjaKonsoli ();

	//Wyœwietlacz 1.
	osCreateProcess (proc1, proc1Stack, PROC1_STACK_SIZE, &pid1, 3, NULL, &error);
	osStartProcess (pid1, &error);

	//Wyœwietlacz 2.
	//osCreateProcess (proc2, proc2Stack, PROC2_STACK_SIZE, &pid2, 3, NULL, &error);
	//osStartProcess (pid2, &error);

	//Karta SD odczyt.
	osCreateProcess (proc3, proc3Stack, PROC3_STACK_SIZE, &pid3, 3, NULL, &error);
	osStartProcess (pid3, &error);

	//Zakoñczenie procesów.
	osDeleteProcess ();
}

static void proc1 (void* arg)
{
	WyswietlTekstNaLcd ();
}

//static void proc2 (void* arg)
//{
//	IODIR |= 0x00006000; //P0.13/14
//	IOSET = 0x00006000;
//
//	lcdInit ();
//	initKeyProc ();
//	int akcja = 0;
//	drawMenu (akcja);
//	lcdContrast (contrast);
//	while (1)
//	{
//		tU8 anyKey; // ruch joistick'a
//
//		anyKey = checkKey ();
//		if (anyKey != KEY_NOTHING)
//		{
//			if (anyKey == KEY_CENTER)
//			{
//				drawMenu (0);
//			} else if (anyKey == KEY_RIGHT)
//			{
//				drawMenu ((++akcja) % 3);
//			} else if (anyKey == KEY_LEFT)
//			{
//				drawMenu ((--akcja) % 3);
//			} else if (anyKey == KEY_DOWN)
//			{
//				drawMenu (akcja);
//			} else if (anyKey == KEY_UP)
//			{
//				drawMenu (akcja);
//			}
//		}
//	}
//}
//
///*
// * 0 - Graj
// * 1 - Zatrzymaj
// * 2 - Nastepny
// */
//static void drawMenu (int _akcja)
//{
//	lcdColor (0, 0);
//	lcdClrscr ();
//
//	lcdRect (14, 0, 102, 128, 0x6d);
//	lcdRect (15, 17, 100, 110, 0);
//
//	lcdGotoxy (24, 1); // po³o¿enie tekstu x=24, y=1
//	lcdColor (0x6d, 0);
//	lcdPuts ("Sterowanie");
//
//	lcdGotoxy (22, 20 + (14 * 1));
//	lcdColor (0x00, 0xe0);
//	//  lcdColor(0x00,0xfd);
//	switch (_akcja)
//	{
//	case 0:
//	{
//		lcdPuts ("Graj");
//		break;
//	}
//	case 1:
//	{
//		lcdPuts ("Zatrzymaj");
//		break;
//	}
//	case 2:
//	{
//		lcdPuts ("Nastepny");
//		break;
//	}
//	}
//}

static void proc3 (void *arg)
{
	tU8 ruchJoysticka;

	IODIR |= 0x00006000; //P0.13/14
	IOSET = 0x00006000;

	lcdInit ();
	initKeyProc ();
	lcdContrast (contrast);

	tU8 odpowiedz = disk_initialize ();

	if (odpowiedz == 0)
	{
		kolorDiody = 2;
	}
	else
	{
		kolorDiody = 1;
	}

	//Stworzenie procesu obs³uguj¹cego migaj¹c¹ diodê (na zielono, czerwono lub niebiesko).
	tU8 error;
	osCreateProcess (ZapalajDiodeProces, zapalajDiode, ZAPALAJDIODE_STACK_SIZE, &pidZapalajDiode, 3, NULL, &error);
	osStartProcess (pidZapalajDiode, &error);

	while (1)
	{
		ruchJoysticka = checkKey ();

		if (ruchJoysticka != KEY_NOTHING)
		{
			if (ruchJoysticka == KEY_LEFT)
			{
				wariantTekstu = 0;
			}
			else if (ruchJoysticka == KEY_RIGHT)
			{
				wariantTekstu = 1;
			}
		}

		if (odpowiedz == 0)
		{
			PoinformujZeWczytanoSd ();

		} else
		{
			PoinformujZeNieWczytanoSd (odpowiedz);
		}

		osSleep (100);
	}
}

static void PoinformujZeWczytanoSd ()
{
	lcdColor (0, 0);
	lcdClrscr ();
	lcdColor (0x00, 0xe0);

	switch (wariantTekstu)
	{
	case 0:
		lcdGotoxy (32, 20 + (14 * 2));
		lcdPuts ("Wczytano");

		lcdGotoxy (44, 20 + (14 * 3));
		lcdPuts ("Plik");

		break;

	case 1:
		lcdGotoxy (32, 20 + (14 * 2));
		lcdPuts ("Brawo!");

		break;
	}
}

static void PoinformujZeNieWczytanoSd (tU8 blad)
{
	lcdColor (0, 0);
	lcdClrscr ();
	lcdColor (0x00, 0xe0);

	switch (wariantTekstu)
	{
	case 0:
		lcdGotoxy (49, 20 + (14 * 2));
		lcdPuts ("Nie");

		lcdGotoxy (31, 20 + (14 * 3));
		lcdPuts ("Wczytano");

		lcdGotoxy (40, 20 + (14 * 4));
		lcdPuts ("Pliku");

		break;

	case 1:
		lcdGotoxy (22, 20 + (14 * 5));

		if (blad == 0x01)
		{
			lcdPuts ("STA_NO_INIT");
		} else if (blad == 0x03)
		{
			lcdPuts ("STA_NO_RDY");
		}
		break;
	}
}

static void ZapalajDiodeProces (void* arg)
{
	ZaplajDiode (kolorDiody);
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
void appTick (tU32 elapsedTime)
{
	msClock += elapsedTime;
}
