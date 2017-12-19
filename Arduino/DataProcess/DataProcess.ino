#include "CmdMessenger.h"
#include "Thread.h"
#include "ThreadController.h"

int numSens = 0;

/* Define available CmdMessenger commands */
enum {
  my_sensor_amount,
  build_to_arduino,
  data_to_arduino,
  error,
};

/* Initialize CmdMessenger -- this should match PyCmdMessenger instance */
const int BAUD_RATE = 19200;
CmdMessenger c = CmdMessenger(Serial, ',', ';', '/');

/* callback */
void on_sensor_amount(void) {
  numSens = c.readBinArg<int>();
}

/* callback */
void on_build_to_arduino(void) {
  int x = c.readBinArg<int>();
  int y = c.readBinArg<int>();
  int z = c.readBinArg<int>();
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
  c.attach(my_sensor_amount, on_sensor_amount);
  c.attach(build_to_arduino, on_build_to_arduino);
  c.attach(data_to_arduino, on_data_to_arduino);
  c.attach(on_unknown_command);
}

void setup() {
  Serial.begin(BAUD_RATE);
  attach_callbacks();
}

void loop() {
  c.feedinSerialData();
}

