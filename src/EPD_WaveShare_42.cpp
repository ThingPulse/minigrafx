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

#include "EPD_WaveShare_42.h"

EPD_WaveShare42::EPD_WaveShare42(uint8_t csPin, uint8_t rstPin, uint8_t dcPin, uint8_t busyPin) : DisplayDriver(400, 300)  {
  this->csPin = csPin;
  this->rstPin = rstPin;
  this->dcPin = dcPin;
  this->busyPin = busyPin;
  width = 400;
  height = 300;

}

int EPD_WaveShare42::getWidth() {
  return 400;
}

int EPD_WaveShare42::getHeight() {
  return 300;
}

void EPD_WaveShare42::setRotation(uint8_t r) {
  // Currently not supported
}
void EPD_WaveShare42::init() {
  /* this calls the peripheral hardware interface, see epdif */
  if (IfInit() != 0) {
      return;
  }
  /* EPD hardware init start */
  Reset();
  SendCommand(POWER_SETTING);
  SendData(0x03);                  // VDS_EN, VDG_EN
  SendData(0x00);                  // VCOM_HV, VGHL_LV[1], VGHL_LV[0]
  SendData(0x2b);                  // VDH
  SendData(0x2b);                  // VDL
  SendData(0xff);                  // VDHR
  SendCommand(BOOSTER_SOFT_START);
  SendData(0x17);
  SendData(0x17);
  SendData(0x17);                  //07 0f 17 1f 27 2F 37 2f
  SendCommand(POWER_ON);
  WaitUntilIdle();
  SendCommand(PANEL_SETTING);
  SendData(0xbf);    // KW-BF   KWR-AF  BWROTP 0f
  SendData(0x0b);
  SendCommand(PLL_CONTROL);
  SendData(0x3c);        // 3A 100HZ   29 150Hz 39 200HZ  31 171HZ
  /* EPD hardware init end */
  return;

}

void EPD_WaveShare42::DigitalWrite(int pin, int value) {
    digitalWrite(pin, value);
}

int EPD_WaveShare42::DigitalRead(int pin) {
    return digitalRead(pin);
}

void EPD_WaveShare42::DelayMs(unsigned int delaytime) {
    delay(delaytime);
}

void EPD_WaveShare42::SpiTransfer(unsigned char data) {
    digitalWrite(this->csPin, LOW);
    SPI.transfer(data);
    digitalWrite(this->csPin, HIGH);
}

int EPD_WaveShare42::IfInit(void) {
    pinMode(this->csPin, OUTPUT);
    pinMode(this->rstPin, OUTPUT);
    pinMode(this->dcPin, OUTPUT);
    pinMode(this->busyPin, INPUT);
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    SPI.begin();
    return 0;
}


void EPD_WaveShare42::writeBuffer(uint8_t *buffer, uint8_t bitsPerPixel, uint16_t *palette) {
  DisplayFrame(buffer);
}

/**
 *  @brief: basic function for sending commands
 */
void EPD_WaveShare42::SendCommand(unsigned char command) {
    DigitalWrite(this->dcPin, LOW);
    SpiTransfer(command);
}

/**
 *  @brief: basic function for sending data
 */
void EPD_WaveShare42::SendData(unsigned char data) {
    DigitalWrite(this->dcPin, HIGH);
    SpiTransfer(data);
}

/**
 *  @brief: Wait until the busy_pin goes HIGH
 */
void EPD_WaveShare42::WaitUntilIdle(void) {
    while(DigitalRead(this->busyPin) == 0) {      //0: busy, 1: idle
        DelayMs(100);
    }
}

/**
 *  @brief: module reset.
 *          often used to awaken the module in deep sleep,
 *          see Epd::Sleep();
 */
void EPD_WaveShare42::Reset(void) {
    DigitalWrite(this->rstPin, LOW);
    DelayMs(200);
    DigitalWrite(this->rstPin, HIGH);
    DelayMs(200);
}

/**
 *  @brief: transmit partial data to the SRAM
 */
void EPD_WaveShare42::SetPartialWindow(const unsigned char* buffer_black, int x, int y, int w, int l) {
    SendCommand(PARTIAL_IN);
    SendCommand(PARTIAL_WINDOW);
    SendData(x >> 8);
    SendData(x & 0xf8);     // x should be the multiple of 8, the last 3 bit will always be ignored
    SendData(((x & 0xf8) + w  - 1) >> 8);
    SendData(((x & 0xf8) + w  - 1) | 0x07);
    SendData(y >> 8);
    SendData(y & 0xff);
    SendData((y + l - 1) >> 8);
    SendData((y + l - 1) & 0xff);
    SendData(0x01);         // Gates scan both inside and outside of the partial window. (default)
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    if (buffer_black != NULL) {
        for(int i = 0; i < w  / 8 * l; i++) {
            SendData(reverse(buffer_black[i]));
        }
    } else {
        for(int i = 0; i < w  / 8 * l; i++) {
            SendData(0x00);
        }
    }
    DelayMs(2);
    SendCommand(PARTIAL_OUT);
}

/**
 *  @brief: set the look-up table
 */
void EPD_WaveShare42::SetLut(void) {
    unsigned int count;
    SendCommand(LUT_FOR_VCOM);                            //vcom
    for(count = 0; count < 44; count++) {
        SendData(lut_vcom0[count]);
    }

    SendCommand(LUT_WHITE_TO_WHITE);                      //ww --
    for(count = 0; count < 42; count++) {
        SendData(lut_ww[count]);
    }

    SendCommand(LUT_BLACK_TO_WHITE);                      //bw r
    for(count = 0; count < 42; count++) {
        SendData(lut_bw[count]);
    }

    SendCommand(LUT_WHITE_TO_BLACK);                      //wb w
    for(count = 0; count < 42; count++) {
        SendData(lut_bb[count]);
    }

    SendCommand(LUT_BLACK_TO_BLACK);                      //bb b
    for(count = 0; count < 42; count++) {
        SendData(lut_wb[count]);
    }
}

/**
 * @brief: refresh and displays the frame
 */
void EPD_WaveShare42::DisplayFrame(const unsigned char* frame_buffer) {
    SendCommand(RESOLUTION_SETTING);
    SendData(width >> 8);
    SendData(width & 0xff);
    SendData(height >> 8);
    SendData(height & 0xff);

    SendCommand(VCM_DC_SETTING);
    SendData(0x12);

    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendCommand(0x97);    //VBDF 17|D7 VBDW 97  VBDB 57  VBDF F7  VBDW 77  VBDB 37  VBDR B7

    if (frame_buffer != NULL) {
        SendCommand(DATA_START_TRANSMISSION_1);
        for(int i = 0; i < width / 8 * height; i++) {
            SendData(0xFF);      // bit set: white, bit reset: black
        }
        DelayMs(2);
        SendCommand(DATA_START_TRANSMISSION_2);
        for(int i = 0; i < width / 8 * height; i++) {
            SendData(reverse(frame_buffer[i]));
        }
        DelayMs(2);
    }

    SetLut();

    SendCommand(DISPLAY_REFRESH);
    DelayMs(100);
    WaitUntilIdle();
}

uint8_t EPD_WaveShare42::reverse(uint8_t in)
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
 * @brief: clear the frame data from the SRAM, this won't refresh the display
 */
void EPD_WaveShare42::ClearFrame(void) {
    SendCommand(RESOLUTION_SETTING);
    SendData(width >> 8);
    SendData(width & 0xff);
    SendData(height >> 8);
    SendData(height & 0xff);

    SendCommand(DATA_START_TRANSMISSION_1);
    DelayMs(2);
    for(int i = 0; i < width / 8 * height; i++) {
        SendData(0xFF);
    }
    DelayMs(2);
    SendCommand(DATA_START_TRANSMISSION_2);
    DelayMs(2);
    for(int i = 0; i < width / 8 * height; i++) {
        SendData(0xFF);
    }
    DelayMs(2);
}

/**
 * @brief: This displays the frame data from SRAM
 */
void EPD_WaveShare42::DisplayFrame(void) {
    SetLut();
    SendCommand(DISPLAY_REFRESH);
    DelayMs(100);
    WaitUntilIdle();
}

/**
 * @brief: After this command is transmitted, the chip would enter the deep-sleep mode to save power.
 *         The deep sleep mode would return to standby by hardware reset. The only one parameter is a
 *         check code, the command would be executed if check code = 0xA5.
 *         You can use Epd::Reset() to awaken and use Epd::Init() to initialize.
 */
void EPD_WaveShare42::Sleep() {
    SendCommand(VCOM_AND_DATA_INTERVAL_SETTING);
    SendData(0x17);                       //border floating
    SendCommand(VCM_DC_SETTING);          //VCOM to 0V
    SendCommand(PANEL_SETTING);
    DelayMs(100);

    SendCommand(POWER_SETTING);           //VG&VS to 0V fast
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    SendData(0x00);
    DelayMs(100);

    SendCommand(POWER_OFF);          //power off
    WaitUntilIdle();
    SendCommand(DEEP_SLEEP);         //deep sleep
    SendData(0xA5);
}
