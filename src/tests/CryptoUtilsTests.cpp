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
