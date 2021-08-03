#include "io/IoThread.hpp"
#include "logger/Logger.hpp"
#include "server/PushServer.hpp"
#include "storage/ConfigurationStorage.hpp"
#include <cassert>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

// NOLINTNEXTLINE (cppcoreguidelines-avoid-non-const-global-variables)
server::PushServer* serverPtr = nullptr;

void signal_handler(int signal) {
    if (signal == SIGINT) {
        if (serverPtr) {
            LOG_INFO << "Ctrl+C pressed...";
            serverPtr->stop();
            serverPtr = nullptr;
        }
    }
}

void print_help(const char* prog) {
    std::cout << prog << " [--noInput] [--help] <PATH TO THE 'configuration.json' FILE>\n";
    std::cout << "--help - Display this help.\n";
    std::cout << "--noInput - Do not accept input. Exit via Ctrl+C. Useful when running headless." << std::endl;
}

int main(int argc, char** argv) {
    assert(argc >= 1);

    // Parse arguments:
    std::string configPath = "configuration.json";
    bool noInput = false;
    for (int i = 1; i < argc; i++) {
        // NOLINTNEXTLINE (cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (std::string{"--noInput"} == argv[i]) {
            noInput = true;
        }
        // NOLINTNEXTLINE (cppcoreguidelines-pro-bounds-pointer-arithmetic)
        else if (std::string{"--help"} == argv[i]) {
            // NOLINTNEXTLINE (cppcoreguidelines-pro-bounds-pointer-arithmetic)
            print_help(argv[0]);
            return EXIT_SUCCESS;
        } else {
            // NOLINTNEXTLINE (cppcoreguidelines-pro-bounds-pointer-arithmetic)
            configPath = argv[i];
        }
    }

    // Launch:
    // NOLINTNEXTLINE (cppcoreguidelines-pro-bounds-pointer-arithmetic)
    logger::init_logger(argv[0]);

    if (noInput) {
        LOG_INFO << "Launching the UWPX Push Server version " << UWPX_PUSH_SERVER_VERSION << " " << UWPX_PUSH_SERVER_NAME << " in NO input mode.";
        LOG_INFO << "Press Ctrl+C to exit.";
        std::signal(SIGINT, signal_handler);
    } else {
        LOG_INFO << "Launching the UWPX Push Server version " << UWPX_PUSH_SERVER_VERSION << " " << UWPX_PUSH_SERVER_NAME << " in input mode.";
        LOG_INFO << "Enter 'q' to exit.";
    }

    // Load configuration:
    storage::ConfigurationStorage configStorage = storage::get_configuration_storage_instance(configPath);

    // Start the server:
    server::PushServer server(configStorage.config);
    server.start();

    if (noInput) {
        serverPtr = &server;
        while (serverPtr) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } else {
        // Start the IO handler:
        io::IoThread ioThread(&server);
        ioThread.start();

        while (ioThread.get_state() != io::IoThread::IoThreadState::WAITING_FOR_JOIN) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        server.stop();
        ioThread.stop();
    }
    return EXIT_SUCCESS;
}
