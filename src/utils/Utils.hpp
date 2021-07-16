#pragma once

#include <cstddef>
#include <string>

namespace utils {
std::string secure_random_password(size_t length);
std::string url_safe_random_token(size_t length);
}  // namespace utils