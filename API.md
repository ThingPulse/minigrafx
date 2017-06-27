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

## changeBitDepth

With changeBitDepth you can change the bit depth even after creating the MiniGrafx object. Please bear in mind that this operation deallocates the reserved frame buffer memory and allocates new memory according to the new
bit depth. Or in other words the content of the frame buffer before calling this method will be lost if it has
not yet been written to the display.

```C++
void changeBitDepth(uint8_t bitsPerPixel, uint16_t *palette);
```

## init

With the init function you initialize the driver as well as the graphics library. Usually you only do this
once in the setup method:

```C++
gfx.init();
```


## commit

The commit() method calls the driver to write the content of the frame buffer to the display. Without calling
commit() the content of the frame buffer will not become visible. You usually do this at the end of all the drawing in one loop iteration:

```C++
gfx.commit();
```

## fillBuffer and clear

With clear() and fillBuffer() you fill the frame buffer with one value; you erase it. The only difference between
the two is that clear() fills the frame buffer with the palette index 0, whereas with fillBuffer(uint8_t) you
can define which color should be used to fill the buffer.

```C++
void clear();
void fillBuffer(uint8_t pal);
```

## setColor

With setColor you define the currently active color for many drawing operations. The parameter is the palette
index of the color you want to set. The range of color is thus between 0 and 2^bitDepth - 1. For a bith depth
of 4 it can be 0 and 15.

```C++
void setColor(uint16_t color);
```

## setPixel and getPixel

Many higher level drawing operations use setPixel to draw into the frame buffer. Using this atomic function might
not be as fast as possible but increases flexibility and portability. getPixel reads out the palette index at
the given index.

```C++
void setPixel(uint16_t x, uint16_t y);
uint16_t getPixel(uint16_t x, uint16_t y);
```
