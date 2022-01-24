#pragma once
#include <optional>
#include <server/PushServer.hpp>
#include <thread>
#include <cli/standaloneasioscheduler.h>

namespace io {
class IoThread {
 public:
    enum class IoThreadState {
        NOT_RUNNING,
        STARTING,
        RUNNING,
        STOP_REQUESTED,
        WAITING_FOR_JOIN,
    };

 private:
    const server::PushServer* server;
    std::optional<std::thread> thread{std::nullopt};
    IoThreadState state{IoThreadState::NOT_RUNNING};
    cli::StandaloneAsioScheduler scheduler;

 public:
    explicit IoThread(const server::PushServer* server);
    IoThread(IoThread&&) = default;
    IoThread(const IoThread&) = delete;
    IoThread& operator=(IoThread&&) = default;
    IoThread& operator=(const IoThread&) = delete;
    ~IoThread();

    IoThreadState get_state();

    void start();
    void stop();

 private:
    void threadRun();
};
}  // namespace io