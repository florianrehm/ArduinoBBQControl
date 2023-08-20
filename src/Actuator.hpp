#include "CommonTypes.hpp"
#include <Servo.h>
#include "config.hpp"

/*Actuator states*/
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

/*Actuator data structure*/
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
    maxPos = 180;
    minPos = 0;
    actState = ACT_UNDEFINED;
    tick = 0;
    err = ERR_NULL;
    servo.attach(ACT_SERVO_PIN);

    return 0;
  };

} Actuator;

/*Actuator update loop*/
ErrorType ActUpdate();

/*Actuator init function*/
int ActInit();

/*Sets actuator to pos for offset calibration *
 *Ignores ACT_INVERT_SERVO setting            */
ErrorType ActSetMotorPos(int pos);

/*Sets ActCtrlState of the actuator*/
void ActSetState(ActCtrlState state);

/*Returns ActCtrlState of the actuator*/
ActCtrlState ActGetState();

/*Sets minimum offset of the actuator   *
 *Used within actuator caliration state *
 *Ignores ACT_INVERT_SERVO setting      */
void ActSetMinPos(int pos);

/*Sets maximum offset of the actuator   *
 *Used within actuator caliration state *
*Ignores ACT_INVERT_SERVO setting       */
void ActSetMaxPos(int pos);

/*Sets actuator to minimum position     *
 *Used within actuator caliration state *
*Ignores ACT_INVERT_SERVO setting       */
void ActSetMotorToMinPos();

/*Returns current position of the actuator*/
int ActGetCurrPos();



