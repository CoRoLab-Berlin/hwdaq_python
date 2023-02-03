/*
# SPDX-FileCopyrightText: 2023 Manuel Weiss <manuel.weiss@bht-berlin.de>
# SPDX-License-Identifier: MIT
# language = c++
*/

#ifndef hwdaq_hpp
#define hwdaq_hpp

// if os is linux
#if defined(__linux__)
#include <fcntl.h>  //open RDWR
#include <linux/spi/spidev.h>
#include <stddef.h>
#include <stdint.h>  //_t type ending
#include <stdio.h>   //err out
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>  //close socket
#include <chrono>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#define LINUX
#elif defined(__unix__) || defined(__APPLE__) || defined(__MACH__)
#include <stdio.h>
#include <cstring>
#include <iostream>
#define MAC
// for testing without hardware only
#endif

#define VOUT_LENGTH 4
#define VIN_LEN 8
#define LTC_WRITE 0b0011

class HWDAQ {

  public:
   HWDAQ();
   ~HWDAQ();
   void getADC(double* vin);
   void setDAC(double vout[VOUT_LENGTH]);

  private:
   int fdADC;
   int fdDAC;

#ifdef LINUX
   struct spi_ioc_transfer dacDataframe;
   struct spi_ioc_transfer adcDataframe;
   uint8_t adcReadCode[8 + 1];
#endif
};

#endif /* hwdaq_hpp */