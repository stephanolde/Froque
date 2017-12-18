/* -----------------------------------------------------------------------------
 * Example .ino file for arduino, compiled with CmdMessenger.h and
 * CmdMessenger.cpp in the sketch directory. 
 *----------------------------------------------------------------------------*/

#include "CmdMessenger.h"

int numSens = 20;
int sensLoc [numSens][2] = {{0,1},{0,2},{1,0},{1,2},{2,1},{2,3},{3,0},{3,2},{4,1},{4,3},{5,0},{5,2},{6,1},{6,3},{7,2},{8,0},{8,1},{9,2},{9,3},{10,1}};
String sensLocString = String(sensLoc);

/* Define available CmdMessenger commands */
enum {
    setup_data,
    my_data_is,
    error,
};

/* Initialize CmdMessenger -- this should match PyCmdMessenger instance */
const int BAUD_RATE = 9600;
CmdMessenger c = CmdMessenger(Serial,',',';','/');

/* Create callback functions to deal with incoming messages */

/* callback */
void on_setup_data(void) {
    c.sendCmd(my_data_is, sensLocString);
}

/* callback */
void on_unknown_command(void) {
    c.sendCmd(error,"Command without callback.");
}

/* Attach callbacks for CmdMessenger commands */
void attach_callbacks(void) {  
    c.attach(setup_data, on_setup_data);
    c.attach(on_unknown_command);
}

void setup() {
    Serial.begin(BAUD_RATE);
    attach_callbacks();    
}

void loop() {
    c.feedinSerialData();
}
