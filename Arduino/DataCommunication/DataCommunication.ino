#include "CmdMessenger.h"
#include "Thread.h"
#include "ThreadController.h"

const int distThreshold = 300;
const int measurementDelay = 50;
const int regSize = 40;
const int detectRange[2] = {1, 150};

// Location x, Location y, Distance z.
/*
const int numSens = 20;
int sensLoc[numSens][3] = {
	{0, 4, 0}, {0, 7, 0}, {1, 1, 0}, {1, 7, 0}, {2, 5, 0},
	{2, 9, 0}, {3, 2, 0}, {3, 7, 0}, {5, 5, 0}, {5, 9, 0},
	{6, 1, 0}, {6, 7, 0}, {7, 3, 0}, {7, 9, 0}, {8, 6, 0},
	{9, 3, 0}, {10, 1, 0}, {11, 5, 0}, {11, 9, 0}, {12, 2, 0}
};
 */
 
const int numSens = 5;
int sensLoc [numSens][3] = {
  {0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}, {2, 1, 0}
};

int sensZero[numSens];

struct sensor {
  byte trigPin;
  byte echoPin;
} 
sensPins[numSens];

const bool sens4Pin = false;    // if only 3 pin sensors are used set to false, with only 4 pin sensors set to true

int distThreshold = 150;
int measurementDelay = 50;
  bool shiftReg[regSize] = {0};
  byte count = 0;
  byte state = 0;
  int distance;
  sensors[numSens];

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
  int newReg;

  for (byte i = 0; i < numSens; i++) {
    sensors[i].distance = US_dist(sensors[i].trigPin, sensors[i].echoPin);

    if (sensors[i].distance > detectRange[0] && sensors[i].distance < detectRange[1]) {
      newReg = true;
      sensors[i].count++;
    }
    else
      newReg = false;

    if (sensors[i].shiftReg[regSize - 1] == true)
      sensors[i].count--;

    for (byte j = regSize - 1; j > 0; j--) {
      sensors[i].shiftReg[j] = sensors[i].shiftReg[j - 1];
    }
    sensors[i].shiftReg[0] = newReg;

    if (sensors[i].count >= 10 && sensors[i].count < 25)
      sensors[i].state = 12
    else if (sensors[i].count >= 25 && sensors[i].count <= 41)
      sensors[i].state = 2;
    else
      sensors[i].state = 0;
    
    sensLoc[i][2] = sensors[i].state;
  }
}

void setup() {
  Serial.begin(BAUD_RATE);
  attach_callbacks();

  sensorThread -> onRun(sensorCallback);
  sensorThread -> setInterval(measurementDelay);

  threadController.add(sensorThread);

  for (byte i = 0; i < numSens; i++)
    sensZero[i] = 0;

  setupSensors();
}

void loop() {
  c.feedinSerialData(); 
  threadController.run();
}

void setupSensors() {
  for (byte i = 0; i < numSens; i++) {
    if (i < 8) {
      sensors[i].trigPin = 2 * i + 54;
      sensors[i].echoPin = 2 * i + 55;
      pinMode(sensors[i].trigPin, OUTPUT);
      pinMode(sensors[i].echoPin, INPUT);
    } else {
      sensors[i].trigPin = 2 * i + 14;        // Allocating pins 30 to 53
      sensors[i].echoPin = 2 * i + 15;
      pinMode(sensors[i].trigPin, OUTPUT);
      pinMode(sensors[i].echoPin, INPUT);
    }
    if (sens4Pin == false) {
      sensors[i].echoPin = 0;
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
  } 
  else {                              // 4 pin distance sensor
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





