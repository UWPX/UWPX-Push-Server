#pragma once

#include "logger/Logger.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

// Required so we can parse std::optional
#define JSON_HAS_CPP_17

namespace storage {
struct WnsConfiguration;
struct XmppConfiguration;
struct TlsConfiguration;
struct TcpConfiguration;
struct DbConfiguration;
struct Configuration;
void to_json(nlohmann::json& j, const WnsConfiguration& c);
void from_json(const nlohmann::json& j, WnsConfiguration& c);
void to_json(nlohmann::json& j, const XmppConfiguration& c);
void from_json(const nlohmann::json& j, XmppConfiguration& c);
void to_json(nlohmann::json& j, const TlsConfiguration& c);
void from_json(const nlohmann::json& j, TlsConfiguration& c);
void to_json(nlohmann::json& j, const TcpConfiguration& c);
void from_json(const nlohmann::json& j, TcpConfiguration& c);
void to_json(nlohmann::json& j, const DbConfiguration& c);
void from_json(const nlohmann::json& j, DbConfiguration& c);
void to_json(nlohmann::json& j, const Configuration& c);
void from_json(const nlohmann::json& j, Configuration& c);
}  // namespace storage

namespace storage {
class Settings;
struct ConfigurationStorage;
class Serializer {
 private:
    friend Settings;
    friend ConfigurationStorage;
    const std::filesystem::path filename;
    nlohmann::json js_int = nlohmann::json::object();

 public:
    explicit Serializer(std::filesystem::path filename) noexcept : filename(std::move(filename)) {}
    explicit Serializer(const std::string&& filename) noexcept : filename(std::filesystem::path(filename)) {}
    explicit Serializer(const std::string_view filename) noexcept : filename(std::filesystem::path(filename)) {}

    bool read_in() {
        std::ifstream i(this->filename);
        if (i.fail()) {
            SPDLOG_ERROR("Failed to read {}", this->filename.string());
            return false;
        }
        try {
            i >> this->js_int;
        } catch (nlohmann::json::parse_error& e) {
            SPDLOG_ERROR("Error parsing {}: {}", this->filename.string(), e.what());
            return false;
        }
        spdlog::info("Read json file {}", this->filename.string());
        return true;
    }

    void write_out() const {
        std::ofstream o(this->filename);
        if (not o.is_open()) {
            SPDLOG_ERROR("Failed to write to {}", this->filename.string());
            return;
        }
        o << std::setw(1) << this->js_int;
    }

    [[nodiscard]] bool exists(const std::string_view s) const {
        return this->js_int.contains(s);
    }

    template <class T>
    [[nodiscard]] std::optional<T> get_data(const std::string_view s) const {
        if (not this->js_int.contains(s)) {
            return std::nullopt;
        }
        T t;
        this->js_int.find(s)->get_to(t);
        return std::make_optional(t);
    }

    template <class T>
    bool get_to(const std::string_view s, T& t) const {
        if (not this->exists(s)) {
            return false;
        }
        this->js_int.find(s)->get_to(t);
        return true;
    }

    template <class T>
    void set_data(const std::string_view key, T value) {
        this->js_int.emplace(key, value);
    }
};
}  // namespace storage

namespace nlohmann {
template <typename T>
struct adl_serializer<std::optional<T>> {
    static void to_json(json& j, const std::optional<T>& opt) {
        if (opt.has_value()) {
            j = opt.value();
        } else {
            j = nullptr;
        }
    }

    static void from_json(const json& j, std::optional<T>& opt) {
        if (j.is_null()) {
            opt = std::nullopt;
        } else {
            opt = j.get<T>();
        }
    }
};

template <typename T>
struct adl_serializer<std::shared_ptr<T>> {
    static void to_json(json& j, const std::shared_ptr<T>& ptr) {
        if (ptr != nullptr) {
            j = *ptr;
        } else {
            j = nullptr;
        }
    }

    static void from_json(const json& j, std::shared_ptr<T>& ptr) {
        if (j.is_null()) {
            ptr = nullptr;
        } else {
            ptr = std::make_shared<T>(j.get<T>());
        }
    }
};

template <>
struct adl_serializer<std::chrono::system_clock::time_point> {
    static void to_json(json& j, const std::chrono::system_clock::time_point& tp);
    static void from_json(const json& j, std::chrono::system_clock::time_point& tp);
};
}  // namespace nlohmann
