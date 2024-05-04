
#include "MrzException.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>
#include "utility.h"

namespace mrz {






	FEException::FEException(int error_code, const std::wstring & message, const std::source_location& location):
		error_code_(error_code),
		message_(message) ,
		message_str_{ws2s(message)},
		file_(location.file_name()), 
		line_(location.line()), 
		function_(location.function_name() ){

		}

	int FEException::errorCode(bool debug) const
	{
		/*if (debug)
			logError(getLocationString());*/
			
		return error_code_;
	}

	void FEException::printLocation()const noexcept
	{
		std::cout << getLocationString() << std::endl;
	}

	const char* FEException::file() const noexcept
	{
		return file_;
	}

	int FEException::line() const noexcept
	{
		return line_;
	}

	const char* FEException::function() const noexcept
	{
		return function_;
	}

	std::string FEException::getLocationString() const
	{
		return  "file:" + std::string(file_) + "  line:" + std::to_string(line_) + "  func:" + std::string( function_);
	}

	void FEException::setMessage(const std::wstring& message)
	{
		message_ = message;
		message_str_ = ws2s(message_);
	}

	const char* FEException::what()const noexcept {
		
		return message_str_.c_str();
	}









}//mrz