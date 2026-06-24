import time
import random
import usb_test_host_platform as usbh
import cdc_test_device_platform as cdc

def main():
	unique_id = usbh.init()
	if unique_id == 0:
		print("Could not find USBH test device!")
		usbh.stop()
		exit(1)
	print(f"USBH Device ID: {usbh.get_device_id():08X}")
	print(f"USBH Unique ID: {unique_id:08X}")

	unique_id = cdc.init()
	if unique_id == 0:
		print("Could not find CDC test device!")
		usbh.stop()
		cdc.stop()
		exit(1)
	print(f"CDC Device ID: {cdc.get_device_id():08X}")
	print(f"CDC Unique ID: {unique_id:08X}")

	print(f"CDC config: {cdc.read_reg(cdc.regs.R_CDC_CONFIG):02X}")
	print(f"Control line state: {cdc.read_reg(cdc.regs.R_CONTROL_LINE_STATE):02X}")
	print(f"CDC bytes available: {cdc.read_reg(cdc.regs.R_CDC_BYTES_AVAILABLE):02X}")

	if usbh.read_reg(usbh.regs.R_STATE) != usbh.defs.S_RUN:
		print(f"Device is not ready: {usbh.get_state_str()}")
		usbh.stop()
		cdc.stop()
		exit(1)
	
	usbh.set_max_packet_size(2, 64)
	usbh.set_max_packet_size(3, 64)
	usbh.write_reg(usbh.regs.R_OUT_NAK_LIMIT_L, 0)
	usbh.write_reg(usbh.regs.R_OUT_NAK_LIMIT_H, 0)
	usbh.write_reg(usbh.regs.R_EP_SEL, 3)

	#flush out any data already in the buffers
	usbh.write_reg(usbh.regs.R_TRANSFER_LEN, 64)
	usbh.write_reg(usbh.regs.R_IN_TRANSFER, 0)
	time.sleep(0.1)
	print(f"USBH response: {usbh.get_response_str()}")
	usbh.write_reg(usbh.regs.R_TRANSFER_LEN, 64)
	usbh.write_reg(usbh.regs.R_IN_TRANSFER, 0)
	time.sleep(0.1)
	print(f"USBH response: {usbh.get_response_str()}")

	#write data to send
	cdc_buf = [random.getrandbits(8) for _ in range(16)]
	
	cdc.write_cdc_buf(0, cdc_buf)
	cdc.write_reg(cdc.regs.R_CDC_WRITE_BYTES, len(cdc_buf))
	print(f"Bytes available for write: {cdc.read_reg(cdc.regs.R_CDC_BYTES_AVAILABLE_FOR_WRITE)}")

	#check the data sent
	usbh.write_reg(usbh.regs.R_TRANSFER_LEN, len(cdc_buf))
	usbh.write_reg(usbh.regs.R_IN_TRANSFER, 0)
	time.sleep(0.1)
	print(f"USBH response: {usbh.get_response_str()}")
	usbh.write_reg(usbh.regs.R_TRANSFER_LEN, len(cdc_buf))
	usbh.write_reg(usbh.regs.R_IN_TRANSFER, 0)
	time.sleep(0.1)
	print(f"USBH response: {usbh.get_response_str()}")

	usbh_buf = usbh.read_transfer_buf(0, len(cdc_buf))
	if cdc_buf == list(usbh_buf):
		print("Received data is correct!")
	else:
		print("Received bad data:")
		print(f"USBH buf: {list(usbh_buf)}")
		print(f"CDC buf: {cdc_buf}")

	cdc.stop()
	usbh.stop()

if __name__ == "__main__":
	main()
	