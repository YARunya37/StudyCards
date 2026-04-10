#include "pch.h"
#include "common.h"
#include "FileLoader.h"
#include <direct.h>

int main()
{
	system("chcp 65001");
	setlocale(LC_ALL, "ru");
	SourceText text; // Исходный текст
	string FilePath = "ООП.md"; // Путь к файлу

	if (loadDocument(FilePath, text))
	{
		cout << "Загрузка прошла успешно! Всего загружено " << text.size << " строк." << endl;

		/*for (int i = 0; i < text.size; i++)
		{
			cout << text.rows[i] << endl;
		}*/
	}
	else
	{
		cout << "Ошибка загрузки!" << endl;
	}
	return 0;
}