#include "CmdMessenger.h"
#include "Thread.h"
#include "ThreadController.h"
#include "FastLED.h"

#define numIns 42     // 42  for polysens
#define numWind 10    // max 10 for polysens
#define numLight 15   // max 15 for polysens
#define numSound 8    // max 8 for polysens
#define numLedsInsert 76


const byte insLoc[numIns][2] = {
  {0, 5}, {0, 7}, {1, 0}, {1, 2}, {1, 5}, {1, 9}, {2, 0},
  {2, 1}, {2, 4}, {2, 7}, {3, 1}, {3, 2}, {3, 3}, {3, 5},
  {3, 6}, {3, 9}, {4, 2}, {4, 3}, {4, 7}, {4, 9}, {5, 0},
  {5, 6}, {6, 4}, {6, 7}, {6, 9}, {7, 2}, {7, 6}, {8, 3},
  {8, 5}, {8, 7}, {8, 9}, {9, 0}, {9, 1}, {9, 2}, {9, 6},
  {9, 9}, {10, 3}, {10, 4}, {10, 9}, {11, 6}, {11, 8}, {12, 0}
};

/*  Inserts roster

    -1   5  -1  15  19  -1  24  -1  30  35  38  -1  -1
    -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  40  -1
     1  -1   9  -1  18  -1  23  -1  29  -1  -1  -1  -1
    -1  -1  -1  14  -1  21  -1  26  -1  34  -1  39  -1
     0   4  -1  13  -1  -1  -1  -1  28  -1  -1  -1  -1
    -1  -1   8  -1  -1  -1  22  -1  -1  -1  37  -1  -1
    -1  -1  -1  12  17  -1  -1  -1  27  -1  36  -1  -1
    -1   3  -1  11  16  -1  -1  25  -1  33  -1  -1  -1
    -1  -1   7  10  -1  -1  -1  -1  -1  32  -1  -1  -1
    -1   2   6  -1  -1  20  -1  -1  -1  31  -1  -1  41
*/

enum {none, light, wind, sound};
const byte insList[numIns] =
{ sound, sound, sound, sound, sound,
  sound, sound, sound, wind, wind,
  wind, wind, wind, wind, wind,
  wind, wind, wind, light, light,
  light, light, light, light, light,
  light, light, light, light, light,
  light, light, light, none, none,
  none, none, none, none, none,
  none, none
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


#define numSens 20
byte sensLoc[numSens][3] = {0};
byte stateMap[13][10] = {0};
byte location = 0;
bool idle = false;

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
  idle = c.readBinArg<bool>();
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
  idle = c.readBinArg<bool>();

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
}

void loop() {
  c.feedinSerialData();

  runInserts();

}

void setupInserts() {

  int lightCount = 0;
  int windCount = 0;

  initFastLED();

  for (int i = 0; i < numIns; i++) {
    switch (insList[i]) {

      case none :
      case sound :
        continue;

      case light :
        inserts[i].type = light;
        inserts[i].loc[0] = insLoc[i][0];
        inserts[i].loc[1] = insLoc[i][1];
        inserts[i].pin = 22 + lightCount;
        inserts[i].lightIndex = lightCount;
        lightCount ++;
        break;

      case wind :
        inserts[i].type = wind;
        inserts[i].loc[0] = insLoc[i][0];
        inserts[i].loc[1] = insLoc[i][1];
        inserts[i].pin = 6 + windCount;
        pinMode(inserts[i].pin, OUTPUT);
        windCount ++;
        break;

    }
  }
}

void initFastLED() {

  FastLED.addLeds<NEOPIXEL, 22>(leds[0], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 23>(leds[1], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 24>(leds[2], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 25>(leds[3], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 26>(leds[4], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 27>(leds[5], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 28>(leds[6], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 29>(leds[7], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 30>(leds[8], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 31>(leds[9], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 32>(leds[10], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 33>(leds[11], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 34>(leds[12], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 35>(leds[13], numLedsInsert);
  FastLED.addLeds<NEOPIXEL, 36>(leds[14], numLedsInsert);

}

void runInserts() {

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

  FastLED.show();

}

void lightInsert(int index) {
  // place the light animations in the case statement
  if (idle == false) {
    switch (stateMap[inserts[index].loc[0]][inserts[index].loc[1]]) {
      case 0:
        colourChange(index, 0, 0, 0);
        break;
      case 1:
        colourChange(index, 150, 200, 0);
        break;
      case 2:
        colourChange(index, 0, 250, 255);
        break;
    }
  }
  else {

    idleLights(index);

  }
}


void colourChange( int index, int R, int G, int B) {

  for (byte i = 0; i < numLedsInsert; i++) {
    leds[index][i] = CRGB(R, G, B);
  }
}

void idleLights(int index) {


}

void windInsert( int index) {
  // Place wind animations in the swich stantements

  if (idle == false) {
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
  }
  else {
    idleWind(index);
  }
}

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

void idleWind(int index){
  digitalWrite(inserts[index].pin, 0);
  inserts[index].relState = 0;
}
