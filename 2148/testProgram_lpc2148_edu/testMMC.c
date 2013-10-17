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
#include "../filesys/fatfs.h"

/*****************************************************************************
 *
 * Description:
 *    Print the result code 
 *
 ****************************************************************************/
static void
printFatError(tFatResult result)
{

  printf("File result code = ");
  switch(result) {
  case FAT_OK:
    printf("FAT_OK");
    break;
  case FAT_ERROR_INV_HAND:
    printf("FAT_ERROR_INV_HAND");
    break;
  case FAT_ERROR_TOO_MANY_OPEN:
    printf("FAT_ERROR_TOO_MANY_OPEN");
    break;
  case FAT_ERROR_EOF:
    printf("FAT_ERROR_EOF");
    break;
  case FAT_ERROR_READ:
    printf("FAT_ERROR_READ");
    break;
  case FAT_ERROR_WRITE:
    printf("FAT_ERROR_WRITE");
    break;
  case FAT_ERROR_NOT_EXIST:
    printf("FAT_ERROR_NOT_EXIST");
    break;
  case FAT_ERROR_A_FOLDER:
    printf("FAT_ERROR_A_FOLDER");
    break;
  case FAT_ERROR_NOT_EMPTY:
    printf("FAT_ERROR_NOT_EMPTY");
    break;
  case FAT_ERROR_EXISTS:
    printf("FAT_ERROR_EXISTS");
    break;
  case FAT_ERROR_A_FILE:
    printf("FAT_ERROR_A_FILE");
    break;
  case FAT_ERROR_INV_MODE:
    printf("FAT_ERROR_INV_MODE");
    break;
  case FAT_ERROR_INV_PARAM:
    printf("FAT_ERROR_INV_PARAM");
    break;
  case FAT_ERROR_INTERNAL:
    printf("FAT_ERROR_INTERNAL");
    break;
  case FAT_ERROR_INVALID_PATH:
    printf("FAT_ERROR_INVALID_PATH");
    break;
  case FAT_ERROR_NO_VALID_PART:
    printf("FAT_ERROR_NO_VALID_PART");
    break;
  case FAT_ERROR_FAT12_NOT_IMPL:
    printf("FAT_ERROR_FAT12_NOT_IMPL");
    break;
  case FAT_ERROR_FAT32_NOT_IMPL:
    printf("FAT_ERROR_FAT32_NOT_IMPL");
    break;
  case FAT_ERROR_NO_MORE_CLUSTERS:
    printf("FAT_ERROR_NO_MORE_CLUSTERS");
    break;
  case FAT_ERROR_END_CLUSTER_CHAIN:
    printf("FAT_ERROR_END_CLUSTER_CHAIN");
    break;
  case FAT_ERROR_SEEK_FAILED:
    printf("FAT_ERROR_SEEK_FAILED");
    break;
  case FAT_ERROR_ROOT_SOMETHING:
    printf("FAT_ERROR_ROOT_SOMETHING");
    break;
  case FAT_ERROR_NO_MEDIA:
    printf("FAT_ERROR_NO_MEDIA");
    break;
  case FAT_ERROR_READ_ONLY:
    printf("FAT_ERROR_READ_ONLY");
    break;
  case FAT_ERROR_FS_NOT_INITIALIZED:
    printf("FAT_ERROR_FS_NOT_INITIALIZED");
    break;
  default:
    printf("UNKNOWN %d", result);
    break;
  }
  printf("\r\n");
}


/*****************************************************************************
 *
 * Description:
 *    A process entry function. 
 *
 ****************************************************************************/
void
testMMC(void)
{
  tFatResult fsResult = 0;
  tFatHandle rootHandle;
 	tU32       fileSize;
  tBool      isDir;
  tU8*       pName;
static tU8 fileName[256];
static tFatDirEntry dirEntry;
  tU32 i;


  printf("\n\n*******************************************************\n");
  printf("*                                                     *\n");
  printf("* The MMC/SD card interface is tested...              *\n");
  printf("*                                                     *\n");
  printf("*******************************************************\n\n\n");

  fsResult = fatInit();
  printFatError(fsResult);
  
  if(fsResult == FAT_OK)
  {
    fsResult = fatOpen("/", "r", &rootHandle);
    if (fsResult == FAT_OK)
    {
  	  fsResult = fatReadDirEntry(rootHandle, &dirEntry);
  	
      printf("\nContent on MMC/SD card:");

      while(fsResult == FAT_OK)
      {
        fatGetEntryName(&dirEntry, &pName);
        fatGetEntrySize(&dirEntry, &fileSize);
        fatGetEntryIsDir(&dirEntry, &isDir);

     		i = 0;
      	if(pName != NULL)
       	{
     	  	while(*pName != '\0')
     		  {
     			  fileName[i++] = *pName;
       			pName += 2;
       		}
       	}
     	  fileName[i] = '\0';

        if (FALSE == isDir)
          printf("\n  '%s'  %d bytes", fileName, fileSize);
        else
          printf("\n  '%s'  Is a subdirectory", fileName);
      
        fsResult = fatReadDirEntry(rootHandle, &dirEntry);
      }
    }
    else
      printFatError(fsResult);
  
    fatClose(&rootHandle);
  }
}
