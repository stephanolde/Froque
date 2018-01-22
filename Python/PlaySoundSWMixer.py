import swmixer
import time

swmixer.init(samplerate=44100, chunksize=1024, stereo=True)
swmixer.start()

snd1 = swmixer.Sound("main.wav")
snd2 = swmixer.Sound("test2.wav")
snd3 = swmixer.Sound("test.wav")

snd3.play(loops=-1, volume=0.3)
while True:
    print "LOL"
    snd2.play(volume=0.7)
    time.sleep(20)

time.sleep(2.0) #don't quit before we hear the sound!
