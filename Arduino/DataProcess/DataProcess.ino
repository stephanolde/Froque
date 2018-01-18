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

struct insert {
  byte type = 0;
  byte loc[2] = {0, 0};

  byte pin = 0;
  byte lightIndex = 0;

  byte relState = 0;
  long relInterval = 0;
  long relTime = 0;
} inserts[numLight + numWind];

CRGB leds[numLight][numLedsInsert];

#define numSens 16
byte sensLoc[numSens][3] = {0};
byte stateMap[13][10] = {0};
byte location = 0;

int idle = 0;

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
  if (location == numSens) {
    location = 0;
  }
  sensLoc[location][0] = c.readBinArg<int>();
  sensLoc[location][1] = c.readBinArg<int>();
  sensLoc[location][2] = c.readBinArg<int>();
  // idle = c.readBinArg<bool>();
  location++;
}

/* callback */
void on_data_to_arduino(void) {
  if (location == numSens) {
    location = 0;
    for (byte i = 0; i < 13; i++) {
      for (byte j = 0; j < 10; j++) {
        stateMap[i][j] = {0};
      }
    }
  }

  sensLoc[location][0] = c.readBinArg<int>();
  sensLoc[location][1] = c.readBinArg<int>();
  sensLoc[location][2] = c.readBinArg<int>();
  //idle = c.readBinArg<bool>();

  for (byte i = -1; i <= 1; i++) {
    for (byte j = -1; j <= 1; j++) {
      if (sensLoc[location][0] + i < 0 || sensLoc[location][0] + i > 12 || sensLoc[location][1] + j < 0 || sensLoc[location][1] + j > 9) {
        continue;
      }
      if (stateMap[sensLoc[location][0] + i][sensLoc[location][1] + j] < sensLoc[location][2]) {
        stateMap[sensLoc[location][0] + i][sensLoc[location][1] + j] = sensLoc[location][2];
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

  pinMode(13, OUTPUT);
  pinMode(A0, INPUT);
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
        inserts[lightCount+windCount].type = light;
        inserts[lightCount+windCount].loc[0] = insLoc[i][0];
        inserts[lightCount+windCount].loc[1] = insLoc[i][1];
        inserts[lightCount+windCount].pin = 22 + 2 * lightCount;
        inserts[lightCount+windCount].lightIndex = lightCount;
        lightCount ++;
        break;

      case wind :
        inserts[lightCount+windCount].type = wind;
        inserts[lightCount+windCount].loc[0] = insLoc[i][0];
        inserts[lightCount+windCount].loc[1] = insLoc[i][1];
        inserts[lightCount+windCount].pin = 6 + windCount;
        pinMode(inserts[lightCount+windCount].pin, OUTPUT);
        windCount ++;
        break;
    }
  }
}

// Called in Setup
// 15 Led inserts
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

// Called in Main loop
void runInserts() {

  idle = digitalRead(A0);

  for (int i = 0; i < numLight + numWind; i++) {
    switch (inserts[i].type) {
      case light :
        lightInsert(i);
        break;
      case wind :
        windInsert(i);
        break;
    }
  }

  if (idle == 1) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
  FastLED.show();
}

void lightInsert(int index) {
  // place the light animations in the case statement
  /*
    if (idle == 1) {
    for (int i = 0; i < numLight; i++) {
    for (int x = 0; x < numLedsInsert; x++) {
      leds[i][x] = CRGB::Cyan;
    }
    }
    FastLED.show();
    // idleLights();
    } else {

    for (int i = 0; i < numLight; i++) {
    for (int x = 0; x < numLedsInsert; x++) {
      leds[i][x] = CRGB::Red;
    }
    }
    FastLED.show();
  */
  switch (stateMap[inserts[index].loc[0]][inserts[index].loc[1]]) {
    case 0:
      for (int x = 0; x < numLedsInsert; x++) {
        leds[inserts[index].lightIndex][x] = CRGB::Cyan;
      }
      break;
    case 1:
      for (int x = 0; x < numLedsInsert; x++) {
        leds[inserts[index].lightIndex][x] = CRGB::Red;
      }
      break;
    case 2:
      for (int x = 0; x < numLedsInsert; x++) {
        leds[inserts[index].lightIndex][x] = CRGB::Green;
      }
      break;
  }
  FastLED.show();

}


void colourChange(int index, int R, int G, int B) {
  for (byte i = 0; i < numLedsInsert; i++) {
    leds[index][i] = CRGB(R, G, B);
  }
}

// Simple testable idle light animation
void idleLights() {
  for (int x = 0; x < numLedsInsert; x++) {
    leds[5][x] = CRGB::Purple;
  }
  FastLED.show();

  for (int x = 0; x < numLedsInsert; x++) {
    leds[5][x] = CRGB::Cyan;
  }
  FastLED.show();
}

void windInsert(int index) {
  // Place wind animations in the switch statements
  if (idle == 0) {
    switch (stateMap[inserts[index].loc[0]][inserts[index].loc[1]]) {
      case 0:
        relaisControl(index, 2000, 0);
        break;
      case 1:
        relaisControl(index, 2000, 0.5);

        break;
      case 2:
        relaisControl(index, 2000, 1);
        break;
    }
  } else {
    idleWind(index);
  }
}

// called by windInsert
// Method to control the relais
void relaisControl(int index, long period, float duty) {
  long currentMillis = millis();

  if (currentMillis - inserts[index].relTime >= inserts[index].relInterval) {
    inserts[index].relTime = currentMillis;

    if (inserts[index].relState == 0) {
      inserts[index].relState = 255;
      inserts[index].relInterval = period * duty;
    } else {
      inserts[index].relState = 0;
      inserts[index].relInterval = period * (1 - duty);
    }
    digitalWrite(inserts[index].pin, inserts[index].relState);
  }
}

// Called by windInsert
void idleWind(int index) {
  digitalWrite(inserts[index].pin, 0);
  inserts[index].relState = 0;
}
