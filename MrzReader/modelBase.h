#pragma once


#include <opencv2/dnn.hpp>
#include "model_utils.h"
namespace mrz {


	class  ModelBase
	{
	public:
		ModelBase(const std::string& model_filename, const std::string& label_filename, bool use_cuda = false);

		virtual bool detect(const cv::Mat& srcImg, std::vector<ModelResult>& output) = 0;
	protected:
		int _netWidth = 640;
		int _netHeight = 640;

		bool use_cuda_;
		std::string model_filename_;
		std::string label_filename_;
		float _classThreshold = 0.25;
		float _nmsThreshold = 0.45;

		cv::dnn::Net net_;
		std::vector<std::string> _className ;
		bool ReadModel();
		void loadLabelFilename();

	};

}//mrz