#include "io/IoThread.hpp"
#include "logger/Logger.hpp"
#include "server/PushServer.hpp"
#include "storage/ConfigurationStorage.hpp"
#include <chrono>
#include <thread>

int main(int argc, char** argv) {
    logger::setup_logger(spdlog::level::debug);
    SPDLOG_INFO("Launching Version: {} {}", UWPX_PUSH_SERVER_VERSION, UWPX_PUSH_SERVER_NAME);

    // Custom configuration path:
    std::string configPath = "configuration.json";
    if (argc >= 2) {
        // NOLINTNEXTLINE (cppcoreguidelines-pro-bounds-pointer-arithmetic)
        configPath = argv[1];
    }

    // Load configuration:
    storage::ConfigurationStorage configStorage = storage::get_configuration_storage_instance(configPath);

    // Start the server:
    server::PushServer server(configStorage.config);
    server.start();

    // Start the IO handler:
    io::IoThread ioThread(&server);
    ioThread.start();

    while (ioThread.getState() != io::IoThread::IoThreadState::WAITING_FOR_JOIN) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    server.stop();
    ioThread.stop();
}
