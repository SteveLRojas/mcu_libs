import serial
import serial.tools.list_ports
import time
import cdc_test_device_regs as regs
import cdc_test_device_defs as defs

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
		try:
			ser.port = port
			ser.open()
			time.sleep(0.5)
			ser.reset_input_buffer()
		except Exception:
			continue

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

def write_cdc_buf(start_addr, data):
	bytes = bytearray()
	write_reg(regs.R_CDC_IDX, start_addr)
	for byte in data:
		bytes.append(regs.R_CDC_DATA | 0x80)
		bytes.append(byte)
	ser.write(bytes)

def read_config_buf():
	bytes = bytearray()
	write_reg(regs.R_CONFIG_IDX, 0)
	for byte in range(50):
		bytes.append(regs.R_CONFIG_DATA)
	ser.write(bytes)
	bytes = ser.read(50)
	return [int.from_bytes(bytes[i:i+2], 'little') for i in range(0, 50, 2)]

def write_config_buf(usbd_config):
	bytes = bytearray()
	write_reg(regs.R_CONFIG_IDX, 0)
	for word in usbd_config:
		bytes.append(regs.R_CONFIG_DATA | 0x80)
		bytes.append(word & 0xFF)
		bytes.append(regs.R_CONFIG_DATA | 0x80)
		bytes.append(word >> 8)
	ser.write(bytes)

def ep_type_to_str(ep_type):
	ep_types = {
		defs.USBD_EP_TYPE_BULK: "USBD_EP_TYPE_BULK",
		defs.USBD_EP_TYPE_BULK_DBL: "USBD_EP_TYPE_BULK_DBL",
		defs.USBD_EP_TYPE_CONTROL: "USBD_EP_TYPE_CONTROL",
		defs.USBD_EP_TYPE_ISO: "USBD_EP_TYPE_ISO",
		defs.USBD_EP_TYPE_INTERRUPT: "USBD_EP_TYPE_INTERRUPT",
	}
	return ep_types.get(ep_type, f"UNKNOWN_EP_TYPE_0x{ep_type:04X}")

def res_type_to_str(res_type):
	res_types = {
		defs.CDC_IN_OUT_RES_DISABLED: "CDC_IN_OUT_RES_DISABLED",
		defs.CDC_IN_OUT_RES_STALL: "CDC_IN_OUT_RES_STALL",
		defs.CDC_IN_OUT_RES_NAK: "CDC_IN_OUT_RES_NAK",
		defs.CDC_IN_OUT_RES_ACK: "CDC_IN_OUT_RES_ACK",
	}
	return res_types.get(res_type, f"UNKNOWN_EP_TYPE_0x{res_type:02X}")

def print_usbd_config(config):
	interrupts = {
		"TRANSFER": defs.USBD_INT_TRANSFER,
		"WAKEUP":   defs.USBD_INT_WAKEUP,
		"SUSPEND":  defs.USBD_INT_SUSPEND,
		"RESET":    defs.USBD_INT_RESET,
		"SOF":      defs.USBD_INT_SOF,
		"ESOF":     defs.USBD_INT_ESOF,
	}
	set_interrupts = [name for name, mask in interrupts.items() if config[24] & mask]

	for ep in range(8):
		print(f"EP{ep} TX buf size: {config[ep]}, RX buf size: {config[ep + 8]}, Type: {ep_type_to_str(config[ep + 16])}")
	print(f"Interrupts enabled: {set_interrupts}")
