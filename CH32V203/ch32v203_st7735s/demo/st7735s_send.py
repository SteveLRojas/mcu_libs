import serial 
import time
import io
import sys

reverse_order = 1
invert_colors = 1

def main():
	if(len(sys.argv) < 3):
		print("Must provide input file name and port name.")
		exit()
	baudrate = 115200
	ser = serial.Serial()
	ser.baudrate = baudrate
	ser.port = sys.argv[2]
	ser.dsrdtr = False
	ser.dtr = False
	ser.timeout = 10
	ser.open()

	time.sleep(0.5)
	ser.reset_input_buffer()

	file = open(sys.argv[1], "rb")
	data = file.read()
	file.close()

	if reverse_order:
		reversed_data = []
		for d in range(len(data) // 3):
			reversed_data.append(data[d * 3 + 2])
			reversed_data.append(data[d * 3 + 1])
			reversed_data.append(data[d * 3])
		data = bytearray(reversed_data)

	# send commands
	if invert_colors:
		commands = bytearray("3031", "utf-8")
	else:
		commands = bytearray("31", "utf-8")
	ser.write(commands)
	# send data
	ser.write(data)

	ser.close()


if __name__ == "__main__":
    # execute only if run as a script
    main()
