#include "_StringFormat.h"

std::string _StringFormat::Format(std::string format, int data)
{
	std::string s;
	s = std::to_string(data);
	return s;
}

std::string _StringFormat::Format(std::string format, std::string data)
{
	std::string s;
	s = data;
	return s;
}

_StringFormat::_StringFormat()
{
}

_StringFormat::~_StringFormat()
{
}