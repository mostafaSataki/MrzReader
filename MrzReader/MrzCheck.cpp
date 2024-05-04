#include "MrzCheck.h"
#include <filesystem>
#include "MrzExceptionList.h"
#include "utility.h"

namespace mrz {
	using namespace std;
	namespace fs = std::filesystem;

	std::wstring checkExistFile(const std::wstring& file_name) {
		if (!fs::exists(fs::path(file_name)))
		{
			throw FileNotExistException(file_name);
		}
		else return file_name;

	}
	std::string checkExistFile(const std::string& file_name) {
		if (!fs::exists(fs::path(file_name)))
		{
			throw FileNotExistException(s2ws(file_name));
		}
		else return file_name;


	}

	void checkReadedFile(const FILE* file)
	{
		if (file == nullptr)
			throw FileReadException();

	}

	void checkEmptyDim(int width, int height, const std::source_location& location)
	{
		int area = width * height;
		if (area == 0)
			throw DimensionWrongException(location);

	}
	void checkWidthStep(int width, int width_step, int& n_channels, const std::source_location& location)
	{
		bool result = true;
		n_channels = (int)(width_step / width);

		int ws = ((int)ceil((width * n_channels) / 4.0)) * 4;
		//if (ws != width_step)
		//	result = false;


		if (!result)
			throw StrideWrongException(location);
	}

	void checkChannelCount(int channel_count, const std::vector<int>& channels, const std::source_location& location) {
		bool result = false;
		for (auto channel : channels)
			if (channel == channel_count) {
				result = true;
				break;
			}
		if (!result)
			throw ChannelCountWrongException(location);
	}



	void checkImageProps(int width, int height, int stride, const char* image_data, int& channel_count, const std::vector<int>& channels,
		const std::source_location& location) {

		checkEmptyDim(width, height, location);
		checkWidthStep(width, stride, channel_count, location);
		checkChannelCount(channel_count, channels, location);
		checkNullPointer(image_data, location);

	}


}//mrz