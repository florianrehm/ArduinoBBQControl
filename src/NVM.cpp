#include "NVM.hpp"
#include "config.hpp"

/*---------Global variable declarations--------*/

/*---------Function declarations---------------*/

unsigned long NvMCalcCRConAddr(int startPos, int length);

/*---------Function implementations------------*/

void NvmInit()
{
  //do something
}

bool NvMClearAll()
{
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
}

bool NvMConfigIsValid()
{
  NVMConfigStore nvmConfig = NvMLoadCurrConfig();

  if(nvmConfig.magicNum == NVM_MAGIG_NUM) //TODO: Check CRC as well
  {
    int crcpos = NVM_CONF_ADDRESS + sizeof(NvmConfigStore_t);
    unsigned long crcRef = 0;
    EEPROM.get(crcpos, crcRef);

    if(crcRef != 0 && crcRef == NvMCalcCRConAddr(NVM_CONF_ADDRESS, sizeof(NvmConfigStore_t)))
    {
      return true;
    }
  }

  return false;
}

NVMConfigStore NvMLoadCurrConfig()
{
  NVMConfigStore conf;
  EEPROM.get(NVM_CONF_ADDRESS, conf);
}

void NvMStoreCurrConfig(NVMConfigStore nvmConfig)
{
  nvmConfig.magicNum = NVM_MAGIG_NUM;
  EEPROM.put(NVM_CONF_ADDRESS, nvmConfig);

  int crcpos = NVM_CONF_ADDRESS + sizeof(NvmConfigStore_t);

  EEPROM.put(crcpos, NvMCalcCRConAddr(NVM_CONF_ADDRESS, sizeof(NvmConfigStore_t)));
}

unsigned long NvMCalcCRConAddr(int startPos, int length) 
{

  const unsigned long crc_table[16] = {

    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,

    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,

    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,

    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c

  };

  if(startPos + length > EEPROM.length())
  {
    return 0;
  }

  unsigned long crc = ~0L;

  for (int index = startPos; index < (startPos + length)  ; ++index) 
  {

    crc = crc_table[(crc ^ EEPROM[index]) & 0x0f] ^ (crc >> 4);

    crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);

    crc = ~crc;

  }

  return crc;
}