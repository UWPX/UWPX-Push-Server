#include "logger/Logger.hpp"
#include "storage/ConfigurationStorage.hpp"
#include "wns/WnsClient.hpp"

int main(int /*argc*/, char** /*argv*/) {
    logger::setup_logger(spdlog::level::debug);
    SPDLOG_INFO("Launching Version: {} {}", UWPX_PUSH_SERVER_VERSION, UWPX_PUSH_SERVER_NAME);

    storage::ConfigurationStorage configStorage = storage::get_configuration_storage_instance();

    wns::WnsClient wnsClient(std::string{configStorage.config.wns.packetId}, std::string{configStorage.config.wns.clientSecret});
    wnsClient.requestToken();
}
