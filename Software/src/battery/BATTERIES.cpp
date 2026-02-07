#include "BATTERIES.h"
#include "../datalayer/datalayer_extended.h"
#include "../devboard/hal/hal.h"
#include "../devboard/utils/logging.h"
#include "CanBattery.h"
#include "RS485Battery.h"

Battery* battery = nullptr;
Battery* battery2 = nullptr;
Battery* battery3 = nullptr;

std::vector<BatteryType> supported_battery_types() {
  std::vector<BatteryType> types;

  for (int i = 0; i < (int)BatteryType::Highest; i++) {
    types.push_back((BatteryType)i);
  }

  return types;
}

const char* name_for_chemistry(battery_chemistry_enum chem) {
  switch (chem) {
    case battery_chemistry_enum::Autodetect:
      return "Autodetect";
    case battery_chemistry_enum::LFP:
      return "LFP";
    case battery_chemistry_enum::NCA:
      return "NCA";
    case battery_chemistry_enum::NMC:
      return "NMC";
    case battery_chemistry_enum::ZEBRA:
      return "Molten Salt";
    default:
      return nullptr;
  }
}

const char* name_for_comm_interface(comm_interface comm) {
  return esp32hal->name_for_comm_interface(comm);
}

const char* name_for_battery_type(BatteryType type) {
  switch (type) {
    case BatteryType::None:
      return "None";
    case BatteryType::ImievCZeroIon:
      return ImievCZeroIonBattery::Name;
    case BatteryType::TestFake:
      return TestFakeBattery::Name;
    default:
      return nullptr;
  }
}

const battery_chemistry_enum battery_chemistry_default = battery_chemistry_enum::NMC;

battery_chemistry_enum user_selected_battery_chemistry = battery_chemistry_default;

BatteryType user_selected_battery_type = BatteryType::TestFake;  // Default to test battery
bool user_selected_second_battery = false;
bool user_selected_triple_battery = false;

Battery* create_battery(BatteryType type) {
  switch (type) {
    case BatteryType::None:
      return nullptr;
    case BatteryType::ImievCZeroIon:
      return new ImievCZeroIonBattery();
    case BatteryType::TestFake:
      return new TestFakeBattery();
    default:
      return nullptr;
  }
}

void setup_battery() {
  if (battery) {
    // Let's not create the battery again.
    return;
  }

  battery = create_battery(user_selected_battery_type);

  if (battery) {
    battery->setup();
  }

  if (user_selected_second_battery && !battery2) {
    switch (user_selected_battery_type) {
      case BatteryType::TestFake:
        battery2 = new TestFakeBattery(&datalayer.battery2, can_config.battery_double);
        break;
      default:
        DEBUG_PRINTF("User tried enabling double battery on non-supported integration!\n");
        break;
    }

    if (battery2) {
      battery2->setup();
    }
  }

  if (user_selected_triple_battery && !battery3) {
    DEBUG_PRINTF("User tried enabling triple battery on non-supported integration!\n");

    if (battery3) {
      battery3->setup();
    }
  }
}

/* User-selected Nissan LEAF settings */
bool user_selected_LEAF_interlock_mandatory = false;
/* User-selected Tesla settings */
bool user_selected_tesla_digital_HVIL = false;
uint16_t user_selected_tesla_GTW_country = 17477;
bool user_selected_tesla_GTW_rightHandDrive = true;
uint16_t user_selected_tesla_GTW_mapRegion = 2;
uint16_t user_selected_tesla_GTW_chassisType = 2;
uint16_t user_selected_tesla_GTW_packEnergy = 1;
/* User-selected EGMP+others settings */
bool user_selected_use_estimated_SOC = false;
uint16_t user_selected_pylon_baudrate = 500;

// Use 0V for user selected cell/pack voltage defaults (On boot will be replaced with saved values from NVM)
uint16_t user_selected_max_pack_voltage_dV = 0;
uint16_t user_selected_min_pack_voltage_dV = 0;
uint16_t user_selected_max_cell_voltage_mV = 0;
uint16_t user_selected_min_cell_voltage_mV = 0;
