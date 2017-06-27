# MiniGrafx API

This chapter explains the available methods of the graphics library.

## Constructor

The constructor creates the graphic library and allocates the frame buffer memory.
Parameters:
* DisplayDriver: the hardware driver for your library. See [How to implement a new driver](Driver.md)
* bitsPerPixel: bit depth, currently supported are 1, 2, 4 and 8
* palette: array with length 2^bitsPerPixel containing the mapping from palette color to display color

```C++
MiniGrafx(DisplayDriver *driver, uint8_t bitsPerPixel, uint16_t *palette);
```

Example:
```C++
#define BITS_PER_PIXEL 1
uint16_t palette[] = {ILI9341_BLACK, // 0
                      ILI9341_WHITE};

ILI9341_SPI tft = ILI9341_SPI(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, BITS_PER_PIXEL, palette);
```

## Basic Operations


```C++
void commit();
void clear();
void fillBuffer(uint8_t pal);
```
