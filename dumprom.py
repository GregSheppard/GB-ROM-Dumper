import serial
import string

buffer = ''
flag = 0 # flag = 1 when ROM is being transferred
byte = 0
bank = 0

ser = serial.Serial('COM6', 250000) #open serial
ser.flush()

#print out ROM header info
title = str(ser.readline()).split('%')[1]
banks = str(ser.readline()).split('%')[1]

print("Cart Title: " + title)
print("Number of banks: " + banks)
f = open("ROMs/"+title+".gb", 'wb') #open file

while True:
    buffer = ser.readline()
    
    #check if ROM info is being transferred
    if b'START' in buffer:
        flag = 1
        continue
    if b'END' in buffer:
        break
    if not buffer:
        break
    
    #if ROM is being transferred dump to file
    if flag:
        test = chr(int(buffer))
        f.write(test.encode('latin_1'))
        byte += 1

    #Print progress
    if byte == 16383:
        byte = 0
        print("Bank " + str(bank) + "/" + str(banks) + " dumped.")
        bank+=1

#cleanup
print("Done.")
ser.close()
f.close()
        
    

    
