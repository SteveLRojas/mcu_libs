import serial
import serial.tools.list_ports
import time
import cdc_test_device_regs as regs

baud = 125000
ser = serial.Serial()

def write_reg(address, value):
	address = address | 0x80
	bytes = address.to_bytes(1, 'big') + value.to_bytes(1, 'big')
	ser.write(bytes)

def read_reg(address):
	bytes = address.to_bytes(1, 'big')
	ser.write(bytes)
	response = ser.read(1)
	return int.from_bytes(response, 'big')

def get_device_id():
	response = read_reg(regs.R_DEVICE_ID_HH)
	response = (response << 8) | read_reg(regs.R_DEVICE_ID_HL) 
	response = (response << 8) | read_reg(regs.R_DEVICE_ID_LH)
	response = (response << 8) | read_reg(regs.R_DEVICE_ID_LL)
	return response

def get_unique_id():
	response = read_reg(regs.R_UNIQUE_ID_HH)
	response = (response << 8) | read_reg(regs.R_UNIQUE_ID_HL)
	response = (response << 8) | read_reg(regs.R_UNIQUE_ID_LH)
	response = (response << 8) | read_reg(regs.R_UNIQUE_ID_LL)
	return response

def init(unique_id = 0):
	display_ports = []
	ports = serial.tools.list_ports.comports()
	for port in ports:
		if hasattr(port, 'vid') and port.vid and ((port.vid == 0x1A86) or (port.vid == 0x10C4)):
			display_ports.append(port.device)
	
	ser.baudrate = baud
	ser.dsrdtr = False
	ser.dtr = False
	ser.timeout = 1.0
	ser.write_timeout = 1.0

	for port in display_ports:
		ser.port = port
		ser.open()
		time.sleep(0.5)
		ser.reset_input_buffer()

		device_id = get_device_id()
		if device_id != 0x31434443:
			ser.close()
			continue
		current_unique_id = get_unique_id()
		if unique_id and (unique_id != current_unique_id):
			ser.close()
			continue
		write_reg(regs.R_CDC_IDX, 0)
		write_reg(regs.R_LINE_CODING_IDX, 0)
		write_reg(regs.R_CONFIG_IDX, 0)
		write_reg(regs.R_PACKET_IDX, 0)
		write_reg(regs.R_EP_SEL, 0)
		return current_unique_id
	return 0

def stop():
	if ser.is_open:
		ser.close()

def read_cdc_buf(start_addr, num_bytes):
	bytes = bytearray()
	write_reg(regs.R_CDC_IDX, start_addr)
	for byte in range(num_bytes):
		bytes.append(regs.R_CDC_DATA)
	ser.write(bytes)
	return ser.read(num_bytes)
