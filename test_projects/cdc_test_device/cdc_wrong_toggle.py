import time
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

	print("USB periph reset")
	cdc.write_reg(cdc.regs.R_USBD_DISABLE, 0)
	time.sleep(0.2)
	cdc.write_reg(cdc.regs.R_USBD_RESET, 0)
	cdc.write_reg(cdc.regs.R_USBD_INIT, 0)
	time.sleep(0.2)
	print(f"State: {usbh.get_state_str()}")

	#set toggle to 1 for EP2
	usbh.write_reg(usbh.regs.R_EP_SEL, 2)
	usbh.write_reg(usbh.regs.R_EP_IDX, 3)
	usbh.write_reg(usbh.regs.R_EP_DATA, usbh.defs.USBH_OUT_TOG_1 | usbh.defs.USBH_OUT_AUTOTOG | usbh.defs.USBH_OUT_EXPECT_ACK)

	#send one packet to EP2
	usbh_buf = []
	for d in range(16):
		usbh_buf.append(0xAA)

	usbh.write_transfer_buf(0, usbh_buf)
	usbh.write_reg(usbh.regs.R_TRANSFER_LEN, len(usbh_buf))
	usbh.write_reg(usbh.regs.R_OUT_TRANSFER, 0)
	time.sleep(0.1)
	print(f"USBH response: {usbh.get_response_str()}")

	#check the bytes received
	bytes_available = cdc.read_reg(cdc.regs.R_CDC_BYTES_AVAILABLE)
	print(f"CDC bytes available: {bytes_available}")
	if bytes_available == 0:
		usbh.stop()
		cdc.stop()
		exit(1)
	cdc.write_reg(cdc.regs.R_CDC_READ_BYTES, bytes_available)
	cdc_buf = cdc.read_cdc_buf(0, bytes_available)
	if list(cdc_buf) == usbh_buf:
		print("Received data is correct!")
	else:
		print("Received bad data:")
		print(f"USBH buf: {usbh_buf}")
		print(f"CDC buf: {cdc_buf}")
	
	cdc.stop()
	usbh.stop()

if __name__ == "__main__":
	main()