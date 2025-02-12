/// \copyright This file is under GPL 3 license. Please read the \p LICENSE file
/// at the root of \p tenacitas directory

/// \author Rodrigo Canellas - rodrigo.canellas at gmail.com

/// \example temperature_sensors_simulator/uix/qt/main.cpp

#include "mainwindow.h"

#include <QApplication>

#include "tnct/async/exp/temperature_sensors_simulator/per/sensors.h"
#include "tnct/async/exp/temperature_sensors_simulator/supplier.h"

int main(int argc, char *argv[]) {
  using namespace tnct::async::exp::temperature_sensors_simulator;

  logger _logger;
  dispatcher _dispatcher(_logger);

  per::sensors<logger, dispatcher> _sensors(_logger, _dispatcher);

  QApplication a(argc, argv);
  MainWindow w(_logger, _dispatcher);
  w.show();
  return a.exec();
}
