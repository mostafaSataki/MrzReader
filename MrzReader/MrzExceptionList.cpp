#include "MrzExceptionList.h"
#include "utility.h"
#include <vector>
#include <string>
namespace mrz {

	 void LastErrorMessage::setLastError(const FEException& e)
	{
		last_error_code_ = (int)e.errorCode();
		last_error_message_ = e.what();
	}

	  std::pair<int, std::string> LastErrorMessage::getLastError()
	{
		return { last_error_code_, last_error_message_ };
	}

	  std::string LastErrorMessage::last_error_message_;
	  int LastErrorMessage::last_error_code_;






	FileNotExistException::FileNotExistException(const std::wstring& filename, const std::source_location& location) :
		FEException((int)BDErrorType::FE_FILE_NOT_EXIST_ERROR, L"فایل وجود ندارد", location),
		filename_{filename}
	{
		setMessage(message_ + L":" + filename_);
	}


	FileReadException::FileReadException(const std::source_location& location) :
		FEException((int)BDErrorType::FE_FILE_READ_ERROR, L"خطا در خواندن فایل", location) {}





	CanNotOpenFileException::CanNotOpenFileException(const std::source_location& location) :
		FEException((int)BDErrorType::FE_CAN_NOT_OPEN_FILE_ERROR, L"فایل باز نمی شود", location) {}

	NullPointerException::NullPointerException(const std::source_location& location) :
		FEException((int)BDErrorType::FE_NULL_POINTER_ERROR, L"اشاره گر تهی", location) {}


	DimensionWrongException::DimensionWrongException(const std::source_location& location) :
		FEException((int)BDErrorType::FE_DIMENSION_WRONG, L"Dimension Wrong.", location) {}

	StrideWrongException::StrideWrongException(const std::source_location& location) :
		FEException((int)BDErrorType::FE_STRIDE_WRONG, L"Stride Wrong.", location) {}

	ChannelCountWrongException::ChannelCountWrongException(const std::source_location& location) :
		FEException((int)BDErrorType::FE_CHANNELS_COUNT_WRONG, L"Channel Count Wrong .", location) {}


}//mrz
