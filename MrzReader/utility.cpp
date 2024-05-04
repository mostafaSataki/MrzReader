#include "utility.h"
#include <Windows.h>
#include <fstream>
#include "MrzCheck.h"
#include <locale>
#include <codecvt>
#include <regex>
#include <algorithm>
namespace mrz {



	std::wstring s2ws(const std::string& str)
	{
		typedef std::codecvt_utf8<wchar_t> convert_typeX;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str);
	}

	std::string ws2s(const std::wstring& wstr)
	{
		typedef std::codecvt_utf8<wchar_t> convert_typeX;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		std::string result = converterX.to_bytes(wstr);
		return result;


	}

	

	std::string toLower(const std::string& value) {
		std::string result = value;
		std::transform(value.begin(), value.end(), result.begin(), ::tolower);
		return result;
	}


	std::string getFileExt(std::string fullName)
	{
		/*std::string fileName = getFileName(fullName);
		size_t index = fileName.find_last_of(".") - 1;
		char result[200];
		int len = int(fileName.size() - index) - 1;
		fileName.copy(result, len, index + 1);
		result[len] = '\0';
		return std::string(result);*/
		std::filesystem::path filePath(fullName);
		return filePath.extension().string();
	}

	std::wstring getFileExt(const std::wstring& full_name)
	{

		std::filesystem::path file_path(full_name);
		return file_path.extension().wstring();
	}


	std::string getExePath()
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		std::string::size_type pos = std::string(buffer).find_last_of("\\/");
		return std::string(buffer).substr(0, pos);
	}

	void getPathStr(fs::path value, std::string& res)
	{
		res = value.string();
	}

	void getPathStr(fs::path value, std::wstring& res)
	{
		res = value.wstring();

	}
	std::vector<std::string> readLabelFile(const std::string& filename)
	{
		std::vector<std::string> result;
		if (filename.empty())
			return result;

		std::ifstream file(filename);
		std::string line;
		while (std::getline(file, line))
		{
			result.push_back(line);

		}
		return result;
	}
	void getImageFilter(std::vector<std::string>& result)
	{

		result.push_back(".bmp");
		result.push_back(".jpg");
		result.push_back(".jpeg");
		result.push_back(".png");
		result.push_back(".tif");
		result.push_back(".tiff");
		result.push_back(".pdf");


	}
	std::vector<std::string> getFolderImages(const std::string& path, bool shuffle)
	{
		return getFiles<std::string>(path, getImageFilter<std::string>());

	}

	std::vector<std::string> ends_with(const std::vector<std::string>& items, const std::string& right) {

		std::vector<std::string> result;
		for (auto& item : items)
			if (item.ends_with(right))
				result.push_back(item);
		return result;
	}

	bool filename_ends_with(const std::string& filename, const std::string& right) {
		auto fname = filenameWithoutExtension(filename);
		return fname.ends_with(right);
	}
	std::vector<std::string> filenames_ends_with(const std::vector<std::string>& items, const std::string& right) {
		std::vector<std::string> result;
		for (const auto& item : items)
			if (filename_ends_with(item, right))
				result.push_back(item);
		return result;
	}

	std::ifstream::pos_type getFileSize(const std::wstring& file_name)
	{
		std::ifstream in(file_name, std::ifstream::ate | std::ifstream::binary);
		return in.tellg();
	}
	bool readBinaryFile(const std::wstring& filename, std::vector<unsigned char>& contents) {
		checkExistFile(filename);
		int size = getFileSize(filename);
		FILE* f = _wfopen(filename.c_str(), L"rb");
		checkReadedFile(f);
		

		contents.clear();
		contents.resize(size);


		fread(contents.data(), 1, contents.size(), f);
		fclose(f);


		return true;
	}

	bool readBinaryFile(const std::string& filename, std::vector<unsigned char>& contents) {
		return readBinaryFile(s2ws(filename), contents);
	}

	std::string filenameWithoutExtension(const std::string& filename) {
		std::filesystem::path path(filename);
		return path.stem().string();
	}
	std::vector<std::string> filenamesWithoutExtension(const  std::vector<std::string>& filenames) {
		std::vector<std::string> result;
		for (auto& filename : filenames)
			result.push_back(filename);
		return result;
	}

	std::vector<std::string> getSubfoldersWithToken(const std::string& path, const std::string& token) {
		std::vector<std::string> result;
		for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
			if (entry.is_directory()) {
				std::string folderName = entry.path().filename().string();
				folderName = toLower(folderName);
				if (folderName.find(token) != std::string::npos) {
					result.push_back(entry.path().string());
				}
			}
		}
		return result;
	}

	bool isTodayInBetween(const std::chrono::year_month_day& start, const std::chrono::year_month_day& end) {
		auto today = std::chrono::year_month_day{ std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()) };
		return (today >= start) && (today <= end);
	}

	bool createRegistryKey(const std::wstring& keyPath, const std::wstring& valueName, DWORD valueData) {
		HKEY hKey;
		if (RegCreateKeyEx(HKEY_CURRENT_USER, keyPath.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
			return false;
		}

		if (RegSetValueEx(hKey, valueName.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&valueData), sizeof(valueData)) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}

		RegCloseKey(hKey);
		return true;
	}

	bool readRegistryValue(const std::wstring& keyPath, const std::wstring& valueName, DWORD& valueData) {
		HKEY hKey;
		if (RegOpenKeyEx(HKEY_CURRENT_USER, keyPath.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
			return false;
		}

		DWORD dwType = REG_DWORD;
		DWORD dwSize = sizeof(DWORD);
		if (RegQueryValueEx(hKey, valueName.c_str(), nullptr, &dwType, reinterpret_cast<LPBYTE>(&valueData), &dwSize) != ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return false;
		}

		RegCloseKey(hKey);
		return true;
	}
	LONG writeRegistryValue(const std::wstring& keyPath, const std::wstring& valueName, DWORD valueData) {
		HKEY hKey;
		LONG lResult;

		// Open the key
		lResult = RegOpenKeyExW(HKEY_CURRENT_USER, keyPath.c_str(), 0, KEY_WRITE, &hKey);
		if (lResult != ERROR_SUCCESS) {
			return lResult;
		}

		// Set the value
		lResult = RegSetValueExW(hKey, valueName.c_str(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&valueData), sizeof(valueData));

		// Close the key
		RegCloseKey(hKey);

		return lResult;
	}

	std::vector<std::string> tokenize(const std::string& str, const std::string& splitter)
	{
		std::vector<std::string> result;
		std::regex r(R"(\s*[)" + splitter + R"(]\s*)");

		std::sregex_token_iterator end;
		for (std::sregex_token_iterator it(str.begin(), str.end(), r, -1); it != end; it++)
			result.push_back(*it);

		return result;

	}
	std::vector<std::wstring> tokenize(const std::wstring& str, const std::wstring& splitter)
	{
		std::vector<std::wstring> result;
		std::wregex r(LR"(\s*[)" + splitter + LR"(]\s*)");

		std::wsregex_token_iterator end;
		for (std::wsregex_token_iterator it(str.begin(), str.end(), r, -1); it != end; it++)
			result.push_back(*it);

		return result;

	}

	void copyString(const std::string& src, char* dst, size_t dstSize) {
		// Ensure that we don't copy more characters than the destination can hold
		size_t length = std::min(src.size(), dstSize - 1);
		strncpy(dst, src.c_str(), length);
		// Null-terminate the destination string
		dst[length] = '\0';
	}
}//mrz