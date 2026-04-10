#pragma once
#include "common.h"

string GetPandocPath();
string getOutputHtmlPath(const string&);
string getFileExtension(const string&);
bool isSupportedFormat(const string&);
bool isPandocAvailable(const string&);
bool ConvertToHtml(const string&,  string&, const string&);
bool loadHtmlFile(const string&, SourceText&);
bool loadDocument(const string&, SourceText&);