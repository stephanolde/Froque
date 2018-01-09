#include "CmdMessenger.h"
#include "Thread.h"
#include "ThreadController.h"
#include "Adafruit_NeoPixel.h"
#include <FastLED.h>

const byte dim_curve[] = {
    0,   1,   1,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,   3,   3,
    3,   3,   3,   3,   3,   3,   3,   4,   4,   4,   4,   4,   4,   4,   4,   4,
    4,   4,   4,   5,   5,   5,   5,   5,   5,   5,   5,   5,   5,   6,   6,   6,
    6,   6,   6,   6,   6,   7,   7,   7,   7,   7,   7,   7,   8,   8,   8,   8,
    8,   8,   9,   9,   9,   9,   9,   9,   10,  10,  10,  10,  10,  11,  11,  11,
    11,  11,  12,  12,  12,  12,  12,  13,  13,  13,  13,  14,  14,  14,  14,  15,
    15,  15,  16,  16,  16,  16,  17,  17,  17,  18,  18,  18,  19,  19,  19,  20,
    20,  20,  21,  21,  22,  22,  22,  23,  23,  24,  24,  25,  25,  25,  26,  26,
    27,  27,  28,  28,  29,  29,  30,  30,  31,  32,  32,  33,  33,  34,  35,  35,
    36,  36,  37,  38,  38,  39,  40,  40,  41,  42,  43,  43,  44,  45,  46,  47,
    48,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,
    63,  64,  65,  66,  68,  69,  70,  71,  73,  74,  75,  76,  78,  79,  81,  82,
    83,  85,  86,  88,  90,  91,  93,  94,  96,  98,  99,  101, 103, 105, 107, 109,
    110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
    146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
    193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};

const int numSens = 5;
int location = 0;

const int numLeds = 39;
const int ledPin = 8;

Adafruit_NeoPixel leds = Adafruit_NeoPixel(numLeds, ledPin, NEO_GRB + NEO_KHZ800);

int sensLoc[numSens][3] = {0};
int ledColour[numSens][3] = {0};
const int numLeds = 5;
CRGB leds[numLeds];

#define DATA_PIN 6
#define CLOCK_PIN 7

/* Define available CmdMessenger commands */
enum {
  my_sensor_amount,
  build_to_arduino,
  data_to_arduino,
  error,
};

/* Initialize CmdMessenger -- this should match PyCmdMessenger instance */
const int BAUD_RATE = 19200;
CmdMessenger c = CmdMessenger(Serial, ',', ';', '/');

/* callback */
void on_sensor_amount(void) {
  const int numSensTemp = c.readBinArg<int>();
  int sensLocTemp [numSensTemp][3] = { };
}

/* callback */
void on_build_to_arduino(void) {
  if (location == 5) {
    location = 0;
  }
  sensLoc[location][0] = c.readBinArg<int>();
  sensLoc[location][1] = c.readBinArg<int>();
  sensLoc[location][2] = c.readBinArg<int>();
  location++;
}

/* callback */
void on_data_to_arduino(void) {
  if (location == numSens) {
    location = 0;
  }
  sensLoc[location][0] = c.readBinArg<int>();
  sensLoc[location][1] = c.readBinArg<int>();
  sensLoc[location][2] = c.readBinArg<int>();
  
  if (sensLoc[location][2] < 35) {
    brightness = 0;
  } else if (sensLoc[location][2] > 140) {
    brightness = 0;
  } else {
    hue = map(sensLoc[location][2], 35, 150, 0, 359);
  }
  getRGB(hue, saturation, brightness, RGB);
  
  for(int i = location * numLeds/numSens; i < (location + 1)* numLeds/numSens; i++) {
    leds.setPixelColor(i, leds.Color(RGB[0], RGB[1], RGB[2]));
  }
  leds.show();
  
  location++;
}

/* callback */
void on_unknown_command(void) {
  c.sendCmd(error, "Command without callback.");
}

/* Attach callbacks for CmdMessenger commands */
void attach_callbacks(void) {
  c.attach(my_sensor_amount, on_sensor_amount);
  c.attach(build_to_arduino, on_build_to_arduino);
  c.attach(data_to_arduino, on_data_to_arduino);
  c.attach(on_unknown_command);
}

void setup() {
  Serial.begin(BAUD_RATE);
  attach_callbacks();

  leds.begin();
  leds.show();
  
  //FastLED.addLeds<P9813, DATA_PIN, CLOCK_PIN, RGB>(leds, numLeds);
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, numLeds);

}

void loop() {
  c.feedinSerialData();
  
  setColours();
  FastLED.show();
}

void setColours(){

  for(byte i = 0; i < numSens; i++){
  
  switch (sensLoc[i][2]) {
      case 0:
        colourChange(i, 0, 0, 0);
        break;
      case 1:
        colourChange(i, 150, 200, 0);
        break;
      case 2:
        colourChange(i, 0, 250, 255);
        break;
    }
  }  
  
}

void getRGB(int hue, int sat, int val, int colors[3]) { 
  /* convert hue, saturation and brightness ( HSB/HSV ) to RGB
     The dim_curve is used only on brightness/value and on saturation (inverted).
     This looks the most natural.      
     source: https://www.kasperkamperman.com/blog/arduino/arduino-programming-hsb-to-rgb/
  */
 
  val = dim_curve[val];
  sat = 255-dim_curve[255-sat];
 
  int r;
  int g;
  int b;
  int base;
 
  if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;  
  } else  { 
 
    base = ((255 - sat) * val)>>8;
 
    switch(hue/60) {
    case 0:
        r = val;
        g = (((val-base)*hue)/60)+base;
        b = base;
    break;
 
    case 1:
        r = (((val-base)*(60-(hue%60)))/60)+base;
        g = val;
        b = base;
    break;
 
    case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%60))/60)+base;
    break;
 
    case 3:
        r = base;
        g = (((val-base)*(60-(hue%60)))/60)+base;
        b = val;
    break;
 
    case 4:
        r = (((val-base)*(hue%60))/60)+base;
        g = base;
        b = val;
    break;
 
    case 5:
        r = val;
        g = base;
        b = (((val-base)*(60-(hue%60)))/60)+base;
    break;
    }
 
    colors[0]=r;
    colors[1]=g;
    colors[2]=b; 
  }   
}

void colourChange( int sensIndex, int R, int G, int B) {

  int targetColour[3] = {R, G, B};

  leds[sensIndex] = CRGB(ledColour[sensIndex][0], ledColour[sensIndex][1], ledColour[sensIndex][2]);
  
  if (ledColour[sensIndex][0] == targetColour[0] && ledColour[sensIndex][1] == targetColour[1] && ledColour[sensIndex][2] == targetColour[2])
    return;

  for (byte i = 0; i < 3; i++) {

    if (ledColour[sensIndex][i] < targetColour[i])
      ledColour[sensIndex][i]++;
    else if (ledColour[sensIndex][i] > targetColour[i])
      ledColour[sensIndex][i]--;
    else
      ledColour[sensIndex][i] = targetColour[i];


  }

  leds[sensIndex] = CRGB(ledColour[sensIndex][0], ledColour[sensIndex][1], ledColour[sensIndex][2]);
  
}
