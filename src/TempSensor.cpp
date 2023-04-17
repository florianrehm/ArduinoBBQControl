#include "TempSensor.hpp"
#include <Arduino.h>

Temperature temp;

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
  int tempSensorVal = analogRead(temp.pinInput);
  float voltage = (tempSensorVal / 1024.0f) * 5.0f;
  float temperature = (voltage - 0.5f) * 100;
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
