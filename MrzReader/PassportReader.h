#pragma once

#include <memory>
#include <string>
#include <opencv2/core.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "mrz_data_type.h"

namespace mrz {

	class PassportSegmentor;
	class ModelResult;
	class YuNet;
	

	class PassportReader
	{
	public:
		PassportReader();
		
		std::shared_ptr< ModelResult> process(const cv::Mat& imagee, cv::Mat& dst_image, MrzData* mrz_data);
	
	protected:
		std::shared_ptr< PassportSegmentor> classifier_;
		std::shared_ptr< YuNet> face_detector_;
		std::shared_ptr<tesseract::TessBaseAPI> api_;
	
		//MrzData mrz_data_;
		bool checkLicense();
		cv::Rect detect2(cv::InputArray _src);

		void threshold(const cv::Mat& src, cv::Mat& dst);
		void read(const cv::Mat& src, MrzData* mrz_data);
		void filterImage(const cv::Mat& src, cv::Mat& dst);
		void setMrzData(const std::string& text, MrzData& data);
		void getLines(const std::string& text, std::string& line1, std::string& line2);
		void parseLine1(const std::string& text, MrzData& data);
		void parseLine2(const std::string& text, MrzData& data);
		std::pair<int, int> findNextSep(const std::string& text, int pos);
	};
}

