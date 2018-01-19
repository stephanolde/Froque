import PyCmdMessenger
import time
import pprint
import threading
#import pyaudio
import wave
import sys

# Arduino containing all the input sensors.
arduino = PyCmdMessenger.ArduinoBoard("/dev/ttyACM2", baud_rate=9600)
# Arduino containing all the output.
arduino2 = PyCmdMessenger.ArduinoBoard("/dev/ttyACM1", baud_rate=19200)

# List of commands and their associated argument formats. These must be in the
# same order as in the sketch.
commands = [["sensor_amount", ""],
    ["my_sensor_amount", "i"],
    ["setup_data", ""],
    ["my_data_is", "sss"],
    ["update_data", ""],
    ["my_value_is", "sss"],
    ["error", "s"]]
			
commands2 = [["my_sensor_amount", "i"],
    ["build_to_arduino", "iii"],
    ["data_to_arduino", "iii"],
    ["my_data_is", "ssss"],
    ["error", "s"]]

my_dictionary = {}

# Initialize the messenger
c = PyCmdMessenger.CmdMessenger(arduino, commands)
c2 = PyCmdMessenger.CmdMessenger(arduino2, commands2)

# Ask how many sensors there are from Arduino
c.send("sensor_amount")
string, sensorAmountt, timeStamp = c.receive()
sensorAmount = int(sensorAmountt[0])

# Send the amount of sensors used to Arduino2
#c2.send("my_sensor_amount", sensorAmount)

#Initial setup with the data given from the arduino
#Puts all the data into a dictionary (HashMap) with the keys (x, y) and value z
c.send("setup_data")
for i in range(0, sensorAmount):
    string, data, timeStamp = c.receive()
    xt, yt, zt = data
    x = int(xt)
    y = int(yt)
    z = int(zt)
    #idle = int(idlet)
    #c2.send("build_to_arduino", x, y, z)
    
    my_dictionary[x, y] = [z]

#data1 = threading.Thread(name = 'DataSender', target = DataSender)
#data2 = threading.Thread(name = 'DataAudio', target = DataAudio)

# Continuously asks for data from the Arduino
# And updates the dictionary accordingly
#class DataSender():
while True:
    c.send("update_data")
    for i in range(0, sensorAmount):
        string, data, timeStamp = c.receive()
        xt, yt, zt = data
        x = int(xt)
        y = int(yt)
        z = int(zt)
        #idle = int(idlet)
        #print(idlet)
        #if (idle > 0):
            #print("HELLO")
        # Send the data of the sensors to Arduino2
        c2.send("data_to_arduino", x, y, z)
        string, data, timeStamp = c2.receive()
        print(data)
        my_dictionary[x, y] = [z]
##    pprint.pprint(my_dictionary)
##    print("=======================================================================")
    #time.sleep(0.1)

#class DataAudio():
#	while True:
#		print("Audio")
