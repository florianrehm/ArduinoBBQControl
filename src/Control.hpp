#ifndef INCLUDE_CONTROL_HPP
#define INCLUDE_CONTROL_HPP 1

#include "CommonTypes.hpp"
#include "config.hpp"
#include <Arduino.h>
#include <avr/wdt.h>

/*Defines the control modes*/
enum CtrlMode
{
  MODE_INITIALIZE     = 0,
  MODE_CALIBRATION    = 1,
  MODE_CONFIG_RELOAD  = 2,
  MODE_CONFIG_ACT_MIN = 3,
  MODE_CONFIG_ACT_MAX = 4,
  MODE_CONFIG_TEMP    = 5,
  MODE_HEATUP         = 6,
  MODE_LID_OPEN       = 7,
  MODE_GAS_LOW        = 8,
  MODE_PAUSE          = 9,
  MODE_ERROR          = 10,
  MODE_FINISHED       = 11,
  MODE_OPERATION      = 12,
  MODE_UNDEFINED,
};

/*Defines the modes for timers used within control logic*/
enum TimerMode
{
  TIMER_IDLE,
  TIMER_RUN,
};

/*Control data strcuture*/
struct Control_t;

typedef struct Control_t
{
  CtrlMode prevMode;
  CtrlMode currMode;
  int targetChamberTemp;
  int targetMeatTemp;
  
  bool lidOpen;
  int ctrlTimerInit;
  int ctrlTimer;
  TimerMode ctrlTimerMode;

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
    
    lidOpen = false;
    ctrlTimer = 0;
    ctrlTimerInit = 0;
    ctrlTimerMode = TIMER_IDLE;

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

/*Control update loop*/
ErrorType CtrlUpdate();

/*Control init function*/
int CtrlInit();

#endif
