#include "CmdMessenger.h"
#include "Thread.h"
#include "ThreadController.h"
#include <FastLED.h>


const int numSens = 5;
int location = 0;
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
