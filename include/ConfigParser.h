#pragma once
#include <unordered_set>
#include "SimpleIni.h"

class ModConfigs
{
public:
	void LoadConfigs(CSimpleIniA& a_ini) {
		a_ini.LoadFile("Data\\InjectionBlocker\\InjectionBlocker.ini");

		bBlockAllItems = std::stoi(a_ini.GetValue("", "bBlockAllItems", "0"));
		bBlockAmmo = std::stoi(a_ini.GetValue("", "bBlockAmmo", "0"));
		bBlockArmor = std::stoi(a_ini.GetValue("", "bBlockArmor", "0"));
		bBlockNPCs = std::stoi(a_ini.GetValue("", "bBlockNPCs", "0"));
		bBlockWeapons = std::stoi(a_ini.GetValue("", "bBlockWeapons", "0"));

		a_ini.Reset();

		logger::warn("-----------------------------");
		logger::warn("Loaded configs:");
		logger::warn("bBlockAllItems = {}", bBlockAllItems);
		logger::warn("bBlockAmmo = {}", bBlockAmmo);
		logger::warn("bBlockArmor = {}", bBlockArmor);
		logger::warn("bBlockNPCs = {}", bBlockNPCs);
		logger::warn("bBlockWeapons = {}", bBlockWeapons);

		std::vector<std::string> fileList;

		auto constexpr directory = R"(Data\InjectionBlocker\)";
		for (const auto& entry : std::filesystem::directory_iterator(directory)) {
			if (entry.exists() && !entry.path().empty() && entry.path().extension() == ".ini"sv && entry.path() != "Data\\InjectionBlocker\\InjectionBlocker.ini") {
				const auto path = entry.path().string();
				fileList.push_back(path);
			}
		}

		if (fileList.empty()) {
			return;
		}

		for (std::string& currentFile : fileList) {
			logger::warn("-----------------------------");

			if (const auto rc = a_ini.LoadFile(currentFile.c_str()); rc < 0) {
				logger::warn("Unable to load file: {}", currentFile);
				continue;
			}
			
			logger::warn("Loaded file: {}", currentFile);

			std::list<CSimpleIniA::Entry> keyList;
			a_ini.GetAllKeys("", keyList);

			for (CSimpleIniA::Entry& currentKey : keyList) {
				static auto dataHandler = RE::TESDataHandler::GetSingleton();
				if (std::stoi(a_ini.GetValue("", currentKey.pItem, "0")) == 1) {
					logger::warn("Attempting to whitelist {}", currentKey.pItem);
					auto formFilePair = SplitString((std::string)currentKey.pItem, '~');
					RE::TESForm* formToAdd = dataHandler->LookupForm(std::stoul(formFilePair.first, nullptr, 16), formFilePair.second);

					if (formToAdd) {
						whitelist.emplace(formToAdd);
						logger::warn("Success");
					} else {
						logger::warn("Fail. Invalid form?");
					}
				}
			}
		}
		logger::warn("-----------------------------");
		a_ini.Reset();
	}

	std::unordered_set<RE::TESForm*> whitelist;
	bool bBlockAllItems;
	bool bBlockAmmo;
	bool bBlockArmor;
	bool bBlockNPCs;
	bool bBlockWeapons;

private:
	std::pair<std::string, std::string> SplitString(const std::string& input, char delimiter)
	{
		size_t found = input.find(delimiter);

		if (found != std::string::npos) {
			return std::make_pair(input.substr(0, found), input.substr(found + 1));
		} else {
			return std::make_pair(input, "");
		}
	}
};
