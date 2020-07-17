#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <boost/core/noncopyable.hpp>
#include <optional>
#include <string>
#include "common/log.h"

namespace util {

	class config : private boost::noncopyable {
	public:
		static void load(const std::string& path);

		template<class T>
		static std::optional<T> get(const std::string& key) {
			auto& content = instance().content_;
			auto it = content.find(key);
			if (it != content.end()) {
				return it.value();
			}
			else {
				WARN("CONFIG", "Entry \"{}\" is undefined", key);
				return {};
			}
		}

		template<class T>
		static T get_or_throw(const std::string& key) {
			auto it = get<T>(key);
			if (it) { 
				return it.value(); 
			}
			else {
				throw std::runtime_error(
					fmt::format("Config file \"{}\" has no field \"{}\"", instance().path_, key));
			}
		}

		template<class T>
		static T get_or_default(const std::string& key, const T& default_value) {
			auto it = get<T>(key);
			if (it) { 
				return it.value(); 
			}
			else { 
				return default_value; 
			}
		}

	private:
		static config& instance();
		config() = default;

		std::string path_;
		json content_;

	public:
		config(config const&) = delete;
		void operator=(config const&) = delete;
		~config() = default;
	}; // class config

} // namespace util
