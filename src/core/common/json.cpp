
#include "json.h"
#include "common/log.h"

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

json load_json_file(const std::string& path) {
	auto full_path = fs::complete(path).string();
	INFO("LOAD_JSON", "Loading file \"{}\"", full_path);

	std::ifstream config_file(full_path);
	if (config_file.bad()) {
		ERR("LOAD_JSON", "Could not open file \"{}\", aborting execution...", full_path);
		abort();
	}

	try {
		return json::parse(config_file);
	}
	catch (json::exception e) {
		ERR("LOAD_JSON", "{}", e.what());
		abort();
	}
}