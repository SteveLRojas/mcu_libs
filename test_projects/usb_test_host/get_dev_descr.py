import usb_test_host_platform as usbh
import usb_test_host_regs as regs
import usbh_defs as defs

def main():
	usbh.init()
	
	print(f"Device ID: {usbh.get_device_id():08X}")
	print(f"Unique ID: {usbh.get_unique_id():08X}")

	if usbh.read_reg(regs.R_STATE) != defs.S_RUN:
		print(f"Device is not ready: {usbh.get_state_str()}")
		exit(1)	

	dev_descr = usbh.get_dev_descr()
	usbh.print_usb_device_descriptor(dev_descr)
	
	usbh.stop()

if __name__ == "__main__":
	main()