#pragma once
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>

namespace mrz {
    struct FaceResult {
        cv::Rect face_region_;
        float confidence_;
        std::vector<cv::Point> landmarks_;
    };
    using FaceResultS = std::vector< FaceResult>;
    class YuNet
    {
    public:
        YuNet(const cv::Size& input_size = cv::Size(320, 320),
            float conf_threshold = 0.5f,
            float nms_threshold = 0.3f,
            int top_k = 5000,
            int backend_id = 3,
            int target_id = 0);

        void setInputSize(const cv::Size& input_size);
        void infer(const cv::Mat image, cv::Mat& dst);
        FaceResultS infer(const cv::Mat image, float confidence_thresh);
        static cv::Mat visualize(const cv::Mat& image, const FaceResultS& faces);
    private:
        std::vector<unsigned char> file_data_;
        static inline const std::string model_filename_ = R"(model\face_detection_yunet_2023mar.onnx)";
        cv::Ptr<cv::FaceDetectorYN> model_;

        std::string model_path_;
        cv::Size input_size_;
        float conf_threshold_;
        float nms_threshold_;
        int top_k_;
        int backend_id_;
        int target_id_;
    };

}//mrz