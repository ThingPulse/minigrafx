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

#include "EPD_WaveShare_43.h"


EPD_WaveShare_43::EPD_WaveShare_43(Stream *serial)  : DisplayDriver(800, 600) {
  this->serial = serial;
}

void EPD_WaveShare_43::init(void) {
  //stream->begin(115200);
  //pinMode(wake_up, HIGH);
  //pinMode(reset, HIGH);
  setMemory(MEM_NAND);
}

void EPD_WaveShare_43::setFastRefresh(boolean isFastRefreshEnabled) {
  // Not enabled at the moment
}

void EPD_WaveShare_43::setRotation(uint8_t r) {

}

void EPD_WaveShare_43::writeBuffer(BufferInfo *bufferInfo) {
  uint8_t color = 0;
  Serial.println("Clearing");
  clear();
  Serial.println("Writing");
  for (int y = 0; y < 400; y++) {
    for (int x = 0; x < 400; x++) {
      color = getPixel(bufferInfo->buffer, x, y);
      if (color == 0) {
        drawPixel(x,y);
      }
    }
    if (y % 10 == 0) {
      yield();
    }
  }
  update();
}

void EPD_WaveShare_43::putchars(const unsigned char * ptr, int n)
{
	int i, x;

	for(i = 0; i < n; i++)
	{
	    x = ptr[i];
		serial->write(x);
	}
}

unsigned char EPD_WaveShare_43::verify(const void * ptr, int n)
{
	int i;
	unsigned char * p = (unsigned char *)ptr;
	unsigned char result;

	for(i = 0, result = 0; i < n; i++)
	{
		result ^= p[i];
	}

	return result;
}

void EPD_WaveShare_43::setColor(unsigned char color, unsigned char bkcolor)
{
	_cmd_buff[0] = FRAME_B;

	_cmd_buff[1] = 0x00;
	_cmd_buff[2] = 0x0B;

	_cmd_buff[3] = CMD_SET_COLOR;

	_cmd_buff[4] = color;
	_cmd_buff[5] = bkcolor;

	_cmd_buff[6] = FRAME_E0;
	_cmd_buff[7] = FRAME_E1;
	_cmd_buff[8] = FRAME_E2;
	_cmd_buff[9] = FRAME_E3;
	_cmd_buff[10] = verify(_cmd_buff, 10);

	putchars(_cmd_buff, 11);
}

void EPD_WaveShare_43::drawPixel(int x0, int y0)
{
	_cmd_buff[0] = FRAME_B;

	_cmd_buff[1] = 0x00;
	_cmd_buff[2] = 0x0D;

	_cmd_buff[3] = CMD_DRAW_PIXEL;

	_cmd_buff[4] = (x0 >> 8) & 0xFF;
	_cmd_buff[5] = x0 & 0xFF;
	_cmd_buff[6] = (y0 >> 8) & 0xFF;
	_cmd_buff[7] = y0 & 0xFF;

	_cmd_buff[8] = FRAME_E0;
	_cmd_buff[9] = FRAME_E1;
	_cmd_buff[10] = FRAME_E2;
	_cmd_buff[11] = FRAME_E3;
	_cmd_buff[12] = verify(_cmd_buff, 12);

	putchars(_cmd_buff, 13);
}

void EPD_WaveShare_43::setMemory(unsigned char mode)
{
	_cmd_buff[0] = FRAME_B;

	_cmd_buff[1] = 0x00;
	_cmd_buff[2] = 0x0A;

	_cmd_buff[3] = CMD_MEMORYMODE;

	_cmd_buff[4] = mode;

	_cmd_buff[5] = FRAME_E0;
	_cmd_buff[6] = FRAME_E1;
	_cmd_buff[7] = FRAME_E2;
	_cmd_buff[8] = FRAME_E3;
	_cmd_buff[9] = verify(_cmd_buff, 9);

	putchars(_cmd_buff, 10);
}

void EPD_WaveShare_43::clear(void)
{
	_cmd_buff[0] = FRAME_B;

	_cmd_buff[1] = 0x00;
	_cmd_buff[2] = 0x09;

	_cmd_buff[3] = CMD_CLEAR;

	_cmd_buff[4] = FRAME_E0;
	_cmd_buff[5] = FRAME_E1;
	_cmd_buff[6] = FRAME_E2;
	_cmd_buff[7] = FRAME_E3;
	_cmd_buff[8] = verify(_cmd_buff, 8);

	putchars(_cmd_buff, 9);
}

void EPD_WaveShare_43::update(void)
{
	memcpy(_cmd_buff, _cmd_update, 8);
	_cmd_buff[8] = verify(_cmd_buff, 8);

	putchars(_cmd_buff, 9);
}

uint16_t EPD_WaveShare_43::getPixel(uint8_t *buffer, uint16_t x, uint16_t y) {
  if (x >= width() || y >= height() || x < 0 || y < 0) return 0;
  uint8_t bitShift = 3;
  uint8_t bitsPerPixel = 1;
  uint8_t bitMask = 1;
  uint8_t pixelsPerByte = 8;
  uint32_t bufferSize = width() * height() / pixelsPerByte;
  // bitsPerPixel: 8, pixPerByte: 1, 0  1 = 2^0
  // bitsPerPixel: 4, pixPerByte: 2, 1  2 = 2^1
  // bitsPerPixel  2, pixPerByte: 4, 2  4 = 2^2
  // bitsPerPixel  1, pixPerByte: 8, 3  8 = 2^3
  uint16_t pos = (y * width() + x) >> bitShift;
  if (pos > bufferSize) {
    return 0;
  }

  uint8_t shift = (x & (pixelsPerByte - 1)) * bitsPerPixel;

  return (buffer[pos] >> shift) & bitMask;
}

void EPD_WaveShare_43::setBaud(long baud)
{
	_cmd_buff[0] = FRAME_B;

	_cmd_buff[1] = 0x00;
	_cmd_buff[2] = 0x0D;

	_cmd_buff[3] = CMD_SET_BAUD;

	_cmd_buff[4] = (baud >> 24) & 0xFF;
	_cmd_buff[5] = (baud >> 16) & 0xFF;
	_cmd_buff[6] = (baud >> 8) & 0xFF;
	_cmd_buff[7] = baud & 0xFF;

	_cmd_buff[8] = FRAME_E0;
	_cmd_buff[9] = FRAME_E1;
	_cmd_buff[10] = FRAME_E2;
	_cmd_buff[11] = FRAME_E3;
	_cmd_buff[12] = verify(_cmd_buff, 12);

	putchars(_cmd_buff, 13);

	delay(10);

}
