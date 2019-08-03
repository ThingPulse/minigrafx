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

#ifndef _MINIGRAFX_DRIVER_H
#define _MINIGRAFX_DRIVER_H

#if ARDUINO >= 100
 #include "Arduino.h"

#else
 #include "WProgram.h"
#endif


struct BufferInfo {
  // the frame buffer
  uint8_t *buffer;
  // number of bits used for one pixel
  uint8_t bitsPerPixel;
  // the palette in case the framebuffer uses a different bit depth than the display
  uint16_t *palette;
  // x origin of the area to copy
  uint16_t windowX;
  // y origin of the area to copy
  uint16_t windowY;
  // width of the area to be copied
  uint16_t windowWidth;
  // height of the area to be copied
  uint16_t windowHeight;
  // x where to copy the window to
  uint16_t targetX;
  // y where to copy the window to
  uint16_t targetY;
  // width of the source buffer
  uint16_t bufferWidth;
  // height of the source buffer
  uint16_t bufferHeight;

};

class DisplayDriver {

 public:

  DisplayDriver(int16_t w, int16_t h); // Constructor

  virtual void setRotation(uint8_t r);
  virtual void init() = 0;
  virtual void writeBuffer(BufferInfo *bufferInfo) = 0;
  virtual void setFastRefresh(boolean isEnabled) = 0;

  int16_t height(void) const;
  int16_t width(void) const;

  uint8_t getRotation(void) const;


 protected:
  const int16_t
    WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
  int16_t _width, _height;
  uint8_t rotation;
  boolean isFastRefreshEnabled = false;

};


#endif // _MINIGRAFX_DRIVER_H
