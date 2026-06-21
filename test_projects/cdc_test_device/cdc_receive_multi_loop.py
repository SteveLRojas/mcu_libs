import time
import random
import usb_test_host_platform as usbh
import usbh_defs as defs
import cdc_test_device_platform as cdc

packet_len = 64
num_packets = 4
num_iterations = 10

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

	if usbh.read_reg(usbh.regs.R_STATE) != defs.S_RUN:
		print(f"Device is not ready: {usbh.get_state_str()}")
		usbh.stop()
		cdc.stop()
		exit(1)
	
	usbh.set_max_packet_size(2, 64)
	usbh.write_reg(usbh.regs.R_OUT_NAK_LIMIT_L, 0)
	usbh.write_reg(usbh.regs.R_OUT_NAK_LIMIT_H, 0)
	cdc.write_reg(cdc.regs.R_EP_SEL, 2)
	
	sent_buf = []
	received_buf = []
	for d in range(num_iterations):
		for i in range(num_packets):
			#send one packet to EP2
			usbh_buf = [random.getrandbits(8) for _ in range(packet_len)]

			usbh.write_transfer_buf(0, usbh_buf)
			usbh.write_reg(usbh.regs.R_TRANSFER_LEN, len(usbh_buf))
			usbh.write_reg(usbh.regs.R_EP_SEL, 2)
			usbh.write_reg(usbh.regs.R_OUT_TRANSFER, 0)
			time.sleep(0.1)
			response = usbh.read_reg(usbh.regs.R_RESPONSE)
			print(f"USBH response: {usbh.get_response_str(response)}")
			if response != defs.USB_PID_ACK:
				print(f"Packet {i} not accepted on iteration {d}")
				print(f"RX_0_LEN: {cdc.read_reg(cdc.regs.R_USBD_RX_TX_0_LEN)}")
				print(f"RX_1_LEN: {cdc.read_reg(cdc.regs.R_USBD_RX_TX_1_LEN)}")
				print(f"OUT toggle: {cdc.read_reg(cdc.regs.R_USBD_OUT_TOGGLE):02X}")
				print(f"IN toggle: {cdc.read_reg(cdc.regs.R_USBD_IN_TOGGLE):02X}")
				print(f"OUT res: {cdc.res_type_to_str(cdc.read_reg(cdc.regs.R_USBD_OUT_RES))}")
				continue
			sent_buf = sent_buf + usbh_buf
	
		#collect the bytes received
		bytes_available = cdc.read_reg(cdc.regs.R_CDC_BYTES_AVAILABLE)
		print(f"CDC bytes available: {bytes_available}")
		if bytes_available == 0:
			usbh.stop()
			cdc.stop()
			exit(1)
		cdc.write_reg(cdc.regs.R_CDC_READ_BYTES, bytes_available)
		cdc_buf = cdc.read_cdc_buf(0, bytes_available)
		received_buf = received_buf + list(cdc_buf)
	
	if received_buf == sent_buf:
		print("Received data is correct!")
	else:
		print("Received bad data:")
		print(f"Sent buf: {sent_buf}")
		print(f"CDC buf: {received_buf}")
	
	cdc.stop()
	usbh.stop()

if __name__ == "__main__":
	main()
	