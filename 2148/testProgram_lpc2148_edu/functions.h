#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "pff2/src/pff.h"
#include "inttypes.h"

typedef enum { //okre�la stan odtwarzacza
	Player_Stoped = 0,
	Player_Playing,
	Player_Paused,
	Player_Prev,
	Player_Next,
	Player_Random
} PlayerStatus;

////wypisuje na terminalu, pe�n� zawarto�� karty
//FRESULT listDir(CHAR* _path, tBool first);
//
////tworzy list� plik�w dla otwarzacza
//uint32 filesList(CHAR* _path, CHAR* list);

#endif
