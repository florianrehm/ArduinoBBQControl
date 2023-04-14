#ifndef INCLUDE_CONTROL_HPP
#define INCLUDE_CONTROL_HPP 1

#include "CommonTypes.hpp"
#include "config.hpp"
#include <Arduino.h>


enum CtrlMode
{
  MODE_INITIALIZE   = 0,
  MODE_CALIBRATION  = 1,
  MODE_CONFIG       = 2,
  MODE_HEATUP       = 3,
  MODE_LID_OPEN     = 4,
  MODE_GAS_LOW      = 5,
  MODE_PAUSE        = 6,
  MODE_ERROR        = 7,
  MODE_FINISHED     = 8,
  MODE_OPERATION    = 9,
  MODE_UNDEFINED,
};

enum ConfigModeType
{
  CONF_TEMP,
  CONF_TIME,
  CONF_MEAT_TEMP,
  CONF_DONE,
  CONF_UNINITIALIZED,
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
    memset(&tempHistory, 0, sizeof(tempHistory));

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

ErrorType CtrlModeConfig();
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
