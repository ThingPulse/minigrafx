
#ifndef _MINIGRAFX_CSN_A2_
#define _MINIGRAFX_CSN_A2_

#include <SPI.h>
#include <Arduino.h>
#include "DisplayDriver.h"
#include <SoftwareSerial.h>


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
