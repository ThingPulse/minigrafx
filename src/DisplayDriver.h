#ifndef _MINIGRAFX_DRIVER_H
#define _MINIGRAFX_DRIVER_H

#if ARDUINO >= 100
 #include "Arduino.h"

#else
 #include "WProgram.h"
#endif
#include "gfxfont.h"

class DisplayDriver {

 public:

  DisplayDriver(int16_t w, int16_t h); // Constructor

  virtual void setRotation(uint8_t r);
  virtual void init() = 0;
  //virtual uint16_t getScreenWidth() = 0;
  //virtual uint16_t getScreenHeight() = 0;
  virtual void writeBuffer(uint8_t *buffer, uint8_t bitsPerPixel, uint16_t *palette) = 0;

  int16_t height(void) const;
  int16_t width(void) const;

  uint8_t getRotation(void) const;


 protected:
  const int16_t
    WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
  int16_t
    _width, _height, // Display w/h as modified by current rotation
    cursor_x, cursor_y;
  uint16_t
    textcolor, textbgcolor;
  uint8_t
    textsize,
    rotation;
  boolean
    wrap,   // If set, 'wrap' text at right edge of display
    _cp437; // If set, use correct CP437 charset (default is off)
  GFXfont
    *gfxFont;
};


#endif // _MINIGRAFX_DRIVER_H
