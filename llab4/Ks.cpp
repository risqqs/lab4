#include "KS.h"
#include "Utils.h"
#include <unordered_map>
#include <fstream>
#include <string>

using namespace std;

int KS::nextId = 0;


KS::KS() {
    id = nextId++;
    name = "";
    countWorkshop = 0;
    countWorkshopInWork = 0;
    other = "";
}

void KS::show() const {
    double unusedPercent = getUnusedPercent();
    cout << endl;
    cout << "------------------------" << endl;
    cout << "КС ID: " << id << endl;
    cout << "Название: " << name << std::endl;
    cout << "Цехов всего: " << countWorkshop <<endl;
    cout << "Цехов в работе: " << countWorkshopInWork << std::endl;
    cout << "Незадействовано: " << unusedPercent << "%" << std::endl;
    cout << "Класс станции: " << other << endl;
    cout << "------------------------" << endl;
}

ostream& operator<<(ostream& out, const KS& ks)
{
    double unusedPercent = ks.getUnusedPercent();
    cout << endl;
    cout << "------------------------" << endl;
    cout << "КС ID: " << ks.id << endl;
    cout << "Название: " << ks.name << std::endl;
    cout << "Цехов всего: " << ks.countWorkshop << std::endl;
    cout << "Цехов в работе: " << ks.countWorkshopInWork << std::endl;
    cout << "Незадействовано: " << unusedPercent << "%" << std::endl;
    cout << "Класс станции: " << ks.other << endl;
    cout << "------------------------" << endl;
    return out;
}

istream& operator>>(istream& in, KS& ks) {
    cout << "Введите название КС: ";
    ks.name = GetName();
    cout << "Введите общее количество цехов: ";
    ks.countWorkshop = GetNumber(1);
    cout << "Введите количество работающих цехов: ";
    ks.countWorkshopInWork = GetNumber(0, ks.countWorkshop);
    cout << "Введите класс станции (string): ";
    ks.other = GetName();
    return in;
}

ofstream& operator<<(ofstream& out, const KS& ks)
{
    //out << "KS" << endl;
    out << ks.id << endl;
    out << ks.name << endl;
    out << ks.countWorkshop << endl;
    out << ks.countWorkshopInWork << endl;
    out << ks.other << endl;
    return out;
}

ifstream& operator>>(ifstream& in, KS& ks)
{
    in >> ks.id;
    in.ignore();
    getline(in, ks.name);
    in >> ks.countWorkshop >> ks.countWorkshopInWork;
    in.ignore();
    getline(in, ks.other);
    return in;
}

double KS::getUnusedPercent() const {
    if (countWorkshop == 0) return 0;
    return ((countWorkshop - countWorkshopInWork) * 100.0) / countWorkshop;
}

void addKS(unordered_map<int, KS>& kss) {
    KS newKS;
    cin >> newKS;
    kss.insert({ newKS.getId(), newKS });
    cout << "КС добавлена! ID: " << newKS.getId() << endl;
}

void showAllKS(const unordered_map<int, KS>& kss) {
    if (kss.empty()) {
        cout << "КС не найдены!" << endl;
        return;
    }
    for (const auto& item : kss) {
        item.second.show();
    }
}

void searchKS(const unordered_map<int, KS>& kss) {
    cout << "Поиск КС:" << endl;
    cout << "1. По названию" << endl;
    cout << "2. По проценту незадействованных цехов" << endl;
    cout << "Выберите вариант: ";

    int choice = GetNumber(1,2);
    string searchName;
    bool found;
    switch (choice) {
    case 1:
        cout << "Введите название для поиска: ";
        searchName = GetName();

        found = false;
        for (const auto& item : kss) {
            if (item.second.getName().find(searchName) != string::npos) {
                item.second.show();
                found = true;
            }
        }
        if (!found) {
            cout << "КС не найдены!" << endl;
        }
        break;
    case 2:
        cout << "Введите минимальный процент незадействованных цехов: ";
        float minPercent = GetNumber(0,100);

        bool found = false;
        for (const auto& item : kss) {
            if (item.second.getUnusedPercent() >= minPercent) {
                item.second.show();
                found = true;
            }
        }
        if (!found) {
            cout << "КС не найдены!" << endl;
        }
        break;
    }
}

// Методы для управления цехами
void KS::startWorkshops(int count) {
    int available = countWorkshop - countWorkshopInWork;
    if (count > available) {
        throw invalid_argument("Нельзя запустить больше " + to_string(available) + " цехов");
    }
    countWorkshopInWork += count;
}

void KS::stopWorkshops(int count) {
    if (count > countWorkshopInWork) {
        throw invalid_argument("Нельзя остановить больше " + to_string(countWorkshopInWork) + " цехов");
    }
    countWorkshopInWork -= count;
}

void editKS(unordered_map<int, KS>& kss) {
    if (kss.empty()) {
        cout << "Нет доступных КС для редактирования!" << endl;
        return;
    }

    cout << "Доступные КС:" << endl;
    for (const auto& item : kss) {
        cout << "ID: " << item.first << " - " << item.second.getName() << endl;
    }

    cout << "Введите ID КС для редактирования: ";
    int id = GetNumber(0);

    auto it = kss.find(id);
    if (it == kss.end()) {
        cout << "КС с ID " << id << " не найдена!" << endl;
        return;
    }

    KS& ks = it->second;

    int choice;
    do {
        cout << "\n=== Редактирование КС '" << ks.getName() << "' ===" << endl;
        cout << "1. Запустить цеха" << endl;
        cout << "2. Остановить цеха" << endl;
        cout << "0. Вернуться в меню" << endl;
        cout << "Выберите действие: ";

        choice = GetNumber<int>(0, 2);

        switch (choice) {
        case 1: {
            int available = ks.getCountWorkshop() - ks.getCountWorkshopInWork();
            if (available == 0) {
                cout << "Все цеха уже работают!" << endl;
                break;
            }
            cout << "Можно запустить до " << available << " цехов: ";
            int count = GetNumber<int>(1, available);
            ks.startWorkshops(count);
            cout << "Запущено " << count << " цехов. Теперь работает: " << ks.getCountWorkshopInWork() << " цехов" << endl;
            break;
        }
        case 2: {
            if (ks.getCountWorkshopInWork() == 0) {
                cout << "Нет работающих цехов!" << endl;
                break;
            }
            cout << "Можно остановить до " << ks.getCountWorkshopInWork() << " цехов: ";
            int count = GetNumber(1, ks.getCountWorkshopInWork());
            ks.stopWorkshops(count);
            cout << "Остановлено " << count << " цехов. Теперь работает: " << ks.getCountWorkshopInWork() << " цехов" << endl;
            break;
        }
        case 0:
            cout << "Выход из редактирования КС" << endl;
            break;
        }
    } while (choice != 0);
}