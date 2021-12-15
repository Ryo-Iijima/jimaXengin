#pragma once
#include <string>

namespace JimaXengine
{
	class _StringFormat
	{
	public:
		static std::string Format(const std::string& format, const int& data);
		static std::string Format(const std::string& format, const std::string& data);

		_StringFormat();
		~_StringFormat();

	private:

	};
}