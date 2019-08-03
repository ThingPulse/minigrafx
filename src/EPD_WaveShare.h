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

#if !defined(EPDWS154_H)
#define EPDWS154_H 1

#include <Arduino.h>
#include <SPI.h>
#include "DisplayDriver.h"


#define EPD_BUSY_LEVEL 0

//#define EPD1X54 1
#define EPD2X9 1
/*#ifdef EPD2X9
  #define xDot 128
  #define yDot 296
  #define DELAYTIME 1500
// static const unsigned char GDVol[] = {0x03,0xea}; // Gate voltage +15V/-15V
#elif  EPD02X13
  #define xDot 128
  #define yDot 250
  #define DELAYTIME 4000
// static const  unsigned char GDVol[] = {0x03,0xea};  // Gate voltage +15V/-15V
#elif  EPD1X54
  #define xDot 200
  #define yDot 200
  #define DELAYTIME 1500
// static const  unsigned char GDVol[] = {0x03,0x00};  // Gate voltage +15V/-15V
#endif*/

//static const unsigned char GDOControl[]={0x01,(yDot-1)%256,(yDot-1)/256,0x00}; //for 1.54inch
static const unsigned char softstart[]={0x0c,0xd7,0xd6,0x9d};
//  static const unsigned char Rambypass[] = {0x21,0x8f};   // Display update
//  static const unsigned char MAsequency[] = {0x22,0xf0};    // clock
//  static const unsigned char SDVol[] = {0x04,0x0a}; // Source voltage +15V/-15V
static const unsigned char VCOMVol[] = {0x2c,0xa8}; // VCOM 7c
//  static const unsigned char BOOSTERFB[] = {0xf0,0x1f}; // Source voltage +15V/-15V
static const unsigned char DummyLine[] = {0x3a,0x1a}; // 4 dummy line per gate
static const unsigned char Gatetime[] = {0x3b,0x08};  // 2us per line
//  static const unsigned char BorderWavefrom[] = {0x3c,0x33};  // Border
static const unsigned char RamDataEntryMode[] = {0x11,0x01};  // Ram data entry mode

#ifdef EPD02X13
  static const unsigned char LUTDefault_full[]={
  0x32, // command
  0x22,0x55,0xAA,0x55,0xAA,0x55,0xAA,0x11,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x01,0x00,0x00,0x00,0x00,
  };
  static const unsigned char LUTDefault_part[]={
  0x32, // command
  0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  };
#else
    //Write LUT register
  static const unsigned char LUTDefault_part[31] = {
    0x32, // command
    0x10,0x18,0x18,0x08,0x18,0x18,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x13,0x14,0x44,0x12,0x00,0x00,0x00,0x00,0x00,0x00
  };
  static const unsigned char LUTDefault_full[31] = {
    0x32, // command
    0x02,0x02,0x01,0x11,0x12,0x12,0x22,0x22,0x66,0x69,0x69,0x59,0x58,0x99,0x99,0x88,0x00,0x00,0x00,0x00,0xF8,0xB4,0x13,0x51,0x35,0x51,0x51,0x19,0x01,0x00
  };
#endif

enum EPD_TYPE {
  EPD1_54,
  EPD02_13,
  EPD2_9
};

class EPD_WaveShare : public DisplayDriver {

  public:
    EPD_WaveShare(EPD_TYPE epdType, uint8_t csPin, uint8_t rstPin, uint8_t dcPin, uint8_t busyPin);

    void setRotation(uint8_t r);
    void init();

    void writeBuffer(BufferInfo *bufferInfo);

    void EPD_init_Part(void);
    void Dis_Clear_full(void);

    static int getWidth(EPD_TYPE epdType);
    static int getHeight(EPD_TYPE epdType);

  private:
    uint8_t csPin;
    uint8_t rstPin;
    uint8_t dcPin;
    uint8_t busyPin;
    uint8_t rotation;
    uint16_t delaytime;
    uint16_t xDot;
    uint16_t yDot;
    uint16_t bufferWidth;
    uint16_t bufferHeight;
    unsigned char GDOControl[4];

    unsigned char ReadBusy(void);
    uint8_t Reverse_bits(uint8_t num);
    uint8_t getPixel(uint8_t *buffer, uint16_t x, uint16_t y);
    void EPD_WriteCMD(unsigned char command);
    void EPD_WriteCMD_p1(unsigned char command,unsigned char para);
    void EPD_POWERON(void);
    void EPD_Write(const unsigned char *value, unsigned char datalen);
    void EPD_WriteDispRam(unsigned char XSize,unsigned int YSize,unsigned char *Dispbuff);
    void EPD_WriteDispRamMono(unsigned char XSize,unsigned int YSize,unsigned char dispdata);
    void EPD_SetRamArea(unsigned char Xstart,unsigned char Xend,unsigned char Ystart,unsigned char Ystart1,unsigned char Yend,unsigned char Yend1);
    void EPD_SetRamPointer(unsigned char addrX,unsigned char addrY,unsigned char addrY1);
    void EPD_part_display(unsigned char RAM_XST,unsigned char RAM_XEND,unsigned char RAM_YST,unsigned char RAM_YST1,unsigned char RAM_YEND,unsigned char RAM_YEND1);
    void EPD_Init(void);
    void EPD_Update(void);
    void EPD_Update_Part(void);
    void EPD_WirteLUT(unsigned char *LUTvalue,unsigned char Size);
    void EPD_init_Full(void);
    void EPD_Dis_Full(unsigned char *DisBuffer,unsigned char Label);
    void EPD_Dis_Part(unsigned char xStart,unsigned char xEnd,unsigned long yStart,unsigned long yEnd,unsigned char *DisBuffer,unsigned char Label);
    void Dis_Char(char acsii,char size,char mode,char next,unsigned char *buffer);
    void setFastRefresh(boolean isFastRefreshEnabled);

    void driver_delay_xms(unsigned long xms);
    void SPI_Write(unsigned char value);


};

#endif
