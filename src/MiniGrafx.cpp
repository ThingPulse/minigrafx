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

See more at https://thigpulse.com

Many thanks go to various contributors such as Adafruit, Waveshare.
*/
#include "MiniGrafx.h"

MiniGrafx::MiniGrafx(DisplayDriver *driver, uint8_t bitsPerPixel, uint16_t *palette) {
  this->driver = driver;
  this->width = driver->width();
  this->initialWidth = driver->width();
  this->height = driver->height();
  this->initialHeight = driver->height();
  this->palette = palette;
  this->bitsPerPixel = bitsPerPixel;
  //initializeBuffer();

}

MiniGrafx::MiniGrafx(DisplayDriver *driver, uint8_t bitsPerPixel, uint16_t *palette, uint16_t width, uint16_t height) {
  this->driver = driver;
  this->width = width;
  this->initialWidth = width;
  this->height = height;
  this->initialHeight = height;
  this->palette = palette;
  this->bitsPerPixel = bitsPerPixel;
}


void MiniGrafx::init() {
  this->initializeBuffer();
  this->driver->init();
}

void MiniGrafx::initializeBuffer() {
  this->bitMask = (1 << bitsPerPixel) - 1;
  this->pixelsPerByte = 8 / bitsPerPixel;
  // bitsPerPixel: 8, pixPerByte: 1, 0  1 = 2^0
  // bitsPerPixel: 4, pixPerByte: 2, 1  2 = 2^1
  // bitsPerPixel  2, pixPerByte: 4, 2  4 = 2^2
  // bitsPerPixel  1, pixPerByte: 8, 3  8 = 2^3
  // TODO: I was too stupid or too lazy to get the formula for this
  switch(bitsPerPixel) {
    case 1:
      this->bitShift = 3;
      break;
    case 2:
      this->bitShift = 2;
      break;
    case 4:
      this->bitShift = 1;
      break;
    case 8:
      this->bitShift = 0;
      break;
    case 16:
      this->bitShift = 0;
      this->bitMask = 0xFFFF;
      this->bufferSize = this->width * this->height * 2;
      this->buffer = (uint8_t*) malloc(sizeof(uint8_t) * bufferSize);
      return;
      break;
  }



  this->bufferSize = this->width * this->height / (pixelsPerByte);
  this->buffer = (uint8_t*) malloc(sizeof(uint8_t) * bufferSize);
  if(!this->buffer) {
    Serial.println("[DEBUG_MINI_GRAFX][init] Not enough memory to create display\n");
  }
}

void MiniGrafx::freeBuffer() {
  free(this->buffer);
}

void MiniGrafx::changeBitDepth(uint8_t bitsPerPixel, uint16_t *palette) {
  free(this->buffer);
  initializeBuffer();
}

uint16_t MiniGrafx::getHeight() {
  return this->height;
}
uint16_t MiniGrafx::getWidth() {
  return this->width;
}

void MiniGrafx::setRotation(uint8_t m) {
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
   case 2:
     this->width  = this->initialWidth;
     this->height = this->initialHeight;
     break;
   case 1:
   case 3:
     this->width  = this->initialHeight;
     this->height = this->initialWidth;
     break;
  }
  this->driver->setRotation(m);
}

void MiniGrafx::setColor(uint16_t color) {

  this->color = color & this->bitMask;
}

void MiniGrafx::setTransparentColor(uint16_t transparentColor) {
  this->transparentColor = transparentColor;
}

void MiniGrafx::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep) {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1) {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx >> 2;
  int16_t ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (; x0<=x1; x0++) {
    if (steep) {
      setPixel(y0, x0);
    } else {
      setPixel(x0, y0);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void MiniGrafx::drawCircle(int16_t x0, int16_t y0, uint16_t radius) {
    int16_t x = 0, y = radius;
  int16_t dp = 1 - radius;
  do {
    if (dp < 0)
      dp = dp + 2 * (++x) + 3;
    else
      dp = dp + 2 * (++x) - 2 * (--y) + 5;

    setPixel(x0 + x, y0 + y);     //For the 8 octants
    setPixel(x0 - x, y0 + y);
    setPixel(x0 + x, y0 - y);
    setPixel(x0 - x, y0 - y);
    setPixel(x0 + y, y0 + x);
    setPixel(x0 - y, y0 + x);
    setPixel(x0 + y, y0 - x);
    setPixel(x0 - y, y0 - x);

  } while (x < y);

  setPixel(x0 + radius, y0);
  setPixel(x0, y0 + radius);
  setPixel(x0 - radius, y0);
  setPixel(x0, y0 - radius);
}

void MiniGrafx::drawRect(int16_t x, int16_t y, int16_t width, int16_t height) {
  drawHorizontalLine(x, y, width);
  drawVerticalLine(x, y, height);
  drawVerticalLine(x + width, y, height);
  drawHorizontalLine(x, y + height, width);
}

void MiniGrafx::fillRect(int16_t xMove, int16_t yMove, int16_t width, int16_t height) {
  for (int16_t x = xMove; x < xMove + width; x++) {
    drawVerticalLine(x, yMove, height);
  }
}

void MiniGrafx::fillCircle(int16_t x0, int16_t y0, int16_t radius) {
  int16_t x = 0, y = radius;
  int16_t dp = 1 - radius;
  do {
    if (dp < 0)
      dp = dp + 2 * (++x) + 3;
    else
      dp = dp + 2 * (++x) - 2 * (--y) + 5;

    drawHorizontalLine(x0 - x, y0 - y, 2*x);
    drawHorizontalLine(x0 - x, y0 + y, 2*x);
    drawHorizontalLine(x0 - y, y0 - x, 2*y);
    drawHorizontalLine(x0 - y, y0 + x, 2*y);


  } while (x < y);
  drawHorizontalLine(x0 - radius, y0, 2 * radius);

}

void MiniGrafx::drawHorizontalLine(int16_t x, int16_t y, int16_t length) {
  int16_t x1 = x;
  int16_t x2 = x + length - 1;
  if (x1 > x2) {
    _swap_int16_t(x1, x2);
  }
  for (int i = x1; i <= x2; i++) {
    setPixel(i, y);
  }


}

void MiniGrafx::drawVerticalLine(int16_t x, int16_t y, int16_t length) {
  for (int16_t i = 0; i < length; i++) {
    setPixel(x, y + i);
  }

}

uint16_t MiniGrafx::drawString(int16_t xMove, int16_t yMove, String strUser) {
  uint16_t lineHeight = readFontData(fontData, HEIGHT_POS);

  // char* text must be freed!
  char* text = utf8ascii(strUser);

  uint16_t yOffset = 0;
  // If the string should be centered vertically too
  // we need to now how heigh the string is.
  if (textAlignment == TEXT_ALIGN_CENTER_BOTH) {
    uint16_t lb = 0;
    // Find number of linebreaks in text
    for (uint16_t i=0;text[i] != 0; i++) {
      lb += (text[i] == 10);
    }
    // Calculate center
    yOffset = (lb * lineHeight) >> 2;
  }

  uint16_t line = 0;
  char* textPart = strtok(text,"\n");
  uint16_t maxStringWidth = 0;
  uint16_t stringWidth = 0;
  while (textPart != NULL) {
    uint16_t length = strlen(textPart);
    stringWidth = getStringWidth(textPart, length);
    maxStringWidth = max(maxStringWidth, stringWidth);
    drawStringInternal(xMove, yMove - yOffset + (line++) * lineHeight, textPart, length, stringWidth);
    textPart = strtok(NULL, "\n");
  }
  free(text);
  return stringWidth;
}

void MiniGrafx::drawStringMaxWidth(int16_t xMove, int16_t yMove, uint16_t maxLineWidth, String strUser) {
  uint16_t firstChar  = readFontData(fontData, FIRST_CHAR_POS);
  uint16_t lineHeight = readFontData(fontData, HEIGHT_POS);

  char* text = utf8ascii(strUser);

  uint16_t length = strlen(text);
  uint16_t lastDrawnPos = 0;
  uint16_t lineNumber = 0;
  uint16_t strWidth = 0;

  uint16_t preferredBreakpoint = 0;
  uint16_t widthAtBreakpoint = 0;

  for (uint16_t i = 0; i < length; i++) {
    strWidth += readFontData(fontData, JUMPTABLE_START + (text[i] - firstChar) * JUMPTABLE_BYTES + JUMPTABLE_WIDTH);

    // Always try to break on a space or dash
    if (text[i] == ' ' || text[i]== '-' || text[i] == '\n') {
      preferredBreakpoint = i;
      widthAtBreakpoint = strWidth;
    }

    if (strWidth >= maxLineWidth || text[i] == '\n') {
      if (preferredBreakpoint == 0) {
        preferredBreakpoint = i;
        widthAtBreakpoint = strWidth;
      }
      drawStringInternal(xMove, yMove + (lineNumber++) * lineHeight , &text[lastDrawnPos], preferredBreakpoint - lastDrawnPos, widthAtBreakpoint);
      lastDrawnPos = preferredBreakpoint + 1;
      // It is possible that we did not draw all letters to i so we need
      // to account for the width of the chars from `i - preferredBreakpoint`
      // by calculating the width we did not draw yet.
      strWidth = strWidth - widthAtBreakpoint;
      preferredBreakpoint = 0;
    }
  }

  // Draw last part if needed
  if (lastDrawnPos < length) {
    drawStringInternal(xMove, yMove + lineNumber * lineHeight , &text[lastDrawnPos], length - lastDrawnPos, getStringWidth(&text[lastDrawnPos], length - lastDrawnPos));
  }

  free(text);
}

void MiniGrafx::drawStringInternal(int16_t xMove, int16_t yMove, char* text, uint16_t textLength, uint16_t textWidth) {
  uint8_t textHeight       = readFontData(fontData, HEIGHT_POS);
  uint8_t firstChar        = readFontData(fontData, FIRST_CHAR_POS);
  uint16_t sizeOfJumpTable = readFontData(fontData, CHAR_NUM_POS)  * JUMPTABLE_BYTES;

  uint16_t cursorX         = 0;
  uint16_t cursorY         = 0;

  switch (textAlignment) {
    case TEXT_ALIGN_LEFT:
      break;
    case TEXT_ALIGN_CENTER_BOTH:
      yMove -= textHeight >> 1;
    // Fallthrough
    case TEXT_ALIGN_CENTER:
      xMove -= textWidth >> 1; // divide by 2
      break;
    case TEXT_ALIGN_RIGHT:
      xMove -= textWidth;
      break;
  }

  // Don't draw anything if it is not on the screen.
  if (xMove + textWidth  < 0 || xMove > this->width ) {return;}
  if (yMove + textHeight < 0 || yMove > this->height) {return;}

  for (uint16_t j = 0; j < textLength; j++) {
    int16_t xPos = xMove + cursorX;
    int16_t yPos = yMove + cursorY;

    byte code = text[j];
    if (code >= firstChar) {
      byte charCode = code - firstChar;
      // 4 Bytes per char code
      byte msbJumpToChar    = readFontData( fontData, JUMPTABLE_START + charCode * JUMPTABLE_BYTES );                  // MSB  \ JumpAddress
      byte lsbJumpToChar    = readFontData( fontData, JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_LSB);   // LSB /
      byte charByteSize     = readFontData( fontData, JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_SIZE);  // Size
      byte currentCharWidth = readFontData( fontData, JUMPTABLE_START + charCode * JUMPTABLE_BYTES + JUMPTABLE_WIDTH); // Width

      // Test if the char is drawable
      if (!(msbJumpToChar == 255 && lsbJumpToChar == 255)) {
        // Get the position of the char data
        uint16_t charDataPosition = JUMPTABLE_START + sizeOfJumpTable + ((msbJumpToChar << 8) + lsbJumpToChar);
        drawInternal(xPos, yPos, currentCharWidth, textHeight, fontData, charDataPosition, charByteSize);
      }

      cursorX += currentCharWidth;
    }
  }
}

uint8_t MiniGrafx::readFontData(const char * start, uint32_t offset) {
  if (this->isPgmFont) {
    return pgm_read_byte(start + offset);
  } else {
    fontFile.seek(offset, SeekSet);
    return fontFile.read();
  }
}

void MiniGrafx::setFont(const char *fontData) {
  this->isPgmFont = true;
  fontFile.close();
  this->fontData = fontData;
}

void MiniGrafx::setFontFile(String fileName) {
  this->isPgmFont = false;
  fontFile.close();
  fontFile = SPIFFS.open(fileName, "r");
  if (!fontFile) {
    Serial.println("Could not open font file " + fileName);
  } else {
    Serial.println("Set font file " + fileName);
  }
}


// Code form http://playground.arduino.cc/Main/Utf8ascii
uint8_t MiniGrafx::utf8ascii(byte ascii) {
  static uint8_t LASTCHAR;

  if ( ascii < 128 ) { // Standard ASCII-set 0..0x7F handling
    LASTCHAR = 0;
    return ascii;
  }

  uint8_t last = LASTCHAR;   // get last char
  LASTCHAR = ascii;

  switch (last) {    // conversion depnding on first UTF8-character
    case 0xC2: return  (ascii);  break;
    case 0xC3: return  (ascii | 0xC0);  break;
    case 0x82: if (ascii == 0xAC) return (0x80);    // special case Euro-symbol
  }

  return  0; // otherwise: return zero, if character has to be ignored
}

// You need to free the char!
char* MiniGrafx::utf8ascii(String str) {
  uint16_t k = 0;
  uint16_t length = str.length() + 1;

  // Copy the string into a char array
  char* s = (char*) malloc(length * sizeof(char));
  if(!s) {
    //DEBUG_OLEDDISPLAY("[OLEDDISPLAY][utf8ascii] Can't allocate another char array. Drop support for UTF-8.\n");
    return (char*) str.c_str();
  }
  str.toCharArray(s, length);

  length--;

  for (uint16_t i=0; i < length; i++) {
    char c = utf8ascii(s[i]);
    if (c!=0) {
      s[k++]=c;
    }
  }

  s[k]=0;

  // This will leak 's' be sure to free it in the calling function.
  return s;
}

void MiniGrafx::setTextAlignment(TEXT_ALIGNMENT textAlignment) {
  this->textAlignment = textAlignment;
}

uint16_t MiniGrafx::getStringWidth(const char* text, uint16_t length) {
  uint16_t firstChar        = readFontData(fontData, FIRST_CHAR_POS);

  uint16_t stringWidth = 0;
  uint16_t maxWidth = 0;

  while (length--) {
    stringWidth += readFontData(fontData, JUMPTABLE_START + (text[length] - firstChar) * JUMPTABLE_BYTES + JUMPTABLE_WIDTH);
    if (text[length] == 10) {
      maxWidth = max(maxWidth, stringWidth);
      stringWidth = 0;
    }
  }

  return max(maxWidth, stringWidth);
}

void inline MiniGrafx::drawInternal(int16_t xMove, int16_t yMove, int16_t width, int16_t height, const char *data, uint16_t offset, uint16_t bytesInData) {

  if (width < 0 || height < 0) return;
  if (yMove + height < 0 || yMove > this->height)  return;
  if (xMove + width  < 0 || xMove > this->width)   return;

  uint8_t  rasterHeight = 1 + ((height - 1) >> 3); // fast ceil(height / 8.0)
  int8_t   yOffset      = yMove & 7;

  bytesInData = bytesInData == 0 ? width * rasterHeight : bytesInData;

  int16_t initYMove   = yMove;
  int8_t  initYOffset = yOffset;

  uint8_t arrayHeight = (int) ceil(height / 8.0);
  for (uint16_t i = 0; i < bytesInData; i++) {
    byte currentByte = readFontData(data, offset + i);

    for (int b = 0; b < 8; b++) {
      if(bitRead(currentByte, b)) {
        uint16_t currentBit = i * 8 + b;
        uint16_t pixelX = (i / arrayHeight);
        uint16_t pixelY = (i % arrayHeight) * 8;
        setPixel(pixelX + xMove, pixelY + yMove + b);
      }
    }
    yield();

  }
}

void MiniGrafx::setPixel(uint16_t xPos, uint16_t yPos) {
  uint16_t x = xPos;
  uint16_t y = yPos;
  if (isMirroredHorizontally) {
    x = width - xPos;
  }
  if (isMirroredVertically) {
    y = height - yPos;
  }
  if (x >= width || y >= height || x < 0 || y < 0 || color < 0 || color == transparentColor) return;
  if (bitsPerPixel == 16) {
    uint32_t pos = (x + y * width) << 1;
    buffer[pos + 1] = color & 0xFF;
    buffer[pos] = color >> 8;
    return;
  }
  // bitsPerPixel: 8, pixPerByte: 1, 0  1 = 2^0
  // bitsPerPixel: 4, pixPerByte: 2, 1  2 = 2^1
  // bitsPerPixel  2, pixPerByte: 4, 2  4 = 2^2
  // bitsPerPixel  1, pixPerByte: 8, 3  8 = 2^3
  uint16_t pos = (y * width + x) >> bitShift;
  if (pos > bufferSize) {
    return;
  }

  uint8_t shift = (x & (pixelsPerByte - 1)) * bitsPerPixel;
  // x: 0 % 2 * 4 = 0
  // x: 1 % 2 * 4 = 0

  //uint8_t shift = ((x) % (pixelsPerByte)) * bitsPerPixel;
  //uint8_t shift = 0;
  uint8_t mask = bitMask << shift;
  uint8_t palColor = color;
  palColor = palColor << shift;
  buffer[pos] = (buffer[pos] & ~mask) | (palColor & mask);
}

uint16_t MiniGrafx::getPixel(uint16_t x, uint16_t y) {
  if (x >= width || y >= height || x < 0 || y < 0 || color < 0 || color > 15) return 0;
  // bitsPerPixel: 8, pixPerByte: 1, 0  1 = 2^0
  // bitsPerPixel: 4, pixPerByte: 2, 1  2 = 2^1
  // bitsPerPixel  2, pixPerByte: 4, 2  4 = 2^2
  // bitsPerPixel  1, pixPerByte: 8, 3  8 = 2^3
  uint16_t pos = (y * width + x) >> bitShift;
  if (pos > bufferSize) {
    return 0;
  }

  uint8_t shift = (x & (pixelsPerByte - 1)) * bitsPerPixel;

  return (buffer[pos] >> shift) & bitMask;
}

void MiniGrafx::fillBuffer(uint8_t pal) {
    uint8_t byteCol = pal;
    for (int i = 0; i < pixelsPerByte; i++) {
      byteCol |= (pal << (bitsPerPixel * i));
    }
    memset(buffer, byteCol, bufferSize);
}

void MiniGrafx::clear() {
  this->fillBuffer(0);
}

void MiniGrafx::commit() {
  BufferInfo bufferInfo;
  bufferInfo.buffer = this->buffer;
  bufferInfo.bitsPerPixel = this->bitsPerPixel;
  bufferInfo.palette = this->palette;
  bufferInfo.targetX = 0;
  bufferInfo.targetY = 0;
  bufferInfo.bufferWidth = this->width;
  bufferInfo.bufferHeight = this->height;
  bufferInfo.windowX = 0;
  bufferInfo.windowY = 0;
  bufferInfo.windowWidth = this->width;
  bufferInfo.windowHeight = this->height;
  this->driver->writeBuffer(&bufferInfo);
}

void MiniGrafx::commit(uint16_t xPos, uint16_t yPos) {
  BufferInfo bufferInfo;
  bufferInfo.buffer = this->buffer;
  bufferInfo.bitsPerPixel = this->bitsPerPixel;
  bufferInfo.palette = this->palette;
  bufferInfo.targetX = xPos;
  bufferInfo.targetY = yPos;
  bufferInfo.bufferWidth = this->width;
  bufferInfo.bufferHeight = this->height;
  bufferInfo.windowX = 0;
  bufferInfo.windowY = 0;
  bufferInfo.windowWidth = this->width;
  bufferInfo.windowHeight = this->height;
  this->driver->writeBuffer(&bufferInfo);
}

void MiniGrafx::commit(uint16_t srcXPos, uint16_t srcYPos, uint16_t srcWidth, uint16_t srcHeight, uint16_t targetXPos, uint16_t targetYPos) {
  BufferInfo bufferInfo;
  bufferInfo.buffer = this->buffer;
  bufferInfo.bitsPerPixel = this->bitsPerPixel;
  bufferInfo.palette = this->palette;
  bufferInfo.targetX = targetXPos;
  bufferInfo.targetY = targetYPos;
  bufferInfo.bufferWidth = this->width;
  bufferInfo.bufferHeight = this->height;
  bufferInfo.windowX = srcXPos;
  bufferInfo.windowY = srcXPos;
  bufferInfo.windowWidth = srcWidth;
  bufferInfo.windowHeight = srcHeight;
  this->driver->writeBuffer(&bufferInfo);
}

void MiniGrafx::setFastRefresh(boolean isFastRefreshEnabled) {
  this->driver->setFastRefresh(isFastRefreshEnabled);
}

void MiniGrafx::drawXbm(int16_t xMove, int16_t yMove, int16_t width, int16_t height, const char *xbm) {
  int16_t widthInXbm = (width + 7) >> 3;
  uint8_t data = 0;

  for(int16_t y = 0; y < height; y++) {
    for(int16_t x = 0; x < width; x++ ) {
      if (x & 7) {
        data >>= 1; // Move a bit
      } else {  // Read new data every 8 bit
        data = pgm_read_byte(xbm + (x >> 3) + y * widthInXbm);
      }
      // if there is a bit draw it
      if (data & 0x01) {
        setPixel(xMove + x, yMove + y);
      }
    }
  }
}
void MiniGrafx::drawBmpFromFile(String filename, uint8_t xMove, uint16_t yMove) {
  Serial.println("In drawBmpFromFile");
  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*20]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint16_t paletteRGB[1 << bitsPerPixel][3];
  for (int i = 0; i < 1 << bitsPerPixel; i++) {
    paletteRGB[i][0] = 255 * (palette[i] & 0xF800 >> 11) / 31;
    paletteRGB[i][1] = 255 * (palette[i] & 0x7E0 >> 5) / 63;
    paletteRGB[i][2] = 255 * (palette[i] & 0x1F) / 31;
  }

  if((xMove >= width) || (yMove >= height)) return;

  /*Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');*/

  bmpFile = SPIFFS.open(filename, "r");
  // Open requested file on SD card
  if (!bmpFile) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    //Serial.print(F("File size: "));
    uint32_t filesize = read32(bmpFile);
    //Serial.println(filesize);
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    //Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    //Serial.print(F("Header size: "));
    uint32_t headerSize = read32(bmpFile);
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((read32(bmpFile) == 0)) { // 0 = uncompressed



        goodBmp = true; // Supported BMP format -- proceed!
        /*Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);*/

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((xMove+w-1) >= width)  w = width  - xMove;
        if((yMove+h-1) >= height) h = height - yMove;

        if ((bmpDepth == 24)) {

          for (row=0; row<h; row++) { // For each scanline...

            // Seek to start of scan line.  It might seem labor-
            // intensive to be doing this on every line, but this
            // method covers a lot of gritty details like cropping
            // and scanline padding.  Also, the seek only takes
            // place if the file position actually needs to change
            // (avoids a lot of cluster math in SD library).
            if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
              pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
            else     // Bitmap is stored top-to-bottom
              pos = bmpImageoffset + row * rowSize;
            if(bmpFile.position() != pos) { // Need seek?
              bmpFile.seek(pos, SeekSet);
              buffidx = sizeof(sdbuffer); // Force buffer reload
            }

            for (col=0; col<w; col++) { // For each pixel...
              // Time to read more pixel data?
              if (buffidx >= sizeof(sdbuffer)) { // Indeed
                bmpFile.read(sdbuffer, sizeof(sdbuffer));
                buffidx = 0; // Set index to beginning
              }

              // Convert pixel from BMP to TFT format, push to display
              b = sdbuffer[buffidx++];
              g = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];

              uint32_t minDistance = 99999999L;
              for (int i = 0; i < (1 << bitsPerPixel); i++) {
                int16_t rd = (r-paletteRGB[i][0]);
                int16_t gd = (g-paletteRGB[i][1]);
                int16_t bd = (b-paletteRGB[i][2]);
                uint32_t distance = rd * rd + gd * gd + bd * bd;
                if (distance < minDistance) {
                  setColor(i);
                  minDistance = distance;
                }
              }
              setPixel(col + xMove, row + yMove);
              //_tft->pushColor(_tft->color565(r,g,b));
              yield();
            } // end pixel
          } // end scanline
        } else if (bmpDepth == 1) {

        }

      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

void MiniGrafx::drawBmpFromPgm(const char *bmp, uint8_t x, uint16_t y) {


  uint32_t bmpWidth, bmpHeight;   // W+H in pixels
  uint16_t bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*80]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = false;        // BMP is stored bottom-to-top
  uint32_t      w, h, row, col;
  uint8_t  r, g, b;

  uint32_t pos = 0, startTime = millis();
  uint16_t numberOfColors = bitsPerPixel == 16 ? 0 : 1 << bitsPerPixel;
  uint16_t paletteRGB[numberOfColors][3];

  for (int i = 0; i < numberOfColors; i++) {
    paletteRGB[i][0] = 255 * (palette[i] & 0xF800 >> 11) / 31;
    paletteRGB[i][1] = 255 * (palette[i] & 0x7E0 >> 5) / 63;
    paletteRGB[i][2] = 255 * (palette[i] & 0x1F) / 31;
  }

  //if((x >= width) || (y >= height)) return;



  // Parse BMP header
  uint32_t dataPointer = 0;


  uint16_t signature = pgm_read_word(bmp);
  dataPointer += 2;
  if(signature == 0x4D42) { // BMP signature
    //Serial.print(F("File size: "));
    uint32_t filesize = pgm_read_dword(bmp + dataPointer);

    dataPointer += 4;
    // Serial.println(filesize);
    //(void)read32(bmpFile); // Read & ignore creator bytes
    dataPointer += 4;
    bmpImageoffset = pgm_read_dword(bmp + dataPointer); // Start of image data
    dataPointer += 4;
    // Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    //Serial.print(F("Header size: "));
    uint32_t headerSize = pgm_read_dword(bmp + dataPointer);
    dataPointer += 4;
    bmpWidth  = pgm_read_dword(bmp + dataPointer);
    dataPointer += 4;
    bmpHeight = pgm_read_dword(bmp + dataPointer);
    dataPointer += 4;
    uint16_t planes = pgm_read_word(bmp + dataPointer);
    dataPointer += 2;

    if(planes == 1) { // # planes -- must be '1'
      bmpDepth = pgm_read_word(bmp + dataPointer); // bits per pixel
      dataPointer += 2;

      //Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      uint32_t compression = 0;//pgm_read_dword(bmp + dataPointer);

      dataPointer += 4;

      if((bmpDepth == 24) && (compression == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        /*Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);*/

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        //if((x+w-1) >= width)  w = width  - x;
        //if((y+h-1) >= height) h = height - y;

        // Set TFT address window to clipped image bounds
        //_tft->setAddrWindow(x, y, x+w-1, y+h-1);

        for (row = 0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).

          /*if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;*/
          /*if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos, SeekSet);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }*/

          for (col=0; col<w; col++) { // For each pixel...

            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              //bmpFile.read(sdbuffer, sizeof(sdbuffer));
              memcpy_P(sdbuffer, bmp + dataPointer, sizeof(sdbuffer));
              dataPointer += sizeof(sdbuffer);
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];

            if (bitsPerPixel == 16) {
              uint16_t color = ((g & 0xF8) << 8) | ((b & 0xFC) << 3) | (r >> 3);
              setColor(color);
            } else {
              uint16_t color = (b + g + r) / (3 * 16);
              setColor(color);
              uint32_t minDistance = 99999999L;
              for (int i = 0; i < (1 << bitsPerPixel); i++) {
                int16_t rd = (r-paletteRGB[i][0]) * 30;
                int16_t gd = (g-paletteRGB[i][1]) * 59;
                int16_t bd = (b-paletteRGB[i][2]) * 11;
                uint32_t distance = rd * rd + gd * gd + bd * bd;
                if (distance < minDistance) {
                  setColor(i);
                  minDistance = distance;
                }
              }
            }
            //Serial.printf("%d,%d\n", row + x, col + y);
            setPixel(x + col, y + row);
            //setPixel(row + x, col + y);
            //_tft->pushColor(_tft->color565(r,g,b));
            yield();
          } // end pixel
        } // end scanline
        /*Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");*/
      } // end goodBmp
    }
  }

  //bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

void MiniGrafx::drawPalettedBitmapFromPgm(uint16_t xMove, uint16_t yMove, const char *palBmp) {
  uint8_t version = pgm_read_byte(palBmp);
  uint8_t bmpBitDepth = pgm_read_byte(palBmp + 1);
  if (bmpBitDepth != bitsPerPixel) {
    Serial.println("Bmp has wrong bit depth");
    return;
  }
  uint16_t width = pgm_read_byte(palBmp + 2) << 8 | pgm_read_byte(palBmp + 3);
  uint16_t height = pgm_read_byte(palBmp + 4) << 8 | pgm_read_byte(palBmp + 5);

  int16_t widthRoundedUp = (width + 7) & ~7;

  uint8_t data;
  uint8_t paletteIndex = 0;
  uint32_t pointer = CUSTOM_BITMAP_DATA_START;
  // bitdepth = 8, initialShift = 0
  // bitdepth = 4, initialShift = 4
  // bitdepth = 2, initialShift = 6
  // bitdepth = 1, initialShift = 7
  uint8_t shift = 8 - bitsPerPixel;
  data = pgm_read_byte(palBmp + pointer);
  uint8_t bitCounter = 0;
  for(int16_t y = 0; y < height; y++) {
    for(int16_t x = 0; x < width; x++ ) {

      if (bitCounter == pixelsPerByte || bitCounter == 0) {
        //Serial.println("Reading new data");
        data = pgm_read_byte(palBmp + pointer);
        pointer++;
        //shift = bitsPerPixel;
        bitCounter = 0;
      }
      shift = 8 - (bitCounter + 1) * bitsPerPixel;
      paletteIndex = (data >> shift) & bitMask;

      //Serial.println(String(x) + ", " + String(y) + ": Pointer:" + String(pointer) + ", data:" + String(data) + ", Bit:" + String(bitCounter) + ", Shift:" + String(shift) + ", IDX:" + String(paletteIndex));
      //Serial.println(paletteIndex);
      // if there is a bit draw it
      setColor(paletteIndex);
      setPixel(xMove + x, yMove + y);
      bitCounter++;
    }
    //pointer++;
    bitCounter = 0;

  }
}

void MiniGrafx::drawPalettedBitmapFromFile(uint16_t xMove, uint16_t yMove, String fileName) {
  Serial.println("Drawing " + fileName);
  File file = SPIFFS.open(fileName, "r");
  if (!file) {
    Serial.print("File " + fileName + " not found");
    return;
  }
  uint32_t fileSize = file.size();
  Serial.println("FileSize:" + String(fileSize));
  char buf[128] = { 0 };
  file.readBytes(buf, 6);
  fileSize -= 6;
  uint8_t version = buf[0];
  uint8_t bmpBitDepth = buf[1];
  Serial.println("Version:" + String(version));
  Serial.println("BitDept:" + String(bmpBitDepth));
  if (bmpBitDepth != bitsPerPixel) {
    Serial.printf("Bmp has wrong bit depth. Device: %d, bmp: %d\n", bitsPerPixel, bmpBitDepth);
    return;
  }
  uint16_t width = buf[2] << 8 | buf[3];
  uint16_t height = buf[4] << 8 | buf[5];


  Serial.println("Width:  " + String(width));
  Serial.println("Height: " + String(height));

  int16_t widthRoundedUp = (width + 7) & ~7;

  uint8_t data = 0;
  uint8_t paletteIndex = 0;
  uint32_t pointer = CUSTOM_BITMAP_DATA_START;
  // bitdepth = 8, initialShift = 0
  // bitdepth = 4, initialShift = 4
  // bitdepth = 2, initialShift = 6
  // bitdepth = 1, initialShift = 7
  uint8_t shift = 8 - bitsPerPixel;

  uint8_t bitCounter = 0;

  for(int16_t y = 0; y < height; y++) {
    for(int16_t x = 0; x < width; x++ ) {

      if (bitCounter == pixelsPerByte || bitCounter == 0) {
        if (fileSize > 0) {
          file.readBytes(buf, 1);
          fileSize--;
        }

        data = buf[0];
        pointer++;
        //shift = bitsPerPixel;
        bitCounter = 0;
      }
      shift = 8 - (bitCounter + 1) * bitsPerPixel;
      paletteIndex = (data >> shift) & bitMask;

      // if there is a bit draw it
      setColor(paletteIndex);
      setPixel(xMove + x, yMove + y);
      bitCounter++;

    }
    //pointer++;
    bitCounter = 0;

  }
}

uint16_t MiniGrafx::read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t MiniGrafx::read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void MiniGrafx::fillBottomFlatTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
  float invslope1 = (x2 - x1) / ((float)(y2 - y1));
  float invslope2 = (x3 - x1) / ((float)(y3 - y1));

  float curx1 = x1;
  float curx2 = x1;

  for (int scanlineY = y1; scanlineY <= y2; scanlineY++)
  {
    //drawHorizontalLine(curx1, scanlineY, curx2 - curx1);
    drawLine(curx1, scanlineY, curx2, scanlineY);
    curx1 += invslope1;
    curx2 += invslope2;
  }
}

void MiniGrafx::fillTopFlatTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3)
{
  float invslope1 = (x3 - x1) / ((float) (y3 - y1));
  float invslope2 = (x3 - x2) / ((float) (y3 - y2));

  float curx1 = x3;
  float curx2 = x3;

  for (int scanlineY = y3; scanlineY > y1; scanlineY--)
  {
    //drawHorizontalLine(curx1, scanlineY, curx2 - curx1);
    drawLine(curx1, scanlineY, curx2, scanlineY);
    curx1 -= invslope1;
    curx2 -= invslope2;
  }
}

void MiniGrafx::fillTriangle(uint16_t x1In, uint16_t y1In, uint16_t x2In, uint16_t y2In, uint16_t x3In, uint16_t y3In)
{
   /* at first sort the three vertices by y-coordinate ascending so v1 is the topmost vertice */
  int x[] = {x1In, x2In, x3In};
  int y[] = {y1In, y2In, y3In};
  for (int a = 0; a < 3; a++) {
    for (int b = a + 1; b < 3; b++) {
      if (y[a] > y[b]) {
        int xTemp, yTemp;
        yTemp = y[b];
        y[b] = y[a];
        y[a] = yTemp;
        xTemp = x[b];
        x[b] = x [a];
        x[a] = xTemp;
      }
    }
  }
  /* here we know that v1.y <= v2.y <= v3.y */
  /* check for trivial case of bottom-flat triangle */
  if (y[1] == y[2])
  {
    fillBottomFlatTriangle(x[0], y[0], x[1], y[1], x[2], y[2]);
  }
  /* check for trivial case of top-flat triangle */
  else if (y[0] == y[1])
  {
    fillTopFlatTriangle(x[0], y[0], x[1], y[1], x[2], y[2]);
  }
  else
  {
    /* general case - split the triangle in a topflat and bottom-flat one */
    int x4 = (int)(x[0] + ((float)(y[1] - y[0]) / (float)(y[2] - y[0])) * (x[2] - x[0]));
    int y4 = y[1];
    fillBottomFlatTriangle(x[0], y[0], x[1], y[1], x4, y4);
    fillTopFlatTriangle(x[1], y[1], x4, y4, x[2], y[2]);
  }
}

void MiniGrafx::drawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3) {
  drawLine(x1, y1, x2, y2);
  drawLine(x2, y2, x3, y3);
  drawLine(x3, y3, x1, y1);
}

//swaps the colors in the given rectangle
void MiniGrafx::colorSwap(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color1, uint16_t color2) {
   uint16_t pix;
  for (int y=y1; y<y2; y++){
    for (int x=x1; x < x2; x++){
      pix = getPixel(x, y);
      if (pix==color1){
        setColor(color2);
        setPixel(x, y);
      }
      else{
        setColor(color1);
        setPixel(x, y);
      }
    }
  }
}

void MiniGrafx::setMirroredHorizontally(boolean isMirroredHorizontally) {
  this->isMirroredHorizontally = isMirroredHorizontally;
}

void MiniGrafx::setMirroredVertically(boolean isMirroredVertically) {
  this->isMirroredVertically = isMirroredVertically;
}
