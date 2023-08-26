#include "TempSensor.hpp"
#include <Arduino.h>

/*---------Global variable declarations--------*/
Temperature temp;

/*---------Function declarations---------------*/

float TmpGetTemperature();

/*---------Function implementations------------*/

int TmpInit()
{
	return temp.init();
}

float TmpGetTemperature()
{
#ifdef TEMP_SIMULATION
  int tempSensorVal = analogRead(TEMP_SIMULATION_PIN);
  float temperature = map(tempSensorVal, 0, 1023, 80, 150);
#else
  float tempSensorVal = analogRead(TEMP_SENSOR_PIN);

  tempSensorVal = map(tempSensorVal, 0, 1023, 0, 4096);

  float Rt = TEMP_SENSOR_RES * ((4096.0/tempSensorVal) - 1);

  float v = log(Rt/220.0f);

  float temperature = (1/(TEMP_SENSOR_CHAR_A + TEMP_SENSOR_CHAR_B*v + TEMP_SENSOR_CHAR_C*v*v)) - 273;
  
#endif

  return temperature;
}

int TmpGetAvgTemperature()
{
  float currTemp = TmpGetTemperature();

  temp.tick = (temp.tick + 1) % TMP_UPDATE_RATE;

  temp.tempBuf = temp.tempBuf + currTemp;

  if(temp.tick == 0)
  {
    temp.avgTemp = (int) (temp.tempBuf / (float) TMP_UPDATE_RATE);
    temp.tempBuf = temp.avgTemp;
  }

  return temp.avgTemp;

}
