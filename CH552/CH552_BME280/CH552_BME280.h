#ifndef _CH552_BME280_H_
#define _CH552_BME280_H_

//HINT: Calibration data from BME280 is in little endian order, change endiannes if compiler uses big endian.
#define BME280_CHANGE_ENDIANNESS 1
#define BME280_DEV_ADDR 0xEC

// Register names:
#define BME280_REG_DIG_T1_LSB 0x88
#define BME280_REG_DIG_T1_MSB 0x89
#define BME280_REG_DIG_T2_LSB 0x8A
#define BME280_REG_DIG_T2_MSB 0x8B
#define BME280_REG_DIG_T3_LSB 0x8C
#define BME280_REG_DIG_T3_MSB 0x8D
#define BME280_REG_DIG_P1_LSB 0x8E
#define BME280_REG_DIG_P1_MSB 0x8F
#define BME280_REG_DIG_P2_LSB 0x90
#define BME280_REG_DIG_P2_MSB 0x91
#define BME280_REG_DIG_P3_LSB 0x92
#define BME280_REG_DIG_P3_MSB 0x93
#define BME280_REG_DIG_P4_LSB 0x94
#define BME280_REG_DIG_P4_MSB 0x95
#define BME280_REG_DIG_P5_LSB 0x96
#define BME280_REG_DIG_P5_MSB 0x97
#define BME280_REG_DIG_P6_LSB 0x98
#define BME280_REG_DIG_P6_MSB 0x99
#define BME280_REG_DIG_P7_LSB 0x9A
#define BME280_REG_DIG_P7_MSB 0x9B
#define BME280_REG_DIG_P8_LSB 0x9C
#define BME280_REG_DIG_P8_MSB 0x9D
#define BME280_REG_DIG_P9_LSB 0x9E
#define BME280_REG_DIG_P9_MSB 0x9F
#define BME280_REG_DIG_H1 0xA1
#define BME280_REG_CHIP_ID 0xD0 // Chip ID
#define BME280_REG_RST 0xE0     // Softreset Reg
#define BME280_REG_DIG_H2_LSB 0xE1
#define BME280_REG_DIG_H2_MSB 0xE2
#define BME280_REG_DIG_H3 0xE3
#define BME280_REG_DIG_H4_MSB 0xE4
#define BME280_REG_DIG_H4_LSB 0xE5
#define BME280_REG_DIG_H5_MSB 0xE6
#define BME280_REG_DIG_H6 0xE7
#define BME280_REG_CTRL_HUMIDITY 0xF2    // Ctrl Humidity Reg
#define BME280_REG_STAT 0xF3             // Status Reg
#define BME280_REG_CTRL_MEAS 0xF4        // Ctrl Measure Reg
#define BME280_REG_CONFIG 0xF5           // Configuration Reg
#define BME280_REG_PRESSURE_MSB 0xF7     // Pressure MSB
#define BME280_REG_PRESSURE_LSB 0xF8     // Pressure LSB
#define BME280_REG_PRESSURE_XLSB 0xF9    // Pressure XLSB
#define BME280_REG_TEMPERATURE_MSB 0xFA  // Temperature MSB
#define BME280_REG_TEMPERATURE_LSB 0xFB  // Temperature LSB
#define BME280_REG_TEMPERATURE_XLSB 0xFC // Temperature XLSB
#define BME280_REG_HUMIDITY_MSB 0xFD     // Humidity MSB
#define BME280_REG_HUMIDITY_LSB 0xFE     // Humidity LSB

#define BME280_CONFIG_T_SB_0MS5		0x00
#define BME280_CONFIG_T_SB_62MS5	0x20
#define BME280_CONFIG_T_SB_125MS	0x40
#define BME280_CONFIG_T_SB_250MS	0x60
#define BME280_CONFIG_T_SB_500MS	0x80
#define BME280_CONFIG_T_SB_1000MS	0xA0
#define BME280_CONFIG_T_SB_10MS		0xC0
#define BME280_CONFIG_T_SB_20MS		0xE0

#define BME280_CONFIG_FILTER_OFF	0x00
#define BME280_CONFIG_FILTER_2		0x04
#define BME280_CONFIG_FILTER_4		0x08
#define BME280_CONFIG_FILTER_8		0x0C
#define BME280_CONFIG_FILTER_16		0x10

#define BME280_MEAS_OSRS_P_SKIP		0x00
#define BME280_MEAS_OSRS_P_1X		0x20
#define BME280_MEAS_OSRS_P_2X		0x40
#define BME280_MEAS_OSRS_P_4X		0x60
#define BME280_MEAS_OSRS_P_8X		0x80
#define BME280_MEAS_OSRS_P_16X		0xA0

#define BME280_MEAS_OSRS_T_SKIP		0x00
#define BME280_MEAS_OSRS_T_1X		0x04
#define BME280_MEAS_OSRS_T_2X		0x08
#define BME280_MEAS_OSRS_T_4X		0x0C
#define BME280_MEAS_OSRS_T_16X		0x10

#define BME280_MEAS_MODE_SLEEP		0x00
#define BME280_MEAS_MODE_FORCED		0x01
#define BME280_MEAS_MODE_NORMAL		0x03

#define BME280_CTRL_HUM_ORSR_H_SKIP	0x00
#define BME280_CTRL_HUM_ORSR_H_1X	0x01
#define BME280_CTRL_HUM_ORSR_H_2X	0x02
#define BME280_CTRL_HUM_ORSR_H_4X	0x03
#define BME280_CTRL_HUM_ORSR_H_8X	0x04
#define BME280_CTRL_HUM_ORSR_H_16X	0x05

#define BME280_STAT_MEASURING		0x08
#define BME280_STAT_IM_UPDATE		0x01

typedef struct _BME280_CALIB
{
    UINT16 dig_T1;
    signed short dig_T2;
    signed short dig_T3;

    UINT16 dig_P1;
    signed short dig_P2;
    signed short dig_P3;
    signed short dig_P4;
    signed short dig_P5;
    signed short dig_P6;
    signed short dig_P7;
    signed short dig_P8;
    signed short dig_P9;

    UINT8 dig_H1;
    signed short dig_H2;
    UINT8 dig_H3;
    signed short dig_H4;
    signed short dig_H5;
    signed char dig_H6;
} bme280_calib_t;

typedef struct _BME280_RAW_MEAS
{
	UINT8 press_msb;
	UINT8 press_lsb;
	UINT8 press_xlsb;
	UINT8 temp_msb;
	UINT8 temp_lsb;
	UINT8 temp_xlsb;
	UINT8 hum_msb;
	UINT8 hum_lsb;
} bme280_raw_meas_t;

extern bme280_calib_t bme280_calib_data;
extern bme280_raw_meas_t bme280_raw_measurements;

#define BME280_CHANGE_WORD_ENDIANNESS(word) do{((UINT8*)(&(word)))[0] ^= ((UINT8*)(&(word)))[1]; ((UINT8*)(&(word)))[1] ^= ((UINT8*)(&(word)))[0]; ((UINT8*)(&(word)))[0] ^= ((UINT8*)(&(word)))[1];}while(0)

#define bme280_set_meas_mode(mode) bme280_write_reg(BME280_REG_CTRL_MEAS, (bme280_read_reg(BME280_REG_CTRL_MEAS) & ~BME280_MEAS_MODE_NORMAL) | (mode))
#define bme280_read_raw_values() bme280_read_regs(BME280_REG_PRESSURE_MSB, (UINT8*)&bme280_raw_measurements, 8)
#define bme280_get_pressure_raw() (((signed long)bme280_raw_measurements.press_msb << 12) | ((signed long)bme280_raw_measurements.press_lsb << 4) | ((signed long)bme280_raw_measurements.press_xlsb >> 4))
#define bme280_get_temperature_raw() (((signed long)bme280_raw_measurements.temp_msb << 12) | ((signed long)bme280_raw_measurements.temp_lsb << 4) | ((signed long)bme280_raw_measurements.temp_xlsb >> 4))
#define bme280_get_humidity_raw() (((signed long)bme280_raw_measurements.hum_msb << 8) | ((signed long)bme280_raw_measurements.hum_lsb))

void bme280_init();
void bme280_write_reg(UINT8 addr, UINT8 val);
UINT8 bme280_read_reg(UINT8 addr);
void bme280_read_regs(UINT8 addr, UINT8* dest, UINT8 len);
void bme280_force_measurement(void);
signed long bme280_convert_temperature(void);
unsigned long bme280_convert_pressure(void);
unsigned long bme280_convert_humidity(void);

#endif
