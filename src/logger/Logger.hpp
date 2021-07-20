#pragma once

#include <glog/logging.h>

// NOLINTNEXTLINE (cppcoreguidelines-avoid-non-const-global-variables)
#define LOG_DEBUG LOG_IF(INFO, VLOG_IS_ON(0))
// NOLINTNEXTLINE (cppcoreguidelines-avoid-non-const-global-variables)
#define LOG_INFO COMPACT_GOOGLE_LOG_##INFO.stream()
// NOLINTNEXTLINE (cppcoreguidelines-avoid-non-const-global-variables)
#define LOG_WARNING COMPACT_GOOGLE_LOG_##WARNING.stream()
// NOLINTNEXTLINE (cppcoreguidelines-avoid-non-const-global-variables)
#define LOG_ERROR COMPACT_GOOGLE_LOG_##ERROR.stream()
// NOLINTNEXTLINE (cppcoreguidelines-avoid-non-const-global-variables)
#define LOG_FATAL COMPACT_GOOGLE_LOG_##FATAL.stream()

namespace logger {
void init_logger(const char* applicationName);

}  // namespace logger
