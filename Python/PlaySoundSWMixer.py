import swmixer
import time
import serial

arduino = serial.Serial('COM7', 19200)
swmixer.init(samplerate=44100, chunksize=1024, stereo=True)
swmixer.start()

sndMain = swmixer.Sound("Sounds/RelaxedBackgroundShorter.wav")
snd1 = swmixer.Sound("Sounds/Owls.wav")
snd2 = swmixer.Sound("Sounds/Crickets.wav")
snd3 = swmixer.Sound("test.wav")

sndArray = [snd1, snd2, snd3]
canPlay = 0
valueArray = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
count = 0

# sndMain.play(loops=-1, volume=0.0)
while True:
    totalValue = 0
    for i in range(0, 15):
        value = arduino.readline()[:-2]
        if value:
            print value
            valueArray[i] = int(value)
            totalValue += int(value)
    # print totalValue
    if 2 in valueArray:
        if canPlay == 0:
            canPlay = 1
            count = 0
            sndMain.play(loops=-1, volume=0.3)

    # Sound is allowed, run the scripts from here
    if canPlay == 1:
        if totalValue > 2:
            snd2.play(volume=0.4)
            time.sleep(snd2.get_length())
            snd1.play(volume=0.3)
            time.sleep(snd1.get_length())
        elif totalValue > 0:
            snd1.play(volume=0.7)
            time.sleep(snd1.get_length())
        time.sleep(1)
        print "LOL"
        snd2.play(volume=0.7)
        time.sleep(5)

    # Sound is not allowed
    else:
        if 2 not in valueArray:
            count += 1
            if count > 5:
                canPlay = 0
                count = 0
        time.sleep(3)
