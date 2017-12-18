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
commands = [["sensor_amount", ""],
            ["my_sensor_amount", "i"],
            ["setup_data", ""],
            ["my_data_is", "sss"],
            ["update_data", ""],
            ["my_value_is", "sss"],
            ["error", "s"]]

# Initialize the messenger
c = PyCmdMessenger.CmdMessenger(arduino, commands)

c.send("sensor_amount")
string, sensorAmount, timeStamp = c.receive()

#Initial setup with the data given from the arduino
c.send("setup_data")
for i in range(0, sensorAmount[0]):
    msg = c.receive()
    string, data, timeStamp = msg
    x, y, z = data
    print(x, y, z)
    

# Continuously asks for data from the Arduino
while True:
    c.send("update_data")
    for i in range(0, 20):
        msg = c.receive()
        string, data, timeStamp = msg
        x, y, z = data
        #print(x, y, z)
	
    #print("=======================================================================")
    time.sleep(5)
	