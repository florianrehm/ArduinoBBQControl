#include "Button.hpp"
#include "config.hpp"

/*---------Global variable declarations--------*/

Button myBtn(CONF_BUTTON);

bool btnPressed = false;

/*---------Function implementations------------*/

void BtnInit()
{
  myBtn.begin();
}

void BtnUpdate()
{
    myBtn.read();               // read the button

    if (myBtn.wasPressed())    // if the button was released, change the LED 
    {
        btnPressed = true;
    }
    else
    {
      btnPressed = false;
    }
}

bool BtnPressed()
{
  return btnPressed;
}