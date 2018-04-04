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

#pragma once

#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_si7021.h"
#include <math.h>

#define MGOS_SI7021_DEFAULT_I2CADDR       (0x40)

#define MGOS_SI7021_MEASRH_HOLD_CMD       (0xE5)
#define MGOS_SI7021_MEASRH_NOHOLD_CMD     (0xF5)
#define MGOS_SI7021_MEASTEMP_HOLD_CMD     (0xE3)
#define MGOS_SI7021_MEASTEMP_NOHOLD_CMD   (0xF3)
#define MGOS_SI7021_READPREVTEMP_CMD      (0xE0)
#define MGOS_SI7021_RESET_CMD             (0xFE)
#define MGOS_SI7021_WRITERHT_REG_CMD      (0xE6)
#define MGOS_SI7021_READRHT_REG_CMD       (0xE7)
#define MGOS_SI7021_WRITEHEATER_REG_CMD   (0x51)
#define MGOS_SI7021_READHEATER_REG_CMD    (0x11)
#define MGOS_SI7021_ID1_CMD               (0xFA0F)
#define MGOS_SI7021_ID2_CMD               (0xFCC9)
#define MGOS_SI7021_FIRMVERS_CMD          (0x84B8)

#ifdef __cplusplus
extern "C" {
#endif

struct mgos_si7021 {
  struct mgos_i2c *i2c;
  uint8_t i2caddr;
  struct mgos_si7021_stats stats;

  float humidity, temperature;
};

#ifdef __cplusplus
}
#endif
