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

#include "CSN_A2_ThermalPrinter.h"

// Constructor when using hardware SPI.  Faster, but must use SPI pins
// specific to each board type (e.g. 11,13 for Uno, 51,52 for Mega, etc.)
CSN_A2::CSN_A2(int8_t rx, int8_t tx) : DisplayDriver(CSN_A2_WIDTH, CSN_A2_HEIGHT) {
  this->_rx = rx;
  this->_tx = tx;
}

void CSN_A2::init(void) {
  this->serial = new SoftwareSerial(this->_rx, this->_tx);
  this->serial->begin(9600);

  this->wake();
  delay(500);
  int zero=0;
  int heatTime = 80;
  int heatInterval = 255;
  char printDensity = 15;
  char printBreakTime = 15;

  this->serial->write(27);
  this->serial->write(64);

  this->serial->write(27);
  this->serial->write(55);
  this->serial->write(7); //Default 64 dots = 8*('7'+1)
  this->serial->write(heatTime); //Default 80 or 800us
  this->serial->write(heatInterval); //Default 2 or 20us
  //Modify the print density and timeout
  this->serial->write(18);
  this->serial->write(35);
  int printSetting = (printDensity<<4) | printBreakTime;
  this->serial->write(printSetting); //Combination of printDensity and printBreakTime
}

void CSN_A2::setFastRefresh(boolean isFastRefreshEnabled) {
  // Not enabled at the moment
}

void CSN_A2::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

}
void CSN_A2::setRotation(uint8_t r) {

}

void CSN_A2::wake() {
  // Printer may have been idle for a very long time, during which the
  // micros() counter has rolled over.  To avoid shenanigans, reset the
  // timeout counter before issuing the wake command.
  this->serial->write(255);
  // Datasheet recomments a 50 mS delay before issuing further commands,
  // but in practice this alone isn't sufficient (e.g. text size/style
  // commands may still be misinterpreted on wake).  A slightly longer
  // delay, interspersed with ESC chars (no-ops) seems to help.
  for(uint8_t i=0; i<10; i++) {
    this->serial->write(27);
    delay(100);
  }
}

void CSN_A2::writeBuffer(BufferInfo *bufferInfo) {
   int w = bufferInfo->bufferWidth;
   int h = bufferInfo->bufferHeight;
   const uint8_t *bitmap = bufferInfo->buffer;

   int maxChunkHeight = 255;

   int rowBytes, rowBytesClipped, rowStart, chunkHeight, chunkHeightLimit,
       x, y, i;

   rowBytes        = (w + 7) / 8; // Round up to next byte boundary
   rowBytesClipped = (rowBytes >= 48) ? 48 : rowBytes; // 384 pixels max width
   bool dtrEnabled = false;
   // Est. max rows to write at once, assuming 256 byte printer buffer.
   if(dtrEnabled) {
     chunkHeightLimit = 255; // Buffer doesn't matter, handshake!
   } else {
     chunkHeightLimit = 256 / rowBytesClipped;
     if(chunkHeightLimit > maxChunkHeight) chunkHeightLimit = maxChunkHeight;
     else if(chunkHeightLimit < 1)         chunkHeightLimit = 1;
   }

   for(i=rowStart=0; rowStart < h; rowStart += chunkHeightLimit) {
     // Issue up to chunkHeightLimit rows at a time:
     chunkHeight = h - rowStart;
     if(chunkHeight > chunkHeightLimit) chunkHeight = chunkHeightLimit;

     this->serial->write(ASCII_DC2);
     this->serial->write('*');
     this->serial->write(chunkHeight);
     this->serial->write(rowBytesClipped);

     for(y=0; y < chunkHeight; y++) {
       for(x=0; x < rowBytesClipped; x++, i++) {
         //timeoutWait();
         delay(1);
         this->serial->write(~reverse(*(bitmap+i)));
       }
       i += rowBytes - rowBytesClipped;
     }
     Serial.print(".");
     //timeoutSet(chunkHeight * dotPrintTime);
   }
   this->serial->print(FF);
   // prevByte = '\n';

}

uint8_t CSN_A2::reverse(uint8_t in){
  uint8_t out;
  out = 0;
  if (in & 0x01) out |= 0x80;
  if (in & 0x02) out |= 0x40;
  if (in & 0x04) out |= 0x20;
  if (in & 0x08) out |= 0x10;
  if (in & 0x10) out |= 0x08;
  if (in & 0x20) out |= 0x04;
  if (in & 0x40) out |= 0x02;
  if (in & 0x80) out |= 0x01;

  return(out);
}

void CSN_A2::timeoutWait() {
    //while((long)(micros() - resumeTime) < 0L); // (syntax is rollover-proof)
    //delay(resumeTime);
}
