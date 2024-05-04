#pragma once

#include "ModelBase.h"
#include "model_utils.h"
#include <opencv2/core.hpp>
#include "ModelResult.h"

namespace mrz { 
	
	class  ModelSeg :public ModelBase{
	public:
		ModelSeg(const std::string& model_filename, const std::string& label_filename, bool use_cuda = false);
		
		~ModelSeg();

		
		virtual bool detect(const  cv::Mat& srcImg,  std::vector<ModelResult>& output)override;

	protected:

		float _maskThreshold = 0.5;
	};

}//mrz