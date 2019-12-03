/*
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

#pragma region "memcheck_crt.h"
#ifndef MEMCHECK_CRT_
#define MEMCHECK_CRT_

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define ENABLE_CRT\
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);\
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);\
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
#else
#define ENABLE_CRT ;
#endif

#endif // !MEMCHECK_CRT_
#pragma endregion "memcheck_crt.h"


#pragma region "hashtable.hpp"
#ifndef HASH_TABLE_H_
#define HASH_TABLE_H_

#include <vector>
#include <functional>

namespace made {

    namespace stl {
        using std::vector;
        using std::string;

        struct Hasher {
            unsigned int operator()(const std::string& str) const {
                unsigned int hash = 0;
                for (unsigned int i = 0; i < str.length(); i++)
                    hash = hash * 2047 + str[i];
                return hash;
            }
        };

        template<class T, class H>
        class HashTable {
            using uint = unsigned int;
            static const uint INITIAL_SIZE = 8;
        public:
            HashTable(const H& _hasher) :
                hasher_(_hasher), 
                table_size_(INITIAL_SIZE), 
                table_(INITIAL_SIZE), 
                key_count_(0), 
                deleted_key_count_(0) {};
            ~HashTable() = default;

            HashTable(const HashTable& t) = delete;
            HashTable& operator=(const HashTable& t) = delete;

            bool Add(const T& key);
            bool Has(const T& key);
            bool Delete(const T& key);
        private:
            struct Element {
                T key;
                bool exists = false;
                bool deleted = false;
            };
            typedef std::function<void(Element*, size_t)> SeekEvent;

            H hasher_;
            size_t table_size_;
            vector<Element> table_;
            uint key_count_;
            uint deleted_key_count_;

            bool Seek(const T& key, SeekEvent&& cb);
            void RehashTable();
            uint Probe(uint hash, uint i);
        };

#pragma region HashTable
        template<class T, class H>
        bool HashTable<T, H>::Add(const T& key) {
            if (key_count_ > table_size_ - (table_size_ >> 2)) {
                RehashTable();
            }

            uint index = hasher_(key) % table_size_;
            Element* elem = &table_[index];
            Element* deleted_elem = nullptr;
            uint deleted_index = 0;
            for (uint i = 0; elem->exists && (i < table_size_); ++i) {
                if (elem->deleted)
                    deleted_elem = deleted_elem ? deleted_elem : elem;
                else if (elem->key == key)
                    return false;
                index = Probe(index, i);
                elem = &table_[index];
            }

            if (deleted_elem) {
                assert(deleted_key_count_ > 0);
                --deleted_key_count_;
                elem = deleted_elem;
            }
            else {
                ++key_count_;
            }

            elem->deleted = false;
            elem->exists = true;
            elem->key = key;
            return true;
        }

        template<class T, class H>
        inline bool HashTable<T, H>::Has(const T& key) {
            return Seek(key, [](Element*, size_t) {});
        }

        template<class T, class H>
        inline bool HashTable<T, H>::Delete(const T& key) {
            return Seek(key, [this](Element* elem, size_t index) {
                elem->deleted = true;
                ++deleted_key_count_;
            });
        }

        template<class T, class H>
        bool HashTable<T, H>::Seek(const T& key, SeekEvent&& cb) {
            uint index = hasher_(key) % table_size_;
            Element* elem = &table_[index];
            for (uint i = 0; elem->exists && (i < table_size_); ++i) {
                if (!elem->deleted && (elem->key == key)) {
                    cb(elem, index);
                    return true;
                }
                index = Probe(index, i);
                elem = &table_[index];
            }
            return false;
        }

        template<class T, class H>
        void HashTable<T, H>::RehashTable() {
            assert(key_count_ >= deleted_key_count_);
            uint actual_key_count = key_count_ - deleted_key_count_;
            assert(table_size_ < SIZE_MAX >> 1);
            table_size_ = table_size_ << 1;
            // Little optimization : resizing can make result table_ smaller if we had too many deleted keys
            // actual_key_count < table_size_ * 0.375
            while ((actual_key_count < ((table_size_ >> 1) - (table_size_ >> 3))) && (table_size_ > INITIAL_SIZE)) {
                table_size_ /= 2;
            }

            vector<Element> newTable = std::move(table_);
            table_ = vector<Element>(table_size_);
            key_count_ = 0;
            deleted_key_count_ = 0;
            for (auto elem : newTable) {
                if (elem.exists && !elem.deleted)
                    Add(elem.key);
            }
        }

        template<class T, class H>
        inline unsigned int HashTable<T, H>::Probe(uint index, uint i) {
            return (index + i + 1) % table_size_;
        }
#pragma endregion HashTable

    }
}
#endif // !HASH_TABLE_H_
#pragma endregion "hashtable.hpp"


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
