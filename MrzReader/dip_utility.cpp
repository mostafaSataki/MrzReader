#include "dip_utility.h"
#include <opencv2/imgproc.hpp>

Contour approximateContour(const Contour& contour)
{
	// Create a container to hold the result
	Contour approx;

	// Start with a small epsilon (precision parameter)
	double epsilon = 0.1;

	do {
		// Use approxPolyDP to simplify the contour
		cv::approxPolyDP(contour, approx, epsilon, true);

		// Increase epsilon for the next iteration
		epsilon += 0.1;
	} while (approx.size() > 4);

	// Return the simplified contour with 4 points
	return approx;
}
Contour findMaxContour(const Contours& contours)
{
	// Check if contours vector is not empty
	if (contours.empty()) {

		return Contour();
	}

	// Initialize maximum area and index of contour with maximum area
	double maxArea = 0;
	int maxAreaContourIdx = 0;

	for (size_t i = 0; i < contours.size(); i++) {
		// Calculate area of contour
		double area = cv::contourArea(contours[i]);

		// Update maximum area and index if current contour area is greater
		if (area > maxArea) {
			maxArea = area;
			maxAreaContourIdx = i;
		}
	}

	// Return contour with maximum area
	return contours[maxAreaContourIdx];
}

Contourf getContourf(const Contour& contour) {
	Contourf result(contour.size());
	for (int i = 0; i < contour.size(); i++)
		result[i] = cv::Point2f(contour[i]);
	return result;
}
enum class Direct { horz, vert };

Contour getContourInDirect(const Contour& contour, Direct direct) {
	auto result = contour;

	if (contour.size() != 4)
		return Contour();

	result = putLeftTopInFisrt(result);

	auto dist1 = cv::norm(result[1] - result[0]);
	auto dist2 = cv::norm(result[2] - result[1]);

	if ((direct == Direct::horz && dist1 < dist2) || (direct == Direct::vert && dist1 > dist2)) {
		
		//auto p = result.back();
		//result.pop_back();

		//result.insert(result.begin(), p);
		std::rotate(result.begin(), result.begin() + 1, result.end());
	}
	return result;

}
void rectifyImage(const cv::Mat& src_img, cv::Mat& dst_img, const Contour& _contour, const cv::Size& dst_size) {

	auto contour1 = getContourInDirect(_contour, Direct::horz);
	auto contour = getContourf(contour1);
	
	// Destination points
	std::vector<cv::Point2f> dst;
	dst.push_back(cv::Point2f(0, 0));
	dst.push_back(cv::Point2f(dst_size.width, 0));
	dst.push_back(cv::Point2f(dst_size.width, dst_size.height));
	dst.push_back(cv::Point2f(0, dst_size.height));

	// Compute the perspective transform
	cv::Mat transform = cv::getPerspectiveTransform(contour, dst);



	// Destination image
	dst_img = cv::Mat(dst_size.height, dst_size.width, CV_8UC3);

	// Apply the perspective transformation
	cv::warpPerspective(src_img, dst_img, transform, dst_img.size());



}

std::string get_size_order(const cv::Size& ref_size) {
	std::string order;
	// Determine the order of ref_size
	if (ref_size.width < ref_size.height) {
		order = "Asc";
	}
	else {
		order = "Desc";
	}
	return order;
}

cv::Size autoDimSize(cv::Size size, const cv::Size& ref_size) {
	std::string order = get_size_order(ref_size);
	// Sort the size
	if (order == "Asc") {
		if (size.width > size.height) {
			std::swap(size.width, size.height);
		}
	}
	else {
		if (size.width < size.height) {
			std::swap(size.width, size.height);
		}
	}
	return size;
}
cv::Rect fitOnSizeCorrect(const cv::Size& src, cv::Size dst_s, float& scale, bool auto_dim) {
	cv::Rect result = cv::Rect();

	if (dst_s.width == -1 && dst_s.height == -1) {
		scale = 0.f;
		return result;

	}
	cv::Size dst = dst_s;
	if (auto_dim) {
		dst = autoDimSize(dst_s, src);
	}


	float s_a = (float)dst.width / src.width;
	float s_b = (float)dst.height / src.height;

	if (dst_s.width == -1) {
		s_a = s_b;
		dst.width = int(src.width * s_b);
	}
	else if (dst_s.height == -1) {
		s_b = s_a;
		dst.height = int(src.height * s_a);
	}


	cv::Size size_a, size_b;

	size_a.width = int(src.width * s_a);
	size_a.height = int(src.height * s_a);

	size_b.width = int(src.width * s_b);
	size_b.height = int(src.height * s_b);


	if (dst.width == -1 && dst.height > 0) {

		scale = s_b;
		return cv::Rect(cv::Point(0, 0), size_b);
	}
	else if (dst.height == -1 && dst.width > 0) {
		scale = s_a;
		return cv::Rect(cv::Point(0, 0), size_a);
	}

	cv::Size dst_size;
	if (size_a.width <= dst.width && size_a.height <= dst.height) {
		dst_size = size_a;
		scale = s_a;
	}
	else if (size_b.width <= dst.width && size_b.height <= dst.height) {
		dst_size = size_b;
		scale = s_b;
	}

	result = cv::Rect(cv::Point(0, 0), dst_size);
	result.x = (dst.width - dst_size.width) / 2;
	result.y = (dst.height - dst_size.height) / 2;


	return result;
}


cv::Mat fitOnSizeMat(cv::InputArray _src, const cv::Size& dst, float& scale, int interpolation, bool auto_dim)
{
	cv::Mat src = _src.getMat();
	auto rct = fitOnSizeCorrect(src.size(), dst, scale, auto_dim);
	cv::Mat result;
	cv::resize(src, result, rct.size(), 0, 0, interpolation);
	return result;
}

void fitOnSizeMat2(cv::InputArray _src, cv::InputOutputArray _dst, const cv::Size& dst, float& scale, int interpolation, bool auto_dim)
{
	cv::Mat src = _src.getMat();

	auto rct = fitOnSizeCorrect(src.size(), dst, scale, auto_dim);

	cv::resize(src, _dst, rct.size(), 0, 0, interpolation);


}

float fitOnSizeMat3(cv::InputArray _src, cv::InputOutputArray _dst, const cv::Size& dst, int interpolation, bool auto_dim)
{
	float scale = 0.f;
	fitOnSizeMat2(_src, _dst, dst, scale, interpolation, auto_dim);
	return scale;
}


cv::Point findLeftTopPoint(const Contour& contour) {
	int leftmost = INT_MAX;
	int topmost = INT_MAX;
	cv::Point leftTopPoint;

	for (const auto& point : contour) {
		if (point.x < leftmost) {
			leftmost = point.x;
			leftTopPoint.x = point.x;
		}
		if (point.y < topmost) {
			topmost = point.y;
			leftTopPoint.y = point.y;
		}
	}

	return leftTopPoint;
}
int findLefTopestPoint(const Contour& contour) {

	auto bb_lt = findLeftTopPoint(contour);
	double minDist = std::numeric_limits<double>::max();
	cv::Point closestPoint;
	int closestPointIndex = 0;

	for (int i = 0; i < contour.size(); ++i) {
		const auto& point = contour[i];
		double dist = sqrt(pow(point.x - bb_lt.x, 2) + pow(point.y - bb_lt.y, 2));
		if (dist < minDist) {
			minDist = dist;
			closestPoint = point;
			closestPointIndex = i;
		}
	}


	return closestPointIndex;
}

Contour putLeftTopInFisrt(const Contour& contour) {

	auto result = contour;
	auto lt_index = findLefTopestPoint(contour);
	
	std::rotate(result.begin(), result.begin() + lt_index, result.end());
	return result;
}

//bool isClockwiseContour(const Contour& contour) {
//	double area = cv::contourArea(contour, false);
//	return area > 0;
//}
bool isClockwiseContour(const Contour& contour) {
	bool result = false;
	int sumOfCrossProducts = 0;

	for (int i = 0; i < contour.size(); i++) {
		const auto& p1 = contour[i];
		const auto& p2 = contour[(i + 1) % contour.size()];

		sumOfCrossProducts += (p2.x - p1.x) * (p2.y + p1.y);
	}

	result = (sumOfCrossProducts > 0);

	return result;
}
void forceClockwiseContour(Contour& contour) {
	if (!isClockwiseContour(contour)) {
		reverse(contour.begin(), contour.end());
	}
}

std::vector<cv::Rect> boundingRects(cv::Mat src)
{

	Contours contours;
	cv::findContours(src.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	return boundingRects(contours);

}
std::vector<cv::Rect> boundingRects(const Contours& src)
{
	std::vector<cv::Rect> result;
	result.reserve(src.size());
	for (int i = 0; i < src.size(); i++)
		result.push_back(cv::boundingRect(src[i]));
	return result;
}
float rectRatio(const cv::Rect& rct)
{
	float ratio1 = (float)rct.width / rct.height;
	float ratio2 = (float)rct.height / rct.width;

	return ratio1 > ratio2 ? ratio1 : -ratio2;
}

std::vector<std::vector<cv::Rect>> groupingRects(const Rects& main_rects, const Rects& sub_rects, float conflict_per)
{
	std::vector<std::vector<cv::Rect>> result(main_rects.size());
	for (int j = 0; j < sub_rects.size(); j++) {
		for (int i = 0; i < main_rects.size(); i++) {
			cv::Rect r = main_rects[i] & sub_rects[j];
			float per1 = (float)r.area() / main_rects[i].area();
			float per2 = (float)r.area() / sub_rects[j].area();
			if (per1 >= conflict_per || per2 > conflict_per) {
				result[i].push_back(sub_rects[j]);
				break;
			}

		}
	}
	return result;


	return result;
}

cv::Rect scaleRect(cv::Rect src_rect, float scale_width, float scale_height) {

	if (scale_height == 0)
		scale_height = scale_width;

	cv::Rect result(src_rect);
	result.x = cvRound(result.x * scale_width);
	result.y = cvRound(result.y * scale_height);
	result.width = cvRound(result.width * scale_width);
	result.height = cvRound(result.height * scale_height);
	return result;

}

void drawRects(cv::Mat src_mat, Rects rects, cv::Scalar color,int thickness)
{
	for (size_t i = 0; i < rects.size(); i++)
	{
		cv::rectangle(src_mat, rects[i], color, thickness);
		
	}

}