
import serial

ser = serial.Serial()
ser.baudrate = 19200
ser.port = 'COM1'
ser.timeout=1
ser.open()

seq1 = []
seq2 = []
number = 0

while True:
  for c in ser.read():
    if number == 0:
      if chr(c) == ';':
        number = 1
        break
      else:
        seq1.append(chr(c))
        joined_seq1 = ''.join(str(v) for v in seq1)
        
    if number == 1:
      if chr(c) == '\n':
        number = 2
        break
      else:
        seq2.append(chr(c))
        joined_seq2 = ''.join(str(v) for v in seq2)
   
    if number == 2:
      index = int(joined_seq1)
      state = int(joined_seq2)
      seq1 = []
      seq2 = []
      number = 0
      break
      
      
    
        
