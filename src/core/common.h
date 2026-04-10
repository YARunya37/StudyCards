#pragma once
#include <string>

struct SourceText
{
	int size = 0;
	string* rows = nullptr;
	~SourceText()
	{
		delete[] rows;
	}
};
