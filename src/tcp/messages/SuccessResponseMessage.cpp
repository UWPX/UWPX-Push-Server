#include "SuccessResponseMessage.hpp"
#include "logger/Logger.hpp"
#include "tcp/messages/AbstractResponseMessage.hpp"

namespace tcp::messages {
SuccessResponseMessage::SuccessResponseMessage(const nlohmann::json& j) : AbstractResponseMessage(false) {
    isValid = from_json(j);  // Has to be called here since it is not available in the initializer list.
}

SuccessResponseMessage::SuccessResponseMessage() : AbstractResponseMessage(STATUS) {}

AbstractMessage::MessageType SuccessResponseMessage::get_type() const { return MessageType::SUCCESS_RESPONSE_MESSAGE; }

}  // namespace tcp::messages