/*
 * Copyright 2018 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mgos.h"
#include "mgos_si7021_internal.h"
#include "mgos_i2c.h"

// Datasheet:
// https://cdn-learn.adafruit.com/assets/assets/000/035/931/original/Support_Documents_TechnicalDocs_Si7021-A20.pdf

// Private functions follow
static uint8_t crc8(const uint8_t *data, int len) {
  const uint8_t poly = 0x31;
  uint8_t       crc  = 0x00;

  for (int j = len; j; --j) {
    crc ^= *data++;
    for (int i = 8; i; --i) {
      crc = (crc & 0x80) ? (crc << 1) ^ poly : (crc << 1);
    }
  }
  return crc;
}

// Private functions end

// Public functions follow
struct mgos_si7021 *mgos_si7021_create(struct mgos_i2c *i2c, uint8_t i2caddr) {
  struct mgos_si7021 *sensor;
  int ret;

  if (!i2c) {
    return NULL;
  }

  // Reset and query register
  ret = mgos_i2c_read_reg_b(i2c, i2caddr, MGOS_SI7021_READRHT_REG_CMD);
  if (ret != 0x3A) {
    LOG(LL_ERROR, ("Chip ID register invalid, expected 0x3A got 0x%02x (%d)", ret, ret));
    return NULL;
  }

  sensor = calloc(1, sizeof(struct mgos_si7021));
  if (!sensor) {
    return NULL;
  }

  memset(sensor, 0, sizeof(struct mgos_si7021));
  sensor->i2caddr = i2caddr;
  sensor->i2c     = i2c;

  LOG(LL_INFO, ("SI7021 created at I2C 0x%02x", i2caddr));
  return sensor;
}

void mgos_si7021_destroy(struct mgos_si7021 **sensor) {
  if (!*sensor) {
    return;
  }

  free(*sensor);
  *sensor = NULL;
  return;
}

bool mgos_si7021_read(struct mgos_si7021 *sensor) {
  double start = mg_time();

  if (!sensor || !sensor->i2c) {
    return false;
  }

  sensor->stats.read++;

  if (start - sensor->stats.last_read_time < MGOS_SI7021_READ_DELAY) {
    sensor->stats.read_success_cached++;
    return true;
  }
  // Read out sensor data here
  //
  uint8_t data[3];
  uint8_t cmd = MGOS_SI7021_MEASRH_NOHOLD_CMD;

  if (!mgos_i2c_write(sensor->i2c, sensor->i2caddr, &cmd, 1, false)) {
    LOG(LL_ERROR, ("Could not write command"));
    return false;
  }
  mgos_usleep(25000);
  if (!mgos_i2c_read(sensor->i2c, sensor->i2caddr, data, 3, true)) {
    LOG(LL_ERROR, ("Could not read command"));
    return false;
  }
  if (data[2] != crc8(&data[0], 2)) {
    LOG(LL_ERROR, ("CRC error on humidity data"));
    return false;
  }

  uint16_t hum      = (data[0] << 8) + data[1];
  float    humidity = hum;
  humidity        *= 125;
  humidity        /= 65536;
  humidity        -= 6;
  sensor->humidity = humidity;

  cmd = MGOS_SI7021_MEASTEMP_NOHOLD_CMD;

  if (!mgos_i2c_write(sensor->i2c, sensor->i2caddr, &cmd, 1, false)) {
    LOG(LL_ERROR, ("Could not write command"));
    return false;
  }
  mgos_usleep(25000);
  if (!mgos_i2c_read(sensor->i2c, sensor->i2caddr, data, 3, true)) {
    LOG(LL_ERROR, ("Could not read command"));
    return false;
  }
  if (data[2] != crc8(&data[0], 2)) {
    LOG(LL_ERROR, ("CRC error on temperature data"));
    return false;
  }

  uint16_t temp        = (data[0] << 8) + data[1];
  float    temperature = temp;
  temperature        *= 175.72;
  temperature        /= 65536;
  temperature        -= 46.85;
  sensor->temperature = temperature;

  LOG(LL_DEBUG, ("temperature=%.2fC humidity=%.1f%%", sensor->temperature, sensor->humidity));
  sensor->stats.read_success++;
  sensor->stats.read_success_usecs += 1000000 * (mg_time() - start);
  sensor->stats.last_read_time      = start;
  return true;
}

float mgos_si7021_getTemperature(struct mgos_si7021 *sensor) {
  if (!mgos_si7021_read(sensor)) {
    return NAN;
  }

  return sensor->temperature;
}

float mgos_si7021_getHumidity(struct mgos_si7021 *sensor) {
  if (!mgos_si7021_read(sensor)) {
    return NAN;
  }

  return sensor->humidity;
}

bool mgos_si7021_getStats(struct mgos_si7021 *sensor, struct mgos_si7021_stats *stats) {
  if (!sensor || !stats) {
    return false;
  }

  memcpy((void *)stats, (const void *)&sensor->stats, sizeof(struct mgos_si7021_stats));
  return true;
}

bool mgos_si7021_i2c_init(void) {
  return true;
}

// Public functions end
