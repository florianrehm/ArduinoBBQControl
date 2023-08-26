#include "Actuator.hpp"
#include <Arduino.h>


/*---------Global variable declarations--------*/

Actuator act;

/*---------Function declarations---------------*/

/*Moves actuator by parameter provided*/
ErrorType ActMoveMotorStep(int step);

/*Verifies calibration by moving actuator step-by-step from minimum position *  *to maximum position. Sets actuator to minimum position afterwards.          */
ErrorType ActCalibration();

/*---------Function implementations------------*/

int ActInit()
{
  ActSetMotorPos(0);
  return act.init();
}

ErrorType ActUpdate()
{
  act.tick = (act.tick + 1) % ACT_UPDATE_RATE;

  if(act.tick == 0)
  {
    switch(act.actState)
    {
      case ACT_UNDEFINED:
      act.err = ERR_INVALID_CALIBRATION;
      break;
      
      case ACT_DECREASE:
      act.err = ActMoveMotorStep(-ACT_STEP);
      break;

      case ACT_INCREASE:
      act.err = ActMoveMotorStep(ACT_STEP);
      break;

      case ACT_MINIMUM:
      act.err = ActSetMotorPos(act.minPos);
      break;

      case ACT_MAXIMUM:
      act.err = ActSetMotorPos(act.maxPos);
      break;

      case ACT_HOLD: //do nothing
      break;

      case ACT_CALIBRATION: //perfom min/max calibration
      act.err = ActCalibration();
      break;
    }

  }

  return act.err;
}

  ErrorType ActMoveMotorStep(int step)
  {

    int newPos = act.pos + step;

    if(newPos >= act.maxPos)
    {
      newPos = act.maxPos;
      ActSetState(ACT_MAXIMUM);
    }
    else if(newPos <= act.minPos)
    {
      newPos = act.minPos;
      ActSetState(ACT_MINIMUM);
    }

    if(ACT_INVERT_SERVO == 0)
    {
      if(act.pos != newPos)
      {
        act.servo.write(newPos);
        act.pos = newPos;
      }
    }
    else
    {
      if(act.pos != newPos)
      {
        act.servo.write(act.maxPos - newPos);
        act.pos = newPos;
      }
    }

    return ERR_NULL;
  }

  ErrorType ActSetMotorPos(int pos)
  {
    if(act.actState != ACT_UNDEFINED)
    {
      if(pos >= act.maxPos)
      {
        pos = act.maxPos;
        ActSetState(ACT_MAXIMUM);
      }
      else if(pos <= act.minPos)
      {
        pos = act.minPos;
        ActSetState(ACT_MINIMUM);
      }
      
      if(ACT_INVERT_SERVO == 0)
      {
        if(pos != act.pos)
        {
          act.servo.write(pos);
          act.pos = pos;
        }
      }
      else  //invert servo
      {
        if(pos != act.pos)
        {
          act.servo.write(act.maxPos - pos);
          act.pos = pos;
        }
      }
    }
    else  // Act state is undefined - ignore min/max values
    {
      if(pos != act.pos)
        {
          act.servo.write(pos);
          act.pos = pos;
        }
    }

    return ERR_NULL;
  }

  ErrorType ActCalibration()
  {
    for(int pos = act.minPos; pos <= act.maxPos; pos++)
    {
      ActSetMotorPos(pos);
      delay(ACT_SERVO_WAIT_MS);
    }

    ActSetMotorPos(act.minPos);
    delay(ACT_SERVO_WAIT_MS);

    act.actState = ACT_HOLD;

    return ERR_NULL;

  }

  void ActSetState(ActCtrlState state)
  {
    act.actState = state;
  }

  ActCtrlState ActGetState()
  {
    return act.actState;
  }
  void ActSetMinPos(int pos)
  {
    act.minPos = pos;
  }

void ActSetMaxPos(int pos)
{
  act.maxPos = pos;
}

int ActGetMinPos()
{
  return act.minPos;
}

int ActGetMaxPos()
{
  return act.maxPos;
}

int ActGetCurrPos()
{
  return act.pos;
}

void ActSetMotorToMinPos()
{
    ActSetMotorPos(act.minPos);
    ActSetState(ACT_MINIMUM);
}

