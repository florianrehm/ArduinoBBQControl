#ifndef INCLUDE_DISPLAY_HPP
#define INCLUDE_DISPLAY_HPP 1

#include "Control.hpp"
#include <LiquidCrystal.h>

/*Defines the data types supported by the display*/
enum dispDataTypes
{
  DISP_TYPE_TARGET_TEMP,
  DISP_TYPE_CHAMB_TEMP,
  DISP_TYPE_SEL_TEMP,
  DISP_TYPE_SEL_ACT_MIN,
  DISP_TYPE_SEL_ACT_MAX,
  DISP_TYPE_TIMER,
  DISP_TYPE_NONE,
};

/*Display data structure*/
struct Display_t;

typedef struct Display_t
{
  CtrlMode dispMode;
  int targetTemp;
  ErrorType err;
  int chambTemp;
  int timer;
  int tick;
  int actPos;

  char* sqStrs[4];
  int sqVals[4];

  void init()
  {
    dispMode = MODE_UNDEFINED;
    targetTemp = 0;
    err = ERR_NULL;
    chambTemp = 0;
    timer = 0;
    tick = 0;
    actPos = 0;
  }

} Display;

/*Display init function*/
int DispInit();

/*Display update loop*/
void DispUpdate();

/*Sets or updates the data buffer to be displayed on the LCD*/
void DispSetCurrState(CtrlMode mode, int actPos, int targetTemp, ErrorType err, int chambTemp, int ctrlTimer);

#endif