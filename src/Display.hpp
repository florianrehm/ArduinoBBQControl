#ifndef INCLUDE_DISPLAY_HPP
#define INCLUDE_DISPLAY_HPP 1

#include "Control.hpp"
#include <LiquidCrystal.h>

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

struct Display_t;

typedef struct Display_t
{
  CtrlMode dispMode;
  int targetTemp;
  ErrorType err;
  int chambTemp;
  int lidTimer;
  int tick;

  char* sqStrs[4];
  int sqVals[4];

  void init()
  {
    dispMode = MODE_UNDEFINED;
    targetTemp = 0;
    err = ERR_NULL;
    chambTemp = 0;
    lidTimer = 0;
    tick = 0;
  }

} Display;

int DispInit();
void DispPrintCurrState();
void DispUpdate();
void DispSetCurrState(CtrlMode mode, int targetTemp, ErrorType err, int chambTemp, int lidTimer);

#endif