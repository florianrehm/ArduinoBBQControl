#include <Arduino.h>
#include "config.hpp"
struct temperature_t;

typedef struct temperature_t
{
    int pinInput;
    float tempBuf;
    short tick;
    int avgTemp;
    int init()
    {
      tick = 0;
	    pinInput = A0;
	    tempBuf = 0;
	    tick = 0;
	    avgTemp = 0;
      return 0;
    };
} Temperature;

float TmpGetTemperature();
int TmpGetAvgTemperature();
int TmpInit();
