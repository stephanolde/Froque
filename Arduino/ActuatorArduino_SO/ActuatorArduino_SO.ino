#include "FastLED.h"
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    76
#define NUM_RINGS   15
#define NUM_WIND    8

CRGB leds[NUM_RINGS + 2][NUM_LEDS]; // +2 so we can precalculate things

const int mapping[15] = {12, 11, 0, 13, 10, 8, 7 , 14, 9, 1, 5, 6, 2, 3, 4};
const int animationMap[15] = {10, 8, 14, 12, 5, 6, 2, 3, 7,  11, 0, 13, 9, 1, 4};
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
  int cycleIncrement = 5; // adjustable value
  int fadeState = 1; // just a starting value between 1 and 4
  int fadeDirections = 1;
  int ringSize[4] = {12, 24, 36, 24}; // added ringsize 4 for fadingcycle ease
  int TimeOut = 1000; // used for downscaling;
  int ActivatedSize = 0;
  int color[3] = {100, 150, 200};
  int maxState;
  long timeSinceWave;
  int windMap[8] = {0, 5, 5, 5, 5, 2, 5, 4};
  int windCorrection[15] = {12, 11, 0, 13, 10, 8, 7, 14, 9, 1, 5, 6, 2, 3, 4};

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
  for (int j = 0; j < 150; j++) {
    for (int i = 0; i < NUM_RINGS; i++) {

      fill_rainbow( leds[i], 12, j + 15, 15 );
      fill_rainbow( leds[i] + 12, 24, j + 7, 9 );
      fill_rainbow( leds[i] + 36, 40, j, 6 );
      if (j < 75) {
        fadeLightBy(leds[i], 76, map(j, 0, 75, 255, 0));
      }
      else {
        fadeLightBy(leds[i], 76, map(j, 75, 150, 0, 255));
      }
    }
    FastLED.show();
  }

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
  }
}



void CycleVariables() {


  if (GlobalVars.brightness >= 245 || GlobalVars.brightness <= 10) {
    GlobalVars.fadeDirections = - GlobalVars.fadeDirections;
  }


  GlobalVars.brightness = GlobalVars.brightness + GlobalVars.fadeDirections * GlobalVars.cycleIncrement;
  if (GlobalVars.brightness < 10) {
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

  /*
    if (millis() - GlobalVars.timeSinceWave >= 60000) {
      WaveOverWall(0,0,0);
      GlobalVars.timeSinceWave = millis();
    }
    else {*/

  if (GlobalVars.ActivatedSize > 7 && GlobalVars.ActivatedSize < 15 && millis() - GlobalVars.timeSinceWave >= 60000) {
    WaveOverWall(0,0,0);
    GlobalVars.timeSinceWave = millis();
    return;
  }

  if (GlobalVars.ActivatedSize == 15) {
    SystemWideAnimation();
  }

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

  //}
}

void animation0(int LEDindex) {
  switch (GlobalVars.fadeState) {
    case 1:
      for (int i = 0; i < 12; i++) {
        leds[LEDindex][i].setHSV( 170, 73, GlobalVars.brightness);
      }
      break;
    case 2:
      for (int i = 12; i < 36; i++) {
        leds[LEDindex][i].setHSV( 170, 73, GlobalVars.brightness);
      }
      break;

    case 3:
      for (int i = 36; i < 76; i++) {
        leds[LEDindex][i].setHSV( 170, 73, GlobalVars.brightness);
      }
      break;
    case 4:
      for (int i = 12; i < 36; i++) {
        leds[LEDindex][i].setHSV( 170, 73, GlobalVars.brightness);
      }
      break;

  }
}

void animation1(int LEDindex) {
  fill_solid(leds[LEDindex], NUM_LEDS, CRGB( 0, 228, 75));

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
    index = GlobalVars.windMap[i];

    switch (LEDRings[index].State) {
      case 1:
        windControl(i, 1000, 0.5);
        break;
      case 2:
        windControl(i, 1000, 1);
        break;
      default:
        windControl(i, 1000, 0.25);
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
    digitalWrite(index + 6, windInserts[index].state);
  }
}

void SystemWideAnimation() {

  // random colored speckles that blink in and fade smoothly
  for (int i = 0; i < 15; i++) {
    fill_solid(leds[animationMap[i]],76,CRGB(random(0,255),random(0,255),random(0,255)));
    
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

void WaveOverWall(int R, int G, int B) {
    
    if (R == 0 && G == 0 && B == 0){
        R = GlobalVars.color[1];
        G = GlobalVars.color[2];
        B = GlobalVars.color[3];
    }
    
    
  FastLED.clear();
  FastLED.show();

  for (int s = 0; s < 15; s++) {
    int j = mapping[s];
    int i = 0;
    while ( i < 19) {
      if (i == 0) {
        leds[j][i].setRGB(R,G,B);
      }

      leds[j][41 + i].setRGB(R,G,B);
      leds[j][75 - i].setRGB(R,G,B);

      if (i > 0) {

        leds[j][12].setRGB(R,G,B);

        int k = map(i, 1, 20, 0, 12);
        leds[j][13 + k].setRGB(R,G,B);
        leds[j][23 - k].setRGB(R,G,B);

      }
      if (i > 4) {
        if ( i == 5) {
          leds[j][0].setRGB(R,G,B);
        }
        int m = map(i, 4, 20, 0, 6);

        leds[j][1 + m].setRGB(R,G,B);
        leds[j][11 - m].setRGB(R,G,B);
      }

      if (i < 5) {
        delay(10);
      } else {
        FastLED.delay(20);
      }
      i++;

    }
  }
}
