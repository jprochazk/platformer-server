function file_exists(name)
   local f=io.open(name,"r")
   if f~=nil then io.close(f) return true else return false end
end

newoption {
	trigger = "build",
	value = "Build type",
	description = "Choose which configuration to use",
	allowed = {
		{ "debug", "Debug" },
		{ "release", "Release" }
	}
}

if not _OPTIONS["build"] then 
	_OPTIONS["build"] = "release"
end
	
build_type = "Debug"
if _OPTIONS["build"] == "release" then 
	build_type = "Release"
end
os.execute("conan install . -s build_type=" .. build_type)

if not file_exists("conanbuildinfo.premake.lua") then
	error("conan install failed");
end

include("conanbuildinfo.premake.lua")

workspace "rpg-server"
	runtime (build_type)
	language "C++"
	cppdialect "C++17"
	
	conan_basic_setup()

	filter "system:windows" 
		buildoptions { "/bigobj" }

	filter "system:linux"
		buildoptions { "-Wall", "-Wextra" }
	
	project "core"
		kind "StaticLib"

		targetdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}")
		objdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}/obj")

		files { 
			"src/core/**.h",
			"src/core/**.cpp"
		}

		includedirs {
			"src/core"
		}

		linkoptions { conan_exelinkflags }

		if build_type == "Debug" then
			print("DEBUG")
			defines { "_DEBUG" }
			symbols "On"
		else
			print("NDEBUG")
			defines { "NDEBUG" }
			optimize "On"
		end

	project "server"
		kind "ConsoleApp"

		targetdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}")
		objdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}/obj")

		files {
			"src/bin/**.h",
			"src/bin/**.cpp"
		}

		includedirs {
			"src/core"
		}

		links {
			"core"
		}

		if build_type == "Debug" then
			print("DEBUG")
			defines { "_DEBUG" }
			symbols "On"
		else
			print("NDEBUG")
			defines { "NDEBUG" }
			optimize "On"
		end

	project "test"
		kind "ConsoleApp"

		targetdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}")
		objdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}/obj")

		files {
			"src/test/**.h",
			"src/test/**.cpp"
		}

		includedirs {
			"src/core"
		}

		links {
			"core"
		}

		if build_type == "Release" then
			print("NDEBUG")
			defines { "NDEBUG" }
			optimize "On"
		else
			print("DEBUG")
			defines { "_DEBUG" }
			symbols "On"
		end
