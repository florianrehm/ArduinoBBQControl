#ifndef INCLUDE_NVM_HPP
#define INCLUDE_NVM_HPP 1

#include <EEPROM.h>

/*Display data structure*/
struct NvmConfigStore_t;

typedef struct NvmConfigStore_t
{
  int actMaxPos;
  int actMinPos;
  int targetTemp;
  int magicNum;

  void init()
  {
    actMaxPos = 0;
    actMinPos = 0;
    targetTemp = 0;
    magicNum = -1;
  }

} NVMConfigStore;

/*NVM init function*/
void NvmInit();

/*Returns if true if NVM is populated with data*
 *Returns false otherwise                      */
bool NvMConfigIsValid();

/*Returns current config from EEPROM*/
NVMConfigStore NvMLoadCurrConfig();

/*Stores current config into EEPROM*/
void NvMStoreCurrConfig(NVMConfigStore nvmConfig);

#endif