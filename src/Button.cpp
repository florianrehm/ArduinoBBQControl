#include "Button.hpp"
#include "config.hpp"

Button myBtn(CONF_BUTTON);

bool btnPressed = false;

void BtnInit()
{
  myBtn.begin();
}

void BtnUpdate()
{
    myBtn.read();               // read the button

    if (myBtn.wasReleased())    // if the button was released, change the LED state
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