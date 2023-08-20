#include "Display.hpp"
#include "config.hpp"

/*---------Global variable declarations--------*/

Display disp;
LiquidCrystal lcd = LiquidCrystal(DISP_LCD_PIN_RS, DISP_LCD_PIN_EN, DISP_LCD_PIN_D4, DISP_LCD_PIN_D5, DISP_LCD_PIN_D6, DISP_LCD_PIN_D7);

const char *DispStrs[][4] = 
{
  {"Init", "", "", "Wait"},
  {"Act Calib.", "", "", "Wait"},
  {"Config", "OK?", "Sel. Off_L:" , ""},
  {"Config", "OK?", "Sel. Off_R:" , ""},
  {"Config", "OK?", "Sel. Temp:", ""},
  {"Heatup", "", "Act: ", "Wait"},
  {"Lid Open", "T: ", "Act:", "Tgt:"},
  {"Error", "Gas Low", "Act:", "Tgt:"},
  {"Pause", "Continue?", "", ""},
  {"Error", "Retry?", "", ""},
  {"Finished", "Resume?", "", ""},
  {"Operation", "", "Act:", "Tgt:"},
};

const dispDataTypes DispVals[][4] = 
{
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_SEL_ACT_MIN, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_SEL_ACT_MAX, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_SEL_TEMP, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_CHAMB_TEMP, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_TIMER, DISP_TYPE_CHAMB_TEMP, DISP_TYPE_TARGET_TEMP},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_CHAMB_TEMP, DISP_TYPE_TARGET_TEMP},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_NONE},
  {DISP_TYPE_NONE, DISP_TYPE_NONE, DISP_TYPE_CHAMB_TEMP, DISP_TYPE_TARGET_TEMP},
  
};

/*---------Function declarations---------------*/

void DispPrintCurrState();

/*---------Function implementations------------*/

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
    for(int i = 0; i < 4; i++)
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

        case DISP_TYPE_SEL_ACT_MIN:
        disp.sqVals[i] = disp.actPos;
        break;
        
        case DISP_TYPE_SEL_ACT_MAX:
        disp.sqVals[i] = disp.actPos;
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
      //lcd.print(" ");

      if(disp.sqVals[i] != -1 && disp.sqVals[i] != 0)
      {
        lcd.print(disp.sqVals[i]);
      }

      lcd.print(" ");
    }

  lcd.setCursor(0, 1);

    for(int i = 2; i < 4; i++)
    {
      lcd.print(disp.sqStrs[i]);
      //lcd.print(" ");

      if(disp.sqVals[i] != -1 && disp.sqVals[i] != 0)
      {
        lcd.print(disp.sqVals[i]);
      }

      lcd.print(" ");
    }
}

void DispSetCurrState(CtrlMode mode, int actPos, int targetTemp, ErrorType err, int chambTemp, int lidTimer)
{
  disp.dispMode = mode;
  disp.targetTemp = targetTemp;
  disp.err = err;
  disp.chambTemp = chambTemp;
  disp.lidTimer = lidTimer;
  disp.actPos = actPos;
}