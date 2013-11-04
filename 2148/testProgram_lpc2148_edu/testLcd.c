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

#define LCD_DATA      0x00ff0000  //P1.16-P1.23
#define LCD_E         0x02000000  //P1.25
#define LCD_RW        0x00400000  //P0.22
#define LCD_RS        0x01000000  //P1.24
#define LCD_BACKLIGHT 0x40000000  //P0.30

/*
 * TODO: Spisaæ klika s³ów o inicjalizacji.
 */
static void initLCD (void)
{
	IODIR1 |= (LCD_DATA | LCD_E | LCD_RS);
	IOCLR1  = (LCD_DATA | LCD_E | LCD_RS);

	IODIR0 |= LCD_RW;
	IOCLR0  = LCD_RW;
	
	IODIR0 |= LCD_BACKLIGHT;
	IOCLR0  = LCD_BACKLIGHT;
}

/*
 * Wywo³anie przerwania opóŸniaj¹cego na oko³o 37 mikrosekund.
 * nop - No Operation
 */
static void delay37us (void)
{
	volatile tU32 i;

	for (i = 0; i < 15*2500; ++i)
	{
		asm volatile (" nop"); //delay 15 ns x 2500 = about 37,5 us delay
	}
}

/*
 * W³¹cza (TRUE) lub wy³¹cza (FALSE) podœwielenie ekranu.
 */
static void PodswietlEkran (tU8 onOff)
{
	if (onOff == TRUE)
	{
		IOSET0 = LCD_BACKLIGHT;
	}
	else
	{
		IOCLR0 = LCD_BACKLIGHT;
	}
}

/*
 * TODO: Spisaæ kilka s³ów o wypisywaniu na ekran.
 */
static void WypiszZnakNaEkran(tU8 reg, tU8 data)
{
	volatile tU8 i;

	if (reg == 0)
	{
		IOCLR1 = LCD_RS;
	}
	else
	{
		IOSET1 = LCD_RS;
	}

	IOCLR0 = LCD_RW;
	IOCLR1 = LCD_DATA;
	IOSET1 = ((tU32)data << 16) & LCD_DATA;
	IOSET1 = LCD_E;

	for(i = 0; i < 16; ++i)
	{
		asm volatile (" nop"); //delay 15 ns x 16 = about 250 ns delay
	}
	IOCLR1 = LCD_E;

	for(i = 0; i < 16; ++i)
	{
		asm volatile (" nop"); //delay 15 ns x 16 = about 250 ns delay
	}
}

/*
 * Wypisuje ci¹g znaków w linii na ekranie.
 */
void WypiszCiag (tU8* tekst)
{
	while (*tekst != '\0')
	{
		WypiszZnakNaEkran(1, *tekst);
		delay37us();

		tekst++;
	}
}

/*
 * Wyœwietlenie na ekranie LCD napisów.
 */
void WyswietlTekstNaLcd(void)
{
	initLCD();
	PodswietlEkran(FALSE);
	osSleep(10);

	while (1)
	{
		PodswietlEkran(TRUE);
		osSleep(50);

		//TODO: Dowiedzieæ siê co to za znaki (a¿ do funkcji WypiszCiag) i co siê dzieje bez ich udzia³u.
		//function set
		WypiszZnakNaEkran(0, 0x30);
		osSleep(1);
		WypiszZnakNaEkran(0, 0x30);
		delay37us();
		WypiszZnakNaEkran(0, 0x30);
		delay37us();
		WypiszZnakNaEkran(0, 0x38);
		delay37us();

		//display off
		WypiszZnakNaEkran(0, 0x08);
		delay37us();

		//display clear
		WypiszZnakNaEkran(0, 0x01);
		osSleep(1); //actually only 1.52 mS needed

		//display control set
		WypiszZnakNaEkran(0, 0x06);
		osSleep(1);

		//display control set
		WypiszZnakNaEkran(0, 0x0c);
		delay37us();

		//cursor home
		WypiszZnakNaEkran(0, 0x02);
		osSleep(1);

		WypiszCiag ("M. Kolodziejczyk");

		//move curstor to second row
		WypiszZnakNaEkran(0, 0x80 | 0x40);
		delay37us();

		WypiszCiag ("P. Zielinski");

		//OpóŸnienie przed znikniêciem.
		osSleep(500);
		//Wy³¹czenie podœwietlenia monitora przed wyczyszczeniem.
		PodswietlEkran(FALSE);
		osSleep(50);

		//TODO: Dowiedzieæ siê jak ten znak czyœci ekran.
		//display clear
		WypiszZnakNaEkran(0, 0x01);
		osSleep(1); //actually only 1.52 mS needed

		//W³¹czenie podœwietlenia monitora po wyczyszczeniu.
		PodswietlEkran(TRUE);
		osSleep(50);
	}

	PodswietlEkran(FALSE);
}
