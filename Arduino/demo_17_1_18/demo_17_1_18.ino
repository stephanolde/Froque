/*
  HC-SR04 Ping distance sensor]
  VCC to arduino 5v GND to arduino GND
  Echo to Arduino pin 13 Trig to Arduino pin 12
  Red POS to Arduino pin 11
  Green POS to Arduino pin 10
  560 ohm resistor to both LED NEG and GRD power rail
  More info at: http://goo.gl/kJ8Gl
  Original code improvements to the Ping sketch sourced from Trollmaker.com
  Some code and wiring inspired by http://en.wikiversity.org/wiki/User:Dstaub/robotcar
*/
#include "FastLED.h"



const int trigPin = A0;
const int echoPin = A1;


DEFINE_GRADIENT_PALETTE( bluefly_gp ) {
  0,   0,  0,  0,
  63,   0, 39, 64,
  191, 175, 215, 235,
  255,   0,  0,  0
};

#define numLedsInsert 76
#define numLight 15
CRGB leds[numLight][numLedsInsert];
byte dothue = 0;
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  initFastLED();
    FastLED.setBrightness(BRIGHTNESS);

    pinMode(13, OUTPUT);
}

void loop() {
  long distance;

  distance = US_dist(trigPin, echoPin);

  Serial.println(distance);
  if (distance > 0 && distance < 25) {
    for (byte i = 0; i < numLight; i++) {
      juggle(i);
      
    }
    for (byte i = 6 ; i < 14; i++){
      digitalWrite(i, HIGH);
    }
    
  } else {
    for (byte i; i < numLight; i++) {
     fadeToBlackBy( leds[i], numLedsInsert, 20);
    }
    for (byte i = 6 ; i < 14; i++){
      digitalWrite(i, LOW);
    }

  }
  FastLED.show();
}

int US_dist(int tPin, int ePin) {
  // When using a 3 pin distance sensor set echoPin = 0
  long duration, dist;

  if (ePin == 0) {                     //3 pin distance sensor
    pinMode(tPin, OUTPUT);
    digitalWrite(tPin, LOW);
    delayMicroseconds(2);
    digitalWrite(tPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(tPin, LOW);
    pinMode(tPin, INPUT);
    duration = pulseIn(tPin, HIGH);
    dist = (duration / 28.34 / 2);    //Distance in cm
  }
  else {                                // 4 pin distance sensor
    digitalWrite(tPin, LOW);
    delayMicroseconds(2);
    digitalWrite(tPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(tPin, LOW);
    duration = pulseIn(ePin, HIGH, 40000);
    dist = (duration / 2) / 29.1;
  }
  return dist;
}

void idleLights(int index) {
  //int x = 0;
  // This inner loop will go over each led in the current strip, one at a time
  for (int i = 0; i < numLedsInsert; i++) {
    for (int x = 0; x < numLight; x++) {
      leds[x][i] = CRGB::Blue;
    }
    FastLED.show();
  }
  for (int i = 0; i < numLedsInsert; i++) {
    for (int x = 0; x < numLight; x++) {
      leds[x][i] = CRGB::Purple;
    }
    FastLED.show();
  }
  for (int i = 0; i < numLedsInsert; i++) {
    for (int x = 0; x < numLight; x++) {
      leds[x][i] = CRGB::Cyan;
    }
    FastLED.show();
  }
  return;
}

void initFastLED() {
  FastLED.addLeds<NEOPIXEL, 22>(leds[0], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 24>(leds[1], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 26>(leds[2], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 28>(leds[3], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 30>(leds[4], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 32>(leds[5], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 34>(leds[6], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 36>(leds[7], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 38>(leds[8], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 40>(leds[9], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 42>(leds[10], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 44>(leds[11], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 46>(leds[12], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 48>(leds[13], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 50>(leds[14], numLedsInsert);
}

void juggle(byte index) {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds[index], numLedsInsert, 20);
  
  for( int i = 0; i < 8; i++) {
    leds[index][beatsin16( i+7, 0, numLedsInsert-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}


