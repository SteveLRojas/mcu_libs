import cdc_test_device_platform as cdc
import cdc_test_device_regs as regs

def main():
	cdc.init()
	
	print(f"Device ID: {cdc.get_device_id():08X}")
	print(f"Unique ID: {cdc.get_unique_id():08X}")
	print(f"CDC config: {cdc.read_reg(regs.R_CDC_CONFIG):02X}")
	print(f"Control line state: {cdc.read_reg(regs.R_CONTROL_LINE_STATE):02X}")
	print(f"Bytes available: {cdc.read_reg(regs.R_CDC_BYTES_AVAILABLE):02X}")
	
	usbd_config = cdc.read_config_buf()
	cdc.print_usbd_config(usbd_config)
	
	cdc.stop()

if __name__ == "__main__":
	main()