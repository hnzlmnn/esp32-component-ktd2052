#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <sdkconfig.h>
#include <stdbool.h>

#include "ktd2052.h"
#include "managed_i2c.h"

static const char* TAG = "ktd2052";

/* I2C access */
static inline esp_err_t read_reg(KTD2052* device, uint8_t reg, uint8_t* data, size_t data_len) {
    if (device->i2c_semaphore != NULL) xSemaphoreTake(device->i2c_semaphore, portMAX_DELAY);
    esp_err_t res = i2c_read_reg(device->i2c_bus, device->i2c_addr, reg, data, data_len);
    if (device->i2c_semaphore != NULL) xSemaphoreGive(device->i2c_semaphore);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "read reg error %d", res);
    }
    return res;
}

static inline esp_err_t write_reg(KTD2052* device, uint8_t reg, uint8_t data) {
    if (device->i2c_semaphore != NULL) xSemaphoreTake(device->i2c_semaphore, portMAX_DELAY);
    esp_err_t res = i2c_write_reg(device->i2c_bus, device->i2c_addr, reg, data);
    if (device->i2c_semaphore != NULL) xSemaphoreGive(device->i2c_semaphore);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "write reg error %d", res);
    }
    return res;
}

static inline esp_err_t write_reg_n(KTD2052* device, uint8_t reg, uint8_t* data, size_t data_len) {
    if (device->i2c_semaphore != NULL) xSemaphoreTake(device->i2c_semaphore, portMAX_DELAY);
    esp_err_t res = i2c_write_reg_n(device->i2c_bus, device->i2c_addr, reg, data, data_len);
    if (device->i2c_semaphore != NULL) xSemaphoreGive(device->i2c_semaphore);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "write reg error %d", res);
    }
    return res;
}

static inline bool is_enabled(KTD2052* device) {
    uint8_t data[1];
    esp_err_t res = read_reg(device, KTD2052_REG_CONTROL, data, 1);
    if (res != ESP_OK) return false;
    return (data[0] & KTD2052_CONTROL_ENABLED) == KTD2052_CONTROL_ENABLED;
}

/* Public functions */

bool ktd2052_connected(KTD2052* device) {
    if (device->i2c_semaphore != NULL) xSemaphoreTake(device->i2c_semaphore, portMAX_DELAY);
    esp_err_t res = i2c_write_buffer(device->i2c_bus, device->i2c_addr, NULL, 0);
    if (device->i2c_semaphore != NULL) xSemaphoreGive(device->i2c_semaphore);
    return res == ESP_OK;
}

esp_err_t ktd2052_init(KTD2052* device) {
    ESP_LOGD(TAG, "init called");

    if (!ktd2052_connected(device)) {
        ESP_LOGI(TAG, "KTD2052 not connected, skipping init");
        return ESP_OK;
    }

    if (!is_enabled(device)) {
        // Wake up the chip from standby
        esp_err_t res = write_reg(device, KTD2052_REG_CONTROL, KTD2052_CONTROL_ENABLED);
        if (res != ESP_OK) return res;
    }

    ESP_LOGD(TAG, "init done");
    return ESP_OK;
}

esp_err_t ktd2052_set_color(KTD2052* device, int led, uint8_t red, uint8_t green, uint8_t blue) {
    if (led < 0 || led > 4) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    int len = led == 0 ? 12 : 3;
    int reg = KTD2052_RED_1;
    if (led > 0) {
        reg += (led - 1) * 3;
    }

    uint8_t buf[len];
    for (int i = 0; i < len; i += 3) {
        buf[i] = red;
        buf[i+1] = green;
        buf[i+2] = blue;
    }

    return write_reg_n(device, reg, buf, len);
}

esp_err_t ktd2052_set_color_pax(KTD2052* device, int led, pax_col_t rgb) {
    return ktd2052_set_color(device, led, (uint8_t) (rgb >> 16), (uint8_t) (rgb >> 8), (uint8_t) rgb);
}
