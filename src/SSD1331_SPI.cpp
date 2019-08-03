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

#include "SSD1331_SPI.h"

// If the SPI library has transaction support, these functions
// establish settings and protect from interference from other
// libraries.  Otherwise, they simply do nothing.
#ifdef SPI_HAS_TRANSACTION
static inline void spi_begin(void) __attribute__((always_inline));
static inline void spi_begin(void) {
#if defined (ARDUINO_ARCH_ARC32)
  // max speed!
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
#else
    // max speed!
  SPI.beginTransaction(SPISettings(40000000, MSBFIRST, SPI_MODE0));
#endif
}
static inline void spi_end(void) __attribute__((always_inline));
static inline void spi_end(void) {
  SPI.endTransaction();
}
#else
#define spi_begin()
#define spi_end()
#endif

// Constructor when using software SPI.  All output pins are configurable.
SSD1331_SPI::SSD1331_SPI(int8_t cs, int8_t dc, int8_t mosi,
           int8_t sclk, int8_t rst, int8_t miso) : DisplayDriver(SSD1331_WIDTH, SSD1331_HEIGHT) {
  _cs   = cs;
  _dc   = dc;
  _mosi  = mosi;
  _miso = miso;
  _sclk = sclk;
  _rst  = rst;
  hwSPI = false;
}


// Constructor when using hardware SPI.  Faster, but must use SPI pins
// specific to each board type (e.g. 11,13 for Uno, 51,52 for Mega, etc.)
SSD1331_SPI::SSD1331_SPI(int8_t cs, int8_t dc, int8_t rst) : DisplayDriver(SSD1331_WIDTH, SSD1331_HEIGHT) {
  _cs   = cs;
  _dc   = dc;
  _rst  = rst;
  hwSPI = true;
  _mosi  = _sclk = 0;
}


void SSD1331_SPI::init(void) {

  if (_rst > 0) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, LOW);
  }

  pinMode(_dc, OUTPUT);
  pinMode(_cs, OUTPUT);

#if defined (USE_FAST_PINIO)
  csport    = portOutputRegister(digitalPinToPort(_cs));
  cspinmask = digitalPinToBitMask(_cs);
  dcport    = portOutputRegister(digitalPinToPort(_dc));
  dcpinmask = digitalPinToBitMask(_dc);
#endif

  if(hwSPI) { // Using hardware SPI
    SPI.begin();

#ifndef SPI_HAS_TRANSACTION
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
  #if defined (_AVR__)
    SPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz (full! speed!)
    mySPCR = SPCR;
  #elif defined(TEENSYDUINO)
    SPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz (full! speed!)
  #elif defined (__arm__)
    SPI.setClockDivider(11); // 8-ish MHz (full! speed!)
  #endif
#endif
  } else {
    pinMode(_sclk, OUTPUT);
    pinMode(_mosi, OUTPUT);
    pinMode(_miso, INPUT);

  }

  // toggle RST low to reset
  if (_rst > 0) {
    digitalWrite(_rst, HIGH);
    delay(5);
    digitalWrite(_rst, LOW);
    delay(20);
    digitalWrite(_rst, HIGH);
    delay(150);
  }


  if (hwSPI) spi_begin();
  // Initialization Sequence
  writeCommand(SSD1331_CMD_DISPLAYOFF);  	// 0xAE
  writeCommand(SSD1331_CMD_SETREMAP); 	// 0xA0
  #if defined SSD1331_COLORORDER_RGB
  writeCommand(0x72);				// RGB Color
  #else
  writeCommand(0x76);				// BGR Color
  #endif
  writeCommand(SSD1331_CMD_STARTLINE); 	// 0xA1
  writeCommand(0x0);
  writeCommand(SSD1331_CMD_DISPLAYOFFSET); 	// 0xA2
  writeCommand(0x0);
  writeCommand(SSD1331_CMD_NORMALDISPLAY);  	// 0xA4
  writeCommand(SSD1331_CMD_SETMULTIPLEX); 	// 0xA8
  writeCommand(0x3F);  			// 0x3F 1/64 duty
  writeCommand(SSD1331_CMD_SETMASTER);  	// 0xAD
  writeCommand(0x8E);
  writeCommand(SSD1331_CMD_POWERMODE);  	// 0xB0
  writeCommand(0x0B);
  writeCommand(SSD1331_CMD_PRECHARGE);  	// 0xB1
  writeCommand(0x31);
  writeCommand(SSD1331_CMD_CLOCKDIV);  	// 0xB3
  writeCommand(0xF0);  // 7:4 = Oscillator Frequency, 3:0 = CLK Div Ratio (A[3:0]+1 = 1..16)
  writeCommand(SSD1331_CMD_PRECHARGEA);  	// 0x8A
  writeCommand(0x64);
  writeCommand(SSD1331_CMD_PRECHARGEB);  	// 0x8B
  writeCommand(0x78);
  writeCommand(SSD1331_CMD_PRECHARGEA);  	// 0x8C
  writeCommand(0x64);
  writeCommand(SSD1331_CMD_PRECHARGELEVEL);  	// 0xBB
  writeCommand(0x3A);
  writeCommand(SSD1331_CMD_VCOMH);  		// 0xBE
  writeCommand(0x3E);
  writeCommand(SSD1331_CMD_MASTERCURRENT);  	// 0x87
  writeCommand(0x06);
  writeCommand(SSD1331_CMD_CONTRASTA);  	// 0x81
  writeCommand(0x91);
  writeCommand(SSD1331_CMD_CONTRASTB);  	// 0x82
  writeCommand(0x50);
  writeCommand(SSD1331_CMD_CONTRASTC);  	// 0x83
  writeCommand(0x7D);
  writeCommand(SSD1331_CMD_DISPLAYON);	//--turn on oled panel
  if (hwSPI) spi_end();
  delay(120);
}

void SSD1331_SPI::spiwrite(uint8_t c) {

  //Serial.print("0x"); Serial.print(c, HEX); Serial.print(", ");

  if (hwSPI) {
#if defined (__AVR__)
  #ifndef SPI_HAS_TRANSACTION
    uint8_t backupSPCR = SPCR;
    SPCR = mySPCR;
  #endif
    SPDR = c;
    while(!(SPSR & _BV(SPIF)));
  #ifndef SPI_HAS_TRANSACTION
    SPCR = backupSPCR;
  #endif
#else
    SPI.transfer(c);
#endif
  } else {
#if defined(ESP8266) || defined(ESP32) || defined (ARDUINO_ARCH_ARC32)
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      if(c & bit) {
  digitalWrite(_mosi, HIGH);
      } else {
  digitalWrite(_mosi, LOW);
      }
      digitalWrite(_sclk, HIGH);
      digitalWrite(_sclk, LOW);
    }
#else
    // Fast SPI bitbang swiped from LPD8806 library
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      if(c & bit) {
  //digitalWrite(_mosi, HIGH);
  *mosiport |=  mosipinmask;
      } else {
  //digitalWrite(_mosi, LOW);
  *mosiport &= ~mosipinmask;
      }
      //digitalWrite(_sclk, HIGH);
      *clkport |=  clkpinmask;
      //digitalWrite(_sclk, LOW);
      *clkport &= ~clkpinmask;
    }
#endif
  }
}

void SSD1331_SPI::setFastRefresh(boolean isFastRefreshEnabled) {
  // Not enabled at the moment
}


void SSD1331_SPI::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  Serial.printf("%d, %d, %d, %d\n", x0, y0, x1, y1);
  if (x0 > 95) x0 = 95;
  if (y0 > 63) y0 = 63;


  if (x1 > 95) x1 = 95;
  if (y1 > 63) y1 = 63;

  if (x0 > x1) {
    uint8_t t = x1;
    x1 = x0;
    x0 = t;
  }
  if (y0 > y1) {
    uint8_t t = y1;
    y1 = y0;
    y0 = t;
  }


  if (hwSPI) spi_begin();
  writeCommand(0x15); // Column addr set
  writeCommand(x0);
  writeCommand(x1);
  if (hwSPI) spi_end();

  if (hwSPI) spi_begin();
  writeCommand(0x75); // Column addr set
  writeCommand(y0);
  writeCommand(y1);
  if (hwSPI) spi_end();

  //startWrite();
}
void SSD1331_SPI::setRotation(uint8_t r) {

}

void SSD1331_SPI::writeBuffer(BufferInfo *bufferInfo) {
      uint8_t *buffer = bufferInfo->buffer;
      uint16_t bufferWidth = bufferInfo->bufferWidth;
      uint16_t bufferHeight = bufferInfo->bufferHeight;
      uint16_t xPos = bufferInfo->targetX;
      uint16_t yPos = bufferInfo->targetY;
      uint16_t *palette = bufferInfo->palette;
      uint8_t bitsPerPixel = bufferInfo->bitsPerPixel;

      setAddrWindow(xPos, yPos, xPos + bufferWidth - 1, yPos + bufferHeight -1 );

      digitalWrite(_dc, HIGH);
      digitalWrite(_cs, LOW);
      if (hwSPI) spi_begin();
      if (bitsPerPixel == 16) {
        SPI.writeBytes(buffer, bufferWidth * bufferHeight * 2);
      } else {
        // line buffer is in 16bit target format
        uint8_t lineBuffer[_width * 2];
        uint16_t pos;
        uint8_t bufferByte;
        uint8_t paletteEntry;
        uint16_t color;
        uint8_t shift;
        uint8_t mask = (1 << bitsPerPixel) - 1;
        uint8_t packagesPerBytes = 8 / bitsPerPixel;
        uint16_t bytePos = 0;
        uint16_t pixelCounter = 0;
        uint16_t bufferSize = bufferWidth * bufferHeight / packagesPerBytes;
        uint8_t bytesPerLine = bufferWidth / packagesPerBytes;
        uint16_t x = 0;
        for (uint16_t y = 0; y < bufferHeight; y++) {

          for (uint16_t b = 0; b < bytesPerLine; b++) {

            for (uint8_t p = 0; p < packagesPerBytes; p++) {
              x = b * packagesPerBytes + p;
              bufferByte = buffer[bytePos];
              shift = p * bitsPerPixel;
              paletteEntry = (bufferByte >> shift) & mask;
              color = palette[paletteEntry];
              lineBuffer[x * 2] = color >> 8;
              lineBuffer[x * 2 + 1] = color;
            }
            bytePos++;
          }
          SPI.writeBytes(lineBuffer, bufferWidth * 2);
        }
      }
      digitalWrite(_cs, HIGH);

      if (hwSPI) spi_end();
}


void SSD1331_SPI::writeCommand(uint8_t c) {
  digitalWrite(_dc, LOW);
  digitalWrite(_sclk, LOW);
  digitalWrite(_cs, LOW);
  spiwrite(c);
  digitalWrite(_cs, HIGH);

}


void SSD1331_SPI::writedata(uint8_t c) {
  digitalWrite(_dc, HIGH);
  digitalWrite(_cs, LOW);

  spiwrite(c);

  digitalWrite(_cs, HIGH);

}
