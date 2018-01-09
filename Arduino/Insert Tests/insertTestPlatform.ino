#include <Thread.h>
#include <ThreadController.h>

ThreadController controll = ThreadController();          // Defining thread and thread controller
Thread* sensorThread = new Thread();

const int numSens = 5;                                                              // Number of ultrasonic distance sensors attached to the arduino
const int sensPins[numSens][2] = {{2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}};          // Pin pairs used by each sensor, {triggerPin, echoPin}. For 3 pin sensors use echoPin = 0
const int sensLoc[numSens][2] = {{0, 1}, {1, 1}, {2, 1}, {0, 0}, {1, 0}};           // The locations of the sensors when placed on a raster

/*
   sensor raster
   displays sensorindex values

    0   1   2
    3   4   -1

*/

int distances[numSens];
const int distThresh = 300;                   // The threshold above which the ultrasonic sensors return no sightings

const int numRanges = 2;                      // It is posible to set multiple ranges to differentiate between objects between distances
const int ranges[numRanges] = {190, 100};
int sensStates[numSens];


//Thread callbacks
void sensCallback() {                             // Thread to read the ultrasonic sensors

  for (byte i = 0; i < numSens; i++) {
    distances[i] = US_dist(sensPins[i][0], sensPins[i][1]);
    
    for (byte j = 0; j < numRanges; j++) {
      if (distances[i] < ranges[j])
        sensStates[i] = numRanges - j;            // sensStates is an array that stores for each sensor in which range an object is detected
    }
    
    if (distances[i] < 0 || distances[i] >= ranges[0])
      sensStates[i] = 0;                          // If no object is detected sensStates will have the value 0 for that sensor

  }

  //sendDist();      //These functions send the distances and the states of the sensors over the serial monitor
  //sendSens();

}

void setup() {

  Serial.begin(9600);

  sensorThread->onRun(sensCallback);    // Setting up thread
  sensorThread->setInterval(80);

  controll.add(sensorThread);           // Setting up thread controller

}

void loop() {

  controll.run();         // Runs the sensors

  insert();

}

int US_dist(int tPin, int ePin) {      // function to read the distance from a ultrasonic distance sensor

  // When using a 3 pin distance sensor set echoPin = 0
  long duration, dist;

  if (ePin == 0) {                    //3 pin distance sensor
    pinMode(tPin, OUTPUT);

    digitalWrite(tPin, LOW);          // Sending a pulse to activate the sensor
    delayMicroseconds(2);
    digitalWrite(tPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(tPin, LOW);

    pinMode(tPin, INPUT);
    duration = pulseIn(tPin, HIGH);   // Waiting for the pulse to return to the sensor
  }
  else {                              // 4 pin distance sensor
    digitalWrite(tPin, LOW);          // Sending a pulse to activate the sensor
    delayMicroseconds(2);
    digitalWrite(tPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(tPin, LOW);

    duration = pulseIn(ePin, HIGH);   // Waiting for the pulse to return to the sensor
  }

  dist = duration / 29 / 2;           // Calculating the distance in cm

  if (dist > distThresh)              // If the measured distance is greater than the threashold, the function will return a -1 to indicate that nothing was detected in range.
    return -1;

  return dist;
}

void sendDist() {         // This function prints the distances measured by the sensors

  for (byte i = 0; i < numSens; i++) {
    Serial.print(distances[i]);
    Serial.print("\t");
  }
  
  Serial.print("\n");

}

void sendStates() {         // This function prints the states of the sensors based on the determined ranges

  for (byte i = 0; i < numSens; i++) {
    Serial.print(sensStates[i]);
    Serial.print("\t");
  }
  
  Serial.print("\n");

}

void insert() {           // Use this function to test inserts


}

int indexSensLoc(int x, int y) {      // This function returns the index of a sensor based on the coördinates on the input.

  for (byte index = 0; index < numSens; index++) {
    if (sensLoc[index][0] == x && sensLoc[index][1] == y)
      return index;
  }
  
  return -1;        // If the coördinates do not match with the known coördinates of the sensors -1 will be returned

}

void getNeighbourStatus(int *states, int i) {     //This function collects the states of the sensors neighbouring sensor i

  int l = 1;
  int neighbourIndex;

  states[0] = 0;

  for (int j = -1; j <= 1; j++) {
    for (int k = -1; k <= 1; k++) {
      if (j == 0 && k == 0)           //The function checks all locations in the raster around the sensor i
        continue;

      states[l] = 0;

      neighbourIndex = indexSensLoc(sensLoc[i][0] + j, sensLoc[i][1] + k);  // Checks wheater a location contains another sensor

      if (neighbourIndex != -1 && sensStates[neighbourIndex] != 0) {        // If the checked location contains a sensor and that sensor sees something, the state is loaded into the states array
        states[l] = sensStates[neighbourIndex];
        states[0] = 1;                                                      // If any of the sensors surrounding sensor i sees something states[0] becomes 1
      }

      l++;
    }
  }
}
