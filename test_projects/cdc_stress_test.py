from tqdm import tqdm
import random
import serial

baud = 125000
port = "/dev/ttyACM0"
ser = serial.Serial()

num_iterations = 10000
num_packets = 17
packet_len_min = 2
packet_len_max = 64

def main():
	ser.baudrate = baud
	ser.dsrdtr = False
	ser.dtr = False
	ser.timeout = 1.0
	ser.write_timeout = 1.0
	ser.port = port
	ser.open()

	p_bar = tqdm(total = num_iterations)
	for d in range(num_iterations):
		bytes_sent = []
		for i in range(num_packets):
			packet_len = random.randint(packet_len_min, packet_len_max)
			tx_packet = [random.getrandbits(8) for _ in range(packet_len)]
			ser.write(bytearray(tx_packet))
			bytes_sent = bytes_sent + tx_packet
		
		bytes_received = ser.read(len(bytes_sent))
		if list(bytes_received) != bytes_sent:
			print("Received bad data:")
			print(f"Bytes sent: {bytes_sent}")
			print(f"Bytes received: {list(bytes_received)}")
		
		p_bar.update(1)
	p_bar.close()

	print("Test completed!")
	ser.close()

if __name__ == "__main__":
	main()
