#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>

/*

The MIT License (MIT)

Copyright (c)  2020 Daniel Perron

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


// RATE
#define RATE_8   0
#define RATE_16  1
#define RATE_32  2
#define RATE_64  3
#define RATE_128 4
#define RATE_250 5
#define RATE_475 6
#define RATE_860 7

// GAIN
#define GAIN_6144MV 0
#define GAIN_4096MV 1
#define GAIN_2048MV 2
#define GAIN_1024MV 3
#define GAIN_512MV 4
#define GAIN_256MV 5
#define GAIN_256MV2 6
#define GAIN_256MV3 7

// multiplexer
#define AIN0_AIN1 0
#define AIN0_AIN3 1
#define AIN1_AIN3 2
#define AIN2_AIN3 3
#define AIN0      4
#define AIN1      5
#define AIN2      6
#define AIN3      7


// register address
#define  REG_CONV 0
#define  REG_CONFIG 1
#define  REG_LO_THRESH 2
#define  REG_HI_THRESH 3




int readADC(int i2c_handle,uint8_t multiplexer, uint8_t rate, uint8_t gain)
{
  time_t start;

  // set config and start conversion
   uint8_t  config[3];
   config[0]= REG_CONFIG;
   config[1]= (multiplexer << 4) | gain << 1 | 0x81;
   config[2]= rate << 5 | 3;

   if(write(i2c_handle, config,3) != 3)
       return -1; //  error writing

  // waiting for data ready escape after 2 seconds
  start = time(NULL);

   if( write(i2c_handle, config,1) != 1)
       return -2;

  while(1)
   {

     if((time(NULL) - start) > 3) break; // to long

     if(read(i2c_handle,&config[1],1) != 1)
       return -3; //  error reading


     if(config[1] & 0x80)
      {
            config[0]=REG_CONV;
            if( write(i2c_handle, config,1) != 1)
               return -4;
            if(read(i2c_handle,&config[1],2) != 2)
              return -5; //  error reading
            return config[1]*256+config[2];

      }
   }
   return -6 ; // no suppose to be there
};





int main (void)
{

  int avgValue;
  char * Vref[]={"6.144","4.096","2.048","1.024","0.512","0.256"};
  int    rate[]={8, 16, 32, 64, 128, 250, 475, 860};
  int r,i;
  int value;

   int i2c_handle;

   int I2C_Current_Slave_Adress=0x48;

   i2c_handle = open("/dev/i2c-3",O_RDWR);
        if(i2c_handle <0)
          {
             printf("unable to get I2C handle\n");
             return -1;
          }

   ioctl(i2c_handle,I2C_SLAVE,I2C_Current_Slave_Adress);

   for(r=0;r<8;r++)
  {
    // let`s calulate number of scan for 2 sec
    int nb_scan =  rate[r] * 2;
    avgValue=0;

    printf("Scan %4d samples at rate %3d Samples/sec for 2 sec  ...", nb_scan, rate[r]);
    fflush(stdout);

    time_t now = time(NULL);
    while (now == time(NULL));
    now = time(NULL);

    unsigned short int passou = 0;
    int16_t primeiro_num = 0;

    for(i=0;i<nb_scan;i++)
     {
      value = readADC(i2c_handle,AIN0,r,GAIN_1024MV);
      if(value <0)
       {
           printf("error\n");
           break;
       }
       
      passou = 1;
      primeiro_num = value;

      if (value > 32767)
         value -= 65535;
     avgValue+=value;
     if(time(NULL)- now >=2) break;

    }


    int diff = time(NULL) - now;

    printf("---> got  %d Samples/sec with avg value of %.1f, first value 0x%X, last value 0x%X\n", i / diff,1.0 * avgValue / i , primeiro_num, value);
  }
  return 0 ;
}
