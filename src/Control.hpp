#ifndef INCLUDE_CONTROL_HPP
#define INCLUDE_CONTROL_HPP 1

#include "CommonTypes.hpp"
#include "config.hpp"
#include <Arduino.h>


enum CtrlMode
{
  MODE_INITIALIZE   = 0,
  MODE_CALIBRATION  = 1,
  MODE_CONFIG_ACT_MIN = 2,
  MODE_CONFIG_ACT_MAX = 3,
  MODE_CONFIG_TEMP    = 4,
  MODE_HEATUP       = 5,
  MODE_LID_OPEN     = 6,
  MODE_GAS_LOW      = 7,
  MODE_PAUSE        = 8,
  MODE_ERROR        = 9,
  MODE_FINISHED     = 10,
  MODE_OPERATION    = 11,
  MODE_UNDEFINED,
};

enum ConfigModeType
{
  CONF_TEMP = 0,
  CONF_ACT_MIN = 1,
  CONF_ACT_MAX = 2,
  CONF_DONE = 3,
  CONF_UNINITIALIZED = 4,
};

enum TimerMode
{
  TIMER_IDLE,
  TIMER_RUN,
};

struct Control_t;

typedef struct Control_t
{
  CtrlMode prevMode;
  CtrlMode currMode;
  int targetChamberTemp;
  int targetMeatTemp;

  ConfigModeType configMode;
  
  bool lidOpen;
  int lidTimerInit;
  int lidTimer;
  TimerMode lidTimerMode;

  int tempTimer;
  int tempTimerInit;
  TimerMode tempTimerMode;
  int tempHistory[CTRL_TEMP_HISTORY_LEN];
  int tempHistoryCount;
  bool tempHistoryBufOverflow;

  int init()
  {
    prevMode = MODE_UNDEFINED;
    currMode = MODE_INITIALIZE;
    targetChamberTemp = 0;
    targetMeatTemp = 0;

    configMode = CONF_UNINITIALIZED;
    
    lidOpen = false;
    lidTimer = 0;
    lidTimerInit = 0;
    lidTimerMode = TIMER_IDLE;

    tempTimer = 0;
    tempTimerInit = 0;
    tempTimerMode = TIMER_IDLE;
    tempHistoryCount = 0;
    tempHistoryBufOverflow = false;

    for(int i = 0; i < CTRL_TEMP_HISTORY_LEN; i++)
    {
      tempHistory[i] = 0;
    }

    return 0;
  }

  void updateMode(CtrlMode newMode)
  {
    prevMode = currMode;
    currMode = newMode;
    Serial.print("New Mode: ");
    Serial.print(newMode);
    Serial.print("\n");
  }

} Control;

ErrorType CtrlUpdate();
int CtrlInit();
CtrlMode CtrlGetMode();

ErrorType CtrlModeConfigTemp();
ErrorType CtrlModeConfigActMin();
ErrorType CtrlModeConfigActMax();
ErrorType CtrlModePause();
ErrorType CtrlModeHeatup();
ErrorType CtrlModeLidOpen();
ErrorType CtrlModeGasLow();
ErrorType CtrlModeFinished();
ErrorType CtrlModeError();
ErrorType CtrlModeOperation();

void CtrlSetTargetTemp(int temp);
bool CtrlDetectOpenLid();
bool CtrlDetectGasLow();
void updateTempHistory();

#endif
