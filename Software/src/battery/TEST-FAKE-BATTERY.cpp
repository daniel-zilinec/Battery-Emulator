#include "TEST-FAKE-BATTERY.h"
#include <Arduino.h>
#include <math.h>
#include "../datalayer/datalayer.h"
#include "../devboard/utils/logging.h"

void TestFakeBattery::
    update_values() { /* This function puts fake values onto the parameters sent towards the inverter */

  datalayer_battery->status.real_soc = 5000;  // 50.00%

  datalayer_battery->status.soh_pptt = 9900;  // 99.00%

  //datalayer_battery->status.voltage_dV = 3700;  // 370.0V , value editable via webserver

  datalayer_battery->status.current_dA = 0;  // 0 A

  datalayer_battery->info.total_capacity_Wh = 30000;  // 30kWh

  datalayer_battery->status.remaining_capacity_Wh = 15000;  // 15kWh

  // Make all cell voltages follow a sine wave: 15s period, 3700mV center, 400mV amplitude
  const float period_ms = 15000.0f;
  const float center_mv = 3700.0f;
  const float amplitude_mv = 400.0f;
  const float phase = (2.0f * PI * (float)millis()) / period_ms;
  const float cell_mv_f = center_mv + amplitude_mv * sinf(phase);
  const uint16_t cell_mv = (uint16_t)roundf(cell_mv_f);

  datalayer_battery->status.cell_max_voltage_mV = cell_mv + random(0, 100);
  datalayer_battery->status.cell_min_voltage_mV = cell_mv;

  datalayer_battery->status.temperature_min_dC = 50;  // 5.0*C

  datalayer_battery->status.temperature_max_dC = 60;  // 6.0*C

  datalayer_battery->status.max_discharge_power_W = 5000;  // 5kW

  datalayer_battery->status.max_charge_power_W = 5000;  // 5kW

  for (int i = 0; i < 97; ++i) {
    datalayer_battery->status.cell_voltages_mV[i] = cell_mv + random(0, 100);
  }

  //Fake that we get CAN messages
  datalayer_battery->status.CAN_battery_still_alive = CAN_STILL_ALIVE;
}

void TestFakeBattery::handle_incoming_can_frame(CAN_frame rx_frame) {
  datalayer_battery->status.CAN_battery_still_alive = CAN_STILL_ALIVE;
}

void TestFakeBattery::transmit_can(unsigned long currentMillis) {
  // Fake battery has no CAN sending
}

void TestFakeBattery::setup(void) {  // Performs one time setup at startup
  randomSeed(analogRead(0));

  strncpy(datalayer.system.info.battery_protocol, Name, 63);
  datalayer.system.info.battery_protocol[63] = '\0';

  datalayer_battery->info.max_design_voltage_dV =
      4040;  // 404.4V, over this, charging is not possible (goes into forced discharge)
  datalayer_battery->info.min_design_voltage_dV = 2450;  // 245.0V under this, discharging further is disabled
  datalayer_battery->info.number_of_cells = 96;

  if (allows_contactor_closing) {
    *allows_contactor_closing = true;
  }
}
