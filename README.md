# Si7021 I2C Driver

A Mongoose library for Silicon Si7021 integrated circuit.

## Sensor details

The Si7021 I2C Humidity and Temperature Sensor is a monolithic CMOS IC
integrating humidity and temperature sensor elements, an analog-to-digital
converter, signal processing, calibration data, and an I2C Interface. The patented
use of industry-standard, low-K polymeric dielectrics for sensing humidity enables
the construction of low-power, monolithic CMOS Sensor ICs with low drift and
hysteresis, and excellent long term stability.

The humidity and temperature sensors are factory-calibrated and the calibration
data is stored in the on-chip non-volatile memory. This ensures that the sensors
are fully interchangeable, with no recalibration or software changes required.

The Si7021 is available in a 3x3 mm DFN package and is reflow solderable. It can
be used as a hardware- and software-compatible drop-in upgrade for existing RH/
temperature sensors in 3x3 mm DFN-6 packages, featuring precision sensing
over a wider range and lower power consumption. The optional factory-installed
cover offers a low profile, convenient means of protecting the sensor during
assembly (e.g., reflow soldering) and throughout the life of the product, excluding
liquids (hydrophobic/oleophobic) and particulates.

The Si7021 offers an accurate, low-power, factory-calibrated digital solution ideal
for measuring humidity, dew-point, and temperature, in applications ranging from
HVAC/R and asset tracking to industrial and consumer platforms.

See [datasheet](https://www.silabs.com/documents/public/data-sheets/Si7021-A20.pdf)
for implementation details.

A great place to buy a ready made and tested unit is at [Adafruit](https://learn.adafruit.com/adafruit-si7021-temperature-plus-humidity-sensor).

## Example application

An example program using a timer to read data from the sensor every 5 seconds:

```
#include "mgos.h"
#include "mgos_i2c.h"
#include "mgos_si7021.h"

static struct mgos_si7021 *s_si7021;

static void timer_cb(void *user_data) {
  float temperature, humidity;

  temperature=mgos_si7021_getTemperature(s_si7021);
  humidity=mgos_si7021_getHumidity(s_si7021);

  LOG(LL_INFO, ("si7021 temperature=%.2f humidity=%.2f", temperature, humidity));

  (void) user_data;
}

enum mgos_app_init_result mgos_app_init(void) {
  struct mgos_i2c *i2c;

  i2c=mgos_i2c_get_global();
  if (!i2c) {
    LOG(LL_ERROR, ("I2C bus missing, set i2c.enable=true in mos.yml"));
  } else {
    s_si7021=mgos_si7021_create(i2c, 0x40); // Default I2C address
    if (s_si7021) {
      mgos_set_timer(5000, true, timer_cb, NULL);
    } else {
      LOG(LL_ERROR, ("Could not initialize sensor"));
    }
  }
  return MGOS_APP_INIT_SUCCESS;
}
```

# Disclaimer

This project is not an official Google project. It is not supported by Google
and Google specifically disclaims all warranties as to its quality,
merchantability, or fitness for a particular purpose.
