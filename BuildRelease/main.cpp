#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = filesystem;
const string LANGPACK = "./FakePlayerHelper/langpack.json";
const string BUILD_H  = "./FakePlayerHelper/build.h";
const string VCXPROJ  = "./FakePlayerHelper/FakePlayerHelper.vcxproj";
bool actions = true;

string readFile(fstream& file) {
	ostringstream oss;
	oss << file.rdbuf();
	return oss.str();
}

string readFile(const string& fn) {
	fstream file(fn, ios::in);
	ostringstream oss;
	oss << file.rdbuf();
	return oss.str();
}

size_t trim(string& s)
{
	if (s.empty()) return 0;
	auto sz = s.size();
	replace(s.begin(), s.end(), '\t', ' ');
	size_t b = s.find_first_not_of(' ');
	size_t e = s.find_last_not_of(' ');
	s = s.substr(b, e - b + 1);
	return sz - s.size();
}

bool modifyFile(string fn, string _regex, string target) {
	fstream file(fn, ios::app | ios::in);
	if (!file.is_open()) {
		cout << "[ERROR] Failed to open file: " << fn << endl;
		return false;
	}
	string content = readFile(file);
	istringstream iss(content);
	string line;
	size_t off = 0;
	bool flag = 0;
	regex rgx(_regex);
	cout << "[INFO] Trying finding a match for " << _regex << " ..." << endl;
	while (getline(iss, line)) {
		size_t _off = line.size() + 1;
		auto space = trim(line);
		if (regex_match(line, rgx)) {
			cout << "[INFO] Regex matched!" << endl;
			cout << "[INFO] Matched line: " << line << endl;
			content.replace(off + space, _off - space - 1, target);
			flag = true;
			break;
		}
		off += _off;
	}
	if (!flag) {
		cout << "[ERROR] Regex not matched!";
		return false;
	}
	cout << "[INFO] Writing...";
	file = fstream(fn, ios::ate | ios::out);
	file << content;
	file.close();
	cout << "DONE!" << endl;
	return true;
}

string getFiles(string dir) {
	string files;
	fs::directory_iterator fit(dir);
	for (auto& it : fit)
		files += '"' + it.path().string() + "\" ";
	return files;
}

int main(int argc, char** argv) {
	ios::sync_with_stdio(false);
	if (argc >= 2) {
		if (argv[1] == string("release")) {
			actions = false;
			try {
				fs::copy_file(BUILD_H, BUILD_H + ".tmp");
				fs::copy_file(VCXPROJ, VCXPROJ + ".tmp");
				if (!fs::exists("./RELEASES/")) 
					fs::create_directories("./RELEASES/");
				else { 
					fs::remove_all("./RELEASES/"); 
					fs::create_directories("./RELEASES/");
				}
			}
			catch (fs::filesystem_error e) {
				cout << "[ERROR] Filesystem error:" << e.what() << endl;
			}
			goto BDS_1_16;
		LA:
			system("MsBuild.exe ./FakePlayerHelper.sln -property:Configuration=Release");
			try {
				fs::copy("./x64/Release/", "./RELEASES/1.16.4/");
			}
			catch (fs::filesystem_error e) {
				cout << "[ERROR] Filesystem error:" << e.what() << endl;
			}
			goto LATEST_BDS;
		LB:
			system("MsBuild.exe ./FakePlayerHelper.sln -property:Configuration=Release");
			try {
				fs::copy("./x64/Release/", "./RELEASES/latest/");
				fs::copy_file("./LICENSE", "./RELEASES/1.16.4/LICENSE");
				fs::copy_file("./LICENSE", "./RELEASES/latest/LICENSE");
				fs::remove(BUILD_H);
				fs::remove(VCXPROJ);
				fs::rename(BUILD_H + ".tmp", BUILD_H);
				fs::rename(VCXPROJ + ".tmp", VCXPROJ);
				auto files = getFiles("./RELEASES/1.16.4/");
				system(("bz c -fmt:zip ./RELEASES/FakePlayerHelper-forBDS1.16.zip " + files).c_str());
				files = getFiles("./RELEASES/latest/");
				system(("bz c -fmt:zip ./RELEASES/FakePlayerHelper-forLatestBDS.zip " + files).c_str());
			}
			catch (fs::filesystem_error e) {
				cout << "[ERROR] Filesystem error:" << e.what() << endl;
			}
			return 0;
		}
		if (argv[1] == string("bds:1.16.4")) {
		BDS_1_16:
			cout << "[INFO] BDS v1.16.4" << endl;
			cout << "[INFO] Creating build config...";
			if (!fs::exists(BUILD_H)) fstream(BUILD_H, ios::app | ios::out);
			fstream file(BUILD_H, ios::ate | ios::out);
			if (!file.is_open()) {
				cout << "\n[ERROR] Failed to open file: " << BUILD_H << endl;
				return 1;
			}
			auto lpk = readFile(LANGPACK);
			if (actions) file << "#define BDS_V1_16\n#define BUILD_RELEASE\n#define ACTIONS_BUILD\n";
			else file << "#define BDS_V1_16\n#define BUILD_RELEASE\n";
			file << "const std::string FPH_LANGPACK_CONTENT = u8R\"(" << lpk << ")\";";
			file.close();
			cout << "DONE!" << endl;
			cout << "[INFO] Modify Visual Studio Project file" << endl;
			cout << "[INFO] Modifying sdditional include directories(Change LiteLoader SDK Version)" << endl;
			modifyFile(VCXPROJ, "<AdditionalIncludeDirectories>(.+)</AdditionalIncludeDirectories>",
				"<AdditionalIncludeDirectories>./LLSDK_1.16;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>");
			if (!actions) goto LA;
		}
		else if (argv[1] == string("bds:latest")) {
		LATEST_BDS:
			cout << "[INFO] BDS Latest" << endl;
			cout << "[INFO] Creating build config...";
			if (!fs::exists(BUILD_H)) fstream(BUILD_H, ios::app | ios::out);
			fstream file(BUILD_H, ios::ate | ios::out);
			if (!file.is_open()) {
				cout << "\n[ERROR] Failed to open file: " << BUILD_H << endl;
				return 1;
			}
			auto lpk = readFile(LANGPACK);
			if (actions) file << "#define BDS_LATEST\n#define BUILD_RELEASE\n#define ACTIONS_BUILD\n";
			else file << "#define BDS_LATEST\n#define BUILD_RELEASE\n";
			file << "const std::string FPH_LANGPACK_CONTENT = u8R\"(" << lpk << ")\";";
			file.close();
			cout << "DONE!" << endl;
			cout << "[INFO] Modifying Visual Studio Project file..." << endl;
			cout << "[INFO] Modifying sdditional include directories(Change LiteLoader SDK Version)" << endl;
			modifyFile(VCXPROJ, "<AdditionalIncludeDirectories>(.+)</AdditionalIncludeDirectories>",
				"<AdditionalIncludeDirectories>./LLSDK;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>");
			if (!actions) goto LB;
		}
		else {
			cout << "[ERROR] Invalid parameters" << endl;
			return 1;
		}
	}
	else {
		cout << "[ERROR] Insufficient parameters" << endl;
		return 1;
	}
	return 0;
}