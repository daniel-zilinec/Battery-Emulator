#ifndef LED_BACKPACK_H
#define LED_BACKPACK_H

#include <Wire.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

// I2C pins (custom pins for ESP32)
#define LED_BACKPACK_SDA 18
#define LED_BACKPACK_SCL 25
#define LED_BACKPACK_I2C_ADDRESS 0x70

// Forward declaration
extern SemaphoreHandle_t i2c_mutex;

class LedBackpack24 {
 private:
  Adafruit_24bargraph bargraph;
  bool initialized = false;
  uint8_t last_bar_count = 0;
  uint8_t last_color = 0;  // 0=green, 1=red, 2=yellow

 public:
  LedBackpack24();
  ~LedBackpack24();

  // Initialize I2C and backpack
  bool begin();

  // Update bargraph: percent (0-100), color: 0=green, 1=red, 2=yellow (for bicolor)
  void update_soc_display(uint8_t percent, uint8_t color = 0);

  // Set individual bars (0-23 for 24-bar unit)
  void set_bar(uint8_t bar_num, bool state, uint8_t color = 0);

  // Clear all bars
  void clear_all();

  // Test pattern (light all bars)
  void test_pattern();

  bool is_initialized() { return initialized; }
};

extern LedBackpack24 led_backpack;

#endif
