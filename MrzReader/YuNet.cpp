#include "YuNet.h"
#include <opencv2/imgproc.hpp>
#include <map>
#include <opencv2/dnn.hpp>
#include "AppInfo.h"
#include <fstream>
#include <iterator>

namespace mrz {
    const std::map<std::string, int> str2backend{
        {"opencv", cv::dnn::DNN_BACKEND_OPENCV}, {"cuda", cv::dnn::DNN_BACKEND_CUDA},
        {"timvx",  cv::dnn::DNN_BACKEND_TIMVX},  {"cann", cv::dnn::DNN_BACKEND_CANN}
    };
    const std::map<std::string, int> str2target{
        {"cpu", cv::dnn::DNN_TARGET_CPU}, {"cuda", cv::dnn::DNN_TARGET_CUDA},
        {"npu", cv::dnn::DNN_TARGET_NPU}, {"cuda_fp16", cv::dnn::DNN_TARGET_CUDA_FP16}
    };


    YuNet::YuNet(const cv::Size& input_size, float conf_threshold, float nms_threshold, int top_k, int backend_id, int target_id)

        : model_path_(mrz::AppInfo::instance()->getFullFilename(model_filename_)), input_size_(input_size),
        conf_threshold_(conf_threshold), nms_threshold_(nms_threshold),
        top_k_(top_k), backend_id_(backend_id), target_id_(target_id)
    {
        
        model_ = cv::FaceDetectorYN::create(model_path_ ,"", input_size_, conf_threshold_, nms_threshold_, top_k_, backend_id_, target_id_);
    }

    void YuNet::setInputSize(const cv::Size& input_size)

    {
        input_size_ = input_size;
        model_->setInputSize(input_size_);
    }
    void YuNet::infer(const cv::Mat image, cv::Mat& dst)

    {
        model_->setInputSize(image.size());

        model_->detect(image, dst);
   
    }

    FaceResultS YuNet::infer(const cv::Mat image,float confidence_thresh)
    {
        cv::Mat faces;
        infer(image, faces);

        FaceResultS result;
        for (int i = 0; i < faces.rows; ++i)
        {
            int x = static_cast<int>(faces.at<float>(i, 0));
            int y = static_cast<int>(faces.at<float>(i, 1));
            int w = static_cast<int>(faces.at<float>(i, 2));
            int h = static_cast<int>(faces.at<float>(i, 3));
            float conf = faces.at<float>(i, 14);
            if (conf > confidence_thresh) {
                FaceResult face_res{ cv::Rect(x,y,w,h),conf };
                


                for (int j = 0; j < 5; ++j)
                {
                    int x = static_cast<int>(faces.at<float>(i, 2 * j + 4)), y = static_cast<int>(faces.at<float>(i, 2 * j + 5));
                    face_res.landmarks_.push_back(cv::Point(x,y));


                }

                result.push_back(face_res);
            }
         }

        return result;
    }


    cv::Mat YuNet::visualize(const cv::Mat& image, const FaceResultS& faces)
    {
        static cv::Scalar box_color{ 0, 255, 0 };
        static std::vector<cv::Scalar> landmark_color{
            cv::Scalar(255,   0,   0), // right eye
            cv::Scalar(0,   0, 255), // left eye
            cv::Scalar(0, 255,   0), // nose tip
            cv::Scalar(255,   0, 255), // right mouth corner
            cv::Scalar(0, 255, 255)  // left mouth corner
        };

        static cv::Scalar text_color{ 0, 255, 0 };
        auto output_image = image.clone();

   
        for (int i = 0; i < faces.size(); ++i)
        {

            cv::rectangle(output_image, faces[i].face_region_, box_color, 2);

            auto tl = faces[i].face_region_.tl();

            cv::putText(output_image, cv::format("%.4f", faces[i].confidence_), cv::Point(tl.x, tl.y + 12), cv::FONT_HERSHEY_DUPLEX, 0.5, text_color);


            for (int j = 0; j < faces[i].landmarks_.size(); ++j)
            {

                cv::circle(output_image, faces[i].landmarks_[j], 2, landmark_color[j], 2);
            }
        }
        return output_image;
    }



}//mrz