#include "Display.hpp"
#include "config.hpp"

Display disp;
LiquidCrystal lcd = LiquidCrystal(LCD_PIN_RS, LCD_PIN_EN, LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7);


const char *DispStrs[][4] = 
{
  {"Initialize", "", "", "Please wait"},
  {"Calibration", "", "", "Please wait"},
  {"Configuration", "OK?", "Select Temp: C", ""},
  {"Heatup", "", "Chamber: C", "Please wait"},
  {"Lid Open", "Timer: ", "Chamber: C", "Target: C"},
  {"Error", "Gas Low", "Chamber: C", "Target: C"},
  {"Pause", "Continue?", "", ""},
  {"Error", "Retry?", "", ""},
  {"Finished", "Resume?", "", ""},
  {"Operation", "", "Chamber: C", "Target: C"},
};



const dispDataTypes DispVals[][4] = 
{
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_SEL_TEMP, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_SEL_TEMP, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_CHAMB_TEMP, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_CHAMB_TEMP, DISP_TYPE_TARGET_TEMP},
  {DISP_TYPE_NONE, DISP_TYPE_TIMER, DISP_TYPE_NONE, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_CHAMB_TEMP, DISP_TYPE_TARGET_TEMP},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_CHAMB_TEMP, DISP_TYPE_TARGET_TEMP},
  
};



int DispInit()
{
  disp.init();
  lcd.begin(16, 2);
  lcd.noAutoscroll();
  lcd.noBlink();
  lcd.noCursor();

  return 0;
}

void DispUpdate()
{

  disp.tick = (disp.tick + 1) % DISP_UPDATE_RATE;

  if(disp.tick == 0)
  {
    for(int i = 0; i < sizeof(4); i++)
    {
      disp.sqStrs[i] = DispStrs[(int)disp.dispMode][i];
      
      switch(DispVals[(int)disp.dispMode][i])
      {
        case DISP_TYPE_NONE:
        disp.sqVals[i] = -1;
        break;

        case DISP_TYPE_CHAMB_TEMP:
        disp.sqVals[i] = disp.chambTemp;
        break;

        case DISP_TYPE_TARGET_TEMP:
        disp.sqVals[i] = disp.targetTemp;
        break;

        case DISP_TYPE_SEL_TEMP:
        disp.sqVals[i] = disp.targetTemp;
        break;

        case DISP_TYPE_TIMER:
        disp.sqVals[i] = disp.lidTimer;
        break;
      }

    }

    DispPrintCurrState();
  }
  
}

void DispPrintCurrState()
{
  lcd.clear();
  lcd.setCursor(0,0);

    for(int i = 0; i < 2; i++)
    {
      lcd.print(disp.sqStrs[i]);
      lcd.print(" ");

      if(disp.sqVals[i] != -1)
      {
        lcd.print(disp.sqVals[i]);
      }

      lcd.print(" ");
    }

  lcd.setCursor(0, 1);

    for(int i = 2; i < 4; i++)
    {
      lcd.print(disp.sqStrs[i]);
      lcd.print(" ");

      if(disp.sqVals[i] != -1)
      {
        lcd.print(disp.sqVals[i]);
      }

      lcd.print(" ");
    }

}

void DispSetCurrState(CtrlMode mode, int targetTemp, ErrorType err, int chambTemp, int lidTimer)
{
  disp.dispMode = mode;
  disp.targetTemp = targetTemp;
  disp.err = err;
  disp.chambTemp = chambTemp;
  lidTimer = lidTimer;
}