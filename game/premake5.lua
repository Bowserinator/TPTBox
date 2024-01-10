
baseName = path.getbasename(os.getcwd());

project (workspaceName)
  	kind "ConsoleApp"
    location "../_build"
    targetdir "../_bin/%{cfg.buildcfg}"

	linkoptions { "-fopenmp" }
	buildoptions {
		"-ftree-vectorize",
		"-ftree-loop-vectorize",
		-- "-fopt-info-vec-optimized" -- Uncomment to display what operations were vectorized
	}

    filter "configurations:Release"
		kind "WindowedApp"
		entrypoint "mainCRTStartup"

	filter "action:vs*"
        debugdir "$(SolutionDir)"
		
	filter {"action:vs*", "configurations:Release"}
			kind "WindowedApp"
			entrypoint "mainCRTStartup"
	filter {}
	
	vpaths 
	{
	  ["Header Files/*"] = { "include/**.h",  "include/**.hpp", "src/**.h", "src/**.hpp", "**.h", "**.hpp"},
	  ["Source Files/*"] = {"src/**.c", "src/**.cpp","**.c", "**.cpp"},
	}

	include "src/simulation"

	files {"**.c", "**.cpp", "**.h", "**.hpp"}

    includedirs { "./" }
    includedirs { "src" }
    includedirs { "include" }

	link_raylib()
	
	-- To link to a lib use link_to("LIB_FOLDER_NAME")