#include <Wire.h>
#include "src/am2315_arduino/am2315.h"

extern TwoWire Wire1;

#define SLAVE_ADDRESS 0x08
#define REGISTER_COUNT 4

#define I2C_RET_OKAY 0x00
#define I2C_RET_OUT_OF_BOUNDS 0x01
#define I2C_RET_TOO_LARGE 0x02
#define I2C_RET_TOO_SMALL 0x03

#define I2C_COMMAND 0x00
#define I2C_DATA 0x01

uint8_t i2c_state = I2C_COMMAND;
uint8_t i2c_status = I2C_RET_OKAY;
uint8_t i2c_register = 0;
size_t read_count = 1;

uint8_t registers[REGISTER_COUNT];
uint8_t *humidity = registers;
uint8_t *temperature = registers + 2;

void setup() {
  Wire.begin();
  
  Wire1.begin(SLAVE_ADDRESS);
  Wire1.onReceive(receiveEvent);
  Wire1.onRequest(requestEvent);
  
  Serial.begin(9600);
}

void loop() {
  int err;

  err = am2315_read_temp_and_humid(temperature, humidity);
  if (err) {
    Serial.println("ERROR IN READING AM2315");
  } else {
    Serial.println("Humidity\tTemperature");
    Serial.print(am2315_convert_humidity(humidity));
    Serial.print("%\t");
    Serial.print(am2315_convert_temperature(temperature));
    Serial.println("C");
  }

  Serial.println();

  delay(2000);
}

void receiveEvent(int howMany) {
  if (i2c_state == I2C_COMMAND) {
    i2c_register = Wire1.read();
    
    if (howMany > 1) {
      read_count = Wire1.read();
    }

    i2c_state = I2C_DATA;

    return;
  }

  if (i2c_register >= REGISTER_COUNT) {
    i2c_status = I2C_RET_OUT_OF_BOUNDS;

    return;
  }

  size_t write_count = howMany;
  if (i2c_register + write_count > REGISTER_COUNT) {
    write_count = REGISTER_COUNT - i2c_register;
    i2c_status = I2C_RET_TOO_LARGE;
  }

  for (size_t i = 0; i < write_count; i++) {
    registers[i2c_register + i] = Wire1.read();
  }

  i2c_state = I2C_COMMAND;
  
  Serial.print("Register Write: ");
  Serial.print(i2c_register);
  Serial.print(" ");
  Serial.println(write_count);
}

void requestEvent() {
  if (i2c_register >= REGISTER_COUNT) {
    i2c_status = I2C_RET_OUT_OF_BOUNDS;

    return;
  } else if (i2c_register + read_count > REGISTER_COUNT) {
    read_count = REGISTER_COUNT - i2c_register;
    i2c_status = I2C_RET_TOO_LARGE;
  }

  for (int i = 0; i < read_count; i++) {
    Wire1.write(registers[i2c_register + i]);
  }

  i2c_state = I2C_COMMAND;

  Serial.print("Register Read: ");
  Serial.print(i2c_register);
  Serial.print(" ");
  Serial.println(read_count);
}
