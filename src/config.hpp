#ifndef INCLUDE_CONFIG_HPP
#define INCLUDE_CONFIG_HPP 1

/*---------Actuator Config---------*/

/*Defines amount of steps the actuator is moved per tick in case* 
 *actuator is within state AC_INCREASE                          */
#define ACT_STEP 1

/*Defines PWM pin connected to servo motor*/
#define ACT_SERVO_PIN 10

/*Defines waittime for actuator to finish change of position */
#define ACT_SERVO_WAIT_MS 100

/*Defines whether the servo motor uses inverse scale (e.g. 0...180 vs 180...0)*/
#define ACT_INVERT_SERVO 0

/*Defines tick rate for updating actuator state and position*/
#define ACT_UPDATE_RATE 2000

/*Defines the upper limit for maximum offset*/
#define ACT_MAX_OFFSET 180

/*Defines the lower limit for minimum offset*/
#define ACT_MIN_OFFSET 0

/*---------Control Config---------*/

/*Defines timeout actuator is set to ACT_HOLD if open lid is detected*/
#define CTRL_LIDOPEN_WAIT_SEC 60

/*Defines amount of samples used for lid open detection*/
#define CTRL_LIDOPEN_HISTORY_LEN 10

/*Defines temperature threshold for lid open detection*/
#define CTRL_LIDOPEN_TEMP_THRESHOLD 15

/*Defines overall size of the temperature history buffer*/
#define CTRL_TEMP_HISTORY_LEN 250

/*Defines amount of samples used for gas low detection*/
#define CTRL_GASLOW_HISTORY_LEN 250

/*Defines temperature threshold for gas low detection*/
#define CTRL_GASLOW_TEMP_THRESHOLD 30

/*Defines time in seconds until next temperature history is stored into buffer*/
#define CTRL_TEMP_HISTORY_TIMER 1

/*Defines amount of ticks for determining normalized temperature sensor value*/
#define CTRL_TEMP_UPDATE_RATE 200

/*Defines the wait time before reloading config in seconds*/
#define CTRL_CONFIG_RELOAD_TIMER 10

/*Defines lower range of accepted temperature deviation (under-temperature)*
 *By setting this value to 0, the control algorithm will always aim to meet*
  the exact temperature*/
#define CTRL_OPERATION_TEMP_LOWER_THRESHOLD -2

/*Defines upper range of accepted temperature deviation (over-temperature) *
 *By setting this value to 0, the control algorithm will always aim to meet*
 *the exact temperature*/
#define CTRL_OPERATION_TEMP_UPPER_THRESHOLD 1

/*Defines the watchdog state
  if set to 0 - watchdog disabled
  if set to 1 - watchdog enabled*/
#define CTRL_ENABLE_WDT 1

/*---------Tempsensor Config---------*/

/*If defined, temperature simulation via potentionmeter will be used instead*
 *of data from the temperature sensor                                       */
//#define TEMP_SIMULATION 1

/*Defines pin for connecting temperature sensor*/
#define TEMP_SENSOR_PIN A5

/*Defines reference resistor used for measuring temperature sensor resistance*/
#define TEMP_SENSOR_RES 47 //kOhm

/*Defines temp sensor characteristic curve      *
 *IKEA sensor (new version) currently supported */
#define TEMP_SENSOR_CHAR_A 0.00334519
#define TEMP_SENSOR_CHAR_B 0.000243825
#define TEMP_SENSOR_CHAR_C 0.00000261726

/*---------HMI Config---------*/

/*Defines pin to connect the configuration wheel (potentionmeter)*/
#define CONF_WHEEL_PIN A0

/*Defines pin to connect the configuration (confirm) button*/
#define CONF_BUTTON 8

/*Defines pin to connect the temperature simulation potentiometer*/
#define TEMP_SIMULATION_PIN A1

/*---------Display Config---------*/

/*Defines tick rate for updating the LCD*/
#define DISP_UPDATE_RATE 200

/*Defines amount of rows for the LCD*/
#define DISP_LCD_ROWS 2

/*Defines amount of columns for the LCD*/
#define DISP_LCD_COLS 20

/*Defines the pins to connect the LCD*/
#define DISP_LCD_PIN_RS 7
#define DISP_LCD_PIN_EN 6
#define DISP_LCD_PIN_D4 5
#define DISP_LCD_PIN_D5 4
#define DISP_LCD_PIN_D6 3
#define DISP_LCD_PIN_D7 2

/*---------NVM Config---------*/
#define NVM_CONF_ADDRESS 0
#define NVM_MAGIG_NUM 1337

#endif