#ifndef TEMPERATURE_SENSORS_SIMULATOR_EVT_ADD_SENSOR_H
#define TEMPERATURE_SENSORS_SIMULATOR_EVT_ADD_SENSOR_H

#include <iostream>

#include <tenacitas.lib/async/example/temperature_sensors_simulator/typ/sensor_id.h>

namespace temperature_sensors_simulator::evt {

struct add_sensor {
  add_sensor() = default;
  explicit add_sensor(typ::sensor_id p_sensor_id) : sensor_id(p_sensor_id) {}

  add_sensor(const add_sensor &) = default;
  add_sensor(add_sensor &&) = default;
  add_sensor &operator=(const add_sensor &) = default;
  add_sensor &operator=(add_sensor &&) = default;

  friend std::ostream &operator<<(std::ostream &p_out,
                                  const add_sensor &p_add_sensor) {
    p_out << p_add_sensor.sensor_id;
    return p_out;
  }

  typ::sensor_id sensor_id;
};

} // namespace temperature_sensors_simulator::evt

#endif // ADD_SENSOR_H
