#include "AppInfo.h"
//#include "Utility.h"
#include <iostream>
#include <memory>
#include <fstream>
#include "MrzCheck.h"

#ifdef _WINDOWS
#include "utility.h"


#endif

namespace mrz {
    AppInfo* AppInfo::instance_ = nullptr;

#ifdef _WINDOWS
    AppInfo* AppInfo::instance(const std::string& lib_root)
    {
        if (instance_ == nullptr)
            instance_ = new AppInfo(lib_root);
        return instance_;
    }
    AppInfo::AppInfo(const std::string& lib_root) :
        lib_root_(lib_root)
    {
        if (lib_root_.empty())
            lib_root_ = getExePath();
    }

#endif


#ifdef __ANDROID__
    AppInfo* AppInfo::instance(AAssetManager* mgr) {

        if (instance_ == nullptr)
            instance_ = new AppInfo(mgr);
        return instance_;
    }



    AppInfo::AppInfo(AAssetManager* mgr) :
        mgr_(mgr)
    {
        //path_ = AppUtility::getExePath();
    }


    std::vector<unsigned char> AppInfo::loadFile2Vec(const std::string& filename) {


        AAsset* asset_file = AAssetManager_open(mgr_, filename.c_str(), AASSET_MODE_BUFFER);

        size_t file_length = static_cast<size_t>(AAsset_getLength(asset_file));
        std::vector<unsigned char> file_buffer(file_length);

        AAsset_read(asset_file, file_buffer.data(), file_length);

        AAsset_close(asset_file);

        return file_buffer;
    }
    AAssetManager* AppInfo::getAssetMgr() {
        return mgr_;
    }

    void AppInfo::loadCascadeClassifier(const std::string& filename,
        std::shared_ptr<cv::CascadeClassifier> classifier) {

        auto data = AppInfo::instance()->loadFile2String(filename);
        cv::FileStorage fs = cv::FileStorage(data, cv::FileStorage::READ | cv::FileStorage::MEMORY);

        classifier->read(fs.getFirstTopLevelNode());

    }

#endif

#ifdef _WINDOWS
    std::string AppInfo::getFullFilename(const std::string& _filename) {
        return join<std::string>(lib_root_, join<std::string>("assets", _filename));
    }
    std::vector<unsigned char> AppInfo::loadFile2Vec(const std::string& _filename) {
        checkExistFile(_filename);
        std::vector<unsigned char> data;
        std::string filename = join<std::string>(lib_root_, join<std::string>("assets", _filename));

        readBinaryFile(filename, data);
        return data;
    }
#endif
    void AppInfo::setPath(const std::string& path)
    {
    }

    void AppInfo::setInfo(const std::string& path)
    {
        /*if (path.empty())
            return;
        path_ = path;*/
    }
    struct membuf : std::streambuf
    {
        membuf(char* begin, char* end) {
            this->setg(begin, begin, end);
        }
    };

    std::string AppInfo::loadFile2String(const std::string& filename) {
        std::vector<unsigned char> vec = loadFile2Vec(filename);
        std::string data(vec.begin(), vec.end());
        return data;
    }
    std::unique_ptr<std::ifstream> AppInfo::loadFile2Istream(const std::string& filename) {
        file_buffer_ = loadFile2Vec(filename);
        std::unique_ptr<std::ifstream> result = std::make_unique<std::ifstream>();
        result->read((char*)file_buffer_.data(), file_buffer_.size());
        return result;

    }






}//mrz 