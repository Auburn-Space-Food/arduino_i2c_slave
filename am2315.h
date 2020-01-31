#ifndef AM2315_H
#define AM2315_H

#include <stdint.h>

/* See https://cdn-shop.adafruit.com/datasheets/AM2315.pdf for
 * for a description of the temperature sensor's I2C requirements
 */

#define AM2315_ADDR 0x5C

#define AM2315_READ_REG 0x03
#define AM2315_WRITE_REG 0x10

#define AM2315_REG_HUMID_HIGH 0x00
#define AM2315_REG_HUMID_LOW 0x01
#define AM2315_REG_TEMP_HIGH 0x02
#define AM2315_REG_TEMP_LOW 0x03

// The message returned by the device includes four bytes of
// data other than the requested register, see page 12
#define AM2315_READ_OVERHEAD 4

int am2315_read_registers(uint8_t base_address, uint8_t nbytes, uint8_t *data_read);
int am2315_read_temp(uint8_t *temperature);
int am2315_read_humid(uint8_t *humidity);
int am2315_read_temp_and_humid(uint8_t *temperature, uint8_t *humidity);

float am2315_convert_temperature(uint8_t *temperature);
float am2315_convert_humidity(uint8_t *humidity);

#endif
