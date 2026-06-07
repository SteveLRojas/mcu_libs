import usb_test_host_platform as usbh
import usb_test_host_regs as regs

def main():
	usbh.init()
	
	print(f"Device ID: {usbh.get_device_id():08X}")
	print(f"Unique ID: {usbh.get_unique_id():08X}")
	print(f"USB state: {usbh.get_state_str()}")
	print(f"USB response: {usbh.get_response_str()}")

	print(f"RX buffer: {usbh.read_rx_buf(0, 16)}")

	usbh.write_reg(regs.R_EP_SEL, 0)
	usbh.write_reg(regs.R_EP_IDX, 1)
	print(f"EP0 max packet size: {usbh.read_reg(regs.R_EP_DATA)}")
	
	usbh.stop()

if __name__ == "__main__":
	main()