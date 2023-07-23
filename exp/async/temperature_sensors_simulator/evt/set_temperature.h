#ifndef TEMPERATURE_SENSORS_SIMULATOR__EVT__SET_TEMPERATURE_H
#define TEMPERATURE_SENSORS_SIMULATOR__EVT__SET_TEMPERATURE_H

#include <cstdint>
#include <iostream>

#include "../dat/sensor_id.h"
#include "../dat/temperature.h"

namespace temperature_sensors_simulator::evt {

struct set_temperature {
  set_temperature() = default;
  explicit set_temperature(dat::sensor_id p_sensor_id,
                           dat::temperature p_temperature)
      : sensor_id(p_sensor_id), temperature(p_temperature) {}

  set_temperature(const set_temperature &) = default;
  set_temperature(set_temperature &&) = default;
  set_temperature &operator=(const set_temperature &) = default;
  set_temperature &operator=(set_temperature &&) = default;

  friend std::ostream &operator<<(std::ostream &p_out,
                                  const set_temperature &p_set_temperature) {
    p_out << '(' << p_set_temperature.sensor_id << ','
          << p_set_temperature.temperature << ')';
    return p_out;
  }

  dat::sensor_id sensor_id;
  dat::temperature temperature;
};

} // namespace temperature_sensors_simulator::evt

#endif // set_temperature_H