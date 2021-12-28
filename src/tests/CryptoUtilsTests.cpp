#include "utils/CryptoUtils.hpp"
#include <catch2/catch.hpp>
#include <string>

constexpr std::string_view TEST_TAG = "[CryptoUtils]";

TEST_CASE("Hashing", TEST_TAG.data()) {
    const std::string s = "Hallo!";
    const std::string result1 = utils::hash_sah256(s);
    const std::string result2 = utils::hash_sah256(s);

    REQUIRE(result1 == result2);
    REQUIRE(result1 == "357a57fe73d6c63bb1923e970f79a924db3a3609eb9797547784a94ee38fc3df");
}

TEST_CASE("URL save random token", TEST_TAG.data()) {
    for (size_t i = 1; i < 100000; i++) {
        const std::string result = utils::url_safe_random_token(i);
        REQUIRE(result.size() == i);
        for (size_t e = 0; e < result.size(); e++) {
            REQUIRE(std::isalnum(result[i]));
        }
    }
}

TEST_CASE("Secure random password", TEST_TAG.data()) {
    for (size_t i = 1; i < 100000; i++) {
        const std::string result = utils::secure_random_password(i);
        REQUIRE(result.size() == i);
        for (size_t e = 0; e < result.size(); e++) {
            REQUIRE(std::isalnum(result[i]));
        }
    }
}
