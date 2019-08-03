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

#ifndef EPD2IN9_H
#define EPD2IN9_H

#include <SPI.h>
#include "DisplayDriver.h"

// Display resolution
#define EPD_WIDTH       128
#define EPD_HEIGHT      296

// EPD2IN9 commands
#define DRIVER_OUTPUT_CONTROL                       0x01
#define BOOSTER_SOFT_START_CONTROL                  0x0C
#define GATE_SCAN_START_POSITION                    0x0F
#define DEEP_SLEEP_MODE                             0x10
#define DATA_ENTRY_MODE_SETTING                     0x11
#define SW_RESET                                    0x12
#define TEMPERATURE_SENSOR_CONTROL                  0x1A
#define MASTER_ACTIVATION                           0x20
#define DISPLAY_UPDATE_CONTROL_1                    0x21
#define DISPLAY_UPDATE_CONTROL_2                    0x22
#define WRITE_RAM                                   0x24
#define WRITE_VCOM_REGISTER                         0x2C
#define WRITE_LUT_REGISTER                          0x32
#define SET_DUMMY_LINE_PERIOD                       0x3A
#define SET_GATE_TIME                               0x3B
#define BORDER_WAVEFORM_CONTROL                     0x3C
#define SET_RAM_X_ADDRESS_START_END_POSITION        0x44
#define SET_RAM_Y_ADDRESS_START_END_POSITION        0x45
#define SET_RAM_X_ADDRESS_COUNTER                   0x4E
#define SET_RAM_Y_ADDRESS_COUNTER                   0x4F
#define TERMINATE_FRAME_READ_WRITE                  0xFF


const unsigned char lut_full_update[] =
{
  0x32,  // command
  0x50, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char lut_partial_update[] =
{
  0x32,  // command
  0x10, 0x18, 0x18, 0x08, 0x18, 0x18, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x13, 0x14, 0x44, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

class EPD_WaveShare29 : public DisplayDriver {
public:
    unsigned long width;
    unsigned long height;

    EPD_WaveShare29(uint8_t csPin, uint8_t rstPin, uint8_t dcPin, uint8_t busyPin);
    ~EPD_WaveShare29();

    void setRotation(uint8_t r);
    void init();

    void writeBuffer(BufferInfo *bufferInfo);

    int Init(const unsigned char* lut);

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
    const unsigned char* lut;

    uint8_t reverse(uint8_t in);
    uint8_t getPixel(uint8_t *buffer, uint16_t x, uint16_t y, uint16_t bufferWidth, uint16_t bufferHeight);
    void SetLut(const unsigned char* lut);
    void SetMemoryArea(int x_start, int y_start, int x_end, int y_end);
    void SetMemoryPointer(int x, int y);
};

#endif /* EPD2IN9_H */

/* END OF FILE */
