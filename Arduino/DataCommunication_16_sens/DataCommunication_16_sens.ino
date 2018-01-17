<<<<<<< HEAD
#include "CmdMessenger.h"
//#include "Thread.h"
//#include "ThreadController.h"

const int distThreshold = 30;
const int measurementDelay = 60;
#define measureTimeOut 40000
const int regSize = 40;
const int detectRange[2] = {1, 150};
long lastSens = 0;

// Location x, Location y, Distance z.

// do not use sensors 0, 10, 18 and 19
const int numSens = 16;
//const byte sensLoc[numSens][2] = {{0, 6}};
/*
const byte sensLoc[numSens][2] = {
  {0, 4}, {0, 6}, {1, 1}, {1, 7}, {2, 4},
  {2, 9}, {3, 2}, {3, 7}, {5, 5}, {5, 9},
  {6, 1}, {6, 7}, {7, 3}, {7, 9}, {8, 6},
  {9, 3}, {10, 1}, {11, 5}, {11, 9}, {12, 2}
};
*/
const byte sensLoc[numSens][2] = {
  {0, 5}, {1, 1}, {1, 7}, {2, 4},
  {2, 9}, {3, 2}, {3, 7}, {5, 5}, {5, 9},
  {6, 7}, {7, 3}, {7, 9}, {8, 6},
  {9, 3}, {10, 1}, {11, 5}
};

/*  Sensor roster

    -1  -1   5  -1  -1   9  -1  13  -1  -1  -1  18  -1
    -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
     1   3  -1   7  -1  -1  11  -1  -1  -1  -1  -1  -1
    -1  -1  -1  -1  -1  -1  -1  -1  14  -1  -1  -1  -1
    -1  -1  -1  -1  -1   8  -1  -1  -1  -1  -1  17  -1
     0  -1   4  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
    -1  -1  -1  -1  -1  -1  -1  12  -1  15  -1  -1  -1
    -1  -1  -1   6  -1  -1  -1  -1  -1  -1  -1  -1  19
    -1   2  -1  -1  -1  -1  10  -1  -1  -1  16  -1  -1
    -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1  -1
*/

const long idleInterval = 30000;
long lastActive;
bool idle = false;

struct sensor {
  byte trigPin = 0;
  byte echoPin = 0;
  int loc[2] = {0, 0};
  bool shiftReg[regSize] = {0};
  byte count = 0;
  byte state = 0;
  int dist;
} sensors[numSens];

const bool sens4Pin = true;    // if only 3 pin sensors are used set to false, with only 4 pin sensors set to true

/* setting up the sensor Thread */
//ThreadController threadController = ThreadController();
//Thread* sensorThread = new Thread();

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
    c.sendCmdArg(sensors[i].loc[0]);
    c.sendCmdArg(sensors[i].loc[1]);
    c.sendCmdArg(sensors[i].state);
    c.sendCmdArg(idle);
    c.sendCmdEnd();
  }
}

/* callback */
void on_update_data(void) {
  for (int i = 0; i < numSens; i++) {
    c.sendCmdStart(my_value_is);
    c.sendCmdArg(sensors[i].loc[0]);
    c.sendCmdArg(sensors[i].loc[1]);
    c.sendCmdArg(sensors[i].state);
    c.sendCmdArg(idle);
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
  int distance;
  bool seen = false;

  

  for (byte i = 0; i < numSens; i++) {

    distance = US_dist(sensors[i].trigPin, sensors[i].echoPin);
    
    if (distance > 1) {
      newReg = true;
      sensors[i].count++;
    }
    else {
      newReg = false;
    }

    if (sensors[i].shiftReg[regSize - 1] == true) {
      sensors[i].count--;
    }


    for (byte j = regSize - 1; j > 0; j--) {
      sensors[i].shiftReg[j] = sensors[i].shiftReg[j - 1];
    }
    sensors[i].shiftReg[0] = newReg;

    if (sensors[i].count >= 1 && sensors[i].count < 5) {
      sensors[i].state = 1;
    }
    else {

      if (sensors[i].count >= 5 && sensors[i].count <= 41) {
        sensors[i].state = 2;
      }
      else {
        sensors[i].state = 0;
      }
    }
    if (sensors[i].state != 0) {
      seen = true;
    }
    sensors[i].dist = distance;
  }

  if (seen == true) {
    lastActive = millis();
    idle = false;
  }
  else {
    if (millis() - lastActive >= idleInterval) {
      idle = false;
    }
  }
/*
  Serial.print("");

  for (byte i = 0; i < numSens; i++) {
    Serial.print(sensors[i].trigPin);
    Serial.print(" ");
    Serial.print(sensors[i].echoPin);
    Serial.print(", (");
    Serial.print(sensors[i].loc[0]);
    Serial.print(", ");
    Serial.print(sensors[i].loc[1]);
    Serial.print("): ");
    Serial.println(sensors[i].state);
  }
*/

}




void setup() {
  Serial.begin(BAUD_RATE);
  attach_callbacks();

  //sensorThread -> onRun(sensorCallback);
  //sensorThread -> setInterval(measurementDelay);

  //threadController.add(sensorThread);

  setupSensors();

}

void loop() {
  c.feedinSerialData();
  //threadController.run();

  if (millis() - lastSens >= measurementDelay){
    lastSens = millis();
    sensorCallback();
  }

}

void setupSensors() {
  byte j = 0;
  for (byte i = 0; i < numSens; i++) {

    if (j == 4 || j == 9 || j == 14 || j == 19)
      j++;
    if (j < 8) {
      sensors[i].trigPin = 2 * j + 54;        // Allocating pins A0 to A15
      sensors[i].echoPin = 2 * j + 55;
      pinMode(sensors[i].trigPin, OUTPUT);
      pinMode(sensors[i].echoPin, INPUT);
    } else {
      sensors[i].trigPin = 2 * j + 14;        // Allocating pins 30 to 53
      sensors[i].echoPin = 2 * j + 15;
      pinMode(sensors[i].trigPin, OUTPUT);
      pinMode(sensors[i].echoPin, INPUT);
    }
    if (sens4Pin == false) {
      sensors[i].echoPin = 0;
    }

    sensors[i].loc[0] = sensLoc[i][0];
    sensors[i].loc[1] = sensLoc[i][1];
    j++;
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
    duration = pulseIn(tPin, HIGH, measureTimeOut);   // Wait for a pulse to return
  }
  else {                              // 4 pin distance sensor
    digitalWrite(tPin, LOW);
    delayMicroseconds(2);
    digitalWrite(tPin, HIGH);         // Begin pulse
    delayMicroseconds(10);
    digitalWrite(tPin, LOW);          // End pulse
    duration = pulseIn(ePin, HIGH, measureTimeOut);   // Wait for a pulse to return
  }
  dist = duration / 2 / 29;     // Calculate the disance in cm

  if (dist > distThreshold) {
    dist = 0;
  }

  return dist;
}
