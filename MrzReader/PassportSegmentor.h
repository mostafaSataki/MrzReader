#pragma once


#include <string>

#include "model_seg.h"

namespace mrz {



	class  PassportSegmentor : public ModelSeg {
	public:
		PassportSegmentor();
	protected:
		static inline const std::string model_filename_ = R"(model\model.onnx)";
		static inline const cv::Size2f norm_size_ = cv::Size2f(640, 640);
		static inline const std::string lable_filename_ = R"(model\model.txt)";

	};


}//BoneDens