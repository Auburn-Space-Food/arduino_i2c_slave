#include <Wire.h>
#include <assert.h>

#include "am2315.h"

int am2315_read_registers(uint8_t base_address, uint8_t nbytes, uint8_t *data_read) {
  assert(data_read != NULL && nbytes != 0);
  
  uint16_t crc;
  uint8_t function_code, nbytes_read;

  // Wake up the sensor
  Wire.beginTransmission(AM2315_ADDR);
  delay(10);
  Wire.endTransmission();
  
  // Tell the sensor which register(s) we want
  Wire.beginTransmission(AM2315_ADDR);
  Wire.write(AM2315_READ_REG);
  Wire.write(base_address);
  Wire.write(nbytes);
  Wire.endTransmission(true); // Device specification wants a stop at the end

  delay(10);

  // Read the register(s)
  nbytes_read = Wire.requestFrom(AM2315_ADDR, nbytes + AM2315_READ_OVERHEAD, true);
  if (nbytes_read != (nbytes + AM2315_READ_OVERHEAD)) {
    return 1;
  }

  // Shouldn't need to call Wire.available(), we've checked that we got the total
  // number of bytes that we want

  function_code = Wire.read(); // Check that the function performed is correct
  if (function_code != AM2315_READ_REG) {
    return 1;
  }

  nbytes_read = Wire.read(); // Consume the number of register read, isn't useful

  // Get the register values
  for (size_t i = 0; i < nbytes; i++) {
    data_read[i] = Wire.read();
  }

  // TODO: check the crc of the message
  crc = Wire.read() & 0x00FF;
  crc |= (Wire.read() << 8) & 0xFF00;

  return 0;
}

int am2315_read_temp(uint8_t *temperature) {
  assert(temperature != NULL);
  
  return am2315_read_registers(AM2315_REG_TEMP_HIGH, 2, temperature);
}

int am2315_read_humid(uint8_t *humidity) {
  assert(humidity != NULL);
  
  return am2315_read_registers(AM2315_REG_HUMID_HIGH, 2, humidity);
}

int am2315_read_temp_and_humid(uint8_t *temperature, uint8_t *humidity) {
  assert(humidity != NULL && temperature != NULL);

  uint8_t read_data[4];
  int ret_val;
  
  ret_val = am2315_read_registers(AM2315_REG_HUMID_HIGH, 4, read_data);
  if (!ret_val) {
    humidity[0] = read_data[0];
    humidity[1] = read_data[1];
    temperature[0] = read_data[2];
    temperature[1] = read_data[3];
  }

  return ret_val;
}

float am2315_convert_humidity(uint8_t *humidity) {
  assert(humidity != NULL);
  
  return ((humidity[0] << 8) | humidity[1]) / 10.0;
}

float am2315_convert_temperature(uint8_t *temperature) {
  assert(temperature != NULL);
  
  float temp = (((temperature[0] & 0x7F) << 8) | temperature[1]) / 10.0;

  if (temperature[0] >> 7) {
    temp *= -1;
  }

  return temp;
}
