#include <Arduino.h>
#include "config.hpp"

/*Temperature sensor data structure*/
struct temperature_t;

typedef struct temperature_t
{
    float tempBuf;
    short tick;
    int avgTemp;
    int init()
    {
      tick = 0;
	    tempBuf = 0;
	    tick = 0;
	    avgTemp = 0;
      return 0;
    };
} Temperature;

/*Returns the average (normalized) temperature from the sensor*
 *if TEMP_SIMULATION is undefined.                            *
 *Returns value from temperature simulation wheel otherwise.  */
int TmpGetAvgTemperature();

/*Temperature init function*/
int TmpInit();
