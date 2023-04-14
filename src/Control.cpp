#include "Arduino.h"
#include "Control.hpp"
#include "TempSensor.hpp"
#include "Actuator.hpp"
#include "Display.hpp"
#include "Button.hpp"

Control ctrl;

int CtrlInit()
{  
  return ctrl.init();
}

ErrorType CtrlUpdate()
{
  ErrorType err = ERR_NULL;

  switch(ctrl.currMode)
  {
    case  MODE_INITIALIZE:  //startup initialization
      TmpInit();
      ActInit();
      DispInit();
      BtnInit();
      ctrl.updateMode(MODE_CALIBRATION);
    break;
    
    case  MODE_CALIBRATION: //Actuator calibration, TempSensor calibration
      err = ActCalibration();
      ctrl.updateMode(MODE_CONFIG);
    break;

    case MODE_CONFIG: //set target temperature, timer
    err = CtrlModeConfig();
    break;

    case MODE_HEATUP:
      err = CtrlModeHeatup();
    break;

    case MODE_OPERATION:
    err = CtrlModeOperation();
    break;

    case MODE_LID_OPEN:
      err = CtrlModeLidOpen();
    break;

    case MODE_GAS_LOW:
      err = CtrlModeGasLow();
    break;

    case MODE_PAUSE:
      err = CtrlModePause();
    break;

    case MODE_ERROR:
      err = CtrlModeError();
    break;

    case MODE_FINISHED:
      err = CtrlModeFinished();
    break;

    case MODE_UNDEFINED:
    break;
  }

  DispSetCurrState(ctrl.currMode, ctrl.targetChamberTemp, err, TmpGetAvgTemperature(), ctrl.lidTimer);
  
  DispUpdate();
  
  updateTempHistory();

  if(err != ERR_NULL)
  {
    ctrl.updateMode(MODE_ERROR);
  }

  return err;
}

ErrorType CtrlModeConfig()
{
  ErrorType err = ERR_NULL;

  ActSetState(ACT_MINIMUM);

  switch(ctrl.configMode)
  {
    case CONF_UNINITIALIZED:
      ctrl.configMode = CONF_TEMP;
    break;

    case CONF_TEMP:
      int val = analogRead(CONF_WHEEL_PIN);
      val = map(val, 0, 1023, 80, 150);

      CtrlSetTargetTemp(val);

      if(BtnPressed() == true)
      {
        ctrl.configMode = CONF_DONE;
      }

    break;

    case CONF_TIME:
    break;

    case CONF_MEAT_TEMP:
    break;

    case CONF_DONE:
      ctrl.updateMode(MODE_HEATUP);
    break;

  }

  if(ctrl.targetChamberTemp < 20 || ctrl.targetChamberTemp > 300)
  {
    err = ERR_CONFIG;
  }

  return err;
}

ErrorType CtrlModePause()
{
  ActSetState(ACT_MINIMUM);
  
  return ERR_NULL;
}

ErrorType CtrlModeHeatup()
{
  if(TmpGetAvgTemperature() < ctrl.targetChamberTemp)
  {
    ActSetState(ACT_MAXIMUM);
  }
  else if(TmpGetAvgTemperature() >= ctrl.targetChamberTemp)
  {
    ctrl.updateMode(MODE_OPERATION);
  }
}

ErrorType CtrlModeOperation()
{
  ErrorType err = ERR_NULL;

  if(CtrlDetectOpenLid() == true)
  {
    ctrl.updateMode(MODE_LID_OPEN);
  }
  else if(CtrlDetectGasLow() == true)
  {
    ctrl.updateMode(MODE_GAS_LOW);
  }
  else  //standard case
  {
    int currTemp = TmpGetAvgTemperature();

    if(currTemp < ctrl.targetChamberTemp)
    {
      ActSetState(ACT_INCREASE);
    }
    else if(currTemp > ctrl.targetChamberTemp)
    {
      ActSetState(ACT_DECREASE);
    }
    else  //target temperature reached
    {
      ActSetState(ACT_HOLD);
    }
  }

  return err;
}

ErrorType CtrlModeLidOpen()
{
  //start countdown and re-start control mode operation
  if(ctrl.lidTimerMode == TIMER_IDLE)
  {
    ctrl.lidOpen = true;
    ctrl.lidTimer = millis() / 1000.0f;
    ctrl.lidTimerInit = millis() / 1000.0f;
    ctrl.lidTimerMode = TIMER_RUN;
    ActSetState(ACT_HOLD);
  }
  else if(ctrl.lidTimerMode == TIMER_RUN && ctrl.lidTimer < ctrl.lidTimerInit + CTRL_LIDOPEN_WAIT_SEC)
  {
    ctrl.lidTimer = millis() / 1000.0f;
    ActSetState(ACT_HOLD);
  }
  else  //timer expired
  {
    ctrl.lidOpen = false;
    ctrl.lidTimer = 0;
    ctrl.lidTimerInit = 0;
    ctrl.lidTimerMode = TIMER_IDLE;
    ctrl.updateMode(MODE_OPERATION);
  }

  return ERR_NULL;
}

ErrorType CtrlModeGasLow()
{
  //switch to mode Error and stop control?
  ActSetState(ACT_MAXIMUM);
}

ErrorType CtrlModeFinished()
{
  //display finished, alarm? and stop control
  ActSetState(ACT_MINIMUM);
}

ErrorType CtrlModeError()
{
  //display error
  ActSetState(ACT_MINIMUM);
}

void updateTempHistory()
{
  //start countdown and re-start control mode operation
  if(ctrl.tempTimerMode == TIMER_IDLE)
  {
    ctrl.tempTimer = millis() / 1000.0f;
    ctrl.tempTimerInit = millis() / 1000.0f;
    ctrl.tempTimerMode = TIMER_RUN;
    ctrl.tempHistoryCount = 0;
    ctrl.tempHistory[ctrl.tempHistoryCount] = TmpGetAvgTemperature();
  }
  else if(ctrl.tempTimerMode == TIMER_RUN && ctrl.tempTimer < ctrl.tempTimerInit + CTRL_TEMP_HISTORY_LEN && ctrl.tempHistoryCount < CTRL_TEMP_HISTORY_LEN)
  {
    ctrl.tempHistoryCount += 1;
    ctrl.tempTimer = millis() / 1000.0f;
    ctrl.tempHistory[ctrl.tempHistoryCount] = TmpGetAvgTemperature();
  }
  else  //timer expired
  {
    ctrl.tempTimer = 0;
    ctrl.tempTimerInit = 0;
    ctrl.tempTimerMode = TIMER_IDLE;
  }
}

void CtrlSetTargetTemp(int temp)
{
  ctrl.targetChamberTemp = temp;
}

bool CtrlDetectOpenLid()
{
  if(CTRL_LIDOPEN_HISTORY_LEN > CTRL_TEMP_HISTORY_LEN)
  {
    ctrl.updateMode(MODE_ERROR);
    return false;
  }

  bool lidOpen = false;
  int startPos = ctrl.tempHistoryCount;
  int endPos = abs(startPos - CTRL_LIDOPEN_HISTORY_LEN) % CTRL_TEMP_HISTORY_LEN;

  if(ctrl.tempHistory[startPos] - ctrl.tempHistory[endPos] <= CTRL_LIDOPEN_TEMP_THRESHOLD)
  {
    lidOpen = true;
  }

  return lidOpen;
}

bool CtrlDetectGasLow()
{
  bool gasLow = false;

  int startPos = ctrl.tempHistoryCount;
  int endPos = abs(startPos - CTRL_TEMP_HISTORY_LEN) % CTRL_TEMP_HISTORY_LEN;

  if( ( ((ctrl.tempHistory[startPos] - ctrl.tempHistory[endPos]) / CTRL_TEMP_HISTORY_LEN) <= CTRL_GAS_LOW_GRADIENT) 
      && ActGetState() == ACT_MAXIMUM
      && TmpGetAvgTemperature() < ctrl.targetChamberTemp)
  {
    gasLow = true;
  }

  return gasLow;
}

CtrlMode CtrlGetMode()
{
  return ctrl.currMode;
}