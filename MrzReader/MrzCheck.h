#pragma once

#include <string>
#include <source_location>
#include <vector>
namespace mrz {
	class NullPointerException;

	std::wstring checkExistFile(const std::wstring& file_name);
	std::string checkExistFile(const std::string& file_name);
	void checkReadedFile(const FILE* file);

	template<typename T>
	void checkNullPointer(T* value, const std::source_location& location = std::source_location::current()) {
		if (value == nullptr) {
			throw NullPointerException(location);
		}
	}


	void checkEmptyDim(int width, int height, const std::source_location& location = std::source_location::current());
	void checkWidthStep(int width, int width_step, int& n_channels, const std::source_location& location = std::source_location::current());

	void checkChannelCount(int channel_count, const std::vector<int>& channels, const std::source_location& location = std::source_location::current());

	void checkImageProps(int width, int height, int stride, const char* image_data, int& channel_count, const std::vector<int>& channels,
		const std::source_location& location = std::source_location::current());
}//mrz
