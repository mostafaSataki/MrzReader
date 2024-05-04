#include "PassportReader.h"
#include "AppInfo.h"
#include "utility.h"
#include "PassportSegmentor.h"
#include <string>
#include "mrz_reader_type.h"
#include "dip_utility.h"
#include "YuNet.h"
#include "mrz_data_type.h"
namespace mrz {
	constexpr auto start_date = std::chrono::year{ 2024 } / std::chrono::month(4) / 24;
	constexpr  auto end_date = std::chrono::year{ 2024 } / std::chrono::month(5) / 24;

	constexpr auto kSoftwareKey = L"Software\\PVD";
	constexpr auto kLicenseKey = L"FE";

	PassportReader::PassportReader()
	{
		api_ = std::make_shared<tesseract::TessBaseAPI>() ;


		if (api_->Init(AppInfo::instance()->getFullFilename("tessdata").c_str(), "eng", tesseract::OEM_TESSERACT_ONLY)) {
			fprintf(stderr, "Could not initialize tesseract.\n");
			exit(1);
		}
		api_->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
		api_->SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789<");
		
		
		classifier_ = std::make_shared<PassportSegmentor>();
		face_detector_ = std::make_shared<YuNet>();
	}
	int findDirect(const cv::Mat& _image) {
		cv::Mat gray;
		cv::cvtColor(_image, gray, cv::COLOR_BGR2GRAY);
		cv::Mat image;
		cv::resize(gray, image, cv::Size(), 0.95, 0.95);
		cv::adaptiveThreshold(image, image, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 25, 10);

	/*	cv::imshow("view", image);
		cv::waitKey(100);*/
		return 0;
	}
	std::shared_ptr< ModelResult> PassportReader::process(const cv::Mat& image,cv::Mat& dst, MrzData* mrz_data )
	{
		//cv::Mat image;
		//fitOnSizeMat3(_image, image, cv::Size(1000, 1500), 1, true);
		std::shared_ptr< ModelResult> result = std::make_shared<ModelResult>();
		result->id = -1;
		result->confidence = 0.f;


		auto mask = cv::Mat(image.size(), CV_8UC1);
		mask.setTo(cv::Scalar::all(0));

		std::vector<ModelResult> segment_results;
		classifier_->detect(image, segment_results);
		for (auto segment_result : segment_results)
		{
			mask(segment_result.box).setTo(cv::Scalar::all(255), segment_result.boxMask);
	
		}
		Contours contours;
		cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


		Contours result_contours;
		for (int i = 0; i < contours.size(); i++) {
			auto bb = cv::boundingRect(contours[i]);
			if (bb.width < 2 || bb.height < 2)
				continue;
			result_contours.push_back(contours[i]);
		}

		if (result_contours.size() > 0) {
			auto max_contour = findMaxContour(result_contours);
			max_contour = approximateContour(max_contour);

			reverse(max_contour.begin(), max_contour.end());
			
			//forceClockwiseContour(max_contour);
			cv::Mat dst_image;
			rectifyImage(image, dst_image, max_contour, cv::Size(1300, 900));
			dst_image.copyTo(dst);
			auto region = detect2(dst_image);

			cv::Mat mrz_patch = dst_image(region);
			cv::Mat thresh_img;
			//cv::cvtColor(mrz_patch, thresh_img,cv::COLOR_BGR2GRAY);
			
			threshold(mrz_patch, thresh_img);
			read(thresh_img,mrz_data);
	

	/*		cv::Mat view = dst_image.clone();
			cv::rectangle(view, region, CV_RGB(0, 255, 0), 5);
			cv::imshow("view", view);
			cv::waitKey(100);*/
			//findDirect(dst_image);
			//auto face_resgions = face_detector_->infer(dst_image, 0.5);
			//if (face_resgions.size() == 0)
			//	cv::rotate(dst_image, dst_image, cv::ROTATE_180);

			//result_contours = { max_contour };

			//cv::Mat view = image.clone();


			//cv::drawContours(view, result_contours, -1, CV_RGB(0, 255, 0), 5);
			//for (int j = 0; j < result_contours[0].size(); j++)
			//	cv::circle(view, result_contours[0][j],10, CV_RGB(255, 0, 0), -1);
			//cv::resize(view, view, cv::Size(), 0.15, 0.15);
			///*cv::imshow("view", dst_image);
			//cv::waitKey(0);*/
		}

		return result;
	}

	
	cv::Rect PassportReader::detect2(cv::InputArray _src)
	{
//#define ECV_SHOW_IMAGE_TOTAL
#ifdef ECV_SHOW_IMAGE_TOTAL
#define ECV_SHOW_IMAGE
#endif

		//#define ECV_SHOW_IMAGE
		cv::Rect result;
		cv::Mat src = _src.getMat();
		auto scale = 600.f / src.rows;
		cv::resize(src, src, cv::Size(), scale, scale, cv::INTER_AREA);
		cv::Mat gray;
		cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
		cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);

		cv::Mat mask;
		cv::adaptiveThreshold(gray, mask, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 41, 10);

		Contours contours;
		cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		auto all_rects = boundingRects(contours);
		std::vector<cv::Rect> chars_rects;
		for (int i = 0; i < all_rects.size(); i++) {
			auto r = all_rects[i];
			auto ratio = rectRatio(r);
			if (ratio > 0)
				cv::drawContours(mask, contours, i, cv::Scalar::all(0), -1);

			else {

				ratio = abs(ratio);
				if (ratio < 1 || ratio > 2.8)
					cv::drawContours(mask, contours, i, cv::Scalar::all(0), -1);
				else chars_rects.push_back(r);
			}



		}

		cv::Mat mask_org;

		cv::dilate(mask, mask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 1), cv::Point(2, 0)), cv::Point(2, 0), 5);
		cv::erode(mask, mask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 1), cv::Point(2, 0)), cv::Point(2, 0), 7);
		cv::dilate(mask, mask, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 1), cv::Point(2, 0)), cv::Point(2, 0), 2);

		auto regs = boundingRects(mask);
		std::vector<cv::Rect> regions;
		std::copy_if(regs.begin(), regs.end(), std::back_inserter(regions), [mask](const cv::Rect& r) {return ((float)r.width / r.height > 10) && (r.width > mask.cols * 0.7); });

#ifdef ECV_SHOW_IMAGE
		cv::Mat view = src.clone();
		drawRects(view, regions, CV_RGB(0, 255, 0), 2);
#endif
		auto rects = groupingRects(regions, chars_rects, 0.5f);

		for (int i = 0; i < rects.size(); i++)
			if (rects[i].size() > 30 && rects[i].size() < 50)
				if (result.empty())
					result = regions[i];
				else result |= regions[i];


#ifdef ECV_SHOW_IMAGE
		cv::rectangle(src, result, CV_RGB(0, 255, 0), 2);
		cv::imshow("view", src);
		cv::waitKey(10);
#undef ECV_SHOW_IMAGE
#endif
		result = scaleRect(result, 1.f / scale);

		return result;
	}

	bool PassportReader::checkLicense() {
		DWORD dwData;
		if (!readRegistryValue(std::wstring(kSoftwareKey), std::wstring(kLicenseKey), dwData)) {
			return createRegistryKey(std::wstring(kSoftwareKey), std::wstring(kLicenseKey), 1);
		}

		auto result = dwData != 0;
		result &= isTodayInBetween(start_date, end_date);
		if (!result)
			writeRegistryValue(std::wstring(kSoftwareKey), std::wstring(kLicenseKey), 0);
		return result;
	}
	void PassportReader::threshold(const cv::Mat& src,cv::Mat& dst) {
		
		cv::cvtColor(src, dst, cv::COLOR_BGR2GRAY);
		cv::Mat thresh;
		cv::adaptiveThreshold(dst, dst, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 55, 25);
	
	}

	void PassportReader::read(const cv::Mat& src, MrzData* mrz_data) {

#ifdef ECV_SHOW_IMAGE_TOTAL
#define ECV_SHOW_IMAGE
#endif

		//#define ECV_SHOW_IMAGE

		
		std::string two_lines_txt;
		cv::Mat src_bin;
		filterImage(src, src_bin);
		
		
		int borderType = cv::BORDER_CONSTANT;
		int borderSize = 50; // Border size in pixels
		cv::Scalar white(255, 255, 255); // White color

		// Create a border around the image
		cv::copyMakeBorder(src_bin, src_bin, borderSize, borderSize, borderSize, borderSize, borderType, white);
#ifdef ECV_SHOW_IMAGE
		imshow("thresh", src);
#endif

		char* outText;
		//cv::GaussianBlur(src_bin, src_bin, cv::Size(5, 5), 0);
		//api_->Init(AppInfo::instance()->getFullFilename("tessdata").c_str(), "eng", tesseract::OEM_TESSERACT_ONLY);
		//api_->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
		api_->SetImage((uchar*)src_bin.data, src_bin.size().width, src_bin.size().height, src_bin.channels(), (int)src_bin.step1(0));

		outText = api_->GetUTF8Text();

#ifdef ECV_SHOW_IMAGE
		printf("OCR output:\n%s", outText);
#undef ECV_SHOW_IMAGE
#endif

		//api_->End();
		clearMrzData(*mrz_data);
		setMrzData(outText, *mrz_data);


		delete[] outText;

		//printMrzData(mrz_data_);

	}


	void PassportReader::filterImage(const cv::Mat& src, cv::Mat& dst) {
	
		cv::Mat src2;
		cv::bitwise_not(src, src2);

		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(src2.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		for (int i = 0; i < contours.size(); i++) {
			auto contour = contours[i];
			int contour_area = (int)cv::contourArea(contour);
			if (contour_area < 10)
				cv::drawContours(src2, contours, i, CV_RGB(0, 0, 0), 1);

		}

		cv::bitwise_not(src2, dst);

		
	}

	void PassportReader::setMrzData(const std::string& text, MrzData& data)
	{
		std::string line1;
		std::string line2;
		getLines(text, line1, line2);
		parseLine1(line1, data);
		parseLine2(line2, data);


	}

	void PassportReader::getLines(const std::string& text, std::string& line1, std::string& line2)
	{
		std::string txt = text;
		txt.erase(std::remove_if(txt.begin(), txt.end(), [](char& item) {return item == ' '; }), txt.end());
		auto lines = tokenize(txt, "\n");
		if (lines.size() != 2)
			return;
		line1 = lines[0];
		line2 = lines[1];

	}

	void PassportReader::parseLine1(const std::string& text, MrzData& data)
	{
		copyString(std::string(text.begin() + 2, text.begin() + 5), data.country_, country_len);
		
		int index = 5;
		auto pos_l = findNextSep(text, index);
		std::string last_name = "";
		if (pos_l.second - pos_l.first == 0) {
			while (pos_l.second - pos_l.first == 0) {
				if (last_name != "")
					last_name += " ";
				last_name += std::string(text.begin() + index, text.begin() + pos_l.first);
				index = pos_l.second + 1;
				pos_l = findNextSep(text, pos_l.second + 1);

			}
		}
		if (last_name != "")
			last_name += " ";
		last_name += std::string(text.begin() + index, text.begin() + pos_l.first);
		copyString(last_name, data.last_name_, last_name_len);

		index = pos_l.second + 1;
		auto pos_g = findNextSep(text, pos_l.second + 1);
		std::string given_name = "";
		if (pos_g.second - pos_g.first == 0) {
			while (pos_g.second - pos_g.first == 0) {

				if (given_name != "")
					given_name += " ";
				given_name += std::string(text.begin() + index, text.begin() + pos_g.first);
				index = pos_g.second + 1;
				pos_g = findNextSep(text, pos_g.second + 1);
			}
		}
		if (given_name != "")
			given_name += " ";
		given_name += std::string(text.begin() + index, text.begin() + pos_g.first);

		copyString(given_name, data.given_name_, given_name_len);

		//data.given_name_ = std::string(text.begin() +pos_l.second + 1, text.begin() +pos_g.first );

	}

	void PassportReader::parseLine2(const std::string& text, MrzData& data)
	{
		std::string document_number = "";
		document_number = std::string(text.begin(), text.begin() + 9);
		auto pos_l = findNextSep(text, 0);
		if (pos_l.first == 8 && pos_l.second == 8)
			document_number.resize(8);
		copyString(document_number, data.document_number_, document_number_len);

		copyString(std::string(text.begin() + 10, text.begin() + 13), data.nationality_, nationality_len);
		
		copyString(std::string(text.begin() + 13, text.begin() + 19), data.brith_date_, brith_date_len);
		
		copyString(std::string(text.begin() + 20, text.begin() + 21), data.sex_, sex_len);

		copyString(std::string(text.begin() + 21, text.begin() + 27), data.expiry_date_, expiry_date_len);
		
		

	}

	std::pair<int, int> PassportReader::findNextSep(const std::string& text, int pos)
	{
		const std::string sep = "<";
		std::pair<int, int> result;
		auto sep_pos = std::find_first_of(text.begin() + pos, text.end(), sep.begin(), sep.end());
		if (sep_pos != text.end()) {
			result.first = (int)std::distance(text.begin(), sep_pos);
			result.second = result.first;

			while (result.second + 1 < text.size() && text[result.second + 1] == sep[0]) {
				result.second++;
			}
		}
		return result;

	}


}//mrz