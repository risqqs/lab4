#include "Pipe.h"
#include "Utils.h"
#include <unordered_map>
#include <algorithm>
#include <string>
#include <fstream>
#include <limits>
#include <cmath>

using namespace std;


int Pipe::nextId = 0;

Pipe::Pipe() {
    id = nextId++;
    name = "";
    length = 0;
    diameter = 0;
    inRepair = false;
}

void Pipe::show() const {
    cout << "------------------------" << endl;
    cout << "Труба ID: " << id << endl;
    cout << "  Название: " << name << endl;
    cout << "  Длина: " << length << " км" << endl;
    cout << "  Диаметр: " << diameter << " мм" << endl;
    cout << "  В ремонте: " << (inRepair ? "Да" : "Нет") << endl;
    cout << "------------------------" << endl;
}

ostream& operator<<(ostream& out, const Pipe& pipe)
{
    cout << endl;
    cout << "------------------------" << endl;
    cout << "Труба ID: " << pipe.id << endl;
    cout << "Название: " << pipe.name << std::endl;
    cout << "Длина: " << pipe.length << " км" << std::endl;
    cout << "Диаметр: " << pipe.diameter << " мм" << std::endl;
    cout << "В ремонте: " << (pipe.inRepair ? "Да" : "Нет") << std::endl;
    cout << "------------------------" << endl;
    return out;
}

istream& operator>>(istream& in, Pipe& pipe) {
    cout << "Введите название Трубы: ";
    pipe.name = GetName();
    cout << "Введите длину трубы в км: ";
    pipe.length = GetNumber(0.01);
    cout << "Введите диаметр трубы в мм: ";
    pipe.diameter = GetNumber(1);
    cout << "Труба в ремонте (1 - ДА | 0 - НЕТ): ";
    pipe.inRepair = GetNumber(0,1);

    return in;
}

ofstream& operator<<(ofstream& out, const Pipe& pipe) {
    //out << "PIPE" << endl;
    out << pipe.id << endl;
    out << pipe.name << endl;
    out << pipe.length << endl;
    out << pipe.diameter << endl;
    out << pipe.inRepair << endl;
    return out;
}

ifstream& operator>>(ifstream& in, Pipe& pipe) {
    in >> pipe.id;
    in.ignore();
    getline(in, pipe.name);
    in >> pipe.length >> pipe.diameter >> pipe.inRepair;
    in.ignore();
    return in;
}

void addPipe(unordered_map<int, Pipe>& pipes) {
    Pipe newPipe;
    cin >> newPipe;
    pipes.insert({newPipe.getId(), newPipe});
    cout << "Труба добавлена! ID: " << newPipe.getId() << endl;
}

void showAllPipes(const unordered_map<int, Pipe>& pipes) {
    if (pipes.empty()) {
        cout << "Трубы не найдены!" << endl;
        return;
    }
    for (const auto& item : pipes) {
        item.second.show();
    }
}

void searchPipes(const unordered_map<int, Pipe>& pipes) {
    cout << "Поиск труб:" << endl;
    cout << "1. По названию" << endl;
    cout << "2. По статусу ремонта" << endl;
    cout << "Выберите вариант: ";

    int choice = GetNumber(1,2);
    string searchName;
    string searchStatus;
    bool found;
    switch (choice) {
    case 1:
        cout << "Введите название для поиска: ";
        searchName = GetName();

        found = false;
        for (const auto& item : pipes) {
            if (item.second.getName().find(searchName) != string::npos) {
                item.second.show();
                found = true;
            }
        }
        if (!found) {
            cout << "Трубы не найдены!" << endl;
        }
        break;
    case 2:
        cout << "Искать трубы в ремонте? (1-да, 0-нет): ";
        bool searchStatus = GetNumber(0,1);

        bool found = false;
        for (const auto& item : pipes) {
            if (item.second.isInRepair() == searchStatus) {
                item.second.show();
                found = true;
            }
        }
        if (!found) {
            cout << "Трубы не найдены!" << endl;
        }
        break;
    }
}

void batchEditPipes(unordered_map<int, Pipe>& pipes) {
    unordered_map<int, Pipe> selectedPipes;
    cout << "Пакетное редактирование труб:" << endl;
    cout << "1. Редактировать все трубы" << endl;
    cout << "2. Выбрать трубы для редактирования" << endl;
    cout << "Выберите вариант: ";

    int choice = GetNumber(1,2);

    if (choice == 1) {
        selectedPipes = pipes;
    }
    else if (choice == 2) {
        cout << "Фильтр для выбора труб:" << endl;
        cout << "1. По названию" << endl;
        cout << "2. По статусу ремонта" << endl;
        cout << "Выберите вариант: ";

        int filterChoice = GetNumber(1, 2);

        if (filterChoice == 1) {
            cout << "Введите название для поиска: ";
            string searchName = GetName();
            for (const auto& item : pipes) {
                if (item.second.getName().find(searchName) != string::npos) {
                    selectedPipes.insert({ item.first,item.second });
                }
            }
        }
        else if (filterChoice == 2) {
            cout << "Искать трубы в ремонте? (1-да, 0-нет): ";
            bool searchStatus = GetNumber(0, 1);
            for (const auto& item : pipes) {
                if (item.second.isInRepair() == searchStatus) {
                    selectedPipes.insert({ item.first,item.second });
                }
            }
        }
    }
    if (selectedPipes.empty()) {
        cout << "Нет выбранных труб!" << endl;
        return;
    }

    cout << "Выбрано " << selectedPipes.size() << " труб:" << endl;
    for (const auto& item : selectedPipes) {
        cout << item.second;
    }

    cout << "Новый статус ремонта (1-в ремонте, 0-работает): ";
    bool newStatus = GetNumber(0, 1);

    for (auto& item : selectedPipes) {
        pipes[item.first].setRepair(newStatus);
    }

    cout << "Изменено " << selectedPipes.size() << " труб" << endl;
}

double Pipe::getCapacity() const {
    if (inRepair) {
        return 0.0;
    }
    
    double d_m = diameter / 1000.0; // перевод диаметра в метры
    double l_m = length * 1000.0; // перевод длины в метры

    double d_pow_5 = pow(d_m, 5);

    double capacity = sqrt(d_pow_5 / l_m);

    capacity *= 1000.0; // для более красивых значений

    return capacity;
}

double Pipe::getWeight() const {
    if (inRepair) {
        return numeric_limits<double>::infinity();
    }

    return length;
}

void Pipe::showExtended() const {
    // Добавляем дополнительную информацию
    cout << "  Дополнительная информация:" << endl;
    cout << "  Пропускная способность: " << getCapacity() << " у.e." << endl;

    double weight = getWeight();
    if (isinf(weight)) {
        cout << "  Вес трубы: БЕСКОНЕЧНО (труба в ремонте)" << endl;
    }
    else {
        cout << "  Вес трубы: " << weight << " км" << endl;
    }
    if (inRepair) {
        cout << "  Статус: В РЕМОНТЕ (недоступна для транспортировки)" << endl;
    }
    else {
        cout << "  Статус: РАБОТАЕТ" << endl;
    }
}