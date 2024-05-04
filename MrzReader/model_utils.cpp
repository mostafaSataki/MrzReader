#include "model_utils.h"
#include <numeric>
//#include "dip_utility.h"

namespace mrz {

	using namespace cv;
	using namespace std;
	bool CheckParams(int netHeight, int netWidth, const int* netStride, int strideSize) {
		if (netHeight % netStride[strideSize - 1] != 0 || netWidth % netStride[strideSize - 1] != 0)
		{
			cout << "Error:_netHeight and _netWidth must be multiple of max stride " << netStride[strideSize - 1] << "!" << endl;
			return false;
		}
		return true;
	}
	bool CheckModelPath(std::string modelPath) {
		if (0 != _access(modelPath.c_str(), 0)) {
			cout << "Model path does not exist,  please check " << modelPath << endl;
			return false;
		}
		else
			return true;

	}
	void LetterBox(const cv::Mat& image, cv::Mat& outImage, cv::Vec4d& params, const cv::Size& newShape,
		bool autoShape, bool scaleFill, bool scaleUp, int stride, const cv::Scalar& color)
	{
		if (false) {
			int maxLen = MAX(image.rows, image.cols);
			outImage = Mat::zeros(Size(maxLen, maxLen), CV_8UC3);
			image.copyTo(outImage(Rect(0, 0, image.cols, image.rows)));
			params[0] = 1;
			params[1] = 1;
			params[3] = 0;
			params[2] = 0;
		}

		cv::Size shape = image.size();
		float r = std::min((float)newShape.height / (float)shape.height,
			(float)newShape.width / (float)shape.width);
		if (!scaleUp)
			r = std::min(r, 1.0f);

		float ratio[2]{ r, r };
		int new_un_pad[2] = { (int)std::round((float)shape.width * r),(int)std::round((float)shape.height * r) };

		auto dw = (float)(newShape.width - new_un_pad[0]);
		auto dh = (float)(newShape.height - new_un_pad[1]);

		if (autoShape)
		{
			dw = (float)((int)dw % stride);
			dh = (float)((int)dh % stride);
		}
		else if (scaleFill)
		{
			dw = 0.0f;
			dh = 0.0f;
			new_un_pad[0] = newShape.width;
			new_un_pad[1] = newShape.height;
			ratio[0] = (float)newShape.width / (float)shape.width;
			ratio[1] = (float)newShape.height / (float)shape.height;
		}

		dw /= 2.0f;
		dh /= 2.0f;

		if (shape.width != new_un_pad[0] && shape.height != new_un_pad[1])
		{
			cv::resize(image, outImage, cv::Size(new_un_pad[0], new_un_pad[1]));
		}
		else {
			outImage = image.clone();
		}

		int top = int(std::round(dh - 0.1f));
		int bottom = int(std::round(dh + 0.1f));
		int left = int(std::round(dw - 0.1f));
		int right = int(std::round(dw + 0.1f));
		params[0] = ratio[0];
		params[1] = ratio[1];
		params[2] = left;
		params[3] = top;
		cv::copyMakeBorder(outImage, outImage, top, bottom, left, right, cv::BORDER_CONSTANT, color);
	}

	void GetMask(const cv::Mat& maskProposals, const cv::Mat& maskProtos, std::vector<ModelResult>& output, const MaskParams& maskParams) {
		//cout << maskProtos.size << endl;

		int net_width = maskParams.netWidth;
		int net_height = maskParams.netHeight;
		int seg_channels = maskProtos.size[1];
		int seg_height = maskProtos.size[2];
		int seg_width = maskProtos.size[3];
		float mask_threshold = maskParams.maskThreshold;
		Vec4f params = maskParams.params;
		Size src_img_shape = maskParams.srcImgShape;

		Mat protos = maskProtos.reshape(0, { seg_channels,seg_width * seg_height });

		Mat matmul_res = (maskProposals * protos).t();
		Mat masks = matmul_res.reshape(output.size(), { seg_width,seg_height });
		vector<Mat> maskChannels;
		split(masks, maskChannels);
		for (int i = 0; i < output.size(); ++i) {
			Mat dest, mask;
			//sigmoid
			cv::exp(-maskChannels[i], dest);
			dest = 1.0 / (1.0 + dest);

			Rect roi(int(params[2] / net_width * seg_width), int(params[3] / net_height * seg_height), int(seg_width - params[2] / 2), int(seg_height - params[3] / 2));
			dest = dest(roi);
			resize(dest, mask, src_img_shape, INTER_NEAREST);

			//crop
			Rect temp_rect = output[i].box;
			mask = mask(temp_rect) > mask_threshold;
			output[i].boxMask = mask;
		}
	}

	void GetMask2(const Mat& maskProposals, const Mat& maskProtos, ModelResult& output, const MaskParams& maskParams) {
		int net_width = maskParams.netWidth;
		int net_height = maskParams.netHeight;
		int seg_channels = maskProtos.size[1];
		int seg_height = maskProtos.size[2];
		int seg_width = maskProtos.size[3];
		float mask_threshold = maskParams.maskThreshold;
		Vec4f params = maskParams.params;
		Size src_img_shape = maskParams.srcImgShape;

		Rect temp_rect = output.box;
		//crop from mask_protos
		int rang_x = floor((temp_rect.x * params[0] + params[2]) / net_width * seg_width);
		int rang_y = floor((temp_rect.y * params[1] + params[3]) / net_height * seg_height);
		int rang_w = ceil(((temp_rect.x + temp_rect.width) * params[0] + params[2]) / net_width * seg_width) - rang_x;
		int rang_h = ceil(((temp_rect.y + temp_rect.height) * params[1] + params[3]) / net_height * seg_height) - rang_y;

		//如果下面的 mask_protos(roi_rangs).clone()位置报错，说明你的output.box数据不对，或者矩形框就1个像素的，开启下面的注释部分防止报错。
		rang_w = MAX(rang_w, 1);
		rang_h = MAX(rang_h, 1);
		if (rang_x + rang_w > seg_width) {
			if (seg_width - rang_x > 0)
				rang_w = seg_width - rang_x;
			else
				rang_x -= 1;
		}
		if (rang_y + rang_h > seg_height) {
			if (seg_height - rang_y > 0)
				rang_h = seg_height - rang_y;
			else
				rang_y -= 1;
		}

		vector<Range> roi_rangs;
		roi_rangs.push_back(Range(0, 1));
		roi_rangs.push_back(Range::all());
		roi_rangs.push_back(Range(rang_y, rang_h + rang_y));
		roi_rangs.push_back(Range(rang_x, rang_w + rang_x));

		//crop
		Mat temp_mask_protos = maskProtos(roi_rangs).clone();
		Mat protos = temp_mask_protos.reshape(0, { seg_channels,rang_w * rang_h });
		Mat matmul_res = (maskProposals * protos).t();
		Mat masks_feature = matmul_res.reshape(1, { rang_h,rang_w });
		Mat dest, mask;

		//sigmoid
		cv::exp(-masks_feature, dest);
		dest = 1.0 / (1.0 + dest);

		int left = floor((net_width / seg_width * rang_x - params[2]) / params[0]);
		int top = floor((net_height / seg_height * rang_y - params[3]) / params[1]);
		int width = ceil(net_width / seg_width * rang_w / params[0]);
		int height = ceil(net_height / seg_height * rang_h / params[1]);

		resize(dest, mask, Size(width, height), INTER_NEAREST);
		Rect mask_rect = temp_rect - Point(left, top);
		mask_rect &= Rect(0, 0, width, height);
		mask = mask(mask_rect) > mask_threshold;
		output.boxMask = mask;

	}

	void DrawPred(Mat& img, vector<ModelResult> result, std::vector<std::string> classNames, vector<Scalar> color) {
		Mat mask = img.clone();
		for (int i = 0; i < result.size(); i++) {
			int left, top;
			left = result[i].box.x;
			top = result[i].box.y;
			int color_num = i;
			rectangle(img, result[i].box, color[result[i].id], 2, 8);
			if (result[i].boxMask.rows && result[i].boxMask.cols > 0)
				mask(result[i].box).setTo(color[result[i].id], result[i].boxMask);
			string label = classNames[result[i].id] + ":" + to_string(result[i].confidence);
			int baseLine;
			Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
			top = max(top, labelSize.height);
			//rectangle(frame, Point(left, top - int(1.5 * labelSize.height)), Point(left + int(1.5 * labelSize.width), top + baseLine), Scalar(0, 255, 0), FILLED);
			putText(img, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 1, color[result[i].id], 2);
		}
		addWeighted(img, 0.5, mask, 0.5, 0, img); //add mask to src
		imshow("1", img);
		//imwrite("out.bmp", img);
		waitKey();
		//destroyAllWindows();

	}


	std::tuple<std::vector<cv::Rect>, std::vector<float>, std::vector<int>, std::vector<std::vector<float>>>
		non_max_suppression(const cv::Mat& output0, int class_names_num, int data_width, double conf_threshold,
			float iou_threshold) {

		std::vector<int> class_ids;
		std::vector<float> confidences;
		//    std::vector<cv::Rect_<float>> boxes;
		std::vector<cv::Rect> boxes;
		std::vector<std::vector<float>> rest;

		int rest_start_pos = class_names_num + 4;
		int rest_features = data_width - rest_start_pos;
		//    int data_width = rest_start_pos + total_features_num;

		int rows = output0.rows;
		float* pdata = (float*)output0.data;

		for (int r = 0; r < rows; ++r) {
			cv::Mat scores(1, class_names_num, CV_32FC1, pdata + 4);
			cv::Point class_id;
			double max_conf;
			minMaxLoc(scores, nullptr, &max_conf, nullptr, &class_id);

			if (max_conf > conf_threshold) {
				std::vector<float> mask_data(pdata + 4 + class_names_num, pdata + data_width);
				class_ids.push_back(class_id.x);
				confidences.push_back((float)max_conf);

				float out_w = pdata[2];
				float out_h = pdata[3];
				float out_left = MAX((pdata[0] - 0.5 * out_w + 0.5), 0);
				float out_top = MAX((pdata[1] - 0.5 * out_h + 0.5), 0);
				cv::Rect_<float> bbox(out_left, out_top, (out_w + 0.5), (out_h + 0.5));
				boxes.push_back(bbox);
				if (rest_features > 0) {
					std::vector<float> rest_data(pdata + rest_start_pos, pdata + data_width);
					rest.push_back(rest_data);
				}
			}
			pdata += data_width; // next prediction
		}

		//
		//float masks_threshold = 0.50;
		//int top_k = 500;
		//const float& nmsde_eta = 1.0f;
		std::vector<int> nms_result;
		cv::dnn::NMSBoxes(boxes, confidences, conf_threshold, iou_threshold, nms_result); // , nms_eta, top_k);
		//    cv::dnn::NMSBoxes(boxes, confidences, );
		std::vector<int> nms_class_ids;
		std::vector<float> nms_confidences;
		//    std::vector<cv::Rect_<float>> boxes;
		std::vector<cv::Rect> nms_boxes;
		std::vector<std::vector<float>> nms_rest;
		for (int idx : nms_result) {
			nms_class_ids.push_back(class_ids[idx]);
			nms_confidences.push_back(confidences[idx]);
			nms_boxes.push_back(boxes[idx]);
			nms_rest.push_back(rest[idx]);
		}
		return std::make_tuple(nms_boxes, nms_confidences, nms_class_ids, nms_rest);
	}


	cv::Rect_<float> scale_boxes(const cv::Size& img1_shape, cv::Rect_<float>& box, const cv::Size& img0_shape,
		std::pair<float, cv::Point2f> ratio_pad, bool padding) {

		float gain, pad_x, pad_y;

		if (ratio_pad.first < 0.0f) {
			gain = std::min(static_cast<float>(img1_shape.height) / static_cast<float>(img0_shape.height),
				static_cast<float>(img1_shape.width) / static_cast<float>(img0_shape.width));
			pad_x = roundf((img1_shape.width - img0_shape.width * gain) / 2.0f - 0.1f);
			pad_y = roundf((img1_shape.height - img0_shape.height * gain) / 2.0f - 0.1f);
		}
		else {
			gain = ratio_pad.first;
			pad_x = ratio_pad.second.x;
			pad_y = ratio_pad.second.y;
		}

		//cv::Rect scaledCoords(box);
		cv::Rect_<float> scaledCoords(box);

		if (padding) {
			scaledCoords.x -= pad_x;
			scaledCoords.y -= pad_y;
		}

		scaledCoords.x /= gain;
		scaledCoords.y /= gain;
		scaledCoords.width /= gain;
		scaledCoords.height /= gain;

		// Clip the box to the bounds of the image
		clip_boxes(scaledCoords, img0_shape);

		return scaledCoords;
	}


	void clip_boxes(cv::Rect& box, const cv::Size& shape) {
		box.x = std::max(0, std::min(box.x, shape.width));
		box.y = std::max(0, std::min(box.y, shape.height));
		box.width = std::max(0, std::min(box.width, shape.width - box.x));
		box.height = std::max(0, std::min(box.height, shape.height - box.y));
	}

	void clip_boxes(cv::Rect_<float>& box, const cv::Size& shape) {
		box.x = std::max(0.0f, std::min(box.x, static_cast<float>(shape.width)));
		box.y = std::max(0.0f, std::min(box.y, static_cast<float>(shape.height)));
		box.width = std::max(0.0f, std::min(box.width, static_cast<float>(shape.width - box.x)));
		box.height = std::max(0.0f, std::min(box.height, static_cast<float>(shape.height - box.y)));
	}


	void clip_boxes(std::vector<cv::Rect>& boxes, const cv::Size& shape) {
		for (cv::Rect& box : boxes) {
			clip_boxes(box, shape);
		}
	}

	void clip_boxes(std::vector<cv::Rect_<float>>& boxes, const cv::Size& shape) {
		for (cv::Rect_<float>& box : boxes) {
			clip_boxes(box, shape);
		}
	}
	void clip_coords(std::vector<float>& coords, const cv::Size& shape) {
		// Assuming coords are of shape [1, 17, 3]
		for (int i = 0; i < coords.size(); i += 3) {
			coords[i] = std::min(std::max(coords[i], 0.0f), static_cast<float>(shape.width - 1));  // x
			coords[i + 1] = std::min(std::max(coords[i + 1], 0.0f), static_cast<float>(shape.height - 1));  // y
		}
	}
	int findMaxSegment(const std::vector<ModelResult>& results)
	{
		int max_index = -1;
		int max_area = 0;
		for (int i = 0; i < results.size();i++) {
			auto result = results[i];

			if (result.box.area() > max_area) {
				max_area = result.box.area();
				max_index = i;
			}
		}
		return max_index;
	}

	std::vector<float> scale_coords(const cv::Size& img1_shape, std::vector<float>& coords, const cv::Size& img0_shape)
	{
		//    cv::Mat scaledCoords = coords.clone();
		std::vector<float> scaledCoords = coords;

		// Calculate gain and padding
		double gain = std::min(static_cast<double>(img1_shape.width) / img0_shape.width, static_cast<double>(img1_shape.height) / img0_shape.height);
		cv::Point2d pad((img1_shape.width - img0_shape.width * gain) / 2, (img1_shape.height - img0_shape.height * gain) / 2);

		// Apply padding
	//    scaledCoords.col(0) = (scaledCoords.col(0) - pad.x);
	//    scaledCoords.col(1) = (scaledCoords.col(1) - pad.y);
		// Assuming coords are of shape [1, 17, 3]
		for (int i = 0; i < scaledCoords.size(); i += 3) {
			scaledCoords[i] -= pad.x;  // x padding
			scaledCoords[i + 1] -= pad.y;  // y padding
		}

		// Scale coordinates
	//    scaledCoords.col(0) /= gain;
	//    scaledCoords.col(1) /= gain;
		// Assuming coords are of shape [1, 17, 3]
		for (int i = 0; i < scaledCoords.size(); i += 3) {
			scaledCoords[i] /= gain;
			scaledCoords[i + 1] /= gain;
		}

		clip_coords(scaledCoords, img0_shape);
		return coords;
	}


	std::vector<int> findMaxConflictIndex(const std::vector<ModelResult>& src, const std::vector < ModelResult>& dst){
		std::vector<int> result;
		for (auto& src_item : src) {
			int max_index = -1;
			int max_area = 0;
			for (int j = 0; j < dst.size();j++) {
				auto& dst_item = dst[j];
				auto conflict_area = (src_item.box & dst_item.box).area();
				if (conflict_area > max_area) {
					max_area = conflict_area;
					max_index = j;
				}
			}
			result.push_back(max_index);
		}
		return result;

	}


}//mrz