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
      ctrl.updateMode(MODE_CONFIG_ACT_MIN);
    break;
    
    case  MODE_CALIBRATION: //Actuator calibration, TempSensor calibration
      err = ActCalibration();
      ctrl.updateMode(MODE_HEATUP);
    break;

    case MODE_CONFIG_ACT_MIN: //set target temperature, timer
    err = CtrlModeConfigActMin();
    break;

    case MODE_CONFIG_ACT_MAX:
    err = CtrlModeConfigActMax();
    break;

    case MODE_CONFIG_TEMP:
    err = CtrlModeConfigTemp();
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

  updateTempHistory();

  DispSetCurrState(ctrl.currMode, ActGetCurrPos(), ctrl.targetChamberTemp, err, TmpGetAvgTemperature(), ctrl.lidTimer);
  DispUpdate();

  if(err != ERR_NULL)
  {
    ctrl.updateMode(MODE_ERROR);
    Serial.print("Error");
    Serial.print(err);
  }

  return err;
}

ErrorType CtrlModeConfigTemp()
{
  ErrorType err = ERR_NULL;

  int val = 0;

  val = analogRead(CONF_WHEEL_PIN);
  val = map(val, 0, 1023, 80, 150);

  CtrlSetTargetTemp(val);

      if(BtnPressed() == true)
      {
        ctrl.updateMode(MODE_CALIBRATION);
      }
    
  if(ctrl.targetChamberTemp < 80 || ctrl.targetChamberTemp > 180)
  {
    err = ERR_CONFIG;
  }

  return err;
}

ErrorType CtrlModeConfigActMin()
{
  ErrorType err = ERR_NULL;

  int val = 0;
  val = analogRead(CONF_WHEEL_PIN);
  val = map(val, 0, 1023, 0, 180);

  ActSetMotorPos(val);

      if(BtnPressed() == true)
      {
        ActSetMinPos(val);
        ctrl.updateMode(MODE_CONFIG_ACT_MAX);
      }

  if(ActGetMinPos() < 0 || ActGetMinPos() > 180)
  {
    err = ERR_CONFIG;
  }

  return err;
}

ErrorType CtrlModeConfigActMax()
{
  ErrorType err = ERR_NULL;

  int val = 0;

      val = analogRead(CONF_WHEEL_PIN);
      val = map(val, 0, 1023, 0, 180);

      ActSetMotorPos(val);

      if(BtnPressed() == true)
      {
        ActSetMaxPos(val);
        ctrl.updateMode(MODE_CONFIG_TEMP);
      }

  if(ActGetMaxPos() < 0 || ActGetMaxPos() > 180)
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
    ctrl.lidTimer = 0;
    ctrl.lidTimerInit = millis() / 1000.0f;
    ctrl.lidTimerMode = TIMER_RUN;
    ActSetState(ACT_HOLD);
  }
  else if(ctrl.lidTimerMode == TIMER_RUN && ctrl.lidTimer < CTRL_LIDOPEN_WAIT_SEC)
  {
    ctrl.lidTimer = (millis() / 1000.0f) - ctrl.lidTimerInit;
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
  if(ctrl.tempTimerMode == TIMER_IDLE)
  {
    ctrl.tempTimer = 0;
    ctrl.tempTimerInit = millis() / 1000.0f;
    ctrl.tempTimerMode = TIMER_RUN;
  }
  else if(ctrl.tempTimerMode == TIMER_RUN && ctrl.tempTimer <  CTRL_TEMP_HISTORY_TIMER)
  {
    ctrl.tempTimer = (millis() / 1000.0f) - ctrl.tempTimerInit;
  }
  else  //timer expired
  {
    ctrl.tempHistory[ctrl.tempHistoryCount] = TmpGetAvgTemperature();
    ctrl.tempHistoryCount += 1;

    if(ctrl.tempHistoryCount == CTRL_TEMP_HISTORY_LEN)
    {
      ctrl.tempHistoryCount = 0;
      ctrl.tempHistoryBufOverflow = true;
    }

    ctrl.tempTimerMode = TIMER_IDLE;
  }

}

void CtrlSetTargetTemp(int temp)
{
  ctrl.targetChamberTemp = temp;
}

bool CtrlDetectOpenLid()
{
  bool lidOpen = false;

  if(CtrlGetMode() == MODE_OPERATION && ctrl.tempHistoryBufOverflow == true)
  {
    
    if(CTRL_LIDOPEN_HISTORY_LEN > CTRL_TEMP_HISTORY_LEN)
    {
      ctrl.updateMode(MODE_ERROR);
      return false;
    }
    
    int startPos = ctrl.tempHistoryCount;
    int endPos = abs(startPos - CTRL_LIDOPEN_HISTORY_LEN) % CTRL_TEMP_HISTORY_LEN;

    if(ctrl.tempHistory[startPos] - ctrl.tempHistory[endPos] >= CTRL_LIDOPEN_TEMP_THRESHOLD)
    {
      lidOpen = true;
    }
  }

  return lidOpen;
}

bool CtrlDetectGasLow()
{
  bool gasLow = false;

   if(CtrlGetMode() == MODE_OPERATION && ctrl.tempHistoryBufOverflow == true)
  {
    
    if(CTRL_GASLOW_HISTORY_LEN > CTRL_TEMP_HISTORY_LEN)
    {
      ctrl.updateMode(MODE_ERROR);
      return false;
    }
    
    int startPos = ctrl.tempHistoryCount;
    int endPos = abs(startPos - CTRL_GASLOW_HISTORY_LEN) % CTRL_TEMP_HISTORY_LEN;

    if(ctrl.tempHistory[startPos] - ctrl.tempHistory[endPos] >= CTRL_GASLOW_TEMP_THRESHOLD && ActGetState() == ACT_MAXIMUM)
    {
      gasLow = true;
    }
  }

  return gasLow;
}

CtrlMode CtrlGetMode()
{
  return ctrl.currMode;
}