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

#ifndef EPD2IN9T5_H
#define EPD2IN9T5_H

#include <SPI.h>
#include "DisplayDriver.h"

// Display resolution
#define EPD_WIDTH       128
#define EPD_HEIGHT      296

// EPD2IN9 commands
#define EPD_PANEL_SETTING                 0x00
#define EPD_POWER_SETTING                 0x01
#define EPD_POWER_OFF                     0x02
#define EPD_POWER_ON                      0x04
#define EPD_BOOSTER_SOFT_START            0x06
#define EPD_DEEPL_SLEEP                   0x07
#define EPD_DISPLAY_START_TRANSMISSION_1  0x10
#define EPD_REFRESH_FRAME                 0x12
#define EPD_DISPLAY_START_TRANSMISSION_2  0x13
#define EPD_SET_LUT_VCOM                  0x20
#define EPD_SET_LUT_WW                    0x21
#define EPD_SET_LUT_BW                    0x22
#define EPD_SET_LUT_WB                    0x23
#define EPD_SET_LUT_BB                    0x24
#define EPD_PLL_SETTING                   0x30
#define EPD_VCOM_DATA_INTERNAL_SETTING    0x50
#define EPD_RESOLUTION_SETTING            0x61
#define EPD_VCOM_DC_SETTING               0x82
#define EPD_PARTIAL_RESOLUTION_SETTING    0x90
#define EPD_PARTIAL_IN                    0x91
#define EPD_PARTIAL_OUT                   0x92


class EPD_WaveShare29T5 : public DisplayDriver {
public:
    unsigned long width;
    unsigned long height;

    EPD_WaveShare29T5(uint8_t csPin, uint8_t rstPin, uint8_t dcPin, uint8_t busyPin);
    ~EPD_WaveShare29T5();

    void setRotation(uint8_t r);
    void init();

    void writeBuffer(BufferInfo *bufferInfo);

    int  IfInit(void);
    void DigitalWrite(int pin, int value);
    int  DigitalRead(int pin);
    void DelayMs(unsigned int delaytime);
    void SpiTransfer(unsigned char data);

    void SendCommand(unsigned char command);
    void SendData(unsigned char data);
    void WaitUntilIdle(void);
    void Reset(void);

    void DisplayFrame(void);
    void Sleep(void);
    void setFastRefresh(boolean isFastRefreshEnabled);

private:
    uint8_t rotation;
    unsigned int reset_pin;
    unsigned int dc_pin;
    unsigned int cs_pin;
    unsigned int busy_pin;

    uint8_t reverse(uint8_t in);
    uint8_t getPixel(uint8_t *buffer, uint16_t x, uint16_t y, uint16_t bufferWidth, uint16_t bufferHeight);
    void SetLut();
    void SetMemoryArea(int x_start, int y_start, int x_end, int y_end);

    static const unsigned char lut_20_vcomDC_2bit[];
    static const unsigned char lut_21_ww_2bit[];
    static const unsigned char lut_22_bw_2bit[];
    static const unsigned char lut_23_wb_2bit[];
    static const unsigned char lut_24_bb_2bit[];
    static const unsigned char* lut_2bit[];

    static const unsigned char lut_20_vcomDC[];
    static const unsigned char lut_21_ww[];
    static const unsigned char lut_22_bw[];
    static const unsigned char lut_23_wb[];
    static const unsigned char lut_24_bb[];
    static const unsigned char* lut_full[];

    static const unsigned char lut_20_vcomDC_partial[];
    static const unsigned char lut_21_ww_partial[];
    static const unsigned char lut_22_bw_partial[];
    static const unsigned char lut_23_wb_partial[];
    static const unsigned char lut_24_bb_partial[];
    static const unsigned char* lut_partial[];

};

#endif /* EPD2IN9T5_H */

/* END OF FILE */
