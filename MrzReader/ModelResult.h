#pragma once

#include <opencv2/core.hpp>

namespace mrz {
	struct  ModelResult {
		int id;
		float confidence;
		std::string label_;
		cv::Rect box;
		cv::Mat boxMask;
		std::vector<float> keypoints;
	};

}//mrz