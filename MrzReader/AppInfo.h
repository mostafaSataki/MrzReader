#ifndef APP_INFO_H
#define APP_INFO_H

#include <string>
#include <vector>

#include <memory>
#include <iostream>
#include <opencv2/opencv.hpp>
#ifdef __ANDROID__
#include <android/asset_manager_jni.h>
#endif


namespace mrz {
	class AppInfo
	{
	public:
#ifdef __ANDROID__
		static AppInfo* instance(AAssetManager* mgr = nullptr);
#endif
#ifdef _WINDOWS
		static AppInfo* instance(const std::string& lib_root = "");
		std::string getFullFilename(const std::string& _filename);
#endif
		AppInfo() noexcept = default;
		AppInfo(const AppInfo&) = delete;
		AppInfo& operator=(const AppInfo&) = delete;
		virtual ~AppInfo() = default;
		void setPath(const std::string& path);
		void setInfo(const std::string& path);


		std::string loadFile2String(const std::string& filename);
		void loadCascadeClassifier(const std::string& filename, std::shared_ptr<cv::CascadeClassifier> classifier);
		std::unique_ptr<std::ifstream> loadFile2Istream(const std::string& filename);

#ifdef __ANDROID__
		AAssetManager* getAssetMgr();

#endif
		
		std::vector<unsigned char> loadFile2Vec(const std::string& filename);

	protected:
		static AppInfo* instance_;

#ifdef __ANDROID__
		AAssetManager* mgr_;
		AppInfo(AAssetManager* mgr = nullptr);
#endif


#ifdef _WINDOWS
		std::string lib_root_;
		AppInfo(const std::string& lib_root);
#endif 


		std::vector<unsigned char> file_buffer_;



	};


}// mrz
#endif //APP_INFO_H