#include "ConfigurationStorage.hpp"
#include "storage/Serializer.hpp"

namespace storage {

ConfigurationStorage::ConfigurationStorage(const std::filesystem::path& config_file) : file_handl(config_file) {
    SPDLOG_DEBUG("Initializing ConfigurationStorage, this should only happen once per execution.");
    if (std::filesystem::exists(config_file)) {
        SPDLOG_INFO("Configuration file {} exists, trying to read it...", config_file.string());
        if (file_handl.read_in()) {
            config = file_handl.js_int.get<Configuration>();
            SPDLOG_DEBUG("Configuration loaded successfully.");
        }
    } else {
        SPDLOG_ERROR("Configuration file '{}' does not exist. Creating a new, empty one.", config_file.string());
        write_configuration();
    }
}

void ConfigurationStorage::write_configuration() {
    file_handl.js_int = config;
    file_handl.write_out();
}

ConfigurationStorage& get_configuration_storage_instance() {
    static ConfigurationStorage storage_instance("configuration.json");
    return storage_instance;
}
}  // namespace storage