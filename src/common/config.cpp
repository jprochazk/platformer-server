
#include "config.h"

namespace util {

config&
config::instance()
{
    static config instance;
    return instance;
}

void
config::load(const std::string& path)
{
    auto& instance = config::instance();

    auto full_path = fs::complete(path).string();
    INFOF("CONFIG", "Loading config file \"{}\"", full_path);

    std::ifstream config_file(full_path);
    if (config_file.bad()) {
        ERRF("CONFIG",
             "Could not open file \"{}\", aborting execution...",
             full_path);
        abort();
    }

    instance.path_ = std::move(full_path);
    try {
        instance.content_ = json::parse(config_file);
    } catch (const json::exception& e) {
        ERRF("CONFIG", "{}", e.what());
        abort();
    }
}

} // namespace util
