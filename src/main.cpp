#include "logger/Logger.hpp"
#include "server/PushServer.hpp"
#include "storage/ConfigurationStorage.hpp"

int main(int /*argc*/, char** /*argv*/) {
    logger::setup_logger(spdlog::level::debug);
    SPDLOG_INFO("Launching Version: {} {}", UWPX_PUSH_SERVER_VERSION, UWPX_PUSH_SERVER_NAME);

    storage::ConfigurationStorage configStorage = storage::get_configuration_storage_instance();
    server::PushServer server(configStorage.config);
    server.start();
}
