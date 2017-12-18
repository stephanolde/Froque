/* -----------------------------------------------------------------------------
 * Example .ino file for arduino, compiled with CmdMessenger.h and
 * CmdMessenger.cpp in the sketch directory. 
 *----------------------------------------------------------------------------*/

#include "CmdMessenger.h"

const int numSens = 20;
// Location x, Location y, Distance z.
int sensLoc [numSens][3] = {
	{0, 1, 0}, {0, 2, 0}, {1, 0, 0}, {1, 2, 0},
	{2, 1, 0}, {2, 3, 0}, {3, 0, 0}, {3, 2, 0},
	{4, 1, 0}, {4, 3, 0}, {5, 0, 0}, {5, 2, 0},
	{6, 1, 0}, {6, 3, 0}, {7, 2, 0}, {8, 0, 0},
	{8, 1, 0}, {9, 2, 0}, {9, 3, 0}, {10, 1, 0}
};

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
CmdMessenger c = CmdMessenger(Serial,',',';','/');

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
void update_data(void) {
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
    c.sendCmd(error,"Command without callback.");
}

/* Attach callbacks for CmdMessenger commands */
void attach_callbacks(void) {
    c.attach(sensor_amount, on_sensor_amount);
    c.attach(setup_data, on_setup_data);
    c.attach(update_data, my_value_is);
    c.attach(on_unknown_command);
}

void setup() {
    Serial.begin(BAUD_RATE);
    attach_callbacks();    
}

void loop() {
    c.feedinSerialData();
}
