#pragma once

#include <optional>
#include <nlohmann/json.hpp>
namespace nlohmann {
	template <typename T>
	struct adl_serializer<std::optional<T>> {
		static void to_json(json& j, const std::optional<T>& opt) {
			if (!opt.has_value()) {
				j = nullptr;
			}
			else {
				j = *opt;
			}
		}

		static void from_json(const json& j, std::optional<T>& opt) {
			if (j.is_null()) {
				opt = {};
			}
			else {
				opt = j.get<T>();
			}
		}
	};
}
using json = nlohmann::json;

json load_json_file(const std::string& path);

// Set `value` to `property` if `property` exists
// Does nothing if `property` doesn't exist
// Used with std::optional
template<typename T>
inline void try_get_to(const json& json, std::string_view name, T& value) {
	auto v = json.find(name);
	if (v != json.end()) {
		v->get_to(value);
	}
}

// Get a property or return default `value`
template<typename T>
inline T try_get_default(const json& json, std::string_view name, const T& value) {
	auto v = json.find(name);
	if (v != json.end()) {
		return v->get<T>();
	}
	return value;
}
