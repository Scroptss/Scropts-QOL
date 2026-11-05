#pragma once
#include "includes.h"
#include <shellapi.h>
#include <vector>
#include <string>

#pragma comment(lib, "shell32.lib")

namespace t7x_scripts
{
	struct ScriptEntry
	{
		std::string name;
		std::string path;
		std::uintmax_t size;
	};

	// T7x runs inside the BO3 process and exposes itself as a module.
	inline bool IsT7xRunning()
	{
		return GetModuleHandleA("t7x.dll") != nullptr;
	}

	inline std::string GetGameDirectory()
	{
		char path[MAX_PATH]{};
		GetModuleFileNameA(nullptr, path, MAX_PATH);
		std::string dir = path;
		const size_t slash = dir.find_last_of("\\/");
		return slash == std::string::npos ? std::string() : dir.substr(0, slash);
	}

	inline std::string GetCustomScriptsDirectory()
	{
		return GetGameDirectory() + "\\t7x\\custom_scripts";
	}

	inline bool CustomScriptsDirectoryExists()
	{
		std::error_code ec;
		return std::filesystem::exists(GetCustomScriptsDirectory(), ec);
	}

	inline std::vector<ScriptEntry> ListScripts()
	{
		std::vector<ScriptEntry> scripts;
		std::error_code ec;
		const std::string dir = GetCustomScriptsDirectory();
		if (!std::filesystem::exists(dir, ec))
			return scripts;

		for (auto it = std::filesystem::recursive_directory_iterator(dir, ec);
			it != std::filesystem::recursive_directory_iterator();
			it.increment(ec))
		{
			if (ec)
				break;

			std::error_code fileEc;
			if (!it->is_regular_file(fileEc) || fileEc)
				continue;
			if (it->path().extension() != ".gsc")
				continue;

			scripts.push_back({
				it->path().filename().string(),
				it->path().string(),
				it->file_size(fileEc)
			});
		}
		return scripts;
	}

	inline void OpenScriptsFolder()
	{
		const std::string dir = GetCustomScriptsDirectory();
		std::error_code ec;
		if (!std::filesystem::exists(dir, ec))
			return;
		ShellExecuteA(nullptr, "open", dir.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	}
}
