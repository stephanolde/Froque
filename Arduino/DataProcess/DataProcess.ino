#include "CmdMessenger.h"
#include "Thread.h"
#include "ThreadController.h"
#include "Adafruit_NeoPixel.h"


const int numSens = 5;
int location = 0;
int sensLoc[numSens][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

const int numLeds = 39;
const int ledPin = 8;

Adafruit_NeoPixel leds = Adafruit_NeoPixel(numLeds, ledPin, NEO_GRB + NEO_KHZ800);


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
  int intencity = 0;
  
  if (location == 5) {
    location = 0;
  }
  sensLoc[location][0] = c.readBinArg<int>();
  sensLoc[location][1] = c.readBinArg<int>();
  sensLoc[location][2] = c.readBinArg<int>();
  
  if (sensLoc[location][2] < 35)
    intencity = 0;
  else if (sensLoc[location][2] > 140)
    intencity = 0;
  else
    intencity = map(sensLoc[location][2], 35, 150, 0, 255);
  
  for( int i = location * numLeds/numSens; i < (location + 1)* numLeds/numSens; i++)
    leds.setPixelColor(i, leds.Color(intencity, intencity, intencity));
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
}

void loop() {
  c.feedinSerialData();
}

