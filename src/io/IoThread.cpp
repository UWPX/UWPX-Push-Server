#include "IoThread.hpp"
#include "logger/Logger.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <cli/cli.h>
#include <cli/clilocalsession.h>
#include <cli/filehistorystorage.h>
#include <cli/standaloneasioscheduler.h>

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
            scheduler.Stop();
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

    cli::CmdHandler rootCmd;
    std::unique_ptr<cli::Menu> rootMenu = std::make_unique<cli::Menu>("push");
    std::unique_ptr<cli::Menu> testMenu = std::make_unique<cli::Menu>("test", "Testing the server");
    std::unique_ptr<cli::Menu> xmppMenu = std::make_unique<cli::Menu>("xmpp", "XMPP based settings");
    xmppMenu->Insert(
        "send",
        {"xml_msg"},
        [](std::ostream& out, const std::string& msg) { out << "Sending: " + msg + "\n"; },
        "Send the given xml message to the XMPP server");
    testMenu->Insert(std::move((xmppMenu)));
    testMenu->Insert(
        "push",
        {"<device_id>"},
        [](std::ostream& out, const std::string& deviceId) { out << "Push to device id: " + deviceId + "\n"; },
        "Stop the server");
    rootMenu->Insert(std::move((testMenu)));

    cli::Cli cliInst(std::move(rootMenu), std::make_unique<cli::FileHistoryStorage>(".cli"));
    cliInst.ExitAction([](auto& out) { out << "Goodbye and thanks for all the fish.\n"; });
    cliInst.StdExceptionHandler(
        [](std::ostream& out, const std::string& cmd, const std::exception& e) {
            out << "Exception caught in cli handler: "
                << e.what()
                << " handling command: "
                << cmd
                << ".\n";
        });
    cli::CliLocalTerminalSession localSession(cliInst, scheduler, std::cout, 1024);
    localSession.ExitAction([this](auto& out) {
        out << "Closing App...\n";
        this->state = IoThreadState::STOP_REQUESTED;
        this->scheduler.Stop();
    });
    scheduler.Run();
    state = IoThreadState::WAITING_FOR_JOIN;
    LOG_DEBUG << "IO thread ready to be joined.";
}

}  // namespace io