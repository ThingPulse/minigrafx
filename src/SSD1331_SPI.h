
#ifndef _MINIGRAFX_SSD1331_
#define _MINIGRAFX_SSD1331_

#include <SPI.h>
#include <Arduino.h>
#include "DisplayDriver.h"

#define SSD1331_WIDTH 96
#define SSD1331_HEIGHT 64

// Timing Delays
#define SSD1331_DELAYS_HWFILL		(3)
#define SSD1331_DELAYS_HWLINE       (1)

// SSD1331 Commands
#define SSD1331_CMD_DRAWLINE 		0x21
#define SSD1331_CMD_DRAWRECT 		0x22
#define SSD1331_CMD_FILL 			0x26
#define SSD1331_CMD_SETCOLUMN 		0x15
#define SSD1331_CMD_SETROW    		0x75
#define SSD1331_CMD_CONTRASTA 		0x81
#define SSD1331_CMD_CONTRASTB 		0x82
#define SSD1331_CMD_CONTRASTC		0x83
#define SSD1331_CMD_MASTERCURRENT 	0x87
#define SSD1331_CMD_SETREMAP 		0xA0
#define SSD1331_CMD_STARTLINE 		0xA1
#define SSD1331_CMD_DISPLAYOFFSET 	0xA2
#define SSD1331_CMD_NORMALDISPLAY 	0xA4
#define SSD1331_CMD_DISPLAYALLON  	0xA5
#define SSD1331_CMD_DISPLAYALLOFF 	0xA6
#define SSD1331_CMD_INVERTDISPLAY 	0xA7
#define SSD1331_CMD_SETMULTIPLEX  	0xA8
#define SSD1331_CMD_SETMASTER 		0xAD
#define SSD1331_CMD_DISPLAYOFF 		0xAE
#define SSD1331_CMD_DISPLAYON     	0xAF
#define SSD1331_CMD_POWERMODE 		0xB0
#define SSD1331_CMD_PRECHARGE 		0xB1
#define SSD1331_CMD_CLOCKDIV 		0xB3
#define SSD1331_CMD_PRECHARGEA 		0x8A
#define SSD1331_CMD_PRECHARGEB 		0x8B
#define SSD1331_CMD_PRECHARGEC 		0x8C
#define SSD1331_CMD_PRECHARGELEVEL 	0xBB
#define SSD1331_CMD_VCOMH 			0xBE


class SSD1331_SPI : public DisplayDriver {

 public:
   SSD1331_SPI(int8_t _CS, int8_t _DC, int8_t _MOSI, int8_t _SCLK, int8_t _RST, int8_t _MISO);
   SSD1331_SPI(int8_t _CS, int8_t _DC, int8_t _RST = -1);

   void init(void);
   void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
   void setRotation(uint8_t r);

   void writeBuffer(uint8_t *buffer, uint8_t bitsPerPixel, uint16_t *palette, uint16_t x, uint16_t y, uint16_t bufferWidth, uint16_t bufferHeight);
   //void pushColor(uint16_t color);
   void setFastRefresh(boolean isFastRefreshEnabled);

   void spiwrite(uint8_t);
   void writeCommand(uint8_t c);
   void writedata(uint8_t d);
private:
  boolean  hwSPI;
  int32_t  _cs, _dc, _rst, _mosi, _miso, _sclk;

};

#endif
