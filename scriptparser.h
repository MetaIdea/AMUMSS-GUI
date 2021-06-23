#pragma once

#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H

#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <regex>
#include <filesystem>
using namespace std;

static bool CopyToGamefolder = true;
static bool UpdateMBinCompiler = true;
static bool CombinedModPak = false;
static bool CheckForModConflicts = false;
static bool ReCreateMapFileTree = false;

#define StringContains(str1,str2) (str1.find(str2) != std::string::npos)
#define StringEmpty(str) (str.length() < 1)
#define TrimString(str,size) str.substring(1,size)

int countSubstring(string& str, string sub) {
	if (sub.length() == 0) return 0;
	int count = 0;
	for (size_t offset = str.find(sub); offset != std::string::npos;
		offset = str.find(sub, offset + sub.length()))
	{
		++count;
	}
	return count;
}

string ExtractString(string& str) {
	if (countSubstring(str, R"(")") > 2) {
		string substr = str.substr(str.find_first_of(']'), str.find_last_of('"') - str.find_first_of(']') + 1);
		string substr2 = substr.substr(substr.find_first_of('"') + 1, substr.find_last_of('"') - substr.find_first_of('"') - 1);
		if (substr2.length() > 0) {
			return substr2;
		}
	}
	return "-";
}

struct SCRIPT_DEFINITION {
	string SCRIPT_PATH;
	string SCRIPT_FILENAME;
	string MOD_NAME;
	string MOD_FILENAME;
	string MOD_DESCRIPTION;
	string MOD_NMS_VERSION;
	string MOD_AUTHOR;
	string MOD_UPDATE_LINK;
	string MOD_PAGE_LINK;
	bool UPDATEABLE;
	bool ACTIVE;
	bool SCRIPT_FROM_COLLECTION;
	string MOD_TOOLTIP;
	string MOD_TYPE;
	size_t MOD_TYPEINDEX;
};

vector<SCRIPT_DEFINITION> ScriptList = {};

void read_directory(vector<string>& FileList, const string& path) {
	std::string pattern(path);
	pattern.append("\\*");
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &FindFileData)) != INVALID_HANDLE_VALUE) {
		do {
			if (strlen(FindFileData.cFileName) > 4) {
				FileList.push_back(FindFileData.cFileName);
			}
		} while (FindNextFile(hFind, &FindFileData) != 0);
		FindClose(hFind);
	}
}

bool ScriptAlreadyActive(string Filename) {
	if (!ScriptList.empty()) {
		for (size_t i = 0; i < ScriptList.size(); i++) {
			if (StringContains(ScriptList[i].SCRIPT_FILENAME, Filename)) {
				return true;
			}
		}
	}
	return false;
}

string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
}

void LoadFileList(string dir) {
	vector<string> FileList = {};
	read_directory(FileList, dir);
	size_t i = ScriptList.size()-1;
	for (size_t j = 0; j < FileList.size(); j++) {
		if (!ScriptAlreadyActive(FileList[j]) && !StringEmpty(FileList[j])) {
			if (StringContains(FileList[j], ".lua")) {
				ScriptList.push_back(SCRIPT_DEFINITION());
				i += 1;
				ScriptList[i].SCRIPT_PATH = dir + "\\" + FileList[j];
				ScriptList[i].SCRIPT_FILENAME = FileList[j];
				ScriptList[i].MOD_FILENAME = "";
				ScriptList[i].MOD_DESCRIPTION = "";
				ScriptList[i].MOD_NMS_VERSION = "";
				ScriptList[i].MOD_AUTHOR = "";
				ScriptList[i].MOD_DESCRIPTION = "";
				ScriptList[i].MOD_PAGE_LINK = "";
				ScriptList[i].MOD_UPDATE_LINK = "";
				ScriptList[i].MOD_TYPE = "";
				ScriptList[i].MOD_TYPEINDEX = 0;
				ScriptList[i].SCRIPT_FROM_COLLECTION = false;
				ScriptList[i].ACTIVE = false;
				if (!StringContains(dir, "Collection")) {
					ScriptList[i].ACTIVE = true;
				}
				else {
					ScriptList[i].SCRIPT_FROM_COLLECTION = true;
				}
				ScriptList[i].UPDATEABLE = false;

				std::ifstream file(ScriptList[i].SCRIPT_PATH);
				std::string ScriptLine;
				while (std::getline(file, ScriptLine)) {
					if (StringEmpty(ScriptList[i].MOD_FILENAME) && (StringContains(ScriptLine, "MOD_FILENAME"))) {
						ScriptList[i].MOD_FILENAME = ExtractString(ScriptLine);
					}
					else if (StringEmpty(ScriptList[i].MOD_DESCRIPTION) && StringContains(ScriptLine, "DESCRIPTION")) {
						ScriptList[i].MOD_DESCRIPTION = ExtractString(ScriptLine);
					}
					else if (StringEmpty(ScriptList[i].MOD_NMS_VERSION) && StringContains(ScriptLine, "NMS_VERSION")) {
						ScriptList[i].MOD_NMS_VERSION = ExtractString(ScriptLine);
					}
					else if (StringEmpty(ScriptList[i].MOD_AUTHOR) && StringContains(ScriptLine, "MOD_AUTHOR")) {
						ScriptList[i].MOD_AUTHOR = ExtractString(ScriptLine);
					}
					else if (StringEmpty(ScriptList[i].MOD_TYPE) && StringContains(ScriptLine, "MOD_TYPE")) {
						ScriptList[i].MOD_TYPE = ExtractString(ScriptLine);
					}
					else if (StringEmpty(ScriptList[i].MOD_PAGE_LINK) && StringContains(ScriptLine, "MOD_PAGE_LINK")) {
						ScriptList[i].MOD_PAGE_LINK = ExtractString(ScriptLine);
					}
					else if (StringEmpty(ScriptList[i].MOD_UPDATE_LINK) && StringContains(ScriptLine, "UPDATE_LINK")) {
						ScriptList[i].MOD_UPDATE_LINK = ExtractString(ScriptLine);
						if (ScriptList[i].MOD_UPDATE_LINK.length() > 3) {
							ScriptList[i].UPDATEABLE = true;
						}
					}
				}
				if (StringEmpty(ScriptList[i].MOD_NAME)) {			
					ScriptList[i].MOD_NAME = FileList[j].substr(0, FileList[j].find_last_of(".")).substr(0,35);
				}
				ScriptList[i].MOD_TOOLTIP = "Description: " + ScriptList[i].MOD_DESCRIPTION + "\n\n" + "Author: " + ScriptList[i].MOD_AUTHOR + "\n\n" + "Minimum NMS Version: " + ScriptList[i].MOD_NMS_VERSION;
			}
		}
	}
}

void InitScriptList() {
	LoadFileList(ExePath() + "\\ModScript");
	LoadFileList(ExePath() + "\\ModScriptCollection");
}

#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

void UpdateScriptFromPastebin(LPCTSTR URL, LPCTSTR filepath) {
	HRESULT hr = URLDownloadToFile(NULL, URL, filepath, NULL, NULL);
	ScriptList.clear();
	InitScriptList();
	//if (FAILED(hr)){}
}

//void ExecuteExternalScript(string ScriptPath) {
//	std::ifstream t(ScriptPath);
//	std::stringstream buffer;
//	buffer << t.rdbuf();
//	ExecuteSAGELuaCode(buffer.str().c_str());
//}

void BuildMods() {
	bool ScriptToProcess = false;
	string SCRIPT_DIR = ExePath() + "\\ModScript\\";
	for (size_t i = 0; i < ScriptList.size(); i++) {
		if (ScriptList[i].ACTIVE) {
			if (ScriptList[i].SCRIPT_FROM_COLLECTION) {
				if (CopyFile(ScriptList[i].SCRIPT_PATH.c_str(), SCRIPT_DIR.c_str(), TRUE)) {
					ScriptToProcess = true;
				}
			}
			else {
				ScriptToProcess = true;
			}
		}
	}

	if (ScriptToProcess) {
		string cmd = "BuildMod.bat";
		if (CopyToGamefolder) { cmd = cmd + " $true"; } else { cmd = cmd + " $false"; }
		if (UpdateMBinCompiler) { cmd = cmd + " $true"; } else { cmd = cmd + " $false"; }
		if (CombinedModPak) { cmd = cmd + " $true"; } else { cmd = cmd + " $false"; }
		if (CheckForModConflicts) { cmd = cmd + " $true"; } else { cmd = cmd + " $false"; }
		if (ReCreateMapFileTree) { cmd = cmd + " $true"; } else { cmd = cmd + " $false"; }
		system(cmd.c_str());
		//ShellExecute(NULL, "open", "cmd", "/c BuildMod.bat", NULL, SW_NORMAL);
	}
}

#endif SCRIPTPARSER_H