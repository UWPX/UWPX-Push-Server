#include "CryptoUtils.hpp"
#include <array>
#include <cassert>
#include <cstddef>
#include <random>
#include <string>
#include <sodium.h>
#include <sodium/crypto_hash_sha256.h>
#include <sodium/randombytes.h>

namespace utils {
std::string secure_random_password(size_t length) {
    assert(length >= 1);

    static const std::string alphanum = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::string result;
    result.resize(length);
    for (size_t i = 0; i < length; i++) {
        result[i] = alphanum[randombytes_uniform(alphanum.length() - 1)];
    }
    return result;
}

std::string url_safe_random_token(size_t length) {
    assert(length >= 1);

    static const std::string alphanum = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distr(0, static_cast<int>(alphanum.length() - 1));

    std::string result;
    result.resize(length);
    for (size_t i = 0; i < length; i++) {
        int e = distr(gen);
        result[i] = alphanum[e];
        assert(result[i]);
    }
    return result;
}

std::string hash_sah256(const std::string& s) {
    std::array<unsigned char, crypto_hash_sha256_BYTES> buf{};
    // NOLINTNEXTLINE (cppcoreguidelines-pro-type-reinterpret-cast)
    crypto_hash_sha256(buf.data(), reinterpret_cast<const unsigned char*>(s.data()), s.length());
    std::array<char, crypto_hash_sha256_BYTES * 2 + 1> bufHex{};
    sodium_bin2hex(bufHex.data(), bufHex.size(), buf.data(), buf.size());
    return std::string{bufHex.data(), bufHex.size() - 1};
}
}  // namespace utils