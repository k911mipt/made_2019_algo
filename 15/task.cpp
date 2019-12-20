/*
# Задание 15. «Приближенное решение метрической неориентированной задачи коммивояжера».

Найдите приближенное решение метрической неориентированной задачи коммивояжера в
полном графе (на плоскости) с помощью минимального остовного дерева.

Оцените качество приближения на случайном наборе точек, нормально распределенномна плоскости
с дисперсией 1. Нормально распределенный набор точек получайте с помощью преобразования Бокса-Мюллера.

При фиксированном N, количестве вершин графа, несколько раз запустите оценку качества приближения.
Вычислите среднее значение и среднеквадратичное отклонение качества приближения для данного N.

Запустите данный эксперимент для всех N в некотором диапазоне, например, [2, 10].
Автоматизируйте запуск экспериментов.
В решении требуется разумно разделить код на файлы. Каждому классу - свой заголовочный файл и файл с реализацией.


*/

#include <iostream>
#include <fstream>
#include <random>
#include <cmath>
#include <unordered_set>

#include <vector>


#include "memcheck_crt.h"
#include "linalg.hpp"

using std::default_random_engine;
using std::uniform_int_distribution;

namespace {
    default_random_engine generator;
    uniform_int_distribution<uint64_t> distribution(0, UINT64_MAX);
}
using namespace linalg;
int main() {
    ENABLE_CRT;
    std::ifstream input_stream("input.txt");
    std::cin.rdbuf(input_stream.rdbuf());
    int n = 5;
    std::vector<Vector> points;

    for (int i = 0; i < n; ++i) {
        points.emplace_back(generator, distribution);
        std::cout << points.back().x << " " << points.back().y << std::endl;
    }
    std::cout << std::endl;

    Vector a(1, 1);
    Vector b(0, -1);

    std::cout << Vector::Cross(a, b) << std::endl;
    std::cout << std::endl;


    return EXIT_SUCCESS;
}