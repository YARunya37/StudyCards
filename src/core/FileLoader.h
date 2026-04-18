#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

string getPandocPath();
string getOutputHtmlPath(const string&);
string getFileExtension(const string&);
bool isSupportedFormat(const string&);
bool isPandocAvailable(const string&);
bool ConvertToHtml(const string&,  string&, const string&);
bool loadDocument(const string&, string&, const string&);
