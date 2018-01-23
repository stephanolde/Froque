// ---------------------------------------------------------------------------
// Polysense Sensor Arduino Sketch.
// Using 15 HC-SR04 distance sensors to determine movement in front of the
// Polysense installation
// ---------------------------------------------------------------------------

#include <NewPing.h>

byte num_Sensors = 15; // Amount of used sensors in the installation

struct Sensor { // Variables which differ for each individual sensor and change throughout the loops.
  int echoTime; // Time between pulse and the echo
  int distance; // Distance of the object in front of the sensor, for easy usability
  int lastSeen; // The internal clocktime at which the sensor registered something the last time
} Sensors[15];  //


int medianMeasuraments = 3; // Amount of measurements per sensor, to remove outliers.
int minDistance = 5;        // Minimum distance threshold for the sensor to react.
int maxDistance = 60;       // Maximum distance threshold for the sensor to react.
int pushTime = 2000;
int sensorDelay = 29;

#define max_Distance 150     // Maximum distance the sensor can detect within the NewPing library.

NewPing sonar[15] {                    // Declaring the sensors.
  NewPing(A0,   A1,  max_Distance),    // syntax is:
  NewPing(A2,   A3,  max_Distance),    // (trigger pin, echo pin, and the maximum waiting distance)
  NewPing(A4,   A5,  max_Distance),
  NewPing(A6,   A7,  max_Distance),
  NewPing(A8,   A9,  max_Distance),
  NewPing(A10,  A11, max_Distance),
  NewPing(A12,  A13, max_Distance),
  NewPing(A14,  A15, max_Distance),
  NewPing(30,   31,  max_Distance),
  NewPing(32,   33,  max_Distance),
  NewPing(34,   35,  max_Distance),
  NewPing(36,   37,  max_Distance),
  NewPing(38,   39,  max_Distance),
  NewPing(40,   41,  max_Distance),
  NewPing(42,   43,  max_Distance)
};



void setup() {

  Serial.begin(19200);

  for (int i = 0; i < 15; i++) { // Declare the communication pins for each sensor.
    pinMode (i+2, OUTPUT);         // To remove outliers on the other Arduino we have used the PIN_PULLUP function
    digitalWrite(i+2, HIGH);       // This filters out some noise and will only give a signal when there is a connection with the ground
                                 // Resulting in having these pins on high from the setup onwards.
  }
}

void loop() {

  measure(); // The function which measures the distance for each sensor and sends a signal to the other Arduino when neccesary.

}

void measure() {
  
  for (int i = 0; i < num_Sensors; i++) {            // Loop through sensor 1 to 15.
    delay(sensorDelay);                             // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.

    Sensors[i].echoTime = sonar[i].ping_median(medianMeasuraments);   // Take the average echotime of three measurements per sensor.
    Sensors[i].distance = sonar[i].convert_cm(Sensors[i].echoTime);   // Convert the echotime to an usable distance.

    if (Sensors[i].distance >= minDistance && Sensors[i].distance <= maxDistance) { // Filter values outside of the predetermined range.
      Sensors[i].lastSeen = millis();                                               // Record the internal clocktime a sensor got a valid echo.
      digitalWrite(i+2, LOW);                                                       // Send a signal to the other Arduino.
      Serial.println(i +';'+ 1);                                                // Sends a sensor index and a state to the PI
    }

    else {
      if (millis() - Sensors[i].lastSeen >= pushTime) {                             // Remove the active signal when it has been pushed for a predetermined time.
        digitalWrite(i+2, HIGH);                                                    // Gives the other Arduino the time to loop and register the value.
        Serial.println(i +';'+ 0);                                               // Sends a sensor index and a state to the PI
      }
    }
  }
}
