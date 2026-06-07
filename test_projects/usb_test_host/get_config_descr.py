import time
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

	descr = usbh.get_full_config_descr()
	
	idx = 0
	while idx < len(descr):
		descr_len = descr[idx]
		descr_typ = descr[idx + 1]
		if descr_typ == defs.USB_DESCR_TYP_CONFIG:
			print("Configuration descriptor:")
			usbh.print_usb_configuration_descriptor(descr[idx:])
		elif descr_typ == defs.USB_DESCR_TYP_INTERF:
			print("Interface descriptor:")
			usbh.print_usb_interface_descriptor(descr[idx:])
		elif descr_typ == defs.USB_DESCR_TYP_ENDP:
			print("Endpoint descriptor:")
			usbh.print_usb_endpoint_descriptor(descr[idx:])
		else:
			print(f"descriptor type: {usbh.usb_descriptor_type_name(descr_typ)}")
		idx += descr_len
	
	usbh.stop()

if __name__ == "__main__":
	main()