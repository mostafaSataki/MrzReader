#pragma once

#include <opencv2/core.hpp>
#include <memory>


enum class Device : int8_t
{
	CPU,
	GPU
};



const auto kDefualtDevice = Device::CPU;

using Contour = std::vector<cv::Point>;
using Contourf = std::vector<cv::Point2f>;
using Contours = std::vector<Contour>;
using ContoursVec = std::vector<Contours>;

using Rects = std::vector<cv::Rect>;

