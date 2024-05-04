#include "mrz_reader_export.h"
#include "MrzCheck.h"
#include "MrzException.h"
#include "AppInfo.h"
#include "MrzExceptionList.h"
#include "PassportReader.h"
#include "modelResult.h"
extern "C" {

	using namespace mrz;

void* createContextMrz(const char* lib_root, int* error_code)
{
	checkNullPointer(lib_root);

	PassportReader* context = nullptr;
	try {
		AppInfo::instance(std::string(lib_root));
		context = new PassportReader();


	}
	catch (FEException& e) {
		LastErrorMessage::setLastError(e);
		*error_code = e.errorCode();
		return nullptr;
	}
	*error_code = 0;

	return context;
}

int freeContextMrz(void* _context)
{
	checkNullPointer(_context);

	auto context = static_cast<PassportReader*>(_context);
	try {


		delete context;
	}
	catch (FEException& e) {
		LastErrorMessage::setLastError(e);
		return e.errorCode();
	}
	return FE_NO_ERROR;
}

int getLastErrorMrz(int* error_code, char* message)
{
	auto res = LastErrorMessage::getLastError();
	*error_code = res.first;
	strcpy(message, res.second.c_str());
	return FE_NO_ERROR;
}

int processMrz(int width, int height, int stride, char* image_data, int out_width, int out_height,
	int out_stride, char* out_image_data, MrzData* mrz_data, void* _context)
{
	checkNullPointer(_context);

	auto context = static_cast<PassportReader*>(_context);
	try
	{
		int channel_count = 0;
		checkImageProps(width, height, stride, image_data, channel_count, { 3 });

		cv::Mat src = cv::Mat(cv::Size(width, height), CV_8UC(channel_count), (void*)image_data, stride);
	
		int out_channel_count = 0;
		checkImageProps(out_width, out_height, out_stride, out_image_data, out_channel_count, { 3 });
		cv::Mat dst = cv::Mat(cv::Size(out_width, out_height), CV_8UC(out_channel_count), (void*)out_image_data, out_stride);

		auto result = context->process(src,dst,mrz_data);

	}
	catch (FEException& e) {
		LastErrorMessage::setLastError(e);
		return e.errorCode();
	}

	return FE_NO_ERROR;
}

}//extern "C"