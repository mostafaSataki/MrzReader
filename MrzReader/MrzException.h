#pragma once

#include <exception>
#include <string>
#include <source_location>
#include <unordered_map>


namespace mrz {
	

	class  FEException : public std::exception {
	public:
		FEException(int error_code, const std::wstring& message, const std::source_location& location = std::source_location::current());
		virtual const char* what()const noexcept override; 
		int errorCode(bool debug = true)const;

		void printLocation()const noexcept;
		const char* file() const noexcept;
		int line() const noexcept;
		const char* function() const noexcept;
		std::string getLocationString()const;
	protected:
		std::wstring message_;
		std::string message_str_;
		int error_code_;
		const char* file_;
		int line_;
		const char* function_;
		void setMessage(const std::wstring& messgae);
	};



	
}//mrz