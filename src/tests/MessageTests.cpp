#include "tcp/messages/AbstractMessage.hpp"
#include "tcp/messages/ErrorResponseMessage.hpp"
#include "tcp/messages/Parser.hpp"
#include <catch2/catch.hpp>
#include <memory>
#include <string_view>

constexpr std::string_view TEST_TAG = "[MessageTests]";

TEST_CASE("Parsing ErrorResponseMessage", TEST_TAG.data()) {
    const std::string s = "{\"version\": 1,\
	\"action\": \"response\",\
	\"status\": 0,\
	\"error\": \"Some error message e.g. Invalid JSON format.\"}";

    std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
}