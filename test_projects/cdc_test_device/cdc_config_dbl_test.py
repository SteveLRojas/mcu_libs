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
	usbh.write_reg(usbh.regs.R_OUT_NAK_LIMIT_L, 0)
	usbh.write_reg(usbh.regs.R_OUT_NAK_LIMIT_H, 0)
	cdc.write_reg(cdc.regs.R_EP_SEL, 2)

	usbd_config = [
		cdc.defs.CDC_ENDP0_SIZE, cdc.defs.CDC_ENDP1_SIZE, 0, cdc.defs.CDC_ENDP3_SIZE, 0, 0, 0, 0,
		cdc.defs.CDC_ENDP0_SIZE, 0, cdc.defs.CDC_ENDP2_SIZE, 0, 0, 0, 0, 0,
		cdc.defs.USBD_EP_TYPE_CONTROL, cdc.defs.USBD_EP_TYPE_INTERRUPT, cdc.defs.USBD_EP_TYPE_BULK_DBL, cdc.defs.USBD_EP_TYPE_BULK_DBL, cdc.defs.USBD_EP_TYPE_BULK, cdc.defs.USBD_EP_TYPE_BULK, cdc.defs.USBD_EP_TYPE_BULK, cdc.defs.USBD_EP_TYPE_BULK,
		cdc.defs.USBD_INT_TRANSFER | cdc.defs.USBD_INT_RESET | cdc.defs.USBD_INT_SOF
	]
	
	cdc.write_config_buf(usbd_config)
	usbd_config = cdc.read_config_buf()
	cdc.print_usbd_config(usbd_config)

	print(f"RX_0_LEN: {cdc.read_reg(cdc.regs.R_USBD_RX_TX_0_LEN)}")
	print(f"RX_1_LEN: {cdc.read_reg(cdc.regs.R_USBD_RX_TX_1_LEN)}")
	print(f"OUT toggle: {cdc.read_reg(cdc.regs.R_USBD_OUT_TOGGLE):02X}")
	print(f"IN toggle: {cdc.read_reg(cdc.regs.R_USBD_IN_TOGGLE):02X}")
	print(f"OUT res: {cdc.res_type_to_str(cdc.read_reg(cdc.regs.R_USBD_OUT_RES))}")

	usbh.write_reg(usbh.regs.R_STATE, usbh.defs.S_BYPASS)
	print(f"State: {usbh.get_state_str()}")

	print("USB device reset")
	cdc.write_reg(cdc.regs.R_USBD_DISABLE, 0)
	time.sleep(0.2)
	cdc.write_reg(cdc.regs.R_USBD_RESET, 0)
	cdc.write_reg(cdc.regs.R_USBD_INIT, 0)

	usbh.write_reg(usbh.regs.R_CONTROLLER_SPEED, usbh.defs.USBH_FULL_SPEED)
	usbh.write_reg(usbh.regs.R_PORT0_CONFIG, usbh.defs.USBH_FULL_SPEED | usbh.defs.USBH_PORT_ENABLE)
	usbh.write_reg(usbh.regs.R_DEV_ADDR, 0x01)
	cdc.write_reg(cdc.regs.R_USBD_SET_ADDR, 0x01)

	usbh.write_reg(usbh.regs.R_STATE, usbh.defs.S_RUN)
	time.sleep(0.1)
	print(f"State: {usbh.get_state_str()}")

	print(f"RX_0_LEN: {cdc.read_reg(cdc.regs.R_USBD_RX_TX_0_LEN)}")
	print(f"RX_1_LEN: {cdc.read_reg(cdc.regs.R_USBD_RX_TX_1_LEN)}")
	print(f"OUT toggle: {cdc.read_reg(cdc.regs.R_USBD_OUT_TOGGLE):02X}")
	print(f"IN toggle: {cdc.read_reg(cdc.regs.R_USBD_IN_TOGGLE):02X}")
	print(f"OUT res: {cdc.res_type_to_str(cdc.read_reg(cdc.regs.R_USBD_OUT_RES))}")

	cdc.write_reg(cdc.regs.R_USBD_OUT_RES, cdc.defs.CDC_IN_OUT_RES_ACK)
	print(f"OUT res: {cdc.res_type_to_str(cdc.read_reg(cdc.regs.R_USBD_OUT_RES))}")

	#reset toggle to 0 for EP2
	usbh.write_reg(usbh.regs.R_EP_SEL, 2)
	usbh.write_reg(usbh.regs.R_EP_IDX, 3)
	usbh.write_reg(usbh.regs.R_EP_DATA, usbh.defs.USBH_OUT_TOG_0 | usbh.defs.USBH_OUT_AUTOTOG | usbh.defs.USBH_OUT_EXPECT_ACK)

	#send one packet to EP2
	usbh_buf = []
	for d in range(16):
		usbh_buf.append(0xAA)

	usbh.write_transfer_buf(0, usbh_buf)
	usbh.write_reg(usbh.regs.R_TRANSFER_LEN, len(usbh_buf))
	usbh.write_reg(usbh.regs.R_OUT_TRANSFER, 0)
	time.sleep(0.1)
	print(f"USBH response: {usbh.get_response_str()}")

	print(f"CDC bytes available: {cdc.read_reg(cdc.regs.R_CDC_BYTES_AVAILABLE):02X}")
	print(f"RX_0_LEN: {cdc.read_reg(cdc.regs.R_USBD_RX_TX_0_LEN)}")
	print(f"RX_1_LEN: {cdc.read_reg(cdc.regs.R_USBD_RX_TX_1_LEN)}")
	print(f"OUT toggle: {cdc.read_reg(cdc.regs.R_USBD_OUT_TOGGLE):02X}")
	print(f"IN toggle: {cdc.read_reg(cdc.regs.R_USBD_IN_TOGGLE):02X}")
	print(f"OUT res: {cdc.res_type_to_str(cdc.read_reg(cdc.regs.R_USBD_OUT_RES))}")
	
	usbh.stop()
	cdc.stop()

if __name__ == "__main__":
	main()