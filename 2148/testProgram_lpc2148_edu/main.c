#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"
#include <printf_P.h>
#include <ea_init.h>
#include <lpc2xxx.h>
#include <consol.h>

#include "usb/lpc_usb.h"
#include "usb/lpc_hid.h"

static void ProceduraGlowna (void* arg);
#define PAMIEC_PROCEDURA_GLOWNA  400
static tU8 ProceduraGlownaPamiec [PAMIEC_PROCEDURA_GLOWNA];
static tU8 ProceduraGlownaId;

static void proc1 (void* arg);
#define PROC1_STACK_SIZE 2048
static tU8 proc1Stack [PROC1_STACK_SIZE];
static tU8 pid1;

void WyswietlTekstNaLcd (void);

volatile tU32 msClock = 0;

/*
 * Funkcja wejœciowa programu.
 */
int main (void)
{
	tU8 error;
	tU8 pid;

	//immediately turn off buzzer (if connected)
	IODIR0 |= 0x00000080;
	IOSET0  = 0x00000080;

	osInit();
	osCreateProcess(ProceduraGlowna, ProceduraGlownaPamiec, PAMIEC_PROCEDURA_GLOWNA, &pid, 1, NULL, &error);
	osStartProcess(pid, &error);

	osStart();
	return 0;
}

/*
 * Procedura g³ówna programu odpalaj¹ca pozosta³e procedury.
 */
static void ProceduraGlowna (void* arg)
{
  tU8 error;

  osCreateProcess(proc1, proc1Stack, PROC1_STACK_SIZE, &ProceduraGlownaId, 3, NULL, &error);
  osStartProcess(ProceduraGlownaId, &error);

  osDeleteProcess();
}

static void proc1 (void* arg)
{
	WyswietlTekstNaLcd();
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
void
appTick(tU32 elapsedTime)
{
  msClock += elapsedTime;
}
