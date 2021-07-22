#include "tcp/messages/AbstractMessage.hpp"
#include "tcp/messages/ErrorResponseMessage.hpp"
#include "tcp/messages/Parser.hpp"
#include "tcp/messages/RequestTestPushMessage.hpp"
#include "tcp/messages/SetChannelUriMessage.hpp"
#include "tcp/messages/SetPushAccountsMessage.hpp"
#include "tcp/messages/SuccessSetPushAccountsMessage.hpp"
#include <catch2/catch.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <vector>

constexpr std::string_view TEST_TAG = "[MessageTests]";

TEST_CASE("Parsing ErrorResponseMessage", TEST_TAG.data()) {
    const std::string s = "{\"version\": 1,\
	\"action\": \"response\",\
	\"status\": 0,\
	\"error\": \"Some error message e.g. Invalid JSON format.\"}";

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "response");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::ERROR_RESPONSE_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::ErrorResponseMessage* actMsg = static_cast<tcp::messages::ErrorResponseMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_error() == "Some error message e.g. Invalid JSON format.");
    REQUIRE(actMsg->get_status() == tcp::messages::ErrorResponseMessage::STATUS);
}

TEST_CASE("Serializing & Parsing ErrorResponseMessage", TEST_TAG.data()) {
    tcp::messages::ErrorResponseMessage m(std::string{"SOME Error"});

    nlohmann::json j;
    m.to_json(j);
    const std::string s = j.dump();

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "response");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::ERROR_RESPONSE_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::ErrorResponseMessage* actMsg = static_cast<tcp::messages::ErrorResponseMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_error() == "SOME Error");
    REQUIRE(actMsg->get_status() == tcp::messages::ErrorResponseMessage::STATUS);
}

TEST_CASE("Parsing SuccessResponseMessage", TEST_TAG.data()) {
    const std::string s = "{\"version\": 1,\
	\"action\": \"response\",\
	\"status\": 1}";

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "response");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::SUCCESS_RESPONSE_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::SuccessResponseMessage* actMsg = static_cast<tcp::messages::SuccessResponseMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_status() == tcp::messages::SuccessResponseMessage::STATUS);
}

TEST_CASE("Serializing & Parsing SuccessResponseMessage", TEST_TAG.data()) {
    tcp::messages::SuccessResponseMessage m;

    nlohmann::json j;
    m.to_json(j);
    const std::string s = j.dump();

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "response");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::SUCCESS_RESPONSE_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::SuccessResponseMessage* actMsg = static_cast<tcp::messages::SuccessResponseMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_status() == tcp::messages::SuccessResponseMessage::STATUS);
}

TEST_CASE("Parsing RequestTestPushMessage", TEST_TAG.data()) {
    const std::string s = "{\"version\": 1,\
	\"action\": \"request_test_push\",\
	\"device_id\": \"5486bd868050a620141f4e81c9f1d2c67ab0de27e5e26d218ca41c9394ee806b\"}";

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "request_test_push");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::REQUEST_TEST_PUSH_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::RequestTestPushMessage* actMsg = static_cast<tcp::messages::RequestTestPushMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_device_id() == "5486bd868050a620141f4e81c9f1d2c67ab0de27e5e26d218ca41c9394ee806b");
}

TEST_CASE("Serializing & Parsing RequestTestPushMessage", TEST_TAG.data()) {
    tcp::messages::RequestTestPushMessage m(std::string{"adawdafsefse"});

    nlohmann::json j;
    m.to_json(j);
    const std::string s = j.dump();

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "request_test_push");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::REQUEST_TEST_PUSH_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::RequestTestPushMessage* actMsg = static_cast<tcp::messages::RequestTestPushMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_device_id() == "adawdafsefse");
}

TEST_CASE("Parsing SetChannelUriMessage", TEST_TAG.data()) {
    const std::string s = "{\"version\": 1,\
	\"action\": \"set_channel_uri\",\
	\"device_id\": \"5486bd868050a620141f4e81c9f1d2c67ab0de27e5e26d218ca41c9394ee806b\",\
    \"channel_uri\": \"ms-app://s-1-15-2-3598129719-3378870262-4208132049-182512184-2493220926-1891298429-4035237700\"}";

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "set_channel_uri");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::SET_CHANNEL_URI_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::SetChannelUriMessage* actMsg = static_cast<tcp::messages::SetChannelUriMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_device_id() == "5486bd868050a620141f4e81c9f1d2c67ab0de27e5e26d218ca41c9394ee806b");
    REQUIRE(actMsg->get_channel_uri() == "ms-app://s-1-15-2-3598129719-3378870262-4208132049-182512184-2493220926-1891298429-4035237700");
}

TEST_CASE("Serializing & Parsing SetChannelUriMessage", TEST_TAG.data()) {
    tcp::messages::SetChannelUriMessage m(std::string{"sersesfefes"}, std::string{"adawdafsefse"});

    nlohmann::json j;
    m.to_json(j);
    const std::string s = j.dump();

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "set_channel_uri");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::SET_CHANNEL_URI_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::SetChannelUriMessage* actMsg = static_cast<tcp::messages::SetChannelUriMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_device_id() == "adawdafsefse");
    REQUIRE(actMsg->get_channel_uri() == "sersesfefes");
}

TEST_CASE("Parsing SetPushAccountsMessage", TEST_TAG.data()) {
    const std::string s = "{\"version\": 1,\
	\"action\": \"set_push_accounts\",\
	\"device_id\": \"5486bd868050a620141f4e81c9f1d2c67ab0de27e5e26d218ca41c9394ee806b\",\
    \"accounts\": [\
    {\"bare_jid\": \"someClient@xmpp.uwpx.org\"},\
    {\"bare_jid\": \"someOtherClient@xmpp.uwpx.org\"}\
    ]}";

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "set_push_accounts");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::SET_PUSH_ACCOUNT_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::SetPushAccountsMessage* actMsg = static_cast<tcp::messages::SetPushAccountsMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_device_id() == "5486bd868050a620141f4e81c9f1d2c67ab0de27e5e26d218ca41c9394ee806b");
    REQUIRE(actMsg->get_accounts().size() == 2);
    REQUIRE(actMsg->get_accounts()[0] == "someClient@xmpp.uwpx.org");
    REQUIRE(actMsg->get_accounts()[1] == "someOtherClient@xmpp.uwpx.org");
}

TEST_CASE("Serializing & Parsing SetPushAccountsMessage", TEST_TAG.data()) {
    tcp::messages::SetPushAccountsMessage m(std::vector<std::string>{"a1", "a2"}, std::string{"adawdafsefse"});

    nlohmann::json j;
    m.to_json(j);
    const std::string s = j.dump();

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "set_push_accounts");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::SET_PUSH_ACCOUNT_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::SetPushAccountsMessage* actMsg = static_cast<tcp::messages::SetPushAccountsMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_device_id() == "adawdafsefse");
    REQUIRE(actMsg->get_accounts().size() == 2);
    REQUIRE(actMsg->get_accounts()[0] == "a1");
    REQUIRE(actMsg->get_accounts()[1] == "a2");
}

TEST_CASE("Parsing SuccessSetPushAccountsMessage", TEST_TAG.data()) {
    const std::string s = "{\"version\": 1,\
	\"action\": \"response\",\
	\"status\": 1,\
	\"push_bare_jid\": \"push@xmpp.uwpx.org\",\
	\"accounts\": [{\"bare_jid\": \"someClient@xmpp.uwpx.org\",\
			\"node\": \"773bds9nf932\",\
			\"secret\": \"sdf/82h)=1\",\
			\"success\": false},\
		{\"bare_jid\": \"someOtherClient@xmpp.uwpx.org\",\
			\"node\": \"8w3rn0MB3m38z2\",\
			\"secret\": \"j$o909mN87!n/0m\",\
            \"success\": true}]}";

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "response");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::SUCCESS_SET_PUSH_ACCOUNT_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::SuccessSetPushAccountsMessage* actMsg = static_cast<tcp::messages::SuccessSetPushAccountsMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_push_bare_jid() == "push@xmpp.uwpx.org");
    REQUIRE(actMsg->get_accounts().size() == 2);
    REQUIRE(actMsg->get_accounts()[0].bareJid == "someClient@xmpp.uwpx.org");
    REQUIRE(actMsg->get_accounts()[0].node == "773bds9nf932");
    REQUIRE(actMsg->get_accounts()[0].secret == "sdf/82h)=1");
    REQUIRE(actMsg->get_accounts()[0].success == false);
    REQUIRE(actMsg->get_accounts()[1].bareJid == "someOtherClient@xmpp.uwpx.org");
    REQUIRE(actMsg->get_accounts()[1].node == "8w3rn0MB3m38z2");
    REQUIRE(actMsg->get_accounts()[1].secret == "j$o909mN87!n/0m");
    REQUIRE(actMsg->get_accounts()[1].success == true);
}

TEST_CASE("Serializing & Parsing SuccessSetPushAccountsMessage", TEST_TAG.data()) {
    tcp::messages::SuccessSetPushAccountsMessage m(std::vector<tcp::messages::SuccessSetPushAccountsMessage::PushAccount>{tcp::messages::SuccessSetPushAccountsMessage::PushAccount("1", "2", "3", true)}, std::string{"adawdafsefse"});

    nlohmann::json j;
    m.to_json(j);
    const std::string s = j.dump();

    const std::shared_ptr<tcp::messages::AbstractMessage> msg = tcp::messages::parse(s);
    REQUIRE(msg);
    REQUIRE(msg->is_valid());
    REQUIRE(msg->get_action() == "response");
    REQUIRE(msg->get_type() == tcp::messages::AbstractMessage::MessageType::SUCCESS_SET_PUSH_ACCOUNT_MESSAGE);
    REQUIRE(msg->get_version() == tcp::messages::AbstractMessage::VERSION);

    tcp::messages::SuccessSetPushAccountsMessage* actMsg = static_cast<tcp::messages::SuccessSetPushAccountsMessage*>(msg.get());
    REQUIRE(actMsg);
    REQUIRE(actMsg->get_push_bare_jid() == "adawdafsefse");
    REQUIRE(actMsg->get_accounts().size() == 1);
    REQUIRE(actMsg->get_accounts()[0].bareJid == "1");
    REQUIRE(actMsg->get_accounts()[0].node == "2");
    REQUIRE(actMsg->get_accounts()[0].secret == "3");
    REQUIRE(actMsg->get_accounts()[0].success == true);
}