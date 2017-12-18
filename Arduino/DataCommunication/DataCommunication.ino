/* -----------------------------------------------------------------------------
 * Example .ino file for arduino, compiled with CmdMessenger.h and
 * CmdMessenger.cpp in the sketch directory. 
 *----------------------------------------------------------------------------*/

#include "CmdMessenger.h"

const int numSens = 20;
// Location x, Location y, Distance z.
int sensLoc [numSens][3] = {
	{0, 1, 0}, {0, 2, 1}, {1, 0, 2}, {1, 2, 3},
	{2, 1, 4}, {2, 3, 5}, {3, 0, 6}, {3, 2, 7},
	{4, 1, 8}, {4, 3, 9}, {5, 0, 10}, {5, 2, 11},
	{6, 1, 12}, {6, 3, 13}, {7, 2, 14}, {8, 0, 15},
	{8, 1, 16}, {9, 2, 17}, {9, 3, 18}, {10, 1, 19}
};

//String sensLocString = String(sensLoc);

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
    for (int i = 0; i < numSens; i++) {
        //c.sendCmd(my_x_is, sensLoc[i][0]);
        //c.sendCmd(my_y_is, sensLoc[i][1]);
        c.sendCmdStart(my_data_is);
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
