#pragma once
#include <opencv2/core.hpp>
#include "mrz_reader_type.h"
#include <opencv2/imgproc.hpp>

Contour putLeftTopInFisrt(const Contour& contour);
Contour approximateContour(const Contour& contour);
Contour findMaxContour(const Contours& contours);
void rectifyImage(const cv::Mat& src_img, cv::Mat& dst_img, const Contour& contour, const cv::Size& dst_size);
std::string get_size_order(const cv::Size& ref_size);
cv::Size autoDimSize(cv::Size size, const cv::Size& ref_size);
cv::Rect fitOnSizeCorrect(const cv::Size& src, cv::Size dst_s, float& scale, bool auto_dim= false);
cv::Mat fitOnSizeMat(cv::InputArray _src, const cv::Size& dst, float& scale, int interpolation = cv::INTER_LINEAR, bool auto_dim = false);
void fitOnSizeMat2(cv::InputArray _src, cv::InputOutputArray _dst, const cv::Size& dst, float& scale, int interpolation = cv::INTER_LINEAR, bool auto_dim = false);
float fitOnSizeMat3(cv::InputArray _src, cv::InputOutputArray _dst, const cv::Size& dst,  int interpolation = cv::INTER_LINEAR, bool auto_dim = false);

bool isClockwiseContour(const Contour& contour);
void forceClockwiseContour(Contour& contour);
std::vector<cv::Rect> boundingRects(const Contours& src);
float rectRatio(const cv::Rect& rct);
std::vector<std::vector<cv::Rect>> groupingRects(const Rects& main_rects, const Rects& sub_rects, float conflict_per);
std::vector<cv::Rect> boundingRects(cv::Mat src);
cv::Rect scaleRect(cv::Rect src_rect, float scale_width, float scale_height = 0.f);
void drawRects(cv::Mat src_mat, Rects rects, cv::Scalar color, int thickness = 2);