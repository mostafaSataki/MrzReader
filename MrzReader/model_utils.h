#pragma once


#include<iostream>
#include <numeric>

#include<io.h>


#include <vector>
#include "model_utils.h"
#include<opencv2/opencv.hpp>
#include <vector>
#include "modelResult.h"
namespace mrz {



	struct  MaskParams {
		//int segChannels = 32;
		//int segWidth = 160;
		//int segHeight = 160;
		int netWidth = 640;
		int netHeight = 640;
		float maskThreshold = 0.5;
		cv::Size srcImgShape;
		cv::Vec4d params;

	};
	 bool CheckModelPath(std::string modelPath);
	 bool CheckParams(int netHeight, int netWidth, const int* netStride, int strideSize);
	 void DrawPred(cv::Mat& img, std::vector<ModelResult> result, std::vector<std::string> classNames, std::vector<cv::Scalar> color);
	 void LetterBox(const cv::Mat& image, cv::Mat& outImage,
		cv::Vec4d& params, //[ratio_x,ratio_y,dw,dh]
		const cv::Size& newShape = cv::Size(640, 640),
		bool autoShape = false,
		bool scaleFill = false,
		bool scaleUp = true,
		int stride = 32,
		const cv::Scalar& color = cv::Scalar(114, 114, 114));
	 void GetMask(const cv::Mat& maskProposals, const cv::Mat& maskProtos, std::vector<ModelResult>& output, const MaskParams& maskParams);
	  void GetMask2(const cv::Mat& maskProposals, const cv::Mat& maskProtos, ModelResult& output, const MaskParams& maskParams);


	  std::tuple<std::vector<cv::Rect>, std::vector<float>, std::vector<int>, std::vector<std::vector<float>>>
		  non_max_suppression(const cv::Mat& output0, int class_names_num, int data_width, double conf_threshold,
			  float iou_threshold);

	  void clip_boxes(cv::Rect& box, const cv::Size& shape);
	  void clip_boxes(cv::Rect_<float>& box, const cv::Size& shape);
	  void clip_boxes(std::vector<cv::Rect>& boxes, const cv::Size& shape);
	  void clip_boxes(std::vector<cv::Rect_<float>>& boxes, const cv::Size& shape);
	  cv::Rect_<float> scale_boxes(const cv::Size& img1_shape, cv::Rect_<float>& box, const cv::Size& img0_shape,
		  std::pair<float, cv::Point2f> ratio_pad = std::make_pair(-1.0f, cv::Point2f(-1.0f, -1.0f)), bool padding = true);

	  void clip_coords(std::vector<float>& coords, const cv::Size& shape);

	  std::vector<float> scale_coords(const cv::Size& img1_shape, std::vector<float>& coords, const cv::Size& img0_shape);


	  int findMaxSegment(const std::vector<ModelResult>& results);
	  
	  


	  std::vector<int> findMaxConflictIndex(const std::vector<ModelResult>& src, const std::vector < ModelResult>& dst);
	  


}//mrz