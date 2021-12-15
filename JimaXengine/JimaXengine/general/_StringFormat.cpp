#include "_StringFormat.h"

std::string JimaXengine::_StringFormat::Format(const std::string& format, const int& data)
{
	std::string s;
	s = std::to_string(data);
	return s;
}

std::string JimaXengine::_StringFormat::Format(const std::string& format, const std::string& data)
{
	std::string s;
	s = data;
	return s;
}

JimaXengine::_StringFormat::_StringFormat()
{
}

JimaXengine::_StringFormat::~_StringFormat()
{
}