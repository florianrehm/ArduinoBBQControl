#ifndef INCLUDE_BUTTON_HPP
#define INCLUDE_BUTTON_HPP 1

#include <JC_Button.h>          // https://github.com/JChristensen/JC_Button

/*Button init function*/
void BtnInit();

/*Returns if true if button is pressed*
 *Returns false otherwise             */
bool BtnPressed();

/*Actuator update loop*/
void BtnUpdate();

#endif