#include "CmdMessenger.h"
#include "Thread.h"
#include "ThreadController.h"

const int numSens = 5;
// Location x, Location y, Distance z.
/*
int sensLoc [numSens][3] = {
  {0, 1, 0}, {0, 2, 0}, {1, 0, 0}, {1, 2, 0},
  {2, 1, 0}, {2, 3, 0}, {3, 0, 0}, {3, 2, 0},
  {4, 1, 0}, {4, 3, 0}, {5, 0, 0}, {5, 2, 0},
  {6, 1, 0}, {6, 3, 0}, {7, 2, 0}, {8, 0, 0},
  {8, 1, 0}, {9, 2, 0}, {9, 3, 0}, {10, 1, 0}
};
*/
int sensLoc [numSens][3] = {
  {0, 0, 0}, {0, 1, 0}, {1, 0, 0},
  {1, 1, 0}, {2, 1, 0}
};

struct sensor{
  byte trigPin;
  byte echoPin;
} sensPins[numSens];

const bool sens4Pin = false;    // if only 3 pin sensors are used set to false, with only 4 pin sensors set to true

int distThreshold = 300;
int measurementDelay = 80;

/* setting up the sensor Thread */
ThreadController threadController = ThreadController();
Thread* sensorThread = new Thread();

//String sensLocString = String(sensLoc);

/* Define available CmdMessenger commands */
enum {
  sensor_amount,
  my_sensor_amount,
  setup_data,
  my_data_is,
  update_data,
  my_value_is,
  error,
};

/* Initialize CmdMessenger -- this should match PyCmdMessenger instance */
const int BAUD_RATE = 9600;
CmdMessenger c = CmdMessenger(Serial, ',', ';', '/');

/* Create callback functions to deal with incoming messages */

/* callback */
void on_sensor_amount(void) {
  c.sendBinCmd(my_sensor_amount, numSens);
}

/* callback */
void on_setup_data(void) {
  for (int i = 0; i < numSens; i++) {
    c.sendCmdStart(my_data_is);
    c.sendCmdArg(sensLoc[i][0]);
    c.sendCmdArg(sensLoc[i][1]);
    c.sendCmdArg(sensLoc[i][2]);
    c.sendCmdEnd();
  }
}

/* callback */
void on_update_data(void) {
  for (int i = 0; i < numSens; i++) {
    c.sendCmdStart(my_value_is);
    c.sendCmdArg(sensLoc[i][0]);
    c.sendCmdArg(sensLoc[i][1]);
    c.sendCmdArg(sensLoc[i][2]);
    c.sendCmdEnd();
  }
}

/* callback */
void on_unknown_command(void) {
  c.sendCmd(error, "Command without callback.");
}

/* Attach callbacks for CmdMessenger commands */
void attach_callbacks(void) {
  c.attach(sensor_amount, on_sensor_amount);
  c.attach(setup_data, on_setup_data);
  c.attach(update_data, on_update_data);
  c.attach(on_unknown_command);
}


void sensorCallback() {
  for (byte i = 0; i < numSens; i++) {
    sensLoc[i][2] = US_dist(sensPins[i].trigPin, sensPins[i].echoPin);    
  }
}

void setup() {
  Serial.begin(BAUD_RATE);
  attach_callbacks();
  
  sensorThread -> onRun(sensorCallback);
  sensorThread -> setInterval(measurementDelay);

  threadController.add(sensorThread);

  setupSensors();
}

void loop() {
  c.feedinSerialData(); 
  threadController.run();
}

void setupSensors() {

  for (byte i = 0; i < numSens; i++) {
    if (i < 8) {                               // Allocating pins A0 to A15
      sensPins[i].trigPin = 2 * i + 54;
      sensPins[i].echoPin = 2 * i + 55;
      pinMode(sensPins[i].trigPin, OUTPUT);
      pinMode(sensPins[i].echoPin, INPUT);
    } else {
      sensPins[i].trigPin = 2 * i + 14;        // Allocating pins 30 to 53
      sensPins[i].echoPin = 2 * i + 15;
      pinMode(sensPins[i].trigPin, OUTPUT);
      pinMode(sensPins[i].echoPin, INPUT);
    }
    if (sens4Pin == false) {
      sensPins[i].echoPin = 0;
    }
  }  
}

int US_dist(int tPin, int ePin) {
  // When using a 3 pin distance sensor set echoPin = 0
  long duration, dist;
  
  if (ePin == 0) {                    //3 pin distance sensor
    pinMode(tPin, OUTPUT);
    digitalWrite(tPin, LOW);          // Check that the output is low
    delayMicroseconds(2);
    digitalWrite(tPin, HIGH);         // Begin pulse
    delayMicroseconds(5);
    digitalWrite(tPin, LOW);          // End pulse
    pinMode(tPin, INPUT);
    duration = pulseIn(tPin, HIGH);   // Wait for a pulse to return
  } else {                              // 4 pin distance sensor
    digitalWrite(tPin, LOW);
    delayMicroseconds(2);
    digitalWrite(tPin, HIGH);         // Begin pulse
    delayMicroseconds(10);
    digitalWrite(tPin, LOW);          // End pulse
    duration = pulseIn(ePin, HIGH);   // Wait for a pulse to return
  }
  dist = duration / 2 / 29;     // Calculate the disance in cm
  
  if (dist > distThreshold) {
    dist = 0;
  }
    
  return dist;
}

