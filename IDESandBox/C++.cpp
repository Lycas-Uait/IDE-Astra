#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <conio.h>
#include <cstdlib>
#include <windows.h>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// Функция для получения команды g++
string getGppCommand() {
    if (fs::exists("MinGW\\bin\\g++.exe")) {
        return "\"MinGW\\bin\\g++.exe\""; // локальный MinGW
    } else if (system("g++ --version >nul 2>&1") == 0) {
        return "g++"; // системный компилятор
    } else {
        return ""; // не найден
    }
}

int main() {
    // Зелёный текст
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (1 << 4) | 7);
    setlocale(LC_ALL, ("Rus"));

    // Создаём папку projects, если её нет
    if (!fs::exists("projects")) fs::create_directory("projects");

    vector<string> lines = {""};
    int x = 0, y = 0;

    while (true) {
        system("cls");
        cout << "IDE SandBox\n";
        cout << "Ctrl+S - start , :exit - exit the program\n\n";

        // Отображаем код с курсором
        for (size_t i = 0; i < lines.size(); ++i) {
            if ((int)i == y)
                cout << lines[i].substr(0,x) << "|" << lines[i].substr(x) << "\n";
            else
                cout << lines[i] << "\n";
        }

        int ch = _getch();

        if (ch == 0 || ch == 224) { // стрелки
            ch = _getch();
            if (ch == 72 && y>0) y--;
            else if (ch == 80 && y<(int)lines.size()-1) y++;
            else if (ch == 75 && x>0) x--;
            else if (ch == 77 && x<(int)lines[y].size()) x++;
        } else if (ch == 13) { // Enter
            string rest = lines[y].substr(x);
            lines[y] = lines[y].substr(0,x);
            lines.insert(lines.begin()+y+1, rest);
            y++; x=0;
        } else if (ch == 8) { // Backspace
            if (x>0) { lines[y].erase(x-1,1); x--; }
            else if (y>0) {
                x = lines[y-1].size();
                lines[y-1] += lines[y];
                lines.erase(lines.begin()+y);
                y--;
            }
        } else if (ch == 19) { // Ctrl+S -> run
            string gppPath = getGppCommand();
            if (gppPath.empty()) {
                cout << "\nКомпилятор g++ не найден!\n";
                cout << "Установите MinGW и добавьте его в PATH(рекомендуеться), или положите MinGW рядом с IDE.\n";
                cout << "Press any key to continue_ ";
                _getch();
                return 0;
            }

            // Сохраняем код
            ofstream file("projects/temp.cpp");
            for(auto &l:lines) file << l << "\n";
            file.close();

            // Компиляция
            string compileCmd = gppPath + " projects\\temp.cpp -o projects\\temp.exe 2> projects\\errors.txt";
            system(compileCmd.c_str());

            // Проверка ошибок
            ifstream errFile("projects/errors.txt");
            string errors((istreambuf_iterator<char>(errFile)), istreambuf_iterator<char>());
            errFile.close();

            if (!errors.empty()) {
                cout << "\n___ Ошибки компиляции ___\n";
                cout << errors << "\n";
                cout << "Нажмите любую клавишу для продолжения...";
                _getch();
            } else {
                system("start cmd /k projects\\temp.exe");
            }
        } else if (isprint(ch)) { // обычный символ
            lines[y].insert(x,1,(char)ch);
            x++;
        }

        // Проверяем, ввод :exit
        string currentLine = lines[y];
        if (currentLine == ":exit") break;

        if (x>lines[y].size()) x = lines[y].size();
    }

    return 0;
}