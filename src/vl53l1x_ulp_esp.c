#include "vl53l1x_ulp_esp.h"

#include <string.h>
#include <stdbool.h>

#include "esp_log.h"

#define TAG "vl53l1x_ulp_esp"

typedef struct {
    bool in_use;
    uint8_t addr8;
    i2c_master_dev_handle_t dev;
} vl53l1x_slot_t;

#ifndef VL53L1X_ULP_MAX_DEVICES
#define VL53L1X_ULP_MAX_DEVICES 4
#endif

static vl53l1x_slot_t s_slots[VL53L1X_ULP_MAX_DEVICES];

// Exposed to platform implementation
bool vl53l1x_ulp_esp_lookup(uint16_t dev_addr_8bit, i2c_master_dev_handle_t *out)
{
    uint8_t a8 = (uint8_t)dev_addr_8bit;
    for (int i = 0; i < VL53L1X_ULP_MAX_DEVICES; i++) {
        if (s_slots[i].in_use && s_slots[i].addr8 == a8) {
            if (out) *out = s_slots[i].dev;
            return true;
        }
    }
    return false;
}

esp_err_t vl53l1x_ulp_esp_add_device(i2c_master_bus_handle_t bus,
                                    uint8_t dev_addr_8bit,
                                    uint16_t *out_dev_addr)
{
    if (!bus) return ESP_ERR_INVALID_ARG;
    if ((dev_addr_8bit & 0x01) != 0) {
        // ST uses even 8-bit addresses for write; 0x52 is typical.
        ESP_LOGW(TAG, "8-bit address 0x%02X is odd; ST convention expects even (write) address", dev_addr_8bit);
    }

    // Already added?
    i2c_master_dev_handle_t existing;
    if (vl53l1x_ulp_esp_lookup(dev_addr_8bit, &existing)) {
        if (out_dev_addr) *out_dev_addr = dev_addr_8bit;
        return ESP_OK;
    }

    int free_idx = -1;
    for (int i = 0; i < VL53L1X_ULP_MAX_DEVICES; i++) {
        if (!s_slots[i].in_use) { free_idx = i; break; }
    }
    if (free_idx < 0) return ESP_ERR_NO_MEM;

    i2c_device_config_t cfg = {
        .device_address = (uint16_t)(dev_addr_8bit >> 1),
        .scl_speed_hz   = 400000,
    };

    i2c_master_dev_handle_t dev = NULL;
    esp_err_t err = i2c_master_bus_add_device(bus, &cfg, &dev);
    if (err != ESP_OK) return err;

    s_slots[free_idx].in_use = true;
    s_slots[free_idx].addr8  = dev_addr_8bit;
    s_slots[free_idx].dev    = dev;

    if (out_dev_addr) *out_dev_addr = dev_addr_8bit;

    ESP_LOGI(TAG, "Added VL53L1X ULP device: addr8=0x%02X addr7=0x%02X", dev_addr_8bit, dev_addr_8bit >> 1);
    return ESP_OK;
}

esp_err_t vl53l1x_ulp_esp_remove_device(uint8_t dev_addr_8bit)
{
    for (int i = 0; i < VL53L1X_ULP_MAX_DEVICES; i++) {
        if (s_slots[i].in_use && s_slots[i].addr8 == dev_addr_8bit) {
            // i2c_master_bus_rm_device exists in IDF v5.5+
            esp_err_t err = i2c_master_bus_rm_device(s_slots[i].dev);
            if (err != ESP_OK) return err;
            memset(&s_slots[i], 0, sizeof(s_slots[i]));
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}
