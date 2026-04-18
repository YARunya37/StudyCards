#include "FileLoader.h"

// #include <QCoreApplication>

//Getting the full path to pandoc
string getPandocPath()
{
   return "/src/utils/pandoc/pandoc.exe";
}
//Getting the path of the output file with the .html extension
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
//Getting the extension of an incoming file
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
//Checking for supported format
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
//Checking if pandoc opens
bool isPandocAvailable(const string& PandocPath)
{
	ifstream input(PandocPath);
	if (input.is_open())
		return 1;
	else return 0;
}
//Конвертация исходного файла в файл в html формате
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
//Uploading files
bool loadDocument(const string& inputPath, string& html, const string& pandocPath)
{
	// 1. Проверка формата
	if (!isSupportedFormat(inputPath)) {
		cout << "Неподдерживаемый формат файла" << endl;
		return false;
	}

	// 2. Путь к Pandoc
    string path = pandocPath.empty() ? getPandocPath() : pandocPath;
    if (!isPandocAvailable(path)) {
        cout << "Pandoc не найден: " << path << endl;
		return false;
	}

	// 3. Конвертация
	string outputPath;
	if (!ConvertToHtml(inputPath, outputPath, pandocPath)) {
		cout << "Ошибка конвертации в HTML" << endl;
		return false;
	}

	// 4. Чтение HTML файла в строку (через fstream + итераторы)
	ifstream input(outputPath);
    if (!input.is_open()) {
        cout << "Не удалось открыть HTML файл" << endl;
        return false;
    }

	// Читаем весь файл в строку
	html = string((istreambuf_iterator<char>(input)),
		istreambuf_iterator<char>());

	input.close();
	return true;
}
