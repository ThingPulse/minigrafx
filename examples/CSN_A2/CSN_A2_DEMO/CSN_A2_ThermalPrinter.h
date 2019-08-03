/**
The MIT License (MIT)
Copyright (c) 2019 by Daniel Eichhorn, ThingPulse
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please note: We are spending a lot of time to write and maintain open source codes
Please support us by buying our products from https://thingpulse.com/shop/

See more at https://thingpulse.com

Many thanks go to various contributors such as Adafruit, Waveshare.
*/

#ifndef _MINIGRAFX_CSN_A2_
#define _MINIGRAFX_CSN_A2_

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "DisplayDriver.h"



#define CSN_A2_WIDTH 384
#define CSN_A2_HEIGHT 384

#define LF    0x0A;
 // Horizontal TAB
#define HT    0x09
#define ESC   0x1B
// group seperator
#define GS    0x1D
// space
#define SP    0x20
// NP form feed; new page
#define FF    0x0C

#define ASCII_DC2  18

class CSN_A2 : public DisplayDriver {

 public:
   CSN_A2(int8_t _RX, int8_t _TX);

   void init(void);
   void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
   void setRotation(uint8_t r);

   void wake();

   void writeBuffer(BufferInfo *bufferInfo);

   void setFastRefresh(boolean isFastRefreshEnabled);

   void spiwrite(uint8_t);
   void writeCommand(uint8_t c);
   void writedata(uint8_t d);
   void timeoutWait();
   uint8_t reverse(uint8_t in);
private:
  boolean  hwSPI;
  int32_t  _rx, _tx;
  SoftwareSerial *serial;

};

#endif
