#pragma  once

#include <fstream>
#include <iostream>

#define LOG_FILE "info.log"
#define LOG_FLAGS std::ofstream::out

enum LogLevel {
  ERROR = 0,
  WARNING = 1,
  INFO = 2,
};

enum LogOutput {
  CONSOLE = 1,
  JOURNAL = 2,
};

class Logger {
private:
  static Logger *logger_;

  LogOutput log_output_;
  std::ofstream log_file_;

protected:
  Logger(){
    log_file_.open(LOG_FILE, LOG_FLAGS);
    log_output_ = JOURNAL;
  };
  Logger(std::string log_filename_);

  ~Logger() { log_file_.close(); };

public:
  // удаление методов копирования и присваивания
  Logger(Logger &other) = delete;
  void operator=(const Logger &) = delete;

  static Logger *get_instance() {
    if (logger_ == nullptr) {
      logger_ = new Logger();
    }
    return logger_;
  };

  void log(LogLevel _level, std::string _message);

  void set_output_file(std::string output_filename_);
};
