#pragma once

#include "mrz_reader_global.h"
#include "mrz_data_type.h"
extern "C" {

	MRZREADER_EXPORT void* createContextMrz(const char* lib_root, int* error_code);
	MRZREADER_EXPORT int freeContextMrz(void* context);
	MRZREADER_EXPORT int getLastErrorMrz(int* error_code, char* message);

	
	MRZREADER_EXPORT int processMrz(int width, int height, int stride, char* image_data, int out_width, int out_height,
		int out_stride, char* out_image_data,MrzData* mrz_data, void* _context);
}