#pragma once

#include<filesystem>

class CSATeConfig {
public:
	std::filesystem::path CS2Path;
};


class CSATeCore;

class ConfigManager {
private:
	CSATeConfig Config{};
	CSATeCore* Core;
public:
	bool Init(CSATeCore* Core);

	bool Config_SetCS2Path(const std::filesystem::path& CS2Path);
	

	bool Config_Save(const std::filesystem::path& ConfigPath)const;

	bool Config_Load(const std::filesystem::path& ConfigPath);

	std::filesystem::path Config_GetCS2Path()const;

	
};