#include "CmdMessenger.h"
#include "Thread.h"
#include "ThreadController.h"
#include "FastLED.h"

DEFINE_GRADIENT_PALETTE( bluefly_gp ) {
  0,   0,  0,  0,
  63,   0, 39, 64,
  191, 175, 215, 235,
  255,   0,  0,  0
};

#define numIns 43     // 43  for polysens
#define numWind 8    // max 8 for polysens
#define numLight 15   // max 15 for polysens
#define numSound 8    // max 8 for polysens
#define numLedsInsert 76
#define LED_TYPE WS2812B
#define COLOR_ORDER

const byte insLoc[numIns][2] = {
  {0, 5}, {0, 7}, {1, 0}, {1, 2}, {1, 5}, {1, 7}, {1, 9},
  {2, 0}, {2, 1}, {2, 3}, {2, 7}, {3, 1}, {3, 2}, {3, 4},
  {3, 6}, {3, 9}, {4, 1}, {4, 3}, {4, 7}, {4, 9}, {5, 0},
  {5, 6}, {6, 4}, {6, 7}, {6, 9}, {7, 0}, {7, 2}, {7, 6},
  {8, 3}, {8, 5}, {8, 7}, {8, 9}, {9, 0}, {9, 1}, {9, 2},
  {9, 6}, {9, 9}, {10, 3}, {10, 4}, {10, 9}, {11, 6}, {11, 8},
  {12, 0}
};

/*
    Inserts roster (stateMap)
    -1   6  -1  15  19  -1  24  -1  31  36  39  -1  -1
    -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  41  -1
     1   5  10  -1  18  -1  23  -1  30  -1  -1  -1  -1
    -1  -1  -1  14  -1  21  -1  27  -1  35  -1  40  -1
     0   4  -1  -1  -1  -1  -1  -1  29  -1  -1  -1  -1
    -1  -1  -1  13  -1  -1  22  -1  -1  -1  38  -1  -1
    -1  -1   9  -1  17  -1  -1  -1  28  -1  37  -1  -1
    -1   3  -1  12  -1  -1  -1  26  -1  34  -1  -1  -1
    -1  -1   8  11  16  -1  -1  -1  -1  33  -1  -1  -1
    -1   2   7  -1  -1  20  -1  25  -1  32  -1  -1  42
*/

enum {none, light, wind, sound};

const byte insList[numIns] =
{ none, light, none, sound, wind,
  sound, wind, light, light, none,
  light, wind, none, light, none,
  sound, none, sound, none, light,
  light, wind, light, light, sound,
  none, light, none, none, light,
  none, wind, none, wind, sound,
  sound, light, light, wind, sound,
  light, wind, none
};

struct lightInsert {
  byte loc[2] = {0, 0};

} lightInserts[numLight];

struct windInsert {
  byte loc[2] = {0, 0};
  byte pin = 0;
  int state = 0;
  long interval = 0;
  long startTime = 0;
} windInserts[numWind];

CRGB leds[numLight][numLedsInsert];

#define numSens 16
byte sensLoc[numSens][3] = {0};
int stateMap[13][10] = {0};
int location = 0;

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
  const int numSensTemp = c.readInt16Arg();
  int sensLocTemp [numSensTemp][3] = { };
}

/* callback */
void on_build_to_arduino(void) {
  if (location == numSens) {
    location = 0;
  }
  sensLoc[location][0] = c.readInt16Arg();
  sensLoc[location][1] = c.readInt16Arg();
  sensLoc[location][2] = c.readInt16Arg();
  location++;
}

/* callback */
void on_data_to_arduino(void) {
  if (location == numSens) {
    location = 0;
    //    for (byte i = 0; i < 13; i++) {
    //      for (byte j = 0; j < 10; j++) {
    //        stateMap[i][j] = {0};
    //      }
    //    }
  }

  sensLoc[location][0] = c.readInt16Arg();
  sensLoc[location][1] = c.readInt16Arg();
  sensLoc[location][2] = c.readInt16Arg();

  for (int i = -1; i <= 1; i++) {
    if (sensLoc[location][0] + i < 0 || sensLoc[location][0] + i > 12) {
      continue;
    }
    for (int j = -1; j <= 1; j++) {
      if (sensLoc[location][1] + j < 0 || sensLoc[location][1] + j > 9) {
        continue;
      }
      if (stateMap[sensLoc[location][0] + i][sensLoc[location][1] + j] < sensLoc[location][2]) {
        stateMap[sensLoc[location][0] + i][sensLoc[location][1] + j] = sensLoc[location][2];
        if (stateMap[sensLoc[location][0]][sensLoc[location][1]] > 0) {
          digitalWrite(13, HIGH);
        } else {
          digitalWrite(13, LOW);
        }
      }
    }
  }
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

  setupInserts();
  initFastLED();

}

void loop() {
  c.feedinSerialData();
  runInserts();
}

// Called in Setup
void setupInserts() {
  int lightCount = 0;
  int windCount = 0;

  for (int i = 0; i < numSens; i++) {
    switch (insList[i]) {
      case none :
      case sound :
        break;

      case light :
        lightInserts[lightCount].loc[0] = insLoc[i][0];
        lightInserts[lightCount].loc[1] = insLoc[i][1];
        lightCount ++;
        break;

      case wind :
        windInserts[windCount].loc[0] = insLoc[i][0];
        windInserts[windCount].loc[1] = insLoc[i][1];
        windInserts[windCount].pin = 6 + windCount;
        pinMode(windInserts[windCount].pin, OUTPUT);
        digitalWrite(windInserts[windCount].pin, LOW);
        windCount ++;
        break;
    }
  }
}

// Called in Setup
// 15 Led inserts
void initFastLED() {
  FastLED.addLeds<LED_TYPE, 22, COLOR_ORDER>(leds[0], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 24, COLOR_ORDER>(leds[1], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 26, COLOR_ORDER>(leds[2], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 28, COLOR_ORDER>(leds[3], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 30, COLOR_ORDER>(leds[4], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 32, COLOR_ORDER>(leds[5], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 34, COLOR_ORDER>(leds[6], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 36, COLOR_ORDER>(leds[7], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 38, COLOR_ORDER>(leds[8], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 40, COLOR_ORDER>(leds[9], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 42, COLOR_ORDER>(leds[10], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 44, COLOR_ORDER>(leds[11], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 46, COLOR_ORDER>(leds[12], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 48, COLOR_ORDER>(leds[13], numLedsInsert).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, 50, COLOR_ORDER>(leds[14], numLedsInsert).setCorrection(TypicalLEDStrip);
}

// Called in Main loop
void runInserts() {

  byte state = 0;

  for ( byte i = 0; i < numLight; i++) {
    state = stateMap[lightInserts[i].loc[0]][lightInserts[i].loc[1]];
    switch (state) {
      case 0:
        lightState0(i);
        break;
      case 1:
        lightState1(i);
        break;
      case 2:
        lightState2(i);
        break;
    }
    FastLED.show();
  }

  for ( byte i = 0; i < numWind; i++) {
    state = stateMap[windInserts[i].loc[0]][windInserts[i].loc[1]];
    switch (state) {
      case 0:
        relaisControl(i, 2000, 0);
        break;
      case 1:
        relaisControl(i, 2000, 0.5);
        break;
      case 2:
        relaisControl(i, 2000, 1);
        break;
    }
  }
}

void lightState0(int index) {
  for (int x = 0; x < numLedsInsert; x++) {
    leds[index][x] = CRGB::Cyan;
  }
}

void lightState1(int index) {
  for (int x = 0; x < numLedsInsert; x++) {
    leds[index][x] = CRGB::Green;
  }
}

void lightState2(int index) {
  for (int x = 0; x < numLedsInsert; x++) {
    leds[index][x] = CRGB::Red;
  }
}

// Method to control the relais
void relaisControl(int index, long period, float duty) {
  long currentMillis = millis();
  if (currentMillis - windInserts[index].startTime >= windInserts[index].interval) {
    windInserts[index].startTime = currentMillis;
    if (windInserts[index].state == 0) {
      windInserts[index].state = 255;
      windInserts[index].interval = period * duty;
    } else {
      windInserts[index].state = 0;
      windInserts[index].interval = period * (1 - duty);
    }
    digitalWrite(windInserts[index].pin, windInserts[index].state);
  }
}
