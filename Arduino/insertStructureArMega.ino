/*
 * This code containts a structure for the inserts 
 * Set the number of inserts and give each insert a type (sound, spell, light or wind)
 * The function setupInserts will fill the insert structures and alocate the pins each insert uses
 * Build for Arduino Due
 */

enum insertType{    // enum to make inserting the types of inserts more userfrendly
  sound, 
  smell, 
  light, 
  wind,
};

const int numSens = 20;
const int numInsPWM = 12;
const int numInsNoPWM = 8;
const enum insertType list[numInsPWM + numInsNoPWM] = {sound, smell, light, wind, sound, smell, light, wind, sound, smell};

struct sensor{
  byte trigPin;
  byte echoPin;
} sensors[numSens];

struct insert{        // Structure for the inserts. Each structure contains the pin a insert uses and the type of insert.
  byte outPin;
  enum insertType type;
} inserts[numInsPWM + numInsNoPWM];

void setupSensors(){

  for (byte i = 0; i < numSens; i++){
    if (i < 8){
      sensors[i].trigPin = 2 * i + 54;
      sensors[i].echoPin = 2 * i + 55;
      pinMode(sensors[i].trigPin, OUTPUT);
      pinMode(sensorss[i].echoPin, INPUT);
    }
    else{
      sensors[i].trigPin = 2 * i + 14;
      sensors[i].echoPin = 2 * i + 15;
      pinMode(sensors[i].trigPin, OUTPUT);
      pinMode(sensorss[i].echoPin, INPUT);
    }
  }  
}

void setupInserts(){  
  
  for (byte i = 0; i <numInsPWM; i++){
    inserts[i].outPin = 2 + i;
    inserts[i].type = list[i];
    pinMode(inserts[i].outPin, OUTPUT);
  }
  for( i = numInsPWM; i < numInsNoPWM; i++){
    inserts[i].outPin = 22 + i - numInsPWM;
    inserts[i].type = list[i];
    pinMode(inserts[i].outPin, OUTPUT);
  }
}


void setup() {
  // put your setup code here, to run once:

  setupSensors();
  setupInserts();
}

void loop() {
  // put your main code here, to run repeatedly:

}
