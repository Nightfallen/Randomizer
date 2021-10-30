#ifndef SOURCE_SETTINGS_HPP
#define SOURCE_SETTINGS_HPP

#include "source/precompiled.h"
#include "source/structures.hpp"

std::string GetFirstSettings()
{
	std::string data;
	data.reserve(1024);

	data.append("Randomizer Window Size: 400, 400\n");
	data.append("Settings Window Size: 600, 600\n");
	data.append("Timer Enabled: 0\n");
	data.append("Timer: 1\n");
	data.append("Theme: 1");

	"1: 1, 1";
	return data;
}

AppSettings ParseSettingsFile(std::string_view path)
{
	std::ifstream ifs(path);

	if (!ifs.is_open())
		assert("app.cfg not found");

	std::string buffer;
	buffer.reserve(128);

	AppSettings settings{};

	while (!ifs.eof())
	{
		std::getline(ifs, buffer);

		auto posColon = buffer.find(':');
		auto posComma = buffer.find(',');

		if (posComma != std::string::npos)
		{
			auto substr1 = buffer.substr(posColon + 1, posComma - posColon - 1);
			auto substr2 = buffer.substr(posComma + 1);
			int x = std::stoi(substr1);
			int y = std::stoi(substr2);

			if (buffer.find("Randomizer") != std::string::npos)
				settings.nWndRnd = { (float)x, (float)y };
			else
				settings.nWndSets = { (float)x, (float)y };
		}
		else
		{
			auto substr = buffer.substr(posColon + 1);
			int val = std::stoi(substr);

			if (buffer.starts_with("Timer Enabled:"))
				settings.bTimer = val;
			if (buffer.starts_with("Timer:"))
				settings.timer = val;
			if (buffer.starts_with("Theme:"))
				settings.theme = val;

		}

	}

	ifs.close();
	return settings;
}

bool SaveSettings(const AppSettings& settings)
{
	auto path = std::filesystem::temp_directory_path().parent_path().parent_path();

	path /= "Roaming";
	path /= "Randomizer App";
	path /= "app.cfg";

	std::ofstream ofs(path);

	if (!ofs.is_open())
		assert("Failed opening settings file on app exit");

	std::string data;
	data.reserve(1024);
	data.append(std::format("Randomizer Window Size: {}, {}\n", settings.nWndRnd.x, settings.nWndRnd.y));
	data.append(std::format("Settings Window Size: {}, {}\n", settings.nWndSets.x, settings.nWndSets.y));
	data.append(std::format("Timer Enabled: {:d}\n", settings.bTimer));
	data.append(std::format("Timer: {}\n", settings.timer));
	data.append(std::format("Theme: {}", settings.theme));

	ofs << data;

	ofs.close();
	return true;
}

AppSettings ReadSettings()
{
	auto path = std::filesystem::temp_directory_path().parent_path().parent_path();

	path /= "Roaming";
	path /= "Randomizer App";
	path /= "app.cfg";

	// No settings file found
	if (!std::filesystem::exists(path))
	{
		std::filesystem::create_directories(path.parent_path());
		std::ofstream ofs(path);
		ofs << GetFirstSettings();
		ofs.close();
	}
	return ParseSettingsFile(path.string());
}

#endif // !SOURCE_SETTINGS_HPP
