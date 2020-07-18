
include "util.lua"

-- CLI options
newoption {
	trigger = "build",
	value = "Build type",
	description = "Choose which configuration to use",
	allowed = {
		{ "debug", "Debug" },
		{ "release", "Release" }
	}
}

local ROOT_PATH = ".."

if _ACTION == nil then
	error ("No action provided, see https://github.com/premake/premake-core/wiki/Using-Premake#using-premake-to-generate-project-files for a list of possible actions", 2)
end

-- Conan setup

if not has_option ("build") then
	print ("No build type supplied, using default: Release")
end

print ("conan install...\n")
local build_type = title_case (get_option_str ("build", "Release"))
os.execute ("conan install .. -if " .. ROOT_PATH .. " -s build_type=" .. build_type)
if not exists (ROOT_PATH .. "/conanbuildinfo.premake.lua") then
	error ("Configuration failed, see above output for details.", 2);
end

include (ROOT_PATH .. "/conanbuildinfo.premake.lua")

-- premake5 setup
workspace "platformer-server"
	location (ROOT_PATH)

-- runtime == "Debug" or "Release"
	configurations { build_type }
	runtime (build_type)
	language "C++"
	cppdialect "C++17"

	conan_basic_setup()

	-- /bigobj is necessary for visual studio
	-- to compile heavily templated code
	-- e.g. boost, magic_enum...
	filter "system:windows" 
		buildoptions { "/bigobj", "/W3" }

	filter "system:linux"
		buildoptions { "-Wall", "-Wextra" }

	-- Executable "server"
	project "server"
		kind "ConsoleApp"

		targetdir (ROOT_PATH .. "/bin/%{prj.name}")
		objdir (ROOT_PATH .. "/bin/%{prj.name}/obj")

		files { 
			ROOT_PATH .. "/src/**.h",
			ROOT_PATH .. "/src/**.cpp"
		}

		includedirs {
			ROOT_PATH .. "/src"
		}

		linkoptions { conan_exelinkflags }

		if build_type == "Debug" then
			defines { "DEBUG" }
			symbols "On"
		else
			defines { "NDEBUG" }
			optimize "On"
		end
