#pragma once

#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#define MAX_DEVICES 3

#define KTD2052_A_ADDRESS    0x74

#define KTD2052_REG_CONTROL 0x02

#define KTD2052_CONTROL_ENABLED 0x80

#define KTD2052_RED_1   0x03
#define KTD2052_GREEN_1 0x04
#define KTD2052_BLUE_1  0x05

#define KTD2052_RED_2   0x06
#define KTD2052_GREEN_2 0x07
#define KTD2052_BLUE_2  0x08

#define KTD2052_RED_3   0x09
#define KTD2052_GREEN_3 0x0a
#define KTD2052_BLUE_3  0x0b

#define KTD2052_RED_4   0x0c
#define KTD2052_GREEN_4 0x0d
#define KTD2052_BLUE_4  0x0e


typedef struct KTD2052 {
    // Pins
    int     i2c_bus;
    uint8_t i2c_addr;
    // Mutex
    SemaphoreHandle_t i2c_semaphore;
} KTD2052;

extern bool ktd2052_connected(KTD2052* device);

extern esp_err_t ktd2052_init(KTD2052* device);

/**
 * Sets the color of one or all LEDs
 * @param device The device reference
 * @param led The LED to set (0 for all)
 * @param red The red value
 * @param green The green value
 * @param blue The blue value
 */
extern esp_err_t ktd2052_set_color(KTD2052* device, int led, uint8_t red, uint8_t green, uint8_t blue);

__END_DECLS
