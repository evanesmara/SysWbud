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
#include <string.h>
#include "eeprom.h"

#define MAX_LENGTH 14

/*****************************************************************************
 *
 * Description:
 *    Test EEPROM
 *
 ****************************************************************************/
tU8
testEEPROM(void)
{
  tU8 eepromTestResultOK;
  tU8 testString1[] = "String #1";
  tU8 testString2[] = "sTrInG #2";
  tU8 testBuf[MAX_LENGTH];
  tS8 errorCode;
  
  eepromTestResultOK = TRUE;
//  printf("\nTest #1 - write string '%s' to address 0x0000", testString1);
  errorCode = eepromWrite(0x0000, testString1, sizeof(testString1));
  if (errorCode == I2C_CODE_OK)
;//    printf("\n        - done (status code OK)");
  else
  {
//    printf("\n        - failed (error code = %d)!", errorCode);
    eepromTestResultOK = FALSE;
  }
  
  if (eepromPoll() == I2C_CODE_OK)
;//    printf("\n        - program cycle completed");
  else
  {
//    printf("\n        - program cycle failed!");
    eepromTestResultOK = FALSE;
  }

//  printf("\nTest #2 - write string '%s' to address 0x00a0", testString2);
  errorCode = eepromWrite(0x00a0, testString2, sizeof(testString2));
  if (errorCode == I2C_CODE_OK)
;//    printf("\n        - done (status code OK)");
  else
  {
//    printf("\n        - failed (error code = %d)!", errorCode);
    eepromTestResultOK = FALSE;
  }
   
  if (eepromPoll() == I2C_CODE_OK)
;//    printf("\n        - program cycle completed");
  else
  {
//    printf("\n        - program cycle failed!");
    eepromTestResultOK = FALSE;
  }

  /*
   * Read from eeprom
   */
//  printf("\nTest #3 - read string from address 0x0000");
  errorCode = eepromPageRead(0x0000, testBuf, MAX_LENGTH);
  if (errorCode == I2C_CODE_OK)
  {
    if (strlen(testBuf) == sizeof(testString1)-1)
;//      printf("\n        - string is '%s'", testBuf);
    else
    {
//      printf("\n        - wrong length (read string is %d characters long)!", strlen(testBuf));
      eepromTestResultOK = FALSE;
    }
  }
  else
  {
//    printf("\n        - failed (error code = %d)!", errorCode);
    eepromTestResultOK = FALSE;
  }

//  printf("\nTest #4 - read string from address 0x00a0");
  errorCode = eepromPageRead(0x00a0, testBuf, MAX_LENGTH);
  if (errorCode == I2C_CODE_OK)
  {
    if (strlen(testBuf) == sizeof(testString2)-1)
;//      printf("\n        - string is '%s'", testBuf);
    else
    {
//      printf("\n        - wrong length (read string is %d characters long)!", strlen(testBuf));
      eepromTestResultOK = FALSE;
    }
  }
  else
  {
//    printf("\n        - failed (error code = %d)!", errorCode);
    eepromTestResultOK = FALSE;
  }

  /*
   * Write/Read from eeprom
   */
//  printf("\nTest #5 - write string '%s' to address 0x0004", testString2);
  errorCode = eepromWrite(0x0004, testString2, sizeof(testString2));
  if (errorCode == I2C_CODE_OK)
;//    printf("\n        - done (status code OK)");
  else
  {
//    printf("\n        - failed (error code = %d)!", errorCode);
    eepromTestResultOK = FALSE;
  }
  
  if (eepromPoll() == I2C_CODE_OK)
;//    printf("\n        - program cycle completed");
  else
  {
//    printf("\n        - program cycle failed!");
    eepromTestResultOK = FALSE;
  }

//  printf("\nTest #6 - read string from address 0x0000");
  errorCode = eepromPageRead(0x0000, testBuf, MAX_LENGTH);
  if (errorCode == I2C_CODE_OK)
  {
    if (strlen(testBuf) == sizeof(testString2)-1+4)
;//      printf("\n        - string is '%s'", testBuf);
    else
    {
//      printf("\n        - wrong length (read string is %d characters long)!", strlen(testBuf));
      eepromTestResultOK = FALSE;
    }
  }
  else
  {
//    printf("\n        - failed (error code = %d)!", errorCode);
    eepromTestResultOK = FALSE;
  }
  
  return eepromTestResultOK;
}

/*****************************************************************************
 *
 * Description:
 *    Test RTC crystal
 *
 ****************************************************************************/
static void
testRtc(void)
{
  CCR  = 0x00000012;
  CCR  = 0x00000010;
  ILR  = 0x00000000;
  CIIR = 0x00000000;
  AMR  = 0x00000000;
  //PREINT  = 449;
  //PREFRAC = 0;
  osSleep(50);
  SEC  = 0;
  MINx  = 0;
  HOUR = 0;
  osSleep(50);
  CCR  = 0x00000011;
  
  osSleep(75);
  printf(".");
  osSleep(50);
  printf(".");
  osSleep(50);
  printf(".");
  osSleep(50);
  printf(".");
  osSleep(50);
  printf(".");
  osSleep(50);
  printf(".");
  osSleep(50);
  printf(".");
  
  if ((SEC == 3) && (MINx == 0) && (HOUR == 0))
    printf(" test OK!\n");
  else
    printf(" check result during ADC/DAC test [%d:%d:%d]\n", HOUR, MINx, SEC);
}

/*****************************************************************************
 *
 * Description:
 *    A process entry function. 
 *
 ****************************************************************************/
void
testI2C(void)
{
 	tU8 data[3];

  printf("\n\n*******************************************************\n");
  printf("*                                                     *\n");
  printf("* The I2C EEPROM and RTC crystal are tested...        *\n");
  printf("*                                                     *\n");
  printf("*******************************************************\n");

  /* test RTC crystal */
  printf("\n\nRTC (Real-Time Clock) test");
	testRtc();

  /* initialize and test I2C */
  i2cInit();
  if (testEEPROM() == TRUE)
    printf("\n\nSummary of EEPROM tests: Passed all tests!");
  else
    printf("\n\nSummary of EEPROM tests: Failed at least one test!");


  printf("\n\n*******************************************************\n");
  printf("*                                                     *\n");
  printf("* The LM75 temperature sensor is tested...            *\n");
  printf("*                                                     *\n");
  printf("*******************************************************\n");
  
  //test LM75
  if (1 == lm75Read(0x90, &data[0], 3))
  {
    tU16 temp;
    temp = (((tU16)data[0]<<8) + (tU16)data[1]) >> 7;
    printf("\rLM75 temp = %d.%d", temp / 2, (temp&1) * 5);
  }
  else
    printf("\nFAILED to read from LM75!!!!!!!!!!!!!!!!!\n");
}
