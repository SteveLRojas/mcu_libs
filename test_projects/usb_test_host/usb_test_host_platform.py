import serial
import serial.tools.list_ports
import time
import usb_test_host_regs as regs
import usbh_defs as defs

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

def get_state_str():
	state = read_reg(regs.R_STATE)
	if state == 0x00:
		return "S_DISCONNECTED"
	elif state == 0x01:
		return "S_RESET_START"
	elif state == 0x02:
		return "S_RESET_END"
	elif state == 0x03:
		return "S_SET_SPEED"
	elif state == 0x04:
		return "S_CONNECTED"
	elif state == 0x05:
		return "S_GET_DEV_DESCR"
	elif state == 0x06:
		return "S_SET_ADDR"
	elif state == 0x07:
		return "S_CONFIGURE"
	elif state == 0x08:
		return "S_IDLE"
	elif state == 0x09:
		return "S_RUN"
	else:
		return f"Invalid state: {state:02X}"

def get_response_str():
	response = read_reg(regs.R_RESPONSE)
	if response == 0x00:
		return "No response"
	elif response == 0x01:
		return "PID_OUT"
	elif response == 0x02:
		return "PID_ACK"
	elif response == 0x03:
		return "PID_DATA0"
	elif response == 0x05:
		return "PID_SOF"
	elif response == 0x09:
		return "PID_IN"
	elif response == 0x0A:
		return "PID_NAK"
	elif response == 0x0B:
		return "PID_DATA1"
	elif response == 0x0C:
		return "PID_PRE"
	elif response == 0x0D:
		return "PID_SETUP"
	elif response == 0x0E:
		return "PID_STALL"
	else:
		return f"Invalid response: {response:02X}"

def read_rx_buf(start_addr, num_bytes):
	bytes = bytearray()
	write_reg(regs.R_RX_IDX, start_addr)
	for byte in range(num_bytes):
		bytes.append(regs.R_RX_DATA)
	ser.write(bytes)
	return ser.read(num_bytes)

def write_rx_buf(start_addr, data):
	bytes = bytearray()
	write_reg(regs.R_RX_IDX, start_addr)
	for byte in data:
		bytes.append(regs.R_RX_DATA | 0x80)
		bytes.append(byte)
	ser.write(bytes)

def read_tx_buf(start_addr, num_bytes):
	bytes = bytearray()
	write_reg(regs.R_TX_IDX, start_addr)
	for byte in range(num_bytes):
		bytes.append(regs.R_TX_DATA)
	ser.write(bytes)
	return ser.read(num_bytes)

def write_tx_buf(start_addr, data):
	bytes = bytearray()
	write_reg(regs.R_TX_IDX, start_addr)
	for byte in data:
		bytes.append(regs.R_TX_DATA | 0x80)
		bytes.append(byte)
	ser.write(bytes)

def read_transfer_buf(start_addr, num_bytes):
	bytes = bytearray()
	write_reg(regs.R_TRANSFER_IDX, start_addr)
	for byte in range(num_bytes):
		bytes.append(regs.R_TRANSFER_DATA)
	ser.write(bytes)
	return ser.read(num_bytes)

def write_transfer_buf(start_addr, data):
	bytes = bytearray()
	write_reg(regs.R_TRANSFER_IDX, start_addr)
	for byte in data:
		bytes.append(regs.R_TRANSFER_DATA | 0x80)
		bytes.append(byte)
	ser.write(bytes)

def set_max_packet_size(ep, max_size):
	write_reg(regs.R_EP_SEL, ep)
	write_reg(regs.R_EP_IDX, 1)
	write_reg(regs.R_EP_DATA, max_size)

def get_dev_descr():
	write_tx_buf(0, defs.usb_request_dev_descr)
	write_reg(regs.R_CONTROL_TRANSFER, 0)
	time.sleep(0.1)
	if read_reg(regs.R_RESPONSE) != defs.USB_PID_ACK:
		print(f"Got bad response: {get_response_str()}")
	return read_transfer_buf(0, 18)

def get_config_descr():
	write_tx_buf(0, defs.usb_request_config_descr)
	write_reg(regs.R_CONTROL_TRANSFER, 0)
	time.sleep(0.1)
	if read_reg(regs.R_RESPONSE) != defs.USB_PID_ACK:
		print(f"Got bad response: {get_response_str()}")
	return read_transfer_buf(0, 9)

def get_full_config_descr():
	request = defs.usb_request_config_descr
	descr = get_config_descr()
	total_len = int.from_bytes(descr[2:4], byteorder="little")
	request[6] = total_len & 0xFF
	request[7] = total_len >> 8
	write_tx_buf(0, request)
	write_reg(regs.R_CONTROL_TRANSFER, 0)
	time.sleep(0.1)
	if read_reg(regs.R_RESPONSE) != defs.USB_PID_ACK:
		print(f"Got bad response: {get_response_str()}")
	return read_transfer_buf(0, total_len)

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
		try:
			ser.port = port
			ser.open()
			time.sleep(0.5)
			ser.reset_input_buffer()
		except Exception:
			continue

		device_id = get_device_id()
		if device_id != 0x31425355:
			ser.close()
			continue
		disp_unique_id = get_unique_id()
		if unique_id and (unique_id != disp_unique_id):
			ser.close()
			continue
		write_reg(regs.R_RX_IDX, 0)
		write_reg(regs.R_TX_IDX, 0)
		write_reg(regs.R_EP_SEL, 0)
		write_reg(regs.R_EP_IDX, 0)
		write_reg(regs.R_TRANSFER_LEN, 0)
		write_reg(regs.R_RESPONSE, 0)
		write_reg(regs.R_TRANSFER_IDX, 0)
		return disp_unique_id
	return 0

def stop():
	if ser.is_open:
		ser.close()

def print_usb_device_descriptor(desc: bytearray):
	if len(desc) < 18:
		raise ValueError("USB device descriptor must be at least 18 bytes")

	fields = [
		("bLength", 1),
		("bDescriptorType", 1),
		("bcdUSB", 2),
		("bDeviceClass", 1),
		("bDeviceSubClass", 1),
		("bDeviceProtocol", 1),
		("bMaxPacketSize0", 1),
		("idVendor", 2),
		("idProduct", 2),
		("bcdDevice", 2),
		("iManufacturer", 1),
		("iProduct", 1),
		("iSerialNumber", 1),
		("bNumConfigurations", 1),
	]

	offset = 0

	for name, size in fields:
		value_bytes = desc[offset:offset + size]

		if size == 1:
			value = value_bytes[0]
			print(f"{name}: {value} (0x{value:02X})")
		else:
			# USB descriptors use little-endian encoding
			value = int.from_bytes(value_bytes, byteorder="little")
			print(f"{name}: {value} (0x{value:04X})")

		offset += size

def print_usb_configuration_descriptor(desc: bytearray):
	if len(desc) < 9:
		raise ValueError("USB configuration descriptor must be at least 9 bytes")

	fields = [
		("bLength", 1),
		("bDescriptorType", 1),
		("wTotalLength", 2),
		("bNumInterfaces", 1),
		("bConfigurationValue", 1),
		("iConfiguration", 1),
		("bmAttributes", 1),
		("bMaxPower", 1),
	]

	offset = 0

	for name, size in fields:
		value_bytes = desc[offset:offset + size]

		if size == 1:
			value = value_bytes[0]
			print(f"{name}: {value} (0x{value:02X})")
		else:
			# USB descriptors use little-endian encoding
			value = int.from_bytes(value_bytes, byteorder="little")
			print(f"{name}: {value} (0x{value:04X})")

		offset += size

def print_usb_interface_descriptor(desc: bytearray):
	if len(desc) < 9:
		raise ValueError("USB interface descriptor must be at least 9 bytes")

	fields = [
		("bLength", 1),
		("bDescriptorType", 1),
		("bInterfaceNumber", 1),
		("bAlternateSetting", 1),
		("bNumEndpoints", 1),
		("bInterfaceClass", 1),
		("bInterfaceSubClass", 1),
		("bInterfaceProtocol", 1),
		("iInterface", 1),
	]

	offset = 0

	for name, size in fields:
		value_bytes = desc[offset:offset + size]

		if size == 1:
			value = value_bytes[0]
			print(f"{name}: {value} (0x{value:02X})")
		else:
			value = int.from_bytes(value_bytes, byteorder="little")
			print(f"{name}: {value} (0x{value:04X})")

		offset += size

def print_usb_endpoint_descriptor(desc: bytearray):
	if len(desc) < 7:
		raise ValueError("USB endpoint descriptor must be at least 7 bytes")

	fields = [
		("bLength", 1),
		("bDescriptorType", 1),
		("bEndpointAddress", 1),
		("bmAttributes", 1),
		("wMaxPacketSize", 2),
		("bInterval", 1),
	]

	offset = 0

	for name, size in fields:
		value_bytes = desc[offset:offset + size]

		if size == 1:
			value = value_bytes[0]
			print(f"{name}: {value} (0x{value:02X})")
		else:
			# USB descriptors use little-endian encoding
			value = int.from_bytes(value_bytes, byteorder="little")
			print(f"{name}: {value} (0x{value:04X})")

		offset += size

def usb_descriptor_type_name(descriptor_type: int) -> str:
	descriptor_types = {
		0x01: "DEVICE",
		0x02: "CONFIGURATION",
		0x03: "STRING",
		0x04: "INTERFACE",
		0x05: "ENDPOINT",
		0x06: "DEVICE_QUALIFIER",
		0x07: "OTHER_SPEED_CONFIGURATION",
		0x08: "INTERFACE_POWER",
		0x0B: "INTERFACE_ASSOCIATION",
		0x0F: "BOS",
		0x10: "DEVICE_CAPABILITY",
		0x21: "HID",
		0x22: "REPORT",
		0x23: "PHYSICAL",
		0x24: "CS_INTERFACE",
		0x25: "CS_ENDPOINT",
	}

	return descriptor_types.get(descriptor_type, f"UNKNOWN_DESCRIPTOR_TYPE_0x{descriptor_type:02X}")
