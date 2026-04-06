#include "CH552.H"
#include "CH552_RCC.h"
#include "CH552_I2C.h"
#include "CH552_BME280.h"

bme280_calib_t bme280_calib_data;
bme280_raw_meas_t bme280_raw_measurements;
signed long bme280_t_fine = 2000L * 256L / 5L;

//HINT: I2C should be initialized before calling this function
void bme280_init()
{
	UINT8 temp_e5;
	
	bme280_read_regs(BME280_REG_DIG_T1_LSB, (UINT8*)&bme280_calib_data, 24);
	bme280_calib_data.dig_H1 = bme280_read_reg(BME280_REG_DIG_H1);
	bme280_read_regs(BME280_REG_DIG_H2_LSB, (UINT8*)&bme280_calib_data.dig_H2, 7);
	
	temp_e5 = ((UINT8*)&bme280_calib_data.dig_H2)[4];
	((UINT8*)&bme280_calib_data.dig_H2)[7] = ((UINT8*)&bme280_calib_data.dig_H2)[6];	//(E7) -> dig_H6
	((UINT8*)&bme280_calib_data.dig_H2)[6] = ((UINT8*)&bme280_calib_data.dig_H2)[5] >> 4;	//(E6)[7:4] -> dig_H5[11:8]
	((UINT8*)&bme280_calib_data.dig_H2)[5] = (((UINT8*)&bme280_calib_data.dig_H2)[5] << 4) | (temp_e5 >> 4);	//(E6)[3:0], (E5)[7:4] -> dig_H5[7:0]
	((UINT8*)&bme280_calib_data.dig_H2)[4] = ((UINT8*)&bme280_calib_data.dig_H2)[3] >> 4;	//(E4)[7:4] -> dig_H4[11:8]
	((UINT8*)&bme280_calib_data.dig_H2)[3] = (((UINT8*)&bme280_calib_data.dig_H2)[3] << 4) | (temp_e5 & 0x0F);	//(E4)[3:0], (E5)[3:0] -> dig_H4[7:0]
	
#if BME280_CHANGE_ENDIANNESS
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_T1);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_T2);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_T3);
	
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_P1);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_P2);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_P3);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_P4);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_P5);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_P6);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_P7);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_P8);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_P9);

	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_H2);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_H4);
	BME280_CHANGE_WORD_ENDIANNESS(bme280_calib_data.dig_H5);
#endif
	bme280_write_reg(BME280_REG_CONFIG, BME280_CONFIG_T_SB_125MS | BME280_CONFIG_FILTER_OFF);
	bme280_write_reg(BME280_REG_CTRL_MEAS, BME280_MEAS_OSRS_P_1X | BME280_MEAS_OSRS_T_1X | BME280_MEAS_MODE_SLEEP);
	bme280_write_reg(BME280_REG_CTRL_HUMIDITY, BME280_CTRL_HUM_ORSR_H_1X);
}

void bme280_write_reg(UINT8 addr, UINT8 val)
{
	i2c_start();
	i2c_write(BME280_DEV_ADDR);
	i2c_write(addr);
	i2c_write(val);
	i2c_stop();
}

UINT8 bme280_read_reg(UINT8 addr)
{
	UINT8 val;
	
	i2c_start();
	i2c_write(BME280_DEV_ADDR);
	i2c_write(addr);
	i2c_start();
	i2c_write(BME280_DEV_ADDR | 0x01);
	host_ack = 0;
	val = i2c_read();
	i2c_stop();
	return val;
}

void bme280_read_regs(UINT8 addr, UINT8* dest, UINT8 len)
{
	UINT8 count;
	
	i2c_start();
	i2c_write(BME280_DEV_ADDR);
	i2c_write(addr);
	i2c_start();
	i2c_write(BME280_DEV_ADDR | 0x01);
	
	host_ack = 1;
	for(count = 0; count < (len - 1); ++count)
	{
		dest[count] = i2c_read();
	}
	host_ack = 0;
	dest[len - 1] = i2c_read();
	i2c_stop();
}

void bme280_force_measurement(void)
{
	bme280_set_meas_mode(BME280_MEAS_MODE_FORCED);
	
	while(bme280_read_reg(BME280_REG_STAT) & BME280_STAT_MEASURING);
	
	bme280_read_raw_values();
}

//HINT: Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// bme280_t_fine carries fine temperature as global value.
signed long bme280_convert_temperature(void)
{
	signed long temp_raw;
	signed long var1;
	signed long var2;
	signed long temp_compensated;
	
	temp_raw = bme280_get_temperature_raw();
	
	var1 = ((((temp_raw >> 3) - ((signed long)bme280_calib_data.dig_T1 << 1))) * ((signed long)bme280_calib_data.dig_T2)) >> 11;
	var2 = (((((temp_raw >> 4) - ((signed long)bme280_calib_data.dig_T1)) * ((temp_raw >> 4) - ((signed long)bme280_calib_data.dig_T1))) >> 12) * ((signed long)bme280_calib_data.dig_T3)) >> 14;
	bme280_t_fine = var1 + var2;
	temp_compensated = (bme280_t_fine * 5 + 128) >> 8;
	return temp_compensated;
}

//HINT: Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
// Convert temperature first to update bme280_t_fine.
unsigned long bme280_convert_pressure(void)
{
	signed long pressure_raw;
	signed long var1;
	signed long var2;
	unsigned long pressure_compensated;
	
	pressure_raw = bme280_get_pressure_raw();
	
	var1 = (((signed long)bme280_t_fine) >> 1) - (signed long)64000;
	var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((signed long)bme280_calib_data.dig_P6);
	var2 = var2 + ((var1 * ((signed long)bme280_calib_data.dig_P5)) << 1);
	var2 = (var2 >> 2) + (((signed long)bme280_calib_data.dig_P4) << 16);
	var1 = (((bme280_calib_data.dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13 )) >> 3) + ((((signed long)bme280_calib_data.dig_P2) * var1) >> 1)) >> 18;
	var1 = ((((32768 + var1)) * ((signed long)bme280_calib_data.dig_P1)) >> 15);
	/*if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}*/
	pressure_compensated = (((unsigned long)(((signed long)1048576) - pressure_raw) - (var2 >> 12))) * 3125;
	if (pressure_compensated < 0x80000000L)
	{
		pressure_compensated = (pressure_compensated << 1) / ((unsigned long)var1);
	}
	else
	{
		pressure_compensated = (pressure_compensated / (unsigned long)var1) * 2;
	}
	var1 = (((signed long)bme280_calib_data.dig_P9) * ((signed long)(((pressure_compensated >> 3) * (pressure_compensated >> 3)) >> 13))) >> 12;
	var2 = (((signed long)(pressure_compensated >> 2)) * ((signed long)bme280_calib_data.dig_P8)) >> 13;
	pressure_compensated = (unsigned long)((signed long)pressure_compensated + ((var1 + var2 + bme280_calib_data.dig_P7) >> 4));
	return pressure_compensated;
}

//HINT: Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
unsigned long bme280_convert_humidity(void)
{
	signed long humidity_raw;
	signed long v_x1_u32r;
	
	humidity_raw = bme280_get_humidity_raw();
	
	v_x1_u32r = bme280_t_fine - ((signed long)76800L);
	v_x1_u32r = (((((humidity_raw << 14) - (((signed long)bme280_calib_data.dig_H4) << 20) - (((signed long)bme280_calib_data.dig_H5) * v_x1_u32r)) +
	((signed long)16384)) >> 15) * (((((((v_x1_u32r * ((signed long)bme280_calib_data.dig_H6)) >> 10) * (((v_x1_u32r * ((signed long)bme280_calib_data.dig_H3)) >> 11) + 
	((signed long)32768))) >> 10) + ((signed long)2097152L)) * ((signed long)bme280_calib_data.dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((signed long)bme280_calib_data.dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400L ? 419430400L : v_x1_u32r);
	return (unsigned long)(v_x1_u32r >> 12);
}
