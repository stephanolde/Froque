import pyaudio
import wave
import sys

chunk = 1024

if len(sys.argv) < 2:
    print "Play audio %s filename.wav" % sys.argv[0]
    sys.exit(-1)
    
wf = wave.open(sys.argv[1], 'rb')

p = pyaudio.PyAudio()

stream = p.open(format = p.get_format_from_width(wf.getsampwidth()),
                channels = 8,
                rate = wf.getframerate(),
                output = True,
                frames_per_buffer = 44100)

data = wf.readframes(chunk)

while data != '':
    stream.write(data)
    data = wf.readframes(chunk)

stream.stop_stream()
stream.close()

p.terminate()