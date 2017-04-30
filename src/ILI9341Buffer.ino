
#include <Adafruit_GFX.h>    // Core graphics library
#include "ILI9341_SPI.h" // Hardware-specific library
#include "MiniGrafx.h"
#include "WeatherStationFonts.h"

#include <SPI.h>

#define TFT_DC D2
#define TFT_CS D1
#define TFT_LED D8
uint16_t palette[] = {ILI9341_BLACK,
                      ILI9341_WHITE,
                      ILI9341_NAVY,
                      ILI9341_DARKCYAN,
                      ILI9341_DARKGREEN,
                      ILI9341_MAROON,
                      ILI9341_PURPLE,
                      ILI9341_OLIVE,
                      ILI9341_LIGHTGREY,
                      ILI9341_DARKGREY,
                      ILI9341_BLUE,
                      ILI9341_GREEN,
                      ILI9341_CYAN,
                      ILI9341_RED,
                      ILI9341_MAGENTA,
                      ILI9341_YELLOW};
#define BITS_PER_PIXEL 4
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
MiniGrafx gfx = MiniGrafx(&tft, 240, 320, BITS_PER_PIXEL, palette);
char iconMap[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O' };

struct Point {
  uint16_t x,y;
  int8_t xd, yd;
  int8_t xv, yv;
  uint8_t color;
};

uint8_t col = 0;

uint16_t counter = 0;
long startMillis = millis();
uint16_t interval = 20;



int SCREEN_WIDTH = 240;
int SCREEN_HEIGHT = 320;

float d = 15;
float px[] = {
  -d,  d,  d, -d, -d,  d,  d, -d };
float py[] = {
  -d, -d,  d,  d, -d, -d,  d,  d };
float pz[] = {
  -d, -d, -d, -d,  d,  d,  d,  d };

int  faces[12][3] = {
    {0,1,4},
    {1,5,4},
    {1,2,5},
    {2,6,5},
    {5,7,4},
    {6,7,5},
    {3,4,7},
    {4,3,0},
    {0,3,1},
    {1,3,2},
    {2,3,6},
    {6,3,7}
  };




float p2x[] = {
  0,0,0,0,0,0,0,0};
float p2y[] = {
  0,0,0,0,0,0,0,0};

float r[] = {
  0,0,0};

#define SHAPE_SIZE 600
// Define how fast the cube rotates. Smaller numbers are faster.
// This is the number of ms between draws.
#define ROTATION_SPEED 0
String fps = "0fps";

void setup() {
  Serial.begin(115200);
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);

  tft.begin();
  tft.fillScreen(ILI9341_BLACK);


  startMillis = millis();
}

void printDouble( double val, unsigned int precision){
// prints val with number of decimal places determine by precision
// NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
// example: printDouble( 3.1415, 100); // prints 3.14 (two decimal places)

   Serial.print (int(val));  //prints the int part
   Serial.print("."); // print the decimal point
   unsigned int frac;
   if(val >= 0)
       frac = (val - int(val)) * precision;
   else
       frac = (int(val)- val ) * precision;
   Serial.print(frac,DEC) ;
}

void sort(float a[], int indexes[], int size) {
    for(int i=0; i<(size-1); i++) {
        for(int o=0; o<(size-(i+1)); o++) {
                if(a[o] > a[o+1]) {
                    float t = a[o];
                    a[o] = a[o+1];
                    a[o+1] = t;
                    int t2 = indexes[o];
                    indexes[o] = indexes[o+1];
                    indexes[o+1] = t2;
                }
        }
    }
}


int shoelace(int x1, int y1, int x2, int y2, int x3, int y3) {
  // (x1y2 - y1x2) + (x2y3 - y2x3)
  return x1 * y2 - y1 * x2 + x2*y3 - y2*x3 + x3*y1 - y3*x1;
}

void drawCube()
{
  double speed = 45.0;
  r[0]=r[0]+PI/speed; // Add a degree
  r[1]=r[1]+PI/speed; // Add a degree
  r[2]=r[2]+PI/speed; // Add a degree
  if (r[0] >= 360.0*PI/90.0) r[0] = 0;
  if (r[1] >= 360.0*PI/90.0) r[1] = 0;
  if (r[2] >= 360.0*PI/90.0) r[2] = 0;

  float ax[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  float ay[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  float az[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  for (int i=0;i<8;i++)
  {
    float px2 = px[i];
    float py2 = cos(r[0])*py[i] - sin(r[0])*pz[i];
    float pz2 = sin(r[0])*py[i] + cos(r[0])*pz[i];

    float px3 = cos(r[1])*px2 + sin(r[1])*pz2;
    float py3 = py2;
    float pz3 = -sin(r[1])*px2 + cos(r[1])*pz2;

    ax[i] = cos(r[2])*px3 - sin(r[2])*py3;
    ay[i] = sin(r[2])*px3 + cos(r[2])*py3;
    az[i] = pz3-150;

    p2x[i] = SCREEN_WIDTH/2+ax[i]*SHAPE_SIZE/az[i];
    p2y[i] = SCREEN_HEIGHT/2+ay[i]*SHAPE_SIZE/az[i];
  }

  gfx.fillBuffer(0);




  for (int i = 0; i < 12; i++) {

    if (shoelace(p2x[faces[i][0]],p2y[faces[i][0]],p2x[faces[i][1]],p2y[faces[i][1]],p2x[faces[i][2]],p2y[faces[i][2]]) > 0) {
      gfx.setColor((i / 2) + 1);
      gfx.fillTriangle(p2x[faces[i][0]],p2y[faces[i][0]],p2x[faces[i][1]],p2y[faces[i][1]],p2x[faces[i][2]],p2y[faces[i][2]]);
    }



  }
  gfx.setColor(1);
  gfx.drawString(2, 2, fps);
  gfx.commit();
}


void loop() {

  drawCube();
  counter++;
  if (counter % interval == 0) {
    long millisSinceUpdate = millis() - startMillis;
    fps = String(interval * 1000.0 / (millisSinceUpdate)) + "fps";
    startMillis = millis();
  }
}
