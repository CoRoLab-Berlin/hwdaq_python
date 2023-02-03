/*
# SPDX-FileCopyrightText: 2023 Manuel Weiss <manuel.weiss@bht-berlin.de>
# SPDX-License-Identifier: MIT
# language = c++
*/

#include "hwdaq.hpp"

HWDAQ::HWDAQ() {
   int SPI_SPEED = 5000000;
   int SPI_MODE = 0;
   static const char* SPI_DAC_DEV = "/dev/spidev0.1";
   static const char* SPI_ADC_DEV = "/dev/spidev0.0";
   // if LINUX definded do this
#ifdef LINUX
   // configuration spi_ioc_transfer DAC
   std::memset(&this->dacDataframe, 0, sizeof(this->dacDataframe));
   this->dacDataframe.len = 3;
   this->dacDataframe.speed_hz = SPI_SPEED;
   this->dacDataframe.bits_per_word = 8;

   // configuration spi_ioc_transfer ADC
   std::memset(&this->adcDataframe, 0, sizeof(this->adcDataframe));
   this->adcDataframe.len = 2;
   this->adcDataframe.speed_hz = SPI_SPEED;
   this->adcDataframe.bits_per_word = 8;

   // open sockets ADC & DAC
   this->fdDAC = open(SPI_DAC_DEV, O_RDWR);
   ioctl(this->fdDAC, SPI_IOC_WR_MODE, SPI_MODE);
   ioctl(this->fdDAC, SPI_IOC_RD_MAX_SPEED_HZ, SPI_SPEED);

   this->fdADC = open(SPI_ADC_DEV, O_RDWR);
   ioctl(this->fdADC, SPI_IOC_WR_MODE, SPI_MODE);
   ioctl(this->fdADC, SPI_IOC_RD_MAX_SPEED_HZ, SPI_SPEED);

   // codes to read ADC channels
   this->adcReadCode[0] = 0b10000100;
   this->adcReadCode[1] = 0b11000100;
   this->adcReadCode[2] = 0b10010100;
   this->adcReadCode[3] = 0b11010100;
   this->adcReadCode[4] = 0b10100100;
   this->adcReadCode[5] = 0b11100100;
   this->adcReadCode[6] = 0b10110100;
   this->adcReadCode[7] = 0b11110100;
   this->adcReadCode[8] = 0b10000100;
#endif
#ifdef MAC
   std::cout << "This is not a linux system" << std::endl;
#endif
}

void HWDAQ::getADC(double* vin) {
   int16_t readBuffer = 0;
   unsigned char txbuf[2];
   unsigned char rxbuf[2];
#ifdef LINUX
   //    std::cout << "This is a linux system" << std::endl;
   memset(txbuf, 0, sizeof(txbuf));
   memset(rxbuf, 0, sizeof(rxbuf));
   this->adcDataframe.tx_buf = (unsigned long)txbuf;
   this->adcDataframe.rx_buf = (unsigned long)rxbuf;

   // register configuration for adc port 0
   txbuf[0] = this->adcReadCode[0];
   ioctl(this->fdADC, SPI_IOC_MESSAGE(1), &this->adcDataframe);

   for (int i = 1; i < VIN_LEN + 1; i++) {
      txbuf[0] = this->adcReadCode[i];
      ioctl(this->fdADC, SPI_IOC_MESSAGE(1), &this->adcDataframe);
      readBuffer = rxbuf[0] << 8 | rxbuf[1];
      vin[i - 1] = (double)readBuffer * 10 / 32767;
   }
#endif
#ifdef MAC
   std::cout << "This is not a linux system" << std::endl;
#endif
}

void HWDAQ::setDAC(double vout[VOUT_LENGTH]) {  // DAC LTC1859 12bit
   uint16_t writeBuffer = 0;
   unsigned char txbuf[3];
   unsigned char rxbuf[3];
#ifdef LINUX
   //    std::cout << "This is a linux system" << std::endl;
   memset(txbuf, 0, sizeof txbuf);
   memset(rxbuf, 0, sizeof rxbuf);
   this->dacDataframe.tx_buf = (unsigned long)txbuf;
   this->dacDataframe.rx_buf = (unsigned long)rxbuf;
   for (int i = 0; i < VOUT_LENGTH; i++) {
      writeBuffer = int(vout[i] * 65535 / 10);
      txbuf[0] = ((LTC_WRITE << 4) | i);
      txbuf[1] = (writeBuffer >> 8);
      txbuf[2] = (writeBuffer & 0xFF);

      ioctl(this->fdDAC, SPI_IOC_MESSAGE(1), &this->dacDataframe);
   }
#endif
#ifdef MAC
   std::cout << "This is not a linux system" << std::endl;
#endif
}

HWDAQ::~HWDAQ() {
   // set all DACs to 4.5
   double vout[VOUT_LENGTH];
   for (int i = 0; i < VOUT_LENGTH; i++) {
      vout[i] = 4.5;
   }
   this->setDAC(vout);
}
