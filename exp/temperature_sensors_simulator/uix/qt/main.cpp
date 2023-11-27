#include "mainwindow.h"

#include <QApplication>

#include "../../alg/dispatcher.h"
#include "../../per/sensors.h"

using namespace temperature_sensors_simulator;

int main(int argc, char *argv[]) {
  using namespace temperature_sensors_simulator;

  alg::dispatcher::ptr _dispatcher{alg::dispatcher::create()};

  per::sensors _sensors{_dispatcher};

  QApplication a(argc, argv);
  MainWindow w(_dispatcher);
  w.show();
  return a.exec();
}