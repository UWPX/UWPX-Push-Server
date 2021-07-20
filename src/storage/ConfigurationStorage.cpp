#include "ConfigurationStorage.hpp"
#include "storage/Serializer.hpp"

namespace storage {

ConfigurationStorage::ConfigurationStorage(const std::filesystem::path& configFilePath) : fileHandle(configFilePath) {
    LOG_DEBUG << "Initializing ConfigurationStorage, this should only happen once per execution.";
    if (std::filesystem::exists(configFilePath)) {
        LOG_INFO << "Configuration file " << configFilePath.string() << " exists, trying to read it...";
        if (fileHandle.read_in()) {
            config = fileHandle.js_int.get<Configuration>();
            LOG_DEBUG << "Configuration loaded successfully.";
        }
    } else {
        LOG_ERROR << "Configuration file '" << configFilePath.string() << "' does not exist. Creating a new, empty one.";
        write_configuration();
    }
}

void ConfigurationStorage::write_configuration() {
    fileHandle.js_int = config;
    fileHandle.write_out();
}

ConfigurationStorage& get_configuration_storage_instance(const std::string& path = "configuration.json") {
    static ConfigurationStorage storage_instance(path);
    return storage_instance;
}
}  // namespace storage