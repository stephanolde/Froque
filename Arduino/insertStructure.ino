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

const int numberOfInserts = 12;
const enum insertType list[numberOfInserts] = {sound, smell, light, wind, sound, smell, light, wind, sound, smell, light, wind};


struct insert{        // Structure for the inserts. Each structure contains the pins a insert uses 
  byte trigPin;       //  (trigger and echo for the sensor and out is a PWM pin speccific to the type of insert)
  byte echoPin;       //  and the type of insert.
  byte outPin;
  enum insertType type;
} inserts[numberOfInserts];


void setupInserts(){  
  //This function builds the structure array "inserts" and initates the used ports.
  
  for (byte i = 0; i <numberOfInserts; i++){
    inserts[i].trigPin = 22 + i;
    inserts[i].echoPin = 22 + i;
    inserts[i].outPin = 2 + i;
    inserts[i].type = list[i];
    pinMode(inserts[i].trigPin, OUTPUT);
    pinMode(inserts[i].echoPin, INPUT);
    pinMode(inserts[i].outPin, OUTPUT);
  }
}


void setup() {
  // put your setup code here, to run once:

  setupInserts();
}

void loop() {
  // put your main code here, to run repeatedly:

}
