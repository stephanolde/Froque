#include "CmdMessenger.h"
#include "Thread.h"
#include "ThreadController.h"

const int numSens = 1;

/* Define available CmdMessenger commands */
enum {
  sensor_amount,
  data_to_arduino,
  test,
  error,
};

/* Initialize CmdMessenger -- this should match PyCmdMessenger instance */
const int BAUD_RATE = 19200;
CmdMessenger c = CmdMessenger(Serial, ',', ';', '/');

/* callback */
void on_sensor_amount(void) {
  numSens = c.readBinArg<int>();
  c.sendBinCmd(test, numSens);
}

/* callback */
void on_data_to_arduino(void) {
	int x = c.readBinArg<int>();
	int y = c.readBinArg<int>();
	int z = c.readBinArg<int>();
}

/* callback */
void on_unknown_command(void) {
  c.sendCmd(error, "Command without callback.");
}

/* Attach callbacks for CmdMessenger commands */
void attach_callbacks(void) {
  c.attach(sensor_amount, on_sensor_amount);
  c.attach(data_to_arduino, on_data_to_arduino);
  c.attach(on_unknown_command);
}

void setup() {
  Serial.begin(BAUD_RATE);
}

void loop() {
	
}