#ifndef _I2CWRAPPER_H_
#define _I2CWRAPPER_H_
#include "ch32v003fun.h"

class I2CWrapper {
  public:
    I2CWrapper();
    ~I2CWrapper();
    void begin();
    void beginTransmission(uint8_t address);
    uint8_t endTransmission();
    uint8_t requestFrom(uint8_t address, uint8_t quantity);
    size_t write(uint8_t data);
    int read();
    int available();
};

#endif