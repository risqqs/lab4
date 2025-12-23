#pragma once
#include <iostream>
#include "Ks.h"
#include "Pipe.h"
#include "Logger.h"
#include "GasNetwork.h"
using namespace std;

// Макрос для удобного логирования
#define LOG(message) Logger::getInstance().log(message)

// Функции для ввода данных
template <typename N>
N GetNumber(const N left_border = 0, const N right_border = std::numeric_limits<N>::max())
{
    N x;
    while (true)
    {
        cin >> x;
        // Проверяем успешность ввода и что следующий символ - конец строки
        if (cin.good() && cin.peek() == '\n' && x >= left_border && x <= right_border)
        {
            cin.ignore(1000, '\n'); // Очищаем буфер
            LOG(to_string(x));
            return x;
        }
        else
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Неправильный ввод! Пожалуйста измените число: " << endl;
            LOG(to_string(x));
        }
    }
};


string GetName();

// Функции для работы с файлами
void saveToFile(const unordered_map<int, Pipe>& pipes, const unordered_map<int, KS>& kss, const GasNetwork& network);
void loadFromFile(unordered_map<int, Pipe>& pipes, unordered_map<int, KS>& kss, GasNetwork& network);

// Функции для работы с объектами
void deleteObject(unordered_map<int, Pipe>& pipes, unordered_map<int, KS>& kss, GasNetwork& network);

void showAvailableKS(const unordered_map<int, KS>& kss);
void showAvailablePipes(const unordered_map<int, Pipe>& pipes, const GasNetwork& network);
int getValidKSId(const unordered_map<int, KS>& kss, const string& prompt);