#include "SuccessResponseMessage.hpp"
#include "logger/Logger.hpp"
#include "tcp/messages/AbstractResponseMessage.hpp"

namespace tcp::messages {
SuccessResponseMessage::SuccessResponseMessage(const nlohmann::json& j) { isValid = from_json(j); }

SuccessResponseMessage::SuccessResponseMessage() : AbstractResponseMessage(STATUS) {}

AbstractMessage::MessageType SuccessResponseMessage::get_type() const { return MessageType::SUCCESS_RESPONSE_MESSAGE; }

}  // namespace tcp::messages