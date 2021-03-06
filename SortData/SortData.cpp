// SortData.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
// Программа сортирует файлы с координатами, скоростями, энергиями и токами по папкам и применяет к ним clar_proc.exe
// Если файлы уже рассортированы, она их не трогает
// Если clar_proc.exe уже применен, то применяет заново, удаляя старые elVAF.dat, ionVAF.dat и прочее
// Если изменить последовательность приема данных в clar_proc.exe или правила записи данных в coos.dat, то эта программа может работать неправильно

#include "pch.h" // Пока пустой заголовочный файл
#include <iostream> // Консольный ввод-вывод
#include <fstream> // Ввод в файл, вывод из файла
#include <string> // Работа со строками
#include <filesystem> // Работа с файлами, например копирование
#include <windows.h> // Системные команды
//#include <QProcess> // Взаимодействие с внешними программами

using namespace std;
namespace fs = experimental::filesystem;

// Вывести вектор
void v_print(vector<string>& vec)
{
	for (int i = 0; i < vec.size(); i++)
		cout << vec[i] << endl;
}

int sort_and_execute(string const& input, int& answ)
{
	vector<string> files; // Вектор строк - файлов
	vector<string> dirs; // Папок
	string s; // Вспомогательная переменная, переопределяемая несколько раз
	string name; // Имя файла
	string s_num; // Номер файла в виде строки
	string new_path; // Путь к копируемому файлу с начальными условиями. Затем переопределяется как путь к созданной папке
	string readout; // Строка из считываемого файла
	string search; // Строка, которую ищем в файле
	string init = "init_par_bro.dat"; // Название файла с начальными условиями
	int answ_1, answ_2; // Ответы пользователя на вопрос программы
	int num; // Номер файла в виде числа
	size_t found; // Указатель на найденную подстроку
	int flag = 0; // Флаг, свидетельствующий о том, что в init найдена Te
	int Te = -1; //Температура электронов
	int Ti = -1; // Температура ионов
	string first_step = "10000";
	string last_step;
	string number_of_steps_to_average = "10";
	string instructions = "SD_instructions.dat";

	string path = "./"; // Текущая папка

	if (!input.empty()) // Не пустая строка
	{
		if (fs::is_directory(input) && fs::exists(input)) // Если папка существует
			path += input + "/";
		else
		{
			cout << endl << "The directory " << input << " doesn't exist" << endl;
			return 0;
		}
	}
	// Если пустая, то считаем, что нужные файлы находятся там же, где эта программа
	
	cout << endl << "The main directory: " << path << endl;

	// Смотрим, какие файлы и папки внутри указанной директории
	for (const auto & p : fs::directory_iterator(path))
	{
		s = p.path().string(); // Путь к объекту, преобразованный в строку
		if (!is_directory(p)) // Если файл (не папка)
		{
			// Выделяем имя файла
			found = s.find_last_of("/\\");
			name = s.substr(found + 1);

			// В том случае, если название файла с нач. данными другое (пока всего возможно 2 варианта, эти строки - по сути костыль)
			found = s.find("inp_data");
			if (found != -1) // -1 означает, что подстрока не найдена
				init = name;
			
			found = name.find_last_of("."); // Номер символа, с которого начинается формат файла
			if (found != -1) // Если формат у файла есть
				if (name.substr(found) == ".dat") // Если формат .dat
					if (name != init)
						files.push_back(name.substr(0, found)); // Добавить в конец вектора строку - имя файла без расширения
		}
	}
	
	cout << "I've found:" << endl;
	v_print(files);

	//cout << "OK? Input 1 to continue: ";
	//cin >> answ_1;
	answ_1 = 1; // Если дать пользователю возможность определить, продолжать дальше или нет, то пострадает скорость выполнения программы и ее автоматизм
	if (answ_1 == 1)
	{
		//cout << "Is it necessary to process sorted data? Input 1 to do it: ";
		//cin >> answ_2;
		answ_2 = answ;

		// Сортируем
		for (int i = 0; i < files.size(); i++)
		{
			name = files[i];

			// Выделить номер файла
			found = name.find_last_of("_");
			s_num = name.substr(found + 1); // Номер файла по типу "007"
			num = stoul(s_num); // Номер файла по типу 7
			s_num = to_string(num); // Номер файла по типу "7"

			// Создать папку, соответствующую номеру файла, если она не существует
			if (!fs::is_directory(path + s_num) || !fs::exists(path + s_num))
				fs::create_directory(path + s_num);
			// Если файла, который копируем, внутри нет, то копируем (иначе вылетит исключение, т.к. перезапись не осуществляется автоматически)
			if (!fs::exists(path + s_num + "/" + name + ".dat"))
				fs::copy(path + name + ".dat", path + s_num + "/" + name + ".dat");

			new_path = path + s_num + "/" + init;
			if (!fs::exists(new_path)) // Если файл с новыми нач. данными еще не создан
			{

				// Заменяем устаревшую информацию о семечке (случайном числе)
				ifstream readFile(path + init); // Считываем оригинальный файл, находящийся в указанной директории
				ofstream outFile(new_path); // Записываем в новый файл, который будет находиться в созданной папке
				search = "Nseed=";

				while (getline(readFile, readout))
				{
					if (readout.substr(0, search.length()) == search)
					{
						found = readout.find("=") + 1;
						s = readout.substr(found); // Случайное число
						readout.replace(found, s.length(), to_string(stoi(s) + 2 * num));
					}
					outFile << readout << endl;
				}
				readFile.close();
				outFile.close();
			}
		}

		// Сортировка закончена
		cout << "I've finished sorting data" << endl;

		// Обрабатываем, если необходимо
		if (answ_2 == 1 && !files.empty()) // Если также хоть какие-то файлы были найдены (вектор файлов не пустой)
		{
			// Смотрим, какие файлы и папки внутри указанной директории (появились новые)
			for (const auto & p : fs::directory_iterator(path))
			{
				s = p.path().string(); // Путь к объекту, преобразованный в строку
				if (is_directory(p)) // Если папка
				{
					//dirs.push_back(s); // Добавить в конец вектора строку - путь к папке

					// Выделяем имя папки
					found = s.find_last_of("/\\");
					name = s.substr(found + 1);
					dirs.push_back(name); // Добавить в конец вектора строку - имя папки
				}
			}

			cout << "and started processing" << endl;

			// В каждой новой папке скармливаем clar_proc.exe координаты и скорости
			for (int i = 0; i < dirs.size(); i++)
			{
				new_path = path + dirs[i] + "/";

				// Ищем в новой папке файл с координатами
				for (const auto & p : fs::directory_iterator(new_path))
				{
					s = p.path().string(); // Путь к объекту, преобразованный в строку
					if (!is_directory(p)) // Если файл (не папка)
					{
						// Выделяем имя файла
						found = s.find_last_of("/\\");
						name = s.substr(found + 1);

						found = name.find("coo_");

						if (found != -1) // -1 означает, что подстрока не найдена
							break;
					}
				}

				// Если так и не нашли
				if (found == -1)
					cout << "The directory " << dirs[i] << "doesn't have coordinates dat-file!" << endl;
				else
				{
					cout << endl << "Processing coordinates in the directory " << new_path << endl;

					ifstream readCoos(new_path + name); // Файл на чтение - координаты
					ofstream outFile(instructions);

					outFile << first_step << endl; // Введен первый шаг

					// Считываем весь файл с координатами и ищем последний шаг
					// TODO: считывать не весь файл. Определить кол-во частиц и переместить указатель на нужное место
					while (getline(readCoos, readout))
					{
						char last_symb = readout[readout.size() - 1];
						if (last_symb != '1' && last_symb != '2') // 1 или 2 отвечает за тип частицы (электрон или ион). Шаги выводятся всегда с 0 на конце
						{
							last_step = readout;
						}
					}
					readCoos.close(); // Закрываем файл с координатами

					outFile << last_step << endl << number_of_steps_to_average << endl; // Введен последний шаг и число шагов для усреднения

					// Извлекаем из файла с нач. данными температуры (если они еще не получены) и записываем в файл с инструкциями
					ifstream readFile(path + init); // Файл на чтение - начальные данные
					if (Te == -1 || Ti == -1) // Если темп. не получены (-1 базовое значение)
					{
						search = "Te=";
						while (getline(readFile, readout))
						{
							if (readout.substr(0, search.length()) == search || flag == 1)
							{
								found = readout.find("=") + 1;
								s = readout.substr(found); // Температура
								outFile << s << endl;
								switch (flag)
								{
								case 0:
									Te = stoi(s);
									break;
								case 1:
									Ti = stoi(s);
									break;
								}
								flag++;
								if (flag == 2)
									break;
							}
						}
					}
					else
						outFile << to_string(Te) << endl << to_string(Ti) << endl;
					readFile.close(); // Закрываем файл с начальными данными
					outFile.close();

					cout << "I'm going to input: " << first_step << " " << last_step << " " << number_of_steps_to_average << " " << Te << " " << Ti << endl;

					s = "clar_proc.exe " + new_path + name + " < " + instructions;
					system(s.c_str()); // Строка преобразована в const char *, иначе system не поймет
					// Осуществлен запуск clar_proc.exe с аргументом coo_<...>.dat

					// На данный момент clar_proc.exe создает файлы не там, где надо, и приходится их копировать, а потом удалять (в самом конце работы проги)
					// TODO: заставить clar_proc.exe создавать файлы там, где надо
					// Перед копированием к отсортированным файлам координат новых результатов clar_proc.exe удаляются старые
					if (fs::exists(new_path + "ionVel.dat"))
						fs::remove(new_path + "ionVel.dat");
					fs::copy("ionVel.dat", new_path + "ionVel.dat");
					if (fs::exists(new_path + "ionVAF.dat"))
						fs::remove(new_path + "ionVAF.dat");
					fs::copy("ionVAF.dat", new_path + "ionVAF.dat");
					if (fs::exists(new_path + "ionAbs.dat"))
						fs::remove(new_path + "ionAbs.dat");
					fs::copy("ionAbs.dat", new_path + "ionAbs.dat");
					if (fs::exists(new_path + "current.dat"))
						fs::remove(new_path + "current.dat");
					fs::copy("current.dat", new_path + "current.dat");
					if (fs::exists(new_path + "elVel.dat"))
						fs::remove(new_path + "elVel.dat");
					fs::copy("elVel.dat", new_path + "elVel.dat");
					if (fs::exists(new_path + "elVAF.dat"))
						fs::remove(new_path + "elVAF.dat");
					fs::copy("elVAF.dat", new_path + "elVAF.dat");
					if (fs::exists(new_path + "elAbs.dat"))
						fs::remove(new_path + "elAbs.dat");
					fs::copy("elAbs.dat", new_path + "elAbs.dat");

				}


			}

		}

	}
	
	return 0;
}

int main()
{
	string input;
	int from, to; // Номера (они же названия) начальной и конечной папок, которые нужно обработать
	int answ;
	size_t found; // Указатель на найденную подстроку

	string instructions = "SD_instructions.dat";
	
	cout << "Input a directory name (or a range of directories through '-'): ";
	getline(cin, input); // Запрашиваем название папки, в которой необходимо рассортировать файлы. getline может принять пустую строку
	
	cout << "Is it necessary to process sorted data? Input 1 to do it: ";
	cin >> answ;
	
	found = input.find("-");
	if (found != -1) // Если найдено тире
	{
		from = stoi(input.substr(0, found));
		to = stoi(input.substr(found + 1));

		for (int i = from; i <= to; i++)
			sort_and_execute(to_string(i), answ);
	}
	else
		sort_and_execute(input, answ);
	
	// Чистка мусора
	fs::remove("ionVel.dat");
	fs::remove("ionVAF.dat");
	fs::remove("ionAbs.dat");
	fs::remove("current.dat");
	fs::remove("elVel.dat");
	fs::remove("elVAF.dat");
	fs::remove("elAbs.dat");
	fs::remove(instructions);

	cout << endl << "I've finished all my work" << endl;
	system("pause");
	return 0;
}