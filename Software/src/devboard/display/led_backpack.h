#ifndef LED_BACKPACK_H
#define LED_BACKPACK_H

#include <Wire.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "../utils/debounce_button.h"

// Voltage range for cell voltage display (2800mV to 4100mV mapped to 1-24 bars)
#define MIN_MV 2800
#define MAX_MV 4100

// I2C pins (custom pins per board)
#if defined(HW_LILYGO2CAN)
#define LED_BACKPACK_SDA 1
#define LED_BACKPACK_SCL 2
#else
#define LED_BACKPACK_SDA 18
#define LED_BACKPACK_SCL 25
#endif
#define LED_BACKPACK_I2C_ADDRESS 0x70

// Forward declaration
extern SemaphoreHandle_t i2c_mutex;

class LedBackpack24 {
 private:
  Adafruit_24bargraph bargraph;
  bool initialized = false;
  uint8_t last_bar_count = 0;
  uint8_t last_color = 0;  // 0=green, 1=red, 2=yellow
  uint16_t min_ever_mv = 0xFFFF;  // Track minimum cell voltage ever seen
  DebouncedButton reset_button;  // Reset button for ever-seen minimum
  bool reset_button_initialized = false;  // Track if reset button was successfully configured

 public:
  LedBackpack24();
  ~LedBackpack24();

  // Initialize I2C and backpack
  bool begin();

  // Update bargraph: percent (0-100), color: 0=green, 1=red, 2=yellow (for bicolor)
  void update_soc_display(uint8_t percent, uint8_t color = 0);

  // Update bargraph: 2800mV -> 1 bar, 4200mV -> 24 bars
  void update_min_cell_voltage_display(uint16_t millivolts, uint8_t color = 0);

  // Set individual bars (0-23 for 24-bar unit)
  void set_bar(uint8_t bar_num, bool state, uint8_t color = 0);

  // Clear all bars
  void clear_all();

  // Test pattern (light all bars)
  void test_pattern();

  // Reset the ever-seen minimum cell voltage
  void reset_min_cell_voltage() { min_ever_mv = 0xFFFF; }

  // Initialize the reset button
  bool init_reset_button();

  // Monitor the reset button (call this in main loop)
  void monitor_reset_button();

  bool is_initialized() { return initialized; }
};

extern LedBackpack24 led_backpack;

#endif
