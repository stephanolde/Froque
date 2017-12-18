import PyCmdMessenger
import time
import pprint

# Initialize an ArduinoBoard instance.
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
my_dictionary = {}
c = PyCmdMessenger.CmdMessenger(arduino, commands)

c.send("sensor_amount")
string, sensorAmount, timeStamp = c.receive()

#Initial setup with the data given from the arduino
#Puts all the data into a dictionary (HashMap) with the keys (x, y) and value z
c.send("setup_data")
for i in range(0, sensorAmount[0]):
    msg = c.receive()
    string, data, timeStamp = msg
    x, y, z = data
    my_dictionary[x, y] = z    

# Continuously asks for data from the Arduino
# And update the dictionary accordingly
while True:
    c.send("update_data")
    for i in range(0, sensorAmount[0]):
        msg = c.receive()
        string, data, timeStamp = msg
        x, y, z = data
        my_dictionary[x, y] = z
	
    pprint.pprint(my_dictionary)
    print("=======================================================================")
    time.sleep(2)
