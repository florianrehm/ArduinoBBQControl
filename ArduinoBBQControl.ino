#include "src/TempSensor.hpp"
#include "src/Actuator.hpp"
#include "src/Control.hpp"
#include "src/Button.hpp"

void setup() 
{
  Serial.begin(9600);
  CtrlInit();
}

void loop() 
{
  
  CtrlUpdate();
  ActUpdate();
  BtnUpdate();

}
