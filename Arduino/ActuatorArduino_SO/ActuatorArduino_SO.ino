#include "FastLED.h"
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    76
#define NUM_RINGS   15
#define NUM_WIND    8

CRGB leds[NUM_RINGS + 2][NUM_LEDS]; // +2 so we can precalculate things

#define COLOR_ORDER GRB

struct LEDring {
  boolean Activated = false;
  int State = 0;
  long StartTime = 0;
  long StateTime;
  long LastSeen;
  int count = 0;
  int check = 1;
} LEDRings[NUM_RINGS];

struct windInsert {
  long startTime = 0;
  long interval = 0;
  bool state = false;
} windInserts[NUM_WIND];

struct GlobalVar {
  int brightness = 100; // just a starting value;
  int cycleIncrement = 10; // adjustable value
  int fadeState = 1; // just a starting value between 1 and 3
  int fadeDirections = 1;
  int ringSize[4] = {12, 24, 36, 24}; // added ringsize 4 for fadingcycle ease
  int TimeOut = 1000; // used for downscaling;
  int ActivatedSize = 0;
  int color[3] = {100, 150, 200};
  int maxState;
  long timeSinceWave;
  int windSensors[8] = {0, 1, 2, 3, 4, 5, 6, 7};

} GlobalVars;


void setup() {
  Serial.begin(9600);

  //Declare input and output pins for active states

  //Declare Statepins
  for (int i = A0; i <= A15; i++) {
    pinMode (i, INPUT_PULLUP);
  }

  // Declare outputs for fan relais
  for (int i = 6; i <= 13; i++) {
    pinMode (i, OUTPUT);
  }

  // Add leds to FastLED
  FastLED.addLeds<LED_TYPE, 22, COLOR_ORDER>(leds[0],  NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 24, COLOR_ORDER>(leds[1],  NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 26, COLOR_ORDER>(leds[2],  NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 28, COLOR_ORDER>(leds[3],  NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 30, COLOR_ORDER>(leds[4],  NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 32, COLOR_ORDER>(leds[5],  NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 34, COLOR_ORDER>(leds[6],  NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 36, COLOR_ORDER>(leds[7],  NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 38, COLOR_ORDER>(leds[8],  NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 40, COLOR_ORDER>(leds[9],  NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 42, COLOR_ORDER>(leds[10], NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 44, COLOR_ORDER>(leds[11], NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 46, COLOR_ORDER>(leds[12], NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 48, COLOR_ORDER>(leds[13], NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 50, COLOR_ORDER>(leds[14], NUM_LEDS).setCorrection(TypicalLEDStrip);





  // Light up all rings for a short time to show setup is done and then clear them.
  for (int i = 0; i < NUM_RINGS; i++) {
    fill_rainbow(leds[i], NUM_LEDS, 10, 5);
  }
  FastLED.show();
  FastLED.delay(3000);
  FastLED.clear();

}

void loop() {

  UpdateStates();
  CycleVariables();
  ShowAnimation();
  RegulateFans(); // not yet active
  LoopProtection();

}

void UpdateStates() {

  GlobalVars.maxState = 0;
  GlobalVars.ActivatedSize = 0;

  for (int i = 0; i < 15; i++) {

    if (LEDRings[i].State == 0) {
      LEDRings[i].StateTime = 0;
      LEDRings[i].LastSeen = 0;
    }
    LEDRings[i].count = 0;
    for (int j = 0; j < 3; j++) {
      if (digitalRead(A0 + i) == 0) {
        LEDRings[i].count++;
      }
    }




    if (LEDRings[i].count >= 2) {
      LEDRings[i].LastSeen = millis();

      if (LEDRings[i].State == 0) {
        LEDRings[i].StartTime = millis();   // ToDo remove startTime for state time
        LEDRings[i].StateTime = millis();
        LEDRings[i].State = 1;
      }

      if (LEDRings[i].State == 1 && millis() - LEDRings[i].StateTime >= 3000 ) {
        LEDRings[i].State = 2;
        LEDRings[i].StateTime = millis();
        LEDRings[i].LastSeen = millis(); // beundingetje
      }
    }


    if (LEDRings[i].State > 0 && millis() - LEDRings[i].LastSeen >= GlobalVars.TimeOut) {
      LEDRings[i].State--;
      LEDRings[i].LastSeen = millis();
      LEDRings[i].StateTime = millis();
    }

    if (LEDRings[i].State != 0) {
      //GlobalVars.ActivatedSize++;
    }

    if (LEDRings[i].State == 0) {
      GlobalVars.ActivatedSize--;
      LEDRings[i].StartTime = 0;
    }


    if (LEDRings[i].State == 2) {
      GlobalVars.maxState = 1;

    }



    /*   if (LEDRings[i].StateTime >= 3000) {
         Serial.println(millis() - LEDRings[i].StateTime);
         Serial.println(millis());

       }*/
  }
}



void CycleVariables() {


  if (GlobalVars.brightness >= 245 || GlobalVars.brightness <= 10) {
    GlobalVars.fadeDirections = - GlobalVars.fadeDirections;
  }


  GlobalVars.brightness = GlobalVars.brightness + GlobalVars.fadeDirections * GlobalVars.cycleIncrement;
  if (GlobalVars.brightness > 245) {
    GlobalVars.fadeState++;
    if (GlobalVars.fadeState == 5) {
      GlobalVars.fadeState = 1;

      for (int i = 0; i < 3; i++) { // only update colors once per cycle
        GlobalVars.color[i] = GlobalVars.color[i] + random(0, 50);
        if (GlobalVars.color[i] > 255) {
          GlobalVars.color[i] = 0;
        }
      }

    }
  }


}

void ShowAnimation() {


  if (millis() - GlobalVars.timeSinceWave >= 60000) {
    WaveOverWall();
    GlobalVars.timeSinceWave = millis();
  }
  else {

    if (GlobalVars.maxState > 0) {
      PreCalculateRainbow();
    }

    //  if (GlobalVars.ActivatedSize >= 5) {
    //
    //  SystemWideAnimation();
    //  }
    //  else {

    // for( int i = 0; i<15; i++){
    //   Serial.print(LEDRings[i].State);
    //   Serial.print("  ");
    // }
    // Serial.println();

    FastLED.clear();
    for (int i = 0; i < 15 ; i++) {

      switch (LEDRings[i].State) {
        case 0:
          animation0(i);
          break;
        case 1:
          animation1(i);
          break;
        case 2:
          animation2(i);
          break;


      }
    }

    FastLED.show();

  }
}



void animation0(int LEDindex) {
  switch (GlobalVars.fadeState) {
    case 1:
      fill_solid(leds[LEDindex], 12, CRGB::Cyan);
      fadeLightBy(leds[LEDindex], 12, GlobalVars.brightness);
      break;
    case 2:
      fill_solid(leds[LEDindex] + 12, 24, CRGB::Cyan);
      fadeLightBy(leds[LEDindex] + 12, 24, GlobalVars.brightness);
      break;

    case 3:
      fill_solid(leds[LEDindex] + 36, 40, CRGB::Cyan);
      fadeLightBy(leds[LEDindex] + 36, 40, GlobalVars.brightness);
      break;
    case 4:
      fill_solid(leds[LEDindex] + 12, 24, CRGB::Cyan);
      fadeLightBy(leds[LEDindex] + 12, 24, GlobalVars.brightness);
      break;

  }
}

void animation1(int LEDindex) {
  fill_solid(leds[LEDindex], NUM_LEDS, CRGB::Red);

}

void animation2(int LEDindex) {
  //fill_rainbow( leds[LEDindex], 12, GlobalVars.brightness + 15, 15 );
  //fill_rainbow( leds[LEDindex] + 12, 24, GlobalVars.brightness + 7, 9 );
  //fill_rainbow( leds[LEDindex] + 36, 40, GlobalVars.brightness, 6 );
  for (int i = 0; i < 76; i++) {
    leds[LEDindex][i] = leds[16][i];

  }
}

void RegulateFans() {

  int index;

  for (int i = 0; i < NUM_WIND; i++) {
    index = GlobalVars.windSensors[i];

    switch (LEDRings[index].State) {
      case 1:
        windControl(i, 1000, 0.25);
        break;
      case 2:
        windControl(i, 1000, 1);
        break;
      default:
        windControl(i, 1000, 0);
        break;
    }
  }
}

void windControl(int index, long period, float duty) {
  long currentMillis = millis();
  if (currentMillis - windInserts[index].startTime >= windInserts[index].interval) {
    windInserts[index].startTime = currentMillis;
    if (windInserts[index].state == false && duty != 0) {
      windInserts[index].state = true;
      windInserts[index].interval = period * duty;
    } else if (windInserts[index].state == true && duty != 1) {
      windInserts[index].state = false;
      windInserts[index].interval = period * (1 - duty);
    }
    digitalWrite(6 + index, windInserts[index].state);
  }
}

void SystemWideAnimation() {

  // random colored speckles that blink in and fade smoothly
  for (int i = 0; i < 15; i++) {
    fadeToBlackBy( leds[i], NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[i][pos] += CHSV( 10 + random8(64), 200, 255);
  }

}

void LoopProtection() {

  for (int i = 0; i < 15 ; i++) {                       // ToDo remove startTime for satate time
    if (millis() - LEDRings[i].StartTime > 30000) {
      LEDRings[i].State = 0;
      LEDRings[i].StartTime = 0;
      LEDRings[i].LastSeen = 0;
    }
  }
}

void PreCalculateRainbow() {
  fill_rainbow( leds[16], 12, GlobalVars.brightness + 15, 15 );
  fill_rainbow( leds[16] + 12, 24, GlobalVars.brightness + 7, 9 );
  fill_rainbow( leds[16] + 36, 40, GlobalVars.brightness, 6 );


}

void WaveOverWall() {
  FastLED.clear();
  for (int j = 0; j < 15; j++) {
    int i = 0;
    while ( i < 21) {

      leds[j][40 + i].setRGB(GlobalVars.color[1], GlobalVars.color[2], GlobalVars.color[3]);
      leds[j][76 - i].setRGB(GlobalVars.color[1], GlobalVars.color[2], GlobalVars.color[3]);

      if (i > 0) {

        leds[j][12].setRGB(GlobalVars.color[1], GlobalVars.color[2], GlobalVars.color[3]);

        leds[j][12 + map(i, 1, 20, 0, 12)].setRGB(GlobalVars.color[1], GlobalVars.color[2], GlobalVars.color[3]);
        leds[j][23 + map(i, 1, 20, 12, 0)].setRGB(GlobalVars.color[1], GlobalVars.color[2], GlobalVars.color[3]);

      }
      if (i > 4) {
        leds[j][map(i, 4, 20, 0, 6)].setRGB(GlobalVars.color[1], GlobalVars.color[2], GlobalVars.color[3]);
        leds[j][6 + map(i, 4, 20, 6, 0)].setRGB(GlobalVars.color[1], GlobalVars.color[2], GlobalVars.color[3]);
      }

      FastLED.show();
      if (i < 10) {
        FastLED.delay(20);
      } else {
        FastLED.delay(5);
      }
      i++;

    }
  }
}
