#include "led_backpack.h"
#include "../utils/logging.h"

// Global backpack instance
LedBackpack24 led_backpack;

LedBackpack24::LedBackpack24() : initialized(false), last_bar_count(0), last_color(0) {
}

LedBackpack24::~LedBackpack24() {
}

bool LedBackpack24::begin() {
  if (i2c_mutex == nullptr) {
    Serial.println("ERROR: I2C mutex not initialized!");
    DEBUG_PRINTF("ERROR: I2C mutex not initialized!\n");
    return false;
  }

  xSemaphoreTake(i2c_mutex, portMAX_DELAY);

  Serial.println("Initializing LED Backpack I2C...");
  
  // Initialize I2C with custom pins
  Wire.begin(LED_BACKPACK_SDA, LED_BACKPACK_SCL);
  Wire.setClock(100000);  // 100 kHz for reliable I2C

  Serial.println("I2C initialized, checking for bargraph at 0x70...");

  // Initialize the bargraph
  if (!bargraph.begin(LED_BACKPACK_I2C_ADDRESS)) {
    Serial.printf("ERROR: LED Backpack (0x%02X) not found on I2C bus!\n", LED_BACKPACK_I2C_ADDRESS);
    DEBUG_PRINTF("LED Backpack (0x%02X) not found on I2C bus!\n", LED_BACKPACK_I2C_ADDRESS);
    xSemaphoreGive(i2c_mutex);
    return false;
  }

  initialized = true;
  Serial.printf("LED Backpack initialized at address 0x%02X\n", LED_BACKPACK_I2C_ADDRESS);
  DEBUG_PRINTF("LED Backpack initialized at address 0x%02X\n", LED_BACKPACK_I2C_ADDRESS);

  // Test: light all bars briefly
  bargraph.writeDisplay();  // Clear first
  delay(100);

  Serial.println("LED Backpack begin() completed successfully");
  xSemaphoreGive(i2c_mutex);
  return true;
}

void LedBackpack24::update_soc_display(uint8_t percent, uint8_t color) {
  if (!initialized || i2c_mutex == nullptr) return;

  xSemaphoreTake(i2c_mutex, portMAX_DELAY);

  // Map 0-100% to 0-24 bars
  uint8_t bar_count = (percent / 100.0) * 24;
  if (bar_count > 24) bar_count = 24;

  // Only update if changed
  if (bar_count == last_bar_count && color == last_color) {
    xSemaphoreGive(i2c_mutex);
    return;
  }

  // Clear display first
  bargraph.clear();

  // Light up bars 0 to bar_count
  for (uint8_t i = 0; i < bar_count; i++) {
    // For bicolor, use LED_RED (1) for low, LED_GREEN (2) for high, or mix colors
    // The 24-bar backpack supports bicolor per LED
    // Set color based on state: green for 0-50%, yellow for 50-75%, red for 75-100%
    uint8_t led_color = LED_GREEN;  // Default green

    if (percent >= 75) {
      led_color = LED_RED;
    } else if (percent >= 50) {
      led_color = LED_YELLOW;
    }

    bargraph.setBar(i, led_color);
  }

  bargraph.writeDisplay();
  last_bar_count = bar_count;
  last_color = color;

  xSemaphoreGive(i2c_mutex);
}

void LedBackpack24::set_bar(uint8_t bar_num, bool state, uint8_t color) {
  if (!initialized || bar_num > 23 || i2c_mutex == nullptr) return;

  xSemaphoreTake(i2c_mutex, portMAX_DELAY);

  if (state) {
    uint8_t led_color = LED_GREEN;

    if (color == 1) {
      led_color = LED_RED;
    } else if (color == 2) {
      led_color = LED_YELLOW;
    }

    bargraph.setBar(bar_num, led_color);
  } else {
    bargraph.setBar(bar_num, LED_OFF);
  }

  bargraph.writeDisplay();

  xSemaphoreGive(i2c_mutex);
}

void LedBackpack24::clear_all() {
  if (!initialized || i2c_mutex == nullptr) return;

  xSemaphoreTake(i2c_mutex, portMAX_DELAY);

  bargraph.clear();
  bargraph.writeDisplay();
  last_bar_count = 0;
  last_color = 0;

  xSemaphoreGive(i2c_mutex);
}

void LedBackpack24::test_pattern() {
  if (!initialized || i2c_mutex == nullptr) return;

  xSemaphoreTake(i2c_mutex, portMAX_DELAY);

  // Light all bars in sequence
  for (uint8_t i = 0; i < 24; i++) {
    bargraph.clear();

    // Set bars 0 to i with alternating colors
    for (uint8_t j = 0; j <= i; j++) {
      uint8_t color = (j % 3 == 0) ? LED_GREEN : (j % 3 == 1) ? LED_RED : LED_YELLOW;
      bargraph.setBar(j, color);
    }

    bargraph.writeDisplay();
    delay(50);  // 50ms per step
  }

  bargraph.clear();
  bargraph.writeDisplay();

  xSemaphoreGive(i2c_mutex);
}
