#include "Arduino.h"
#include "Control.hpp"
#include "TempSensor.hpp"
#include "Actuator.hpp"
#include "Display.hpp"
#include "Button.hpp"
#include "NVM.hpp"

/*---------Global variable declarations--------*/

Control ctrl;

/*---------Function declarations---------------*/
ErrorType CtrlModeReloadConfig();
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
ErrorType CtrlModeActCalibration();
void CtrlSetTargetTemp(int temp);
bool CtrlDetectOpenLid();
bool CtrlDetectGasLow();
void updateTempHistory();
CtrlMode CtrlGetMode();
void CtrlClearTempHistory();

/*---------Function implementations------------*/

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
      ctrl.updateMode(MODE_CONFIG_RELOAD);
    break;

    case MODE_CONFIG_RELOAD:  //Promt user whether to set new config, otherwise reload
      err = CtrlModeReloadConfig();
    break;
    
    case  MODE_CALIBRATION: //Actuator go from min to max
      err = CtrlModeActCalibration();
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

  DispSetCurrState(ctrl.currMode, ActGetCurrPos(), ctrl.targetChamberTemp, err, TmpGetAvgTemperature(), ctrl.ctrlTimer);
  DispUpdate();

  if(err != ERR_NULL)
  {
    ctrl.updateMode(MODE_ERROR);
    Serial.print("Error");
    Serial.print(err);
  }

  //Reset watchdog
  wdt_reset();

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
  val = map(val, 0, 1023, ACT_MIN_OFFSET, ACT_MAX_OFFSET);

  ActSetMotorPos(val);

  if(BtnPressed() == true)
  {
    if(val < ACT_MIN_OFFSET || val > ACT_MAX_OFFSET)
    {
      err = ERR_CONFIG;
    }

    ActSetMinPos(val);
    ctrl.updateMode(MODE_CONFIG_ACT_MAX);
  }

  return err;
}

ErrorType CtrlModeConfigActMax()
{
  ErrorType err = ERR_NULL;

  int val = 0;

  val = analogRead(CONF_WHEEL_PIN);
  val = map(val, 0, 1023, ACT_MIN_OFFSET, ACT_MAX_OFFSET);

  ActSetMotorPos(val);

  if(BtnPressed() == true)
  {
    if(val < ACT_MIN_OFFSET || val > ACT_MAX_OFFSET)
    {
      err = ERR_CONFIG;
    }

    ActSetMaxPos(val);
    ctrl.updateMode(MODE_CONFIG_TEMP);
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

    /**
    we are now at max. Temp is expected to increase. Avoid temperature increasing too fast by going from min to optimal position
    **/
    ActSetMotorToMinPos();

    ctrl.updateMode(MODE_OPERATION);  //start normal control mode
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

  //FIXME: In case of short-to-ground, display shows random values but software continues to run. Re-Initialize Display fixes the displayed data
  if(BtnPressed() == true)
  {
    Serial.println("Disp Init");
    DispInit();
  }

  return err;
}

ErrorType CtrlModeLidOpen()
{
  //start countdown and re-start control mode operation
  if(ctrl.ctrlTimerMode == TIMER_IDLE)
  {
    ctrl.lidOpen = true;
    ctrl.ctrlTimer = 0;
    ctrl.ctrlTimerInit = millis() / 1000.0f;
    ctrl.ctrlTimerMode = TIMER_RUN;
    ActSetState(ACT_HOLD);
  }
  else if(ctrl.ctrlTimerMode == TIMER_RUN && ctrl.ctrlTimer < CTRL_LIDOPEN_WAIT_SEC)
  {
    ctrl.ctrlTimer = (millis() / 1000.0f) - ctrl.ctrlTimerInit;
    ActSetState(ACT_HOLD);
  }
  else  //timer expired
  {
    ctrl.lidOpen = false;
    ctrl.ctrlTimer = 0;
    ctrl.ctrlTimerInit = 0;
    ctrl.ctrlTimerMode = TIMER_IDLE;
    ctrl.updateMode(MODE_OPERATION);

    CtrlClearTempHistory(); //collect new temp history to avoid starting timer again after expired due to temp. difference
  }

  return ERR_NULL;
}

ErrorType CtrlModeActCalibration()
{
  if(ActGetState() == ACT_UNDEFINED)
  {
    ActSetState(ACT_CALIBRATION);
  }
  else if(ActGetState() == ACT_HOLD)  // Calibration finished
  {
    NVMConfigStore conf;
    conf.actMinPos = ActGetMinPos();
    conf.actMaxPos = ActGetMaxPos();
    conf.targetTemp = ctrl.targetChamberTemp;
    NvMStoreCurrConfig(conf);

    ctrl.updateMode(MODE_HEATUP);

    /*Sometimes, if there is a short-to-ground due to different grounding   *
     *of grill and controller, the application gets stuck. Watchdog aims to *
     *identify this issue and perform HW reset                              */
    if(CTRL_ENABLE_WDT == 1)
    {
      wdt_enable(WDTO_2S);
    }

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

ErrorType CtrlModeReloadConfig()
{
  ErrorType err = ERR_NULL;

  if(NvMConfigIsValid() == false)
  {
    ctrl.updateMode(MODE_CONFIG_ACT_MIN);
  }
  else  // config is valid
  {
    if(ctrl.ctrlTimerMode == TIMER_IDLE)
    {
      ctrl.ctrlTimer = 0;
      ctrl.ctrlTimerInit = millis() / 1000.0f;
      ctrl.ctrlTimerMode = TIMER_RUN;
    }
    else if(ctrl.ctrlTimerMode == TIMER_RUN && ctrl.ctrlTimer <  CTRL_CONFIG_RELOAD_TIMER)  //timer running
    {
      ctrl.ctrlTimer = (millis() / 1000.0f) - ctrl.ctrlTimerInit;

      if(BtnPressed() == true)
      {
        ctrl.ctrlTimerMode == TIMER_IDLE;
        ctrl.updateMode(MODE_CONFIG_ACT_MIN);

        return ERR_NULL;
      }

    }
    else  //timer expired - reload config
    {
      ctrl.ctrlTimerMode = TIMER_IDLE;

      NVMConfigStore conf = NvMLoadCurrConfig();

      ActSetMinPos(conf.actMinPos);
      ActSetMaxPos(conf.actMaxPos);
      CtrlSetTargetTemp(conf.targetTemp);

      if(conf.actMinPos < ACT_MIN_OFFSET || conf.actMinPos > ACT_MAX_OFFSET)
      {
        err = ERR_CONFIG;
      }

      if(conf.actMaxPos < ACT_MIN_OFFSET || conf.actMaxPos > ACT_MAX_OFFSET)
      {
        err = ERR_CONFIG;
      }

      if(err == ERR_NULL)
      {
        ctrl.updateMode(MODE_CALIBRATION);
      }
    }
  }

  return err;
}

CtrlMode CtrlGetMode()
{
  return ctrl.currMode;
}

void CtrlClearTempHistory()
{
  ctrl.tempHistoryCount = 0;
  ctrl.tempHistoryBufOverflow = false;
}