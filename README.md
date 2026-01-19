# VL53L1X ULP for ESP-IDF

This is an ESP-IDF (v5.5+) port of ST's VL53L1X Ultra Low Power (ULP) API, packaged as an ESP-IDF component.


# VL53L1X_Library (ESP-IDF)

VL53L1X **Ultra Low Power (ULP)** driver as an ESP-IDF component, using the ESP-IDF v5.5+ **new I2C master driver** (`driver/i2c_master.h`).

This repository ports ST's *VL53L1X ULP API* (from STSW-IMG032) to ESP-IDF by implementing the required platform functions (`VL53L1X_ULP_RdByte/...`, `VL53L1X_ULP_WaitMs`) on top of `i2c_master_transmit_receive()` and friends.

## Address convention (0x52 vs 0x29)

ST's ULP API uses the **8-bit** I2C address convention.
- Default: **0x52** (write) / 0x53 (read)

ESP-IDF expects a **7-bit** address. This component accepts ST's 8-bit address at the API boundary and shifts right internally.

## Usage (minimal)

```c
#include "VL53L1X_ULP_api.h"
#include "vl53l1x_ulp_esp.h"

// after creating I2C bus handle:
uint16_t dev = 0;
ESP_ERROR_CHECK(vl53l1x_ulp_esp_add_device(bus, 0x52, &dev));

uint16_t id = 0;
VL53L1X_ULP_GetSensorId(dev, &id);
```

## License

- ST source files are dual-licensed (ST proprietary or BSD-3-Clause, as stated in file headers).
- This component's ESP-IDF platform adaptation is intended to be BSD-3-Clause.
