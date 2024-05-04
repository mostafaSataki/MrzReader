#include"model_seg.h"

namespace mrz {

	using namespace std;
	using namespace cv;
	using namespace cv::dnn;

	

	ModelSeg::ModelSeg(const std::string& model_filename, const std::string& label_filename, bool use_cuda ):
		ModelBase( model_filename,  label_filename, use_cuda )
	{
	}

	ModelSeg::~ModelSeg()
	{
	}



	bool ModelSeg::detect(const  Mat& srcImg,  std::vector<ModelResult>& output) {
		Mat blob;
		output.clear();
		int col = srcImg.cols;
		int row = srcImg.rows;
		Mat netInputImg;
		Vec4d params;
		LetterBox(srcImg, netInputImg, params, cv::Size(_netWidth, _netHeight));
		blobFromImage(netInputImg, blob, 1 / 255.0, cv::Size(_netWidth, _netHeight), cv::Scalar(0, 0, 0), true, false);
		net_.setInput(blob);
		std::vector<cv::Mat> net_output_img;
		vector<string> output_layer_names{ "output0", "output1" };
		net_.forward(net_output_img, output_layer_names); //get outputs
		std::vector<int> class_ids;// res-class_id
		std::vector<float> confidences;// res-conf 
		std::vector<cv::Rect> boxes;// res-box
		std::vector<vector<float>> picked_proposals;  //output0[:,:, 4 + _className.size():net_width]===> for mask
		Mat output0 = Mat(Size(net_output_img[0].size[2], net_output_img[0].size[1]), CV_32F, (float*)net_output_img[0].data).t();  //[bs,116,8400]=>[bs,8400,116]
		int rows = output0.rows;
		int net_width = output0.cols;
		float* pdata = (float*)output0.data;

		for (int r = 0; r < rows; ++r) {
			cv::Mat scores(1, _className.size(), CV_32FC1, pdata + 4);
			Point classIdPoint;
			double max_class_socre;
			minMaxLoc(scores, 0, &max_class_socre, 0, &classIdPoint);
			max_class_socre = (float)max_class_socre;
			if (max_class_socre >= _classThreshold) {
				vector<float> temp_proto(pdata + 4 + _className.size(), pdata + net_width);
				picked_proposals.push_back(temp_proto);
				//rect [x,y,w,h]
				float x = (pdata[0] - params[2]) / params[0];
				float y = (pdata[1] - params[3]) / params[1];
				float w = pdata[2] / params[0];
				float h = pdata[3] / params[1];
				int left = MAX(int(x - 0.5 * w + 0.5), 0);
				int top = MAX(int(y - 0.5 * h + 0.5), 0);
				class_ids.push_back(classIdPoint.x);
				confidences.push_back(max_class_socre);
				boxes.push_back(Rect(left, top, int(w + 0.5), int(h + 0.5)));
			}
			pdata += net_width;//next line
		}
		//NMS
		vector<int> nms_result;
		NMSBoxes(boxes, confidences, _classThreshold, _nmsThreshold, nms_result);
		std::vector<vector<float>> temp_mask_proposals;
		Rect holeImgRect(0, 0, srcImg.cols, srcImg.rows);
		for (int i = 0; i < nms_result.size(); ++i) {

			int idx = nms_result[i];
			ModelResult result;
			result.id = class_ids[idx];
			result.confidence = confidences[idx];
			result.box = boxes[idx] & holeImgRect;
			temp_mask_proposals.push_back(picked_proposals[idx]);
			output.push_back(result);
		}
		MaskParams mask_params;
		mask_params.params = params;
		mask_params.srcImgShape = srcImg.size();
		mask_params.netHeight = _netHeight;
		mask_params.netWidth = _netWidth;
		mask_params.maskThreshold = _maskThreshold;
		for (int i = 0; i < temp_mask_proposals.size(); ++i) {
			GetMask2(Mat(temp_mask_proposals[i]).t(), net_output_img[1], output[i], mask_params);
		}





		if (output.size())
			return true;
		else
			return false;
	}


}//mrz