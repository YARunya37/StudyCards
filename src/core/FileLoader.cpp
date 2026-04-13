#include "pch.h"
#include "FileLoader.h"
#include <direct.h>

string GetPandocPath()
{
	return "..\\utils\\pandoc\\pandoc.exe";
}
string getOutputHtmlPath(const string& inputPath)
{
	int LenFile = inputPath.size();
	string outputPath = "";
	int LastPointIndex = -1;
	for (int i = 0; i < LenFile; i++)
	{
		if (inputPath[i] == '.')
		{
			LastPointIndex = i;
		}
	}
	if (LastPointIndex > 0)
	{
		outputPath = inputPath.substr(0, LastPointIndex) + ".html";
		return outputPath;
	}
	return outputPath + ".html";
}
string getFileExtension(const string& FilePath)
{
	int LenFile = FilePath.size();
	string Extension;
	int LastPointIndex = -1;
	for (int i = 0; i < LenFile; i++)
	{
		if (FilePath[i] == '.')
		{
			LastPointIndex = i;
		}
	}
	for (int i = LastPointIndex + 1; i < LenFile; i++)
	{
		Extension += FilePath[i];
	}
	if (LastPointIndex > 0)
	{
		return Extension;
	}
	return "";
}
bool isSupportedFormat(const string& FilePath)
{
	string ext = getFileExtension(FilePath);
	if (ext == "docx" || ext == "doc" ||
		ext == "md" || ext == "markdown")
	{
		return true;
	}
	else return false;
}
bool loadHtmlFile(const string& FilePath, SourceText& text)
{
	ifstream input(FilePath);
	if (!input.is_open())
	{
		cout << "Файл не открылся" << endl;
		return 0;
	}
	string row;
	while (getline(input, row))
	{
		text.size++;
	}
	if (text.rows != nullptr)
	{
		delete[] text.rows;
	}
	text.rows = new string[text.size];
	input.clear();// очищает состояние потока (флаги)
	input.seekg(0);// Возвращаемся в начало для чтения
	for (int i = 0; getline(input, row) && i < text.size; i++)
	{
		text.rows[i] = row;
	}
	input.close();
	return 1;
}
bool isPandocAvailable(const string& PandocPath)
{
	ifstream input(PandocPath);
	if (input.is_open())
		return 1;
	else return 0;
}
bool ConvertToHtml(const string& FilePath, string& outputPath, const string& PandocPath)
{
	if (!isPandocAvailable(PandocPath))
		return false;

	outputPath = getOutputHtmlPath(FilePath);

	string command = "\"" + PandocPath + "\"" + " -s -t html --wrap=none --extract-media=./images " + FilePath + " -o " + outputPath;

	int result = system(command.c_str()); // .c_str() преобразует string в const char*, (это нужно, 
	//потому что system() принимает const char* (старый C-стиль строки)

	if (result == 0)
	{
		cout << "Конвертация прошла успешно!" << endl;
		return true;
	}
	else
	{
		cout << "Ошибка! Код: " << result << endl;
	}
	return false;
}
bool loadDocument(const string& FilePath, SourceText& text)
{
	if (!isSupportedFormat(FilePath))
	{
		return false;
	}

	string outputPath; // выходной файл
	string PandocPath = GetPandocPath(); // путь к pandoc(конвертатор)

	if (!ConvertToHtml(FilePath, outputPath, PandocPath))
	{
		return false;
	}
	if (!loadHtmlFile(outputPath, text))
	{
		return false;
	}
	return true;
}
