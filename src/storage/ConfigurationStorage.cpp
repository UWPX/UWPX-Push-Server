#include "ConfigurationStorage.hpp"
#include "storage/Serializer.hpp"

namespace storage {

ConfigurationStorage::ConfigurationStorage(const std::filesystem::path& configFilePath) : fileHandle(configFilePath) {
    SPDLOG_DEBUG("Initializing ConfigurationStorage, this should only happen once per execution.");
    if (std::filesystem::exists(configFilePath)) {
        SPDLOG_INFO("Configuration file {} exists, trying to read it...", configFilePath.string());
        if (fileHandle.read_in()) {
            config = fileHandle.js_int.get<Configuration>();
            SPDLOG_DEBUG("Configuration loaded successfully.");
        }
    } else {
        SPDLOG_ERROR("Configuration file '{}' does not exist. Creating a new, empty one.", configFilePath.string());
        write_configuration();
    }
}

void ConfigurationStorage::write_configuration() {
    fileHandle.js_int = config;
    fileHandle.write_out();
}

ConfigurationStorage& get_configuration_storage_instance() {
    static ConfigurationStorage storage_instance("configuration.json");
    return storage_instance;
}
}  // namespace storage