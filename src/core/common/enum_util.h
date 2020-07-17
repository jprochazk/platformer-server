#pragma once

#include <magic_enum.hpp>

namespace common {
	
	template<typename Enum>
	constexpr auto to_underlying(Enum value) {
		return static_cast<magic_enum::underlying_type_t<Enum>>(value);
	}

	template<typename Enum>
	constexpr std::optional<Enum> from_underlying(magic_enum::underlying_type_t<Enum> value) {
		return magic_enum::enum_cast<Enum>(value);
	}

	template<typename Enum>
	constexpr std::string_view to_string(Enum value) {
		return magic_enum::enum_names<Enum>()[*magic_enum::enum_index(value)];
	}

	template<typename Enum>
	constexpr std::string_view to_string(magic_enum::underlying_type_t<Enum> value) {
		if (auto converted = from_underlying(value); converted.has_value()) {
			return to_string(*converted);
		}
		else {
			return "UNKNOWN";
		}
	}

} // namespace common