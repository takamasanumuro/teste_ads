#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "instrumentation_driver.h"

#define CHANNELS    4

struct InstrumentationDriver
{
    int i2cAddress;
    char *mqttChannels[CHANNELS];
}

InstrumentationDriver* InstrumentationDriver_create(int i2cAddress)
{
    InstrumentationDriver* driver = (InstrumentationDriver*)malloc(sizeof(InstrumentationDriver));
    if(driver==NULL) return NULL;
    driver->i2cAddress = i2cAddress;
    return driver;
}

short int InstrumentationDriver_setChannel(struct InstrumentationDriver* driver, unsigned short int adcChannel, char* mqttChannel, uint16_t mqttChannelSize)
{
    driver->mqttChannels[adcChannel] = (char*)malloc(mqttChannelSize*sizeof(char));
    if(driver->mqttChannels[adcChannel]==NULL) return -1;
    else return 0;
}

int InstrumentationDriver_getI2cAddress(InstrumentationDriver* driver)
{
    return(driver->i2cAddress);
}