#include "IoThread.hpp"
#include "logger/Logger.hpp"
#include <iostream>
#include <string>
#include <thread>

namespace io {
IoThread::IoThread(const server::PushServer* server) : server(server) {}

IoThread::~IoThread() {
    assert(state == IoThreadState::NOT_RUNNING);
}

IoThread::IoThreadState IoThread::get_state() {
    return state;
}

void IoThread::start() {
    assert(state == IoThreadState::NOT_RUNNING);
    LOG_DEBUG << "Starting the IO thread...";
    state = IoThreadState::STARTING;
    thread = std::make_optional<std::thread>(&IoThread::threadRun, this);
}

void IoThread::stop() {
    if (state == IoThreadState::STARTING || state == IoThreadState::RUNNING || state == IoThreadState::WAITING_FOR_JOIN) {
        if (state != IoThreadState::WAITING_FOR_JOIN) {
            LOG_DEBUG << "Stopping the IO thread...";
            state = IoThreadState::STOP_REQUESTED;
        }
        LOG_DEBUG << "Joining the IO thread...";
        thread->join();
        state = IoThreadState::NOT_RUNNING;
        thread = std::nullopt;
        LOG_INFO << "IO thread joined.";
    } else {
        LOG_DEBUG << "No need to stop the IO thread - not running (state: " << static_cast<int>(state) << ")!";
    }
}

void IoThread::threadRun() {
    assert(state == IoThreadState::STARTING || state == IoThreadState::STOP_REQUESTED);
    if (state != IoThreadState::STARTING) {
        return;
    }
    state = IoThreadState::RUNNING;
    LOG_INFO << "IO thread started.";

    std::string s;
    while (state == IoThreadState::RUNNING) {
        std::cout << "$ ";
        std::cin >> s;
        if (s == "help") {
            printHelp();
        } else if (s == "q") {
            state = IoThreadState::STOP_REQUESTED;
        }
    }
    state = IoThreadState::WAITING_FOR_JOIN;
    LOG_DEBUG << "IO thread ready to be joined.";
}

void IoThread::printHelp() {
    std::cout << "-------------------HELP-------------------\n";
    std::cout << "'help' - This help.\n";
    std::cout << "'test push <device_id>' - Sends a test push to the given device ID.\n";
    std::cout << "'test xmpp publish <nodeName>' - Publishes and subscribes to the given PubSub node.\n";
    std::cout << "'q' or 'exit' - Stop the server.\n";
    std::cout << "------------------------------------------\n";
}

}  // namespace io