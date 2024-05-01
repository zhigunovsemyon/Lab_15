/* Пусть задано N прямоугольников, каждый из них задан
с помощью координат его левого верхнего угла, ширины и высоты.
Определить прямоугольник наименьшего периметра, вывести
координаты его вершин и периметр.*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#define ARR_INIT 30
#define BUFF_INIT 100

//Структура "прямоугольник"
typedef struct
{
	int x;
	int y;
	int hsize;
	int vsize;
} Rectangle;

int WayToFill(void);
int HowManyTriangles(unsigned maxAmount, FILE* source);
int	CalcPerimeter(Rectangle* Rect);
int GetSmallestRect(Rectangle Rects[], int amountOfRects);
int ReadSingleRect(char source[], Rectangle* Rect);
int FillArrayOfRects(FILE* source, Rectangle rects[], int arrSize);
int InterfaceSolution(int maxSize, Rectangle rects[]);
int SilentSolution(int maxSize, Rectangle rects[], FILE* source);
void AskUserForFilename(char filename[], int maxSize);
void PrintRectangle(Rectangle Rect, int perimeter);

int
main(void)
{
	setlocale(LC_ALL, "ru_RU.UTF-8");
	Rectangle rects[ARR_INIT];
	int arrSize;

	//Опрос пользователя на способ заполнения массива 
	if (WayToFill()) //Заполнение из клавиатуры
	{
		wprintf(L"Выбрано чтение с клавиатуры\n");
		//Функция заполнения массива с клавиауты, спрашивающая пользователя в процессе
		arrSize = InterfaceSolution(ARR_INIT, rects);

		//Если при попытке прочитать прямоугольник была ошибка, выводится соответствующая ошибка
		if (arrSize == 0)
		{
			wprintf(L"Введено неправильное значение! Завершение работы\n");
			return 1;
		}
	}
	else //Заполнение из файла
	{
		wprintf(L"Выбрано чтение из файла\n");
		FILE* ourfile;
		char filename[BUFF_INIT];

		//Опрос у пользователя названия файла, открытие его
		AskUserForFilename(filename, BUFF_INIT);
		ourfile = fopen(filename, "rt");

		//Заполнение массива из файла, возврат размера массива, либо кода ошибки
		arrSize = SilentSolution(ARR_INIT, rects, ourfile);

		//Обработка различных кодов ошибок на основе возвращённого значения
		switch (arrSize)
		{
		case 0:
			wprintf(L"Ошибка при чтении размера массива! Завершение работы\n");
			fclose(ourfile);
			return 1;
		case -1:
			wprintf(L"Ошибка при чтении параметров прямоугольника! Завершение работы\n");
			fclose(ourfile);
			return 1;
		case -2:
			wprintf(L"Не удалось открыть файл! Завершение работы\n");
			return 1;
		case -3:	//Код -3 означает, что размер массива в файле был слишком большим. Значит было...
			//прочитано максимально возможное количество. Выводится соответствующее сообщение.
			//Размеру массива обратно присваивается максимальный размер
			arrSize = ARR_INIT;
			wprintf(L"ВНИМАНИЕ! В данном файле слишком большое число структур, было прочитано только %d\n", arrSize);
			//Дальше осуществляется работа в штатном режиме
		}
		fclose(ourfile);
	}

	int smallest = GetSmallestRect(rects, arrSize);	//Получение индекса минимального прямоугольника
	//Вывод соответствующего прямоугольника на экран
	PrintRectangle(rects[smallest], CalcPerimeter(rects + smallest));

	return 0;
}

//Выбор пользователем способа заполнения массива. 
int
WayToFill(void)
{
	char buff[BUFF_INIT];
	char answer;
	do
	{
		wprintf(L"Введите f для заполения массива из файла,\n%ls",
			L"либо k для ввода с клавиатуры: ");
		fgets(buff, BUFF_INIT - 1, stdin);
		sscanf(buff, "%c", &answer);

		if (answer == 'f')
			return 0;		//Возврат кода заполнения из файла
		if (answer == 'k')
			return 1;		//Возврат кода заполнения с клавиатуры

		//При неправильном вводе опрос проводится заново
	} while (1);
}

//Спрашивает у пользователя название для открытия файла
void
AskUserForFilename(char filename[], int maxSize)
{
	wprintf(L"Введите название файла: ");
	fgets(filename, maxSize - 1, stdin);
	sscanf(filename, "%[^\n]s", filename); //Ведёт чтение из строки
}

//Определить, сколько прямоугольников будет вводится
int
HowManyTriangles(unsigned max, FILE* source)
{
	char buff[BUFF_INIT];
	int amount;
	fgets(buff, BUFF_INIT - 1, source);
	if (sscanf(buff, "%d", &amount) == 0)
		return 0; //Код ошибки при отсутствии прочтённого числа
	if (amount <= 0)
		return 0; //Код ошибки при неправильном значении
	if (amount >= max)
		return -1; //Код ошибки при большом количестве файлов

	return amount;	//Возврат количества при корректном вводе
}

//Способ наполнения массива, взаимодействующий с пользователем
int
InterfaceSolution(int maxSize, Rectangle rects[])
{
	int arrSize;
	do
	{
		wprintf(L"Сколько прямоугольников вы хотите ввести? ");
		arrSize = HowManyTriangles(maxSize, stdin);	//Ввод размера массива
		if (arrSize > 0)							//Если был возвращён корректный размер массива...
			break;									//Цикл ввода прерывается
		wprintf(L"Неправильное значение!\n");		//В противном случае выводится сообщение об ошибке...
	} while (1);									//Цикл начинается заново

	wprintf(L"Введите через пробел координату x левого верхнего угла прямоугольника,\n%ls",
		L"координату y, размер по горизонтали и по вертикали\n");
	if (FillArrayOfRects(stdin, rects, arrSize))	//Наполнение массива через функцию
		return 0;	//Если в результате заполнения был возвращён флаг ошибки, то этот флаг возвращается 

	return arrSize; //Возврат размера массива при правильной отработке
}

//Способ заполнения, не взаимодействующий с пользователем
int
SilentSolution(int maxSize, Rectangle rects[], FILE* source)
{
	if (source == NULL)
		return -2; //Код ошибки, значащий невозможность работы с файлом
	int arrSize = HowManyTriangles(maxSize, source);	//Получение размера массива из источника
	if (arrSize == 0)
		return 0;	//Возврат кода ошибки, если размер не был прочитан или некорректный

	//При получении кода о чрезмерном размере входных данных, идёт обработка с максимально возможным значением
	if (arrSize == -1)
	{
		if (FillArrayOfRects(source, rects, maxSize))	//Наполнение массива через функцию
			return -1;	//Завершение работы при возврате функцией флага ошибки 

		return -3;	//Код чрезмерного размера входных данных для корректной работы в дальнейшем
	}
	//Вызов функции чтения файла в массив
	if (FillArrayOfRects(source, rects, arrSize)) //Наполнение массива через функцию 
		return -1;//Завершение работы при возврате функцией флага ошибки 

	return arrSize; //Возврат размера массива, если все данные были корректными
}

//Чтение одного прямоугольника из текста. Возвращает 0 при успешном выполнении, 1 при ошибке
int
ReadSingleRect(char source[], Rectangle* Rect)
{
	if (sscanf(source, "%d %d %d %d", &Rect->x, &Rect->y, &Rect->hsize, &Rect->vsize) != 4)
		return 1;	//Код ошибки, если количество элементов в строке было меньше 4
	if (Rect->hsize <= 0)	//Размер должен быть положительным числом
		return 1;			//Возврат кода ошибки в противном случае
	if (Rect->vsize <= 0)	//Размер должен быть положительным числом
		return 1;			//Возврат кода ошибки в противном случае

	return 0;	//Возврат флага успешного чтения
}

//Чтение нескольких прямоугольников из текста с заданным форматом
int
FillArrayOfRects(FILE* source, Rectangle rects[], int arrSize)
{
	char textbuff[BUFF_INIT];
	for (int i = 0; i < arrSize; ++i)
	{
		fgets(textbuff, BUFF_INIT - 1, source);		//Ввод текста из источника
		if (ReadSingleRect(textbuff, rects + i))	//Если при попытке прочитать прямоугольник была ошибка...
			return 1;	//возвращается флаг ошибки
	}
	return 0;	//Возврат 0, если не было ошибки
}

//Поиск индекса самого прямоугольника c наименьшим периметром
int
GetSmallestRect(Rectangle Rects[], int amountOfRects)
{
	int candidate = 0;	//Для начала утверждается, что наименьший прямоугольник - первый

	//Цикл перебирает все прямоугольники и считает их периметры.
	//Перед началом цикла считает и сохраняет периметр первого прямоугольника
	for (int i = 1, returnedPerim, smallestPerim = CalcPerimeter(Rects); i < amountOfRects; ++i)
	{
		returnedPerim = CalcPerimeter(Rects + i);
		if (smallestPerim < returnedPerim)	//Если периметр i-го прямоугольника больше минимального
			continue;						//Цикл переходит на следующий элемент массива

		//Если периметр прямогольника меньше минимального, значение перезаписывается
		smallestPerim = returnedPerim;
		candidate = i;	//Индекс этого прямоугольника сохраняется
	}
	return candidate;		//Возврат индекса наименьшего прямоугольника
}

int	//Вычисление периметра прямоугольника
CalcPerimeter(Rectangle* Rect)
{
	return 2 * (Rect->hsize + Rect->vsize);
}

void //Вывод расположения и периметра прямоугольника
PrintRectangle(Rectangle Rect, int perimeter)
{
	wprintf(L"Левый верхний угол прямогольника - (%d;%d)\n", Rect.x, Rect.y);
	wprintf(L"Периметр прямоугольника - %d\n", perimeter);
}
