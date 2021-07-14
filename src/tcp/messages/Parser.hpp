#pragma once

#include "AbstractMessage.hpp"
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>

namespace tcp::messages {
const std::shared_ptr<AbstractMessage> parse(const std::string& s);
const std::shared_ptr<AbstractMessage> parseJson(const nlohmann::json& j);
}  // namespace tcp::messages
