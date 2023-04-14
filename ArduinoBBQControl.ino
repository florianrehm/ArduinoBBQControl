#include "src/TempSensor.hpp"
#include "src/Actuator.hpp"
#include "src/Control.hpp"
#include "src/Button.hpp"

void setup() 
{
  Serial.begin(9600);
  CtrlInit();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  CtrlUpdate();
  ActUpdate();
  BtnUpdate();

}
