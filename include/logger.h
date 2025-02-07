#pragma once
#include "../include/colors.h"
#include <iostream>

class Logger {
public:
  Logger();

  void log(const std::string msg);
  void info(const std::string msg);
  void debug(const std::string msg);
  void warning(const std::string msg);
  void error(const std::string msg);
  void panic(const std::string msg);
};
