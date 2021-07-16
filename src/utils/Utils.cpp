#include "Utils.hpp"
#include <cassert>
#include <random>
#include <string>

namespace utils {
std::string secure_random_password(size_t length) {
    assert(length >= 1);

    std::string result;
    result.resize(length);
    for (size_t i = 0; i < length; i++) {
        // TODO: Use libsodium here: https://libsodium.gitbook.io/doc/generating_random_data
    }
    return result;
}

std::string url_safe_random_token(size_t length) {
    assert(length >= 1);

    static const std::string alphanum = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distr(static_cast<int>(alphanum.size()));

    std::string result;
    result.resize(length);
    for (size_t i = 0; i < length; i++) {
        result[i] = alphanum[distr(gen)];
    }
    return result;
}
}  // namespace utils