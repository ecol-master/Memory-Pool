#include <fstream>
#include <iostream>

enum class LogLevel {
  //
  ERROR = 0,
  WARNING = 1,
  INFO = 2,
};

enum class LogOutput {
  //
  CONSOLE,
  FILE
};

class Logger {
private:
  static Logger *logger_;

  LogOutput log_output_;
  std::ofstream log_file_;

protected:
  Logger() { log_output_ = LogOutput::CONSOLE; };
  Logger(std::string log_filename_);

  ~Logger() = default;

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

  void Log(LogLevel _level, std::string _message);

  void set_output_file(std::string output_filename_);
};
