#include "logger/Logger.hpp"
#include <glog/logging.h>

namespace logger {
void init_logger(const char* applicationName) {
    google::InitGoogleLogging(applicationName);
    google::LogToStderr();
}
}  // namespace logger
