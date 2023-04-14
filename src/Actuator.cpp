#include "Actuator.hpp"
#include <Arduino.h>

Actuator act;

int ActInit()
{
  ActSetMotorPos(0);
  return act.init();
}

ErrorType ActUpdate()
{
  act.tick = (act.tick + 1) % ACT_UPDATE_RATE_MS;

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

    if(act.pos != newPos)
    {
      act.servo.write(newPos);
      delay(ACT_SERVO_WAIT_MS);
      act.pos = newPos;
    }

    return ERR_NULL;
  }

  ErrorType ActSetMotorPos(int pos)
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

    if(pos != act.pos)
    {
      act.servo.write(pos);
      delay(ACT_SERVO_WAIT_MS);
      act.pos = pos;
    }

    return ERR_NULL;
  }

  ErrorType ActCalibration()
  {

    for(int pos = act.minPos; pos <= act.maxPos; pos++)
    {
      ActSetMotorPos(pos);
    }

    ActSetMotorPos(act.minPos);

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
