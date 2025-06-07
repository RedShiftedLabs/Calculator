#include "Application.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  return (Calculator().show(), QApplication::exec());
}