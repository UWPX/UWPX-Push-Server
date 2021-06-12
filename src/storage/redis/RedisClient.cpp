#include "RedisClient.hpp"

namespace storage::redis {
RedisClient::RedisClient(const storage::DbConfiguration& config) : redis(config.url) {}
}  // namespace storage::redis