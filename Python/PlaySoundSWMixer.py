import swmixer
import time
import serial

arduino = serial.Serial('COM5', 9600)
swmixer.init(samplerate=44100, chunksize=1024, stereo=True)
swmixer.start()

snd1 = swmixer.Sound("main.wav")
snd2 = swmixer.Sound("test2.wav")
snd3 = swmixer.Sound("test.wav")

snd3.play(loops=-1, volume=0.3)
while True:
    totalValue = 0
    for i in range(0, 15):
        value = arduino.readline()[:-2]
        if value:
            print value
            totalValue += int(value)
    print totalValue
    if totalValue > 2:
        snd2.play(volume=0.4)
        time.sleep(4.0)
        snd1.play(volume=0.3)
        time.sleep(8.0)
    elif totalValue > 0:
        snd1.play(volume=0.7)
        time.sleep(5)
    time.sleep(1)
    print "LOL"
    snd2.play(volume=0.7)
    time.sleep(20)

time.sleep(2.0) #don't quit before we hear the sound!
