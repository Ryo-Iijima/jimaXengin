#pragma once
#include <string>

class _StringFormat
{
public:
	static std::string Format(std::string format, int data);
	static std::string Format(std::string format, std::string data);

	_StringFormat();
	~_StringFormat();

private:

};