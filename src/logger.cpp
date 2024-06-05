#include "../includes/logger.h"
#include <string>

Logger* Logger::logger_{nullptr};

void Logger::Log(LogLevel _level, std::string _message) {
  std::string msg_prefix_;

  switch (_level) {
  case LogLevel::INFO:
    msg_prefix_ = "[INFO]";
    break;
  case LogLevel::ERROR:
    msg_prefix_ = "[ERROR]";
    break;
  case LogLevel::WARNING:
    msg_prefix_ = "[WARNING]";
    break;
  default:
    msg_prefix_ = "[]";
    break;
  }

  std::string log_msg_ = msg_prefix_ + ": " + _message;
  if (log_output_ == LogOutput::CONSOLE) {
    std::cout << log_msg_ << "\n";
  } else {
    log_file_ << log_msg_ << "\n";
  }
}

void Logger::set_output_file(std::string output_filename_) {
  log_output_ = LogOutput::FILE;
  log_file_.open(output_filename_);

  if (!log_file_.good()) {
    std::cout << "can not set output file: " << output_filename_ << "\n";
    log_output_ = LogOutput::CONSOLE;
  }
}
