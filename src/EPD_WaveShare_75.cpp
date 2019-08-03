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

#include <stdlib.h>
#include "EPD_WaveShare_75.h"

EPD_WaveShare75::~EPD_WaveShare75() {
};

EPD_WaveShare75::EPD_WaveShare75(uint8_t cs_pin, uint8_t reset_pin, uint8_t dc_pin, uint8_t busy_pin) : DisplayDriver(EPD_WIDTH, EPD_HEIGHT) {
    this->reset_pin = reset_pin;
    this->dc_pin = dc_pin;
    this->cs_pin = cs_pin;
    this->busy_pin = busy_pin;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

void EPD_WaveShare75::init(void) {
    if (IfInit() != 0) {
        return;
    }
    Reset();

    SendCommand(POWER_SETTING);
    SendData(0x37);
    SendData(0x00);

    SendCommand(PANEL_SETTING);
    SendData(0xCF);
    SendData(0x08);

    SendCommand(BOOSTER_SOFT_START);
    SendData(0xc7);
    SendData(0xcc);
    SendData(0x28);

    SendCommand(POWER_ON);
    WaitUntilIdle();

    SendCommand(PLL_CONTROL);
    SendData(0x3c);

    SendCommand(TEMPERATURE_CALIBRATION);
    SendData(0x00);

    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0x77);

    SendCommand(TCON_SETTING);
    SendData(0x22);

    SendCommand(TCON_RESOLUTION);
    SendData(0x02);     //source 640
    SendData(0x80);
    SendData(0x01);     //gate 384
    SendData(0x80);

    SendCommand(VCM_DC_SETTING);
    SendData(0x1E);      //decide by LUT file

    SendCommand(0xe5);           //FLASH MODE
    SendData(0x03);

    return;
}

void EPD_WaveShare75::setRotation(uint8_t r) {
  this->rotation = r;
  switch(r) {
    case 0:
      bufferWidth = width;
      bufferHeight = height;
      break;
    case 1:
      bufferWidth = height;
      bufferHeight = width;
      break;
    case 2:
      bufferWidth = width;
      bufferHeight = height;
      break;
    case 3:
      bufferWidth = height;
      bufferHeight = width;
      break;
  }
}

/**
 *  @brief: basic function for sending commands
 */
void EPD_WaveShare75::SendCommand(unsigned char command) {
    DigitalWrite(dc_pin, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void EPD_WaveShare75::SendData(unsigned char data) {
    DigitalWrite(dc_pin, HIGH);
    SpiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void EPD_WaveShare75::WaitUntilIdle(void) {
    while(DigitalRead(busy_pin) == 0) {      //0: busy, 1: idle
        DelayMs(100);
    }
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void EPD_WaveShare75::Reset(void) {
    DigitalWrite(reset_pin, LOW);                //module reset
    DelayMs(200);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);
}

/**
 * Transmits the frame_buffer to the display.
 * If bitsPerPixel is greater than 1, the color palette is used to transform
 * the colors in the frame buffer to real color values of the display.
 * If bitsPerPixel is 1, then the value 1 in the frame buffer is interpreted
 * as white, other values are treated as black.
 * @param frame_buffer Contents to be displayed.
 * @param bitsPerPixel Number of Bits used to represent a pixel.
 * @param palette Color palette used, if bitsPerPixel is greater then 1.
 */
void EPD_WaveShare75::DisplayFrame(const unsigned char* frame_buffer, uint8_t bitsPerPixel, uint16_t* palette) {
    uint16_t x = 0;
    uint16_t y = 0;
    uint8_t pixelsPerByte = 8 / bitsPerPixel;
    uint8_t data;
    SendCommand(DATA_START_TRANSMISSION_1);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width / pixelsPerByte; j++) {
            for (int b = 0; b < pixelsPerByte / 2; b++) {
                data = 0;
                for (int c = 0; c < 2; c++) {
                    data <<= 4;
                    switch (rotation) {
                        case 0:
                            x = j * pixelsPerByte + b * 2 + c;
                            y = i;
                            break;
                        case 1:
                            x = bufferWidth - i - 1;
                            y = j * pixelsPerByte + b * 2 + c;
                            break;
                        case 2:
                            x = bufferWidth - (j * pixelsPerByte + b * 2 + c) - 1;
                            y = bufferHeight - i - 1;
                            break;
                        case 3:
                            x = i;
                            y = bufferHeight - (j * pixelsPerByte + b * 2 + c) - 1;
                            break;
                    }
                    uint8_t byteValue = getPixel(frame_buffer, x, y, bitsPerPixel);
                    // Compatibility: Black/White mode without color palette.
                    if (bitsPerPixel == 1) {
                        data |= (byteValue == 1 ?  0x03 : 0x00);
                    } else {
                        if (byteValue >= 0 && byteValue <= 2) {
                            data |= (uint8_t) palette[ byteValue ];
                        }
                    }
                }
                SendData(data);
            }
            yield();
        }
    }
    SendCommand(DISPLAY_REFRESH);
    DelayMs(100);
    WaitUntilIdle();
}

/**
 * Gets the pixel value at the given coordinate.
 * This method is used for compatibility with the older implementations
 * which don't support colors. It can only be used if the number of
 * bits per pixel is 1.
 * @param buffer Frame buffer.
 * @param x X position of the pixel.
 * @param x Y position of the pixel.
 * @return The color value of the pixel.
 */
uint8_t EPD_WaveShare75::getPixel(const unsigned char* buffer, uint16_t x, uint16_t y) {
    return getPixel(buffer, x, y, 1);
}

/**
 * Gets the pixel value at the given coordinate.
 * @param buffer Frame buffer.
 * @param x X position of the pixel.
 * @param x Y position of the pixel.
 * @param bitsPerPixel Number of bits used to represent a pixel.
 * @return If bitsPerPixel is 1 the color value of the pixel or if bitsPerPixel is
 *      greater than 1 the index in the color palette.
 */
uint8_t EPD_WaveShare75::getPixel(const unsigned char* buffer, uint16_t x, uint16_t y, uint8_t bitsPerPixel) {
  uint8_t bitMask = (1 << bitsPerPixel) - 1;
  uint8_t pixelsPerByte = 8 / bitsPerPixel;

  if (x >= bufferWidth || y >= bufferHeight) {
    return 0;
  }

  uint32_t pos = (y * bufferWidth + x) / pixelsPerByte;
  uint8_t shift = (x & (pixelsPerByte - 1)) * bitsPerPixel;
  return (buffer[pos] >> shift) & bitMask;
}

uint8_t EPD_WaveShare75::reverse(uint8_t in)
{
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


void EPD_WaveShare75::setFastRefresh(boolean isFastRefreshEnabled) {
  // Not enabled at the moment
}

void EPD_WaveShare75::writeBuffer(BufferInfo *bufferInfo) {
  DisplayFrame(bufferInfo->buffer, bufferInfo->bitsPerPixel, bufferInfo->palette);
}

/**
 *  @brief: After this command is transmitted, the chip would enter the
 *          deep-sleep mode to save power.
 *          The deep sleep mode would return to standby by hardware reset.
 *          The only one parameter is a check code, the command would be
 *          executed if check code = 0xA5.
 *          You can use EPD_Reset() to awaken
 */
void EPD_WaveShare75::Sleep(void) {
    SendCommand(POWER_OFF);
    WaitUntilIdle();
    SendCommand(DEEP_SLEEP);
    SendData(0xa5);
}

int EPD_WaveShare75::IfInit(void) {
    pinMode(this->cs_pin, OUTPUT);
    pinMode(this->reset_pin, OUTPUT);
    pinMode(this->dc_pin, OUTPUT);
    pinMode(this->busy_pin, INPUT);
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    SPI.begin();
    return 0;
}

void EPD_WaveShare75::DelayMs(unsigned int delaytime) {
    delay(delaytime);
}

void EPD_WaveShare75::DigitalWrite(int pin, int value) {
    digitalWrite(pin, value);
}

int EPD_WaveShare75::DigitalRead(int pin) {
    return digitalRead(pin);
}

void EPD_WaveShare75::SpiTransfer(unsigned char data) {
    digitalWrite(this->cs_pin, LOW);
    SPI.transfer(data);
    digitalWrite(this->cs_pin, HIGH);
}

/* END OF FILE */
