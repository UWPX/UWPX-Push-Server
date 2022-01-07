#include "TestPushMessage.hpp"
#include "logger/Logger.hpp"

namespace tcp::messages {
TestPushMessage::TestPushMessage() : AbstractMessage(std::string{ACTION}) {}

TestPushMessage::TestPushMessage(const nlohmann::json& j) {
    // NOLINTNEXTLINE (clang-analyzer-optin.cplusplus.VirtualCall) Not relevant in this case
    isValid = from_json(j);
}

AbstractMessage::MessageType TestPushMessage::get_type() const {
    return MessageType::TEST_PUSH;
}

}  // namespace tcp::messages