# ------------------------------------------------------------------------------
# Python program using the library to interface with the arduino sketch above.
# ------------------------------------------------------------------------------

import PyCmdMessenger
import time

# Initialize an ArduinoBoard instance.  This is where you specify baud rate and
# serial timeout.  If you are using a non ATmega328 board, you might also need
# to set the data sizes (bytes for integers, longs, floats, and doubles).  
arduino = PyCmdMessenger.ArduinoBoard("/dev/ttyACM0", baud_rate=9600)

# List of commands and their associated argument formats. These must be in the
# same order as in the sketch.
commands = [["setup_data", ""],
            ["my_data_is", "sss"],
            ["error", "s"]]

# Initialize the messenger
c = PyCmdMessenger.CmdMessenger(arduino, commands)

# Send
# Receive. Should give ["my_data_is",[String(Data)],TIME_RECIEVED]

while True:
	c.send("setup_data")
	
	for i in range(0, 20):
            msg = c.receive()
            print(msg)
	
	print("=================================================")
	time.sleep(5)
	