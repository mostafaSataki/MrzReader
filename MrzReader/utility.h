#pragma once
#include <filesystem>
#include <source_location>

#include <fstream>
#include <chrono>
#include <Windows.h>
namespace mrz {


	namespace fs = std::filesystem;


	std::wstring s2ws(const std::string& str);
	std::string ws2s(const std::wstring& wstr);


	std::string getExePath();
	template<typename T>
	bool exist(const T& file_name)
	{
		fs::path p(file_name);
		return exists(p);

	}

	template<typename T>
	void path2Str(fs::path p, T& str) {
		if constexpr (std::is_same<T, std::string>::value)
			str = p.string();
		else if constexpr (std::is_same<T, std::wstring>::value)
			str = p.wstring();


	}


	template<typename T>
	T getFileName(const T& file_name)
	{
		T result;
		fs::path p(file_name);
		path2Str(p.filename(), result);
		return result;

	}
	template<typename T>
	std::vector<T> getFileNames(const std::vector<T>& filenames)
	{
		std::vector<T> result;
		for (const auto& filename : filenames)
			result.push_back(getFileName(filename));
		return result;

	}

	template<typename T>
	T join(const T& media1, const T& media2)
	{
		auto p = fs::path(media1);
		p /= media2;
		T result;
		path2Str(p, result);

		return result;
	}
	std::string toLower(const std::string& value);

	template<typename T>
	T getLowerStr(const T& value) {
		T result = value;
		std::transform(result.begin(), result.end(), result.begin(),
			[](unsigned char c) { return std::tolower(c); });

		return result;

	}

	template<typename T>
	std::vector<T> getLowerStrS(const std::vector<T>& items) {
		std::vector<T> result;
		for (auto& item : items)
			result.push_back(getLowerStr(item));
		return result;
	}
	std::string getFileExt(std::string fullName);

	std::wstring getFileExt(const std::wstring& full_name);



	void getPathStr(fs::path value, std::string& res);
	void getPathStr(fs::path value, std::wstring& res);


	std::vector<std::string> readLabelFile(const std::string& filename);

	void getImageFilter(std::vector<std::string>& result);

	template<class T>
	T extension(const fs::path& file_name) {
		T result;

		if constexpr (std::is_same<T, std::string>::value)
			result = file_name.extension().string();
		else if constexpr (std::is_same<T, std::wstring>::value)
			result = file_name.extension().wstring();
		return result;
	}
	template<typename T >
	std::vector<T> getImageFilter() {
		std::vector<T> result;
		getImageFilter(result);
		return result;
	}

	template <class T>
	bool checkFileExt(const fs::path& file_name, const std::vector<T>& filter) {
		auto cur_ext = extension<T>(file_name);
		std::transform(cur_ext.begin(), cur_ext.end(), cur_ext.begin(), ::tolower);
		auto pos = std::find_if(filter.begin(), filter.end(),
			[cur_ext](const T& item) {return cur_ext == item; });
		return pos != filter.end();

	}

	template <class T>
	void getFiles(const fs::path& folder, std::vector<T>& files, const std::vector<T>& filter) {

		if (!exists(folder)) {
			return;
		}
		if (is_regular_file(folder)) {
			if (checkFileExt(folder, filter)) {
				T file_name;
				getPathStr(folder, file_name);
				files.push_back(file_name);
			}
			else {
				//cout << folder << endl;
			}

		}
		else if (is_directory(folder)) {
			for (auto& entry : fs::directory_iterator(folder)) {
				//T file_name;
				//getPathStr(entry, file_name);
				//cout << file_name << endl;
				getFiles(entry, files, filter);
			}
		}

	}


	template <class T>
	std::vector<T> getFiles(const T& folder, const std::vector<T>& filter) {
		std::vector<T> result;
		getFiles(folder, result, filter);
		return result;
	}

	std::vector<std::string> getFolderImages(const std::string& path, bool shuffle = false);
	std::vector<std::string> ends_with(const std::vector<std::string>& items, const std::string& right);
	std::ifstream::pos_type getFileSize(const std::wstring& file_name);
	bool readBinaryFile(const std::wstring& filename, std::vector<unsigned char>& contents);
	bool readBinaryFile(const std::string& filename, std::vector<unsigned char>& contents);

	std::string filenameWithoutExtension(const std::string& filename);
	std::vector<std::string> filenamesWithoutExtension(const  std::vector<std::string>& filenames);
	std::vector<std::string> filenames_ends_with(const std::vector<std::string>& items, const std::string& right);

	template<typename T>
	T createFolderAppSide(const T& folder) {
		fs::path exe_path(getExePath());
		exe_path /= folder;
		if (!exists(exe_path))
			fs::create_directory(exe_path);

		T result;
		getPathStr(exe_path, result);
		return result;

	}

	template <typename T>
	void deleteFile(const T& filename) {
		if (exist(filename))
			fs::remove(filename);
	}


	std::vector<std::string> getSubfoldersWithToken(const std::string& path, const std::string& token);
	bool isTodayInBetween(const std::chrono::year_month_day& start, const std::chrono::year_month_day& end);
	bool createRegistryKey(const std::wstring& keyPath, const std::wstring& valueName, DWORD valueData);
	bool readRegistryValue(const std::wstring& keyPath, const std::wstring& valueName, DWORD& valueData);
	LONG writeRegistryValue(const std::wstring& keyPath, const std::wstring& valueName, DWORD valueData);

	std::vector<std::string> tokenize(const std::string& str, const std::string& splitter);
	
	std::vector<std::wstring> tokenize(const std::wstring& str, const std::wstring& splitter);
	void copyString(const std::string& src, char* dst, size_t dstSize);
}//mrz