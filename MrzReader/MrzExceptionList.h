#pragma once
#include "MrzException.h"

namespace mrz {
	//typedef enum lxw_error {
	typedef enum BDErrorType {
		FE_NO_ERROR = 0,

		
		FE_FILE_NOT_EXIST_ERROR,
		FE_FILE_READ_ERROR,

		FE_CAN_NOT_OPEN_FILE_ERROR, 
		FE_NULL_POINTER_ERROR,
		FE_DIMENSION_WRONG,
		FE_STRIDE_WRONG,
		FE_CHANNELS_COUNT_WRONG


		
	}FEErrorType;

	class LastErrorMessage {
	public:
		static void setLastError(const FEException& e);

		static std::pair<int, std::string> getLastError();
	private:
		static int last_error_code_;
		static std::string last_error_message_;
	};



	class  FileNotExistException :public FEException {
	public:
		FileNotExistException(const std::wstring& filename, const std::source_location& location = std::source_location::current());
	protected:
		std::wstring filename_;
	};

	class  FileReadException :public FEException {
	public:
		FileReadException( const std::source_location& location = std::source_location::current());

	};
	

	class  ReadErrorJsonException :public FEException {
	public:
		ReadErrorJsonException(const std::source_location& location = std::source_location::current());

	};
	

	class  CanNotOpenFileException :public FEException {
	public:
		CanNotOpenFileException(const std::source_location& location = std::source_location::current());

	};

	class  NullPointerException :public FEException {
	public:
		NullPointerException(const std::source_location& location = std::source_location::current());

	};

	class DimensionWrongException :public FEException {
	public:
		DimensionWrongException(const std::source_location& location = std::source_location::current());
	};


	class StrideWrongException :public FEException {
	public:
		StrideWrongException(const std::source_location& location = std::source_location::current());
	};


	class ChannelCountWrongException :public FEException {
	public:
		ChannelCountWrongException(const std::source_location& location = std::source_location::current());
	};



}//mrz