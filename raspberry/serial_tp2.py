import serial

ser = serial.Serial('/dev/ttyAMA0',115200)

input = input()

if input == 'GET_T':
        ser.write(b"GET_T\r")
if input == 'GET_P':
        ser.write(b'GET_P')
if input == 'SET_K':
        val = input()
        ser.write(b'SET_K='+val)
if input == 'GET_K':
        ser.write(b'GET_K')
if input == 'GET_A':
        ser.write(b'GET_A')

r = ser.read(50)

