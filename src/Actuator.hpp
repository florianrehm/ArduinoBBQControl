#include "CommonTypes.hpp"
#include <Servo.h>
#include "config.hpp"



enum ActCtrlState
{
    ACT_INCREASE,
    ACT_DECREASE,
    ACT_HOLD,
    ACT_CALIBRATION,
    ACT_UNDEFINED,
    ACT_MAXIMUM,
    ACT_MINIMUM,
};

struct actuator_t;
typedef struct actuator_t
{
  int pos;
  int maxPos; //in degrees
  int minPos; //in degrees
  int tick;
  ErrorType err;
  ActCtrlState actState;
  Servo servo;

  int init()
  {
    pos = 0;
    maxPos = 90;
    minPos = 0;
    actState = ACT_UNDEFINED;
    tick = 0;
    err = ERR_NULL;
    servo.attach(ACT_SERVO_PIN);

    return 0;
  };

} Actuator;

ErrorType ActUpdate();
int ActInit();
ErrorType ActSetMotorPos(int pos);
ErrorType ActMoveMotorStep(int step);
ErrorType ActCalibration();
void ActSetState(ActCtrlState state);
ActCtrlState ActGetState();



