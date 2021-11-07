#ifndef SOURCE_SETTINGS_HPP
#define SOURCE_SETTINGS_HPP

#include "source/precompiled.h"
#include "source/structures.hpp"

// returns path like 'C:\Users\SomeUser\AppData\Local\Randomizer App\app.cfg'
auto GetAppConfigPath()
{
	auto path = std::filesystem::temp_directory_path().parent_path().parent_path();
	path /= "Randomizer App";
	path /= "app.cfg";
	return path;
}

std::string GetFirstSettings()
{
	std::string data;
	data.reserve(1024);

	data.append("Randomizer Window Size: 400, 400\n");
	data.append("Settings Window Size: 600, 600\n");
	data.append("Randomizer Window Position: 0, 0\n");
	data.append("Settings Window Position: 0, 0\n");
	data.append("Timer Enabled: 0\n");
	data.append("Timer: 1\n");
	data.append("Theme: 1\n");
	data.append("Hotkey Random Number: 5, 6");

	return data;
}

std::string GenerateHotkeySettingsRecord(std::vector<int> hotkey)
{
	int keys[2]{};
	for (int i = 0; auto && key : hotkey)
		keys[i++] = key;

	return std::format("Hotkey Random Number: {}, {}", keys[0], keys[1]);
}

bool SaveSettings(const AppSettings& settings)
{
	auto path = GetAppConfigPath();

	std::ofstream ofs(path);

	if (!ofs.is_open())
		assert("Failed opening settings file on app exit");

	std::string data;
	data.reserve(1024);
	data.append(std::format("Randomizer Window Size: {}, {}\n", settings.nWndRnd.x, settings.nWndRnd.y));
	data.append(std::format("Settings Window Size: {}, {}\n", settings.nWndSets.x, settings.nWndSets.y));
	data.append(std::format("Randomizer Window Position: {}, {}\n", settings.posWndRnd.x, settings.posWndRnd.y));
	data.append(std::format("Settings Window Position: {}, {}\n", settings.posWndSets.x, settings.posWndSets.y));
	data.append(std::format("Timer Enabled: {:d}\n", settings.bTimer));
	data.append(std::format("Timer: {}\n", settings.timer));
	data.append(std::format("Theme: {}\n", settings.theme));
	data.append(GenerateHotkeySettingsRecord(settings.hotkeyRnd.keys));

	ofs << data;
	ofs.close();
	return true;
}

auto ParseSettingsRecord(const std::string& record) -> std::pair<std::string, std::string>
{
	const char* regexName = "[a-zA-Z0-9-_ ]+";
	const char* regexValPos = "[0-9]+, [0-9]+";
	const char* regexVal = "[a-zA-Z0-9-_:]+";
	const char* regexValQuote = "\"[a-zA-Z0-9-_: ]\"";

	auto full_regex = std::format("({0}: (({1})|({2})|({3})))", regexName, regexValPos, regexVal, regexValQuote);

	const std::regex txt_regex("(([a-zA-Z0-9-_ ]+): ((([0-9]+), ([0-9]+))|([a-zA-Z0-9-_:]+)|(\"[a-zA-Z0-9-_: ]\"$)))");
	const std::regex test_regex(full_regex);

	std::smatch matched;
	std::string kek = "";
	if (std::regex_match(record, matched, txt_regex))
	{
		// Second and Third regex match groups are 'key: value'
		return { matched[2], matched[3] };
	}
	else
		assert("Bad config file");
	return {};
}

ImVec2 SettingsGetPos(const std::string& str)
{
	auto posComma = str.find(',');
	int x = std::stoi(str.substr(0, posComma));
	int y = std::stoi(str.substr(posComma + 1 + 1));
	return { (float)x, (float)y };
}

int SettingsGetValue(const std::string& str)
{
	return std::stoi(str);
}

std::vector<int> SettingsGetHotkeyVector(const std::string& str)
{
	auto posComma = str.find(',');
	int key1 = std::stoi(str.substr(0, posComma));
	int key2 = std::stoi(str.substr(posComma + 1 + 1));
	if (key2)
		return { key1, key2 };
	else if (key1)
		return { key1 };
	return {};
}

HotKey SettingsGetHotkey(const std::vector<int>& keys)
{
	return HotKey(keys);
}

AppSettings TranslateSettings(const std::map<std::string, std::string>& records)
{
	using namespace std::string_literals;
	AppSettings settings{};

	settings.nWndRnd	= SettingsGetPos(records.at("Randomizer Window Size"));
	settings.nWndSets	= SettingsGetPos(records.at("Settings Window Size"));
	settings.posWndRnd	= SettingsGetPos(records.at("Randomizer Window Position"));
	settings.posWndSets = SettingsGetPos(records.at("Settings Window Position"));
	settings.bTimer		= SettingsGetValue(records.at("Timer Enabled"));
	settings.timer		= SettingsGetValue(records.at("Timer"));
	settings.theme		= SettingsGetValue(records.at("Theme"));
	settings.hotkeyRnd	= SettingsGetHotkey(SettingsGetHotkeyVector(records.at("Hotkey Random Number")));

	return settings;
}

AppSettings ParseSettingsFile(std::string_view path)
{
	std::ifstream ifs(path);

	if (!ifs.is_open())
		assert("app.cfg not found");

	std::string buffer;
	buffer.reserve(128);

	AppSettings settings{};
	std::map<std::string, std::string> records;
	while (!ifs.eof())
	{
		std::getline(ifs, buffer);

		auto [name, value] = ParseSettingsRecord(buffer);
		records[name] = value;
	}

	return TranslateSettings(records);
}

AppSettings ReadSettings()
{
	auto path = GetAppConfigPath();

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