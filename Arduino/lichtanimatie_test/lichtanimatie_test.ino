#include "FastLED.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    76
#define sliderPin A3
#define buttonPin 7
CRGB leds[NUM_LEDS];
int slider = 0;
int state = 0;
int hue = 100;
int huestep = 20;
int huestart = 100;
int hueend = 230;
int wait = 800;
int snelheid = 20;
int fadeamount = 5;
int fadeamount2 = 5;
int fadeamount3 = 5;
int fadeamount4 = 5;
int fadeamount5 = 5;
int startwaarde = 0; 
int startwaarde2 = 0;
int startwaarde3 = 0;
int startwaarde4 = 0;


int brightness5 = 0;
int brightness4 = 0;
int brightness3 = 0;
int brightness2 = 0;
int brightness = 0;
#define BRIGHTNESS         200
#define FRAMES_PER_SECOND  120
 

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(9600);
}



// List of patterns to cycle through.  Each is defined as a separate function below.
  
void loop()
{

//slider = analogRead(sliderPin);
//state = map(slider,0,1023,0,3);
state = digitalRead(buttonPin);
Serial.println(startwaarde);
switch(state){
  case 0:
  idle();
  break;
  case 1:
  state1();
  break;
  case 2:
  break;
}
FastLED.show();

}

void idle()
{
  while(startwaarde < 102){
  binnenfade();}
  while(startwaarde2 < 102){
  middenfade();}
  while(startwaarde3 <102){
  buitenfade();}
  while(startwaarde4 <102){
  middenfade2();}
}

void rainbowring1() 
{
  FastLED.clear();
  fill_rainbow(leds, 12, 10, 15); 
  FastLED.delay(wait);
  FastLED.clear();
  fill_rainbow(leds+12,24,10,10);
  FastLED.delay(wait);
  FastLED.clear();
  fill_rainbow(leds+36,40,10,6);
  FastLED.delay(wait);
  FastLED.clear();
  fill_rainbow(leds+12,24,10,10);
  FastLED.delay(wait);
}

void colorswitching() 
{
  hue = hue + 5;
  if (hue >= 255){hue = 150;}
  CHSV color = CHSV(hue, 255, BRIGHTNESS);
  
  FastLED.clear();
  fill_solid( leds, 12, color);
  FastLED.delay(wait);
  FastLED.clear();
  fill_solid( leds+12, 24, color);
  FastLED.delay(wait);
  FastLED.clear();
  fill_solid( leds+36, 40, color);
  FastLED.delay(wait);
  FastLED.clear();
  fill_solid( leds+12, 24, color);
  FastLED.delay(wait);
  
}

void binnenfade()
{
CHSV color = CHSV(hue, 255, brightness);
FastLED.clear();
fill_solid(leds,12,color);

brightness = brightness + fadeamount;
  // reverse the direction of the fading at the ends of the fade: 
  if(brightness == 0 || brightness == 255)
  {
    fadeamount = -fadeamount ; 
  }
  FastLED.delay(snelheid);
  startwaarde = startwaarde + 1;
  if (startwaarde == 101){startwaarde2 = 0;}
}

void middenfade()
{
CHSV color2 = CHSV(hue, 255, brightness2);
FastLED.clear();
fill_solid(leds+12,24,color2);

brightness2 = brightness2 + fadeamount2;
  // reverse the direction of the fading at the ends of the fade: 
  if(brightness2 == 0 || brightness2 == 255)
  {
    fadeamount2 = -fadeamount2 ; 
  }
  FastLED.delay(snelheid);
  startwaarde2 = startwaarde2 + 1;
  if (startwaarde2 == 101){startwaarde3 = 0;}
}

void buitenfade()
{
CHSV color3 = CHSV(hue, 255, brightness3);
FastLED.clear();
fill_solid(leds+36,40,color3);

brightness3 = brightness3 + fadeamount3;
  // reverse the direction of the fading at the ends of the fade: 
  if(brightness3 == 0 || brightness3 == 255)
  {
    fadeamount3 = -fadeamount3 ; 
  }
  FastLED.delay(snelheid);
  startwaarde3 = startwaarde3 + 1;
  if (startwaarde3 == 101){startwaarde4 = 0;}
}
void middenfade2()
{
CHSV color4 = CHSV(hue, 255, brightness4);
FastLED.clear();
fill_solid(leds+12,24,color4);

brightness4 = brightness4 + fadeamount4;
  // reverse the direction of the fading at the ends of the fade: 
  if(brightness4 == 0 || brightness4 == 255)
  {
    fadeamount4 = -fadeamount4 ; 
  }
  FastLED.delay(snelheid);
  startwaarde4 = startwaarde4 + 1;
  if (startwaarde4 == 101){startwaarde = 0;
  hue = hue + huestep;
  if(hue <= huestart || hue >= hueend){huestep = -huestep;}
  }
  
}


void state1 ()
{CHSV color5 = CHSV(hue, 255, brightness5);
fill_solid(leds,76,color5);
brightness5 = brightness5 + fadeamount5;
  // reverse the direction of the fading at the ends of the fade: 
  FastLED.delay(15);
 if(brightness5 >= 255 && state == 0){brightness5 = 0;}
}


