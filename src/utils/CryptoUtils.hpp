#pragma once

#include <cstddef>
#include <string>

namespace utils {
/**
 * Generates a random, cryptographically secure password with the given amount of characters.
 * Uses only alpha numerical values as characters.
 **/
std::string secure_random_password(size_t length);
/**
 * Generates a random, NOT cryptographically secure token/word with the given amount of charactres.
 * Uses only alpha numerical values as characters.
 **/
std::string url_safe_random_token(size_t length);
}  // namespace utils