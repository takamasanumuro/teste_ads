#include <stdint.h>

typedef struct InstrumentationDriver InstrumentationDriver;

InstrumentationDriver* InstrumentationDriver_create(int i2cAddress);
short int InstrumentationDriver_setChannel(InstrumentationDriver* driver, unsigned short int adcChannel, char* mqttChannel, uint16_t mqttChannelSize);
int InstrumentationDriver_getI2cAddress(InstrumentationDriver* driver);