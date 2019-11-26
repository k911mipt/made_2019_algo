﻿/*
# Задание 8_1. Хеш-таблица

| Ограничение времени | 0.2 секунды                      |
|---------------------|----------------------------------|
| Ограничение памяти  | 15Mb                             |
| Ввод                | стандартный ввод или input.txt   |
| Вывод               | стандартный вывод или output.txt |

Реализуйте структуру данных типа “множество строк” на основе динамической хеш-таблицы 
с открытой адресацией. Хранимые строки непустые и состоят из строчных латинских букв.
Хеш-функция строки должна быть реализована с помощью вычисления значения многочлена методом Горнера.
Начальный размер таблицы должен быть равным 8-ми. Перехеширование выполняйте при 
добавлении элементов в случае, когда коэффициент заполнения таблицы достигает 3/4.
Структура данных должна поддерживать операции добавления строки в множество, удаления 
строки из множества и проверки принадлежности данной строки множеству.

1_1. Для разрешения коллизий используйте квадратичное пробирование. 
i-ая проба g(k, i)=g(k, i-1) + i (mod m). m - степень двойки.

## Формат входных данных

Каждая строка входных данных задает одну операцию над множеством. Запись операции состоит 
из типа операции и следующей за ним через пробел строки, над которой проводится операция.

Тип операции  – один из трех символов:
* +  означает добавление данной строки в множество;
* -  означает удаление  строки из множества;
* ?  означает проверку принадлежности данной строки множеству.

При добавлении элемента в множество НЕ ГАРАНТИРУЕТСЯ, что он отсутствует в этом множестве. 
При удалении элемента из множества НЕ ГАРАНТИРУЕТСЯ, что он присутствует в этом множестве.


## Формат вывода

Программа должна вывести для каждой операции одну из двух строк OK или FAIL.
* Для операции '?': OK, если элемент присутствует во множестве. FAIL иначе.
* Для операции '+': FAIL, если добавляемый элемент уже присутствует во множестве и потому не может быть добавлен. OK иначе.
* Для операции '-': OK, если элемент присутствовал во множестве и успешно удален. FAIL иначе.

| stdin   | stdout |
|---------|--------|
| + hello | OK     |
| + bye   | OK     |
| ? bye   | OK     |
| + bye   | FAIL   |
| - bye   | OK     |
| ? bye   | FAIL   |
| ? hello | OK     |

*/


#include <iostream>
#include <fstream>

#include <string>
#include <cassert>
#include "memcheck_crt.h"
#include "hashtable.hpp"

using namespace made::stl;

using std::vector;
using std::string;
using std::cin;
using std::cout;
int main() {
    ENABLE_CRT;
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());

    Hasher hasher;
    HashTable<string, Hasher> table(hasher);

    char command = 0;
    string word;
    while (cin >> command >> word) {
        switch (command) {
        case '+':
            cout << (table.Add(word) ? "OK" : "FAIL") << "\n";
            break;
        case '-':
            cout << (table.Delete(word) ? "OK" : "FAIL") << "\n";
            break;
        case '?':
            cout << (table.Has(word) ? "OK" : "FAIL") << "\n";
            break;
        default:
            assert(false);
        }
    }
    return EXIT_SUCCESS;
}