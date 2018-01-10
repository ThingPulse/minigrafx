
/**
The MIT License (MIT)
Copyright (c) 2017 by Daniel Eichhorn
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

Please note: I am spending a lot of my free time in developing Software and Hardware
for these projects. Please consider supporting me by
a) Buying my hardware kits from https://blog.squix.org/shop
b) Send a donation: https://www.paypal.me/squix/5USD
c) Or using this affiliate link while shopping: https://www.banggood.com/?p=6R31122484684201508S

See more at https://blog.squix.org

This code is based on a driver from http://waveshare.com
*/

#include <stdlib.h>
#include "EPD_WaveShare_29.h"

EPD_WaveShare29::~EPD_WaveShare29() {
};

EPD_WaveShare29::EPD_WaveShare29(uint8_t csPin, uint8_t rstPin, uint8_t dcPin, uint8_t busyPin) : DisplayDriver(EPD_WIDTH, EPD_HEIGHT) {
    this->reset_pin = rstPin;
    this->dc_pin = dcPin;
    this->cs_pin = csPin;
    this->busy_pin = busyPin;
    width = EPD_WIDTH;
    height = EPD_HEIGHT;
};

void EPD_WaveShare29::setRotation(uint8_t r) {
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
void EPD_WaveShare29::init() {
  Init(lut_full_update);
}

void EPD_WaveShare29::setFastRefresh(boolean isFastRefreshEnabled) {
  if (isFastRefreshEnabled != this->isFastRefreshEnabled) {
    if (isFastRefreshEnabled) {
      Init(lut_partial_update);
    } else {
      Init(lut_full_update);
    }
  }
  this->isFastRefreshEnabled = isFastRefreshEnabled;

}

void EPD_WaveShare29::writeBuffer(uint8_t *buffer, uint8_t bitsPerPixel, uint16_t *palette) {
  //SetFrameMemory(buffer, 0, 0, EPD_WIDTH, EPD_HEIGHT);
  uint16_t x = 0;
  uint16_t y = 0;
  int x_end;
  int y_end;
  uint16_t image_width = width;
  uint16_t image_height = height;
  uint16_t bufferSize = width * height / 8;
  uint16_t xDot = bufferWidth;
  uint16_t yDot = bufferHeight;
  uint8_t data;
  if (
      buffer == NULL ||
      x < 0 || image_width < 0 ||
      y < 0 || image_height < 0
  ) {
      return;
  }
  /* x point must be the multiple of 8 or the last 3 bits will be ignored */
  x &= 0xF8;
  image_width &= 0xF8;
  if (image_width >= this->bufferWidth) {
      x_end = this->bufferWidth - 1;
  } else {
      x_end = image_width - 1;
  }
  if (image_height >= this->bufferHeight) {
      y_end = this->bufferHeight - 1;
  } else {
      y_end = image_height - 1;
  }
  SetMemoryArea(0, 0, EPD_WIDTH - 1, EPD_HEIGHT - 1);
  SetMemoryPointer(0, 0);
  SendCommand(WRITE_RAM);
  /* send the image data */
  for (int i = 0; i < EPD_HEIGHT; i++) {
      for (int j = 0; j < (EPD_WIDTH) / 8; j++) {

          data = 0;
          for (int b = 0; b < 8; b++) {
            data = data << 1;
            switch (rotation) {
              case 0:
                x = (j * 8 + b);
                y = i;
                break;
              case 1:
                x = bufferWidth - i;
                y = (j * 8 + b);
                break;
              case 2:
                x = xDot - (j * 8 + b);
                y = yDot - i;
                break;
              case 3:
                x = i;
                y = bufferHeight - (j * 8 + b);
                break;
            }
            data = data | (getPixel(buffer, x, y) & 1);

          }
          SendData(data);
          //SendData(reverse(buffer[(i + j * (image_width / 8))]));
          //SendData(i);
          yield();
      }
  }

  DisplayFrame();
}

int EPD_WaveShare29::IfInit(void) {
    digitalWrite(this->cs_pin, OUTPUT);
    pinMode(this->reset_pin, OUTPUT);
    pinMode(this->dc_pin, OUTPUT);
    pinMode(this->busy_pin, INPUT);
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
    SPI.begin();
    return 0;
}


void EPD_WaveShare29::DigitalWrite(int pin, int value) {
    digitalWrite(pin, value);
}

int EPD_WaveShare29::DigitalRead(int pin) {
    return digitalRead(pin);
}

void EPD_WaveShare29::DelayMs(unsigned int delaytime) {
    delay(delaytime);
}

void EPD_WaveShare29::SpiTransfer(unsigned char data) {
    digitalWrite(this->cs_pin, LOW);
    SPI.transfer(data);
    digitalWrite(this->cs_pin, HIGH);
}

int EPD_WaveShare29::Init(const unsigned char* lut) {
    /* this calls the peripheral hardware interface, see epdif */
    if (IfInit() != 0) {
        return -1;
    }
    /* EPD hardware init start */
    this->lut = lut;
    Reset();
    SendCommand(DRIVER_OUTPUT_CONTROL);
    SendData((EPD_HEIGHT - 1) & 0xFF);
    SendData(((EPD_HEIGHT - 1) >> 8) & 0xFF);
    SendData(0x00);                     // GD = 0; SM = 0; TB = 0;
    SendCommand(BOOSTER_SOFT_START_CONTROL);
    SendData(0xD7);
    SendData(0xD6);
    SendData(0x9D);
    SendCommand(WRITE_VCOM_REGISTER);
    SendData(0xA8);                     // VCOM 7C
    SendCommand(SET_DUMMY_LINE_PERIOD);
    SendData(0x1A);                     // 4 dummy lines per gate
    SendCommand(SET_GATE_TIME);
    SendData(0x08);                     // 2us per line
    SendCommand(DATA_ENTRY_MODE_SETTING);
    SendData(0x03);                     // X increment; Y increment
    SetLut(this->lut);
    /* EPD hardware init end */
    return 0;
}

/**
 *  @brief: basic function for sending commands
 */
void EPD_WaveShare29::SendCommand(unsigned char command) {
    DigitalWrite(dc_pin, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void EPD_WaveShare29::SendData(unsigned char data) {
    DigitalWrite(dc_pin, HIGH);
    SpiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes LOW
 */
void EPD_WaveShare29::WaitUntilIdle(void) {
    while(DigitalRead(busy_pin) == HIGH) {      //LOW: idle, HIGH: busy
        DelayMs(100);
    }
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void EPD_WaveShare29::Reset(void) {
    DigitalWrite(reset_pin, LOW);                //module reset
    DelayMs(200);
    DigitalWrite(reset_pin, HIGH);
    DelayMs(200);
}

/**
 *  @brief: set the look-up table register
 */
void EPD_WaveShare29::SetLut(const unsigned char* lut) {
    this->lut = lut;
    SendCommand(WRITE_LUT_REGISTER);
    /* the length of look-up table is 30 bytes */
    for (int i = 0; i < 30; i++) {
        SendData(this->lut[i]);
    }
}

/**
 *  @brief: put an image buffer to the frame memory.
 *          this won't update the display.
 */
void EPD_WaveShare29::SetFrameMemory(
    const unsigned char* image_buffer,
    int x,
    int y,
    int image_width,
    int image_height
) {
    int x_end;
    int y_end;

    if (
        image_buffer == NULL ||
        x < 0 || image_width < 0 ||
        y < 0 || image_height < 0
    ) {
        return;
    }
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    x &= 0xF8;
    image_width &= 0xF8;
    if (x + image_width >= this->bufferWidth) {
        x_end = this->bufferWidth - 1;
    } else {
        x_end = x + image_width - 1;
    }
    if (y + image_height >= this->bufferHeight) {
        y_end = this->bufferHeight - 1;
    } else {
        y_end = y + image_height - 1;
    }
    SetMemoryArea(x, y, x_end, y_end);
    SetMemoryPointer(x, y);
    SendCommand(WRITE_RAM);
    /* send the image data */
    for (int j = 0; j < y_end - y + 1; j++) {
        for (int i = 0; i < (x_end - x + 1) / 8; i++) {
            SendData(reverse(image_buffer[i + j * (image_width / 8)]));
        }
    }

}

uint8_t EPD_WaveShare29::getPixel(uint8_t *buffer, uint16_t x, uint16_t y) {
  uint8_t bitsPerPixel = 1;
  uint8_t bitMask = (1 << bitsPerPixel) - 1;
  uint8_t pixelsPerByte = 8 / bitsPerPixel;
  uint8_t bitShift = 3;

  if (x >= bufferWidth || y >= bufferHeight) return 0;
  // bitsPerPixel: 8, pixPerByte: 1, 0  1 = 2^0
  // bitsPerPixel: 4, pixPerByte: 2, 1  2 = 2^1
  // bitsPerPixel  2, pixPerByte: 4, 2  4 = 2^2
  // bitsPerPixel  1, pixPerByte: 8, 3  8 = 2^3
  uint16_t pos = (y * bufferWidth + x) >> bitShift;

  uint8_t shift = (x & (pixelsPerByte - 1)) * bitsPerPixel;

  return (buffer[pos] >> shift) & bitMask;
}

/**
 *  @brief: put an image buffer to the frame memory.
 *          this won't update the display.
 *
 *          Question: When do you use this function instead of
 *          void SetFrameMemory(
 *              const unsigned char* image_buffer,
 *              int x,
 *              int y,
 *              int image_width,
 *              int image_height
 *          );
 *          Answer: SetFrameMemory with parameters only reads image data
 *          from the RAM but not from the flash in AVR chips (for AVR chips,
 *          you have to use the function pgm_read_byte to read buffers
 *          from the flash).
 */
void EPD_WaveShare29::SetFrameMemory(const unsigned char* image_buffer) {
    SetMemoryArea(0, 0, this->width - 1, this->height - 1);
    SetMemoryPointer(0, 0);
    SendCommand(WRITE_RAM);
    /* send the image data */
    for (int i = 0; i < this->width / 8 * this->height; i++) {
        SendData(pgm_read_byte(&image_buffer[i]));
    }
}

/**
 *  @brief: clear the frame memory with the specified color.
 *          this won't update the display.
 */
void EPD_WaveShare29::ClearFrameMemory(unsigned char color) {
    SetMemoryArea(0, 0, this->width - 1, this->height - 1);
    SetMemoryPointer(0, 0);
    SendCommand(WRITE_RAM);
    /* send the color data */
    for (int i = 0; i < this->width / 8 * this->height; i++) {
        SendData(color);
    }
}

/**
 *  @brief: update the display
 *          there are 2 memory areas embedded in the e-paper display
 *          but once this function is called,
 *          the the next action of SetFrameMemory or ClearFrame will
 *          set the other memory area.
 */
void EPD_WaveShare29::DisplayFrame(void) {
    SendCommand(DISPLAY_UPDATE_CONTROL_2);
    SendData(0xC4);
    SendCommand(MASTER_ACTIVATION);
    SendCommand(TERMINATE_FRAME_READ_WRITE);
    WaitUntilIdle();
}

/**
 *  @brief: private function to specify the memory area for data R/W
 */
void EPD_WaveShare29::SetMemoryArea(int x_start, int y_start, int x_end, int y_end) {
    SendCommand(SET_RAM_X_ADDRESS_START_END_POSITION);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    SendData((x_start >> 3) & 0xFF);
    SendData((x_end >> 3) & 0xFF);
    SendCommand(SET_RAM_Y_ADDRESS_START_END_POSITION);
    SendData(y_start & 0xFF);
    SendData((y_start >> 8) & 0xFF);
    SendData(y_end & 0xFF);
    SendData((y_end >> 8) & 0xFF);
}

/**
 *  @brief: private function to specify the start point for data R/W
 */
void EPD_WaveShare29::SetMemoryPointer(int x, int y) {
    SendCommand(SET_RAM_X_ADDRESS_COUNTER);
    /* x point must be the multiple of 8 or the last 3 bits will be ignored */
    SendData((x >> 3) & 0xFF);
    SendCommand(SET_RAM_Y_ADDRESS_COUNTER);
    SendData(y & 0xFF);
    SendData((y >> 8) & 0xFF);
    WaitUntilIdle();
}

uint8_t EPD_WaveShare29::reverse(uint8_t in)
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

/**
 *  @brief: After this command is transmitted, the chip would enter the
 *          deep-sleep mode to save power.
 *          The deep sleep mode would return to standby by hardware reset.
 *          You can use Epd::Init() to awaken
 */
void EPD_WaveShare29::Sleep() {
    digitalWrite(this->cs_pin, LOW);
    digitalWrite(this->reset_pin, LOW);
    digitalWrite(this->dc_pin, LOW);
    SendCommand(DEEP_SLEEP_MODE);
    //WaitUntilIdle();
}



/* END OF FILE */
