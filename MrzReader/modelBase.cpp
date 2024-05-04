#include "modelBase.h"
#include "utility.h"
#include "MrzCheck.h"
#include "AppInfo.h"
#include "model_utils.h"

namespace mrz {

	using namespace cv::dnn;


	ModelBase::ModelBase(const std::string& model_filename, const std::string& label_filename, bool use_cuda):
		model_filename_{ model_filename },
		label_filename_{ label_filename },
		use_cuda_{ use_cuda }
	{
		ReadModel();
		loadLabelFilename();
	}

	void ModelBase::loadLabelFilename()
	{
		auto full_filename = AppInfo::instance()->getFullFilename(label_filename_);
		checkExistFile(full_filename);
		_className = readLabelFile(full_filename);
	}

	bool ModelBase::ReadModel() {
		try 
		{
			
			auto full_filename = AppInfo::instance()->getFullFilename(model_filename_);
			checkExistFile(full_filename);
			
			net_ = readNet(full_filename);
			
#if CV_VERSION_MAJOR==4 &&CV_VERSION_MINOR==7&&CV_VERSION_REVISION==0
			net_.enableWinograd(false);  //bug of opencv4.7.x in AVX only platform ,https://github.com/opencv/opencv/pull/23112 and https://github.com/opencv/opencv/issues/23080 
			//net.enableWinograd(true);		//If your CPU supports AVX2, you can set it true to speed up
#endif
		}
		catch (const std::exception&) {
			return false;
		}

		if (use_cuda_) {
			//cuda
			net_.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
			net_.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA); //or DNN_TARGET_CUDA_FP16
		}
		else {
			//cpu
			//cout << "Inference device: CPU" << endl;
			net_.setPreferableBackend(cv::dnn::DNN_BACKEND_DEFAULT);
			net_.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
		}
		return true;
	}

}//mrz