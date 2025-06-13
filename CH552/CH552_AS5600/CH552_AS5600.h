#ifndef _CH552_AS5600_H_
#define _CH552_AS5600_H_

// HINT: for 5V operation the AGC range is 0 to 255, for 3.3V it is reduced to 0 to 128.
#define AS_DEV_ADDR_WRITE	0x6C
#define AS_DEV_ADDR_READ	0x6D

#define AS_REG_ZMCO			0x00
#define AS_REG_ZPOS 		0x01
#define AS_REG_MPOS			0x03
#define AS_REG_MANG			0x05
#define AS_REG_CONF			0x07
#define AS_REG_RAW_ANGLE	0x0C
#define AS_REG_ANGLE		0x0E
#define AS_REG_STATUS		0x0B
#define AS_REG_AGC			0x1A
#define AS_REG_MAGNITUDE	0x1B
#define AS_REG_BURN			0xFF

#define AS_CONF_PM_NOM		0x0000
#define AS_CONF_PM_LPM1		0x0001
#define AS_CONF_PM_LPM2		0x0002
#define AS_CONF_PM_LPM3		0x0003
#define AS_CONF_HYST_OFF	0x0000
#define AS_CONF_HYST_1LSB	0x0004
#define AS_CONF_HYST_2LSB	0x0008
#define AS_CONF_HYST_3LSB	0x000C
#define AS_CONF_OUTS_FULL	0x0000
#define AS_CONF_OUTS_10_90	0x0010
#define AS_CONF_OUTS_PWM	0x0020
#define AS_CONF_PWMF_115	0x0000
#define AS_CONF_PWMF_230	0x0040
#define AS_CONF_PWMF_460	0x0080
#define AS_CONF_PWMF_920	0x00C0
#define AS_CONF_SF_16X		0x0000
#define AS_CONF_SF_8X		0x0100
#define AS_CONF_SF_4X		0x0200
#define AS_CONF_SF_2X		0x0300
#define AS_CONF_FTH_NONE	0x0000
#define AS_CONF_FTH_6LSB	0x0400
#define AS_CONF_FTH_7LSB	0x0800
#define AS_CONF_FTH_9LSB	0x0C00
#define AS_CONF_FTH_18LSB	0x1000
#define AS_CONF_FTH_21LSB	0x1400
#define AS_CONF_FTH_24LSB	0x1800
#define AS_CONF_FTH_10LSB	0x1C00
#define AS_CONF_WD_OFF		0x0000
#define AS_CONF_WD_ON		0x2000

#define AS_STATUS_MH		0x08
#define AS_STATUS_ML		0x10
#define AS_STATUS_MD		0x20

UINT8 as5600_read_byte(UINT8 addr);
UINT16 as5600_read_word(UINT8 addr);
void as5600_write_byte(UINT8 addr, UINT8 val);
void as5600_write_word(UINT8 addr, UINT16 val);

#endif
