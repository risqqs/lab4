#include "Utils.h"
#include "Pipe.h"
#include "KS.h"
#include <unordered_map>
#include <fstream>
#include <string>
#include "Logger.h"
#include "GasNetwork.h"


using namespace std;
#define LOG(message) Logger::getInstance().log(message)

string GetName()
{
    string name;
    while (true)
    {
        //cin.ignore(1000, '\n');
        getline(cin, name);
        LOG(name);
        if (!name.empty())
        {
            return name;
        }
        else
        {
            cout << "Неправильный ввод! Введите пожалуйста правильное имя: " << endl;
        }
    }
    return string();
}

void saveToFile(const unordered_map<int, Pipe>& pipes, const unordered_map<int, KS>& kss, const GasNetwork& network) {
    cout << "Введите имя файла для сохранения: ";
    string filename = GetName();

    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла!" << endl;
        return;
    }

    // Сохраняем трубы
    file << pipes.size() << endl;
    for (const auto& item : pipes) {
        file << item.second;
    }

    // Сохраняем КС
    file << kss.size() << endl;
    for (const auto& item : kss) {
        file << item.second;
    }

    const auto& connections = network.getConnections();
    file << connections.size() << endl;
    for (const auto& item : connections) {
        const Connection& conn = item.second;
        file << conn.getId() << endl;
        file << conn.getPipeId() << endl;
        file << conn.getStartKS() << endl;
        file << conn.getEndKS() << endl;
    }

    file.close();
    cout << "Данные сохранены в файл: " << filename << endl;
}

void loadFromFile(unordered_map<int, Pipe>& pipes, unordered_map<int, KS>& kss, GasNetwork& network) {
    cout << "Введите имя файла для загрузки: ";
    string filename = GetName();

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Ошибка открытия файла!" << endl;
        return;
    }

    // Очищаем текущие данные
    pipes.clear();
    kss.clear();

    // Загружаем трубы
    int pipeCount;
    file >> pipeCount;
    file.ignore();

    for (int i = 0; i < pipeCount; i++) {
        Pipe pipe;
        file >> pipe;
        pipes.insert({ pipe.getId(),pipe });
    }

    // Загружаем КС
    int ksCount;
    file >> ksCount;
    file.ignore();

    for (int i = 0; i < ksCount; i++) {
        KS ks;
        file >> ks;
        kss.insert({ ks.getId(),ks });
    }

    int connectionCount;
    file >> connectionCount;    
    file.ignore();
    for (int i = 0; i < connectionCount; i++) {
        int connId, pipeId, startKS, endKS;
        file >> connId >> pipeId >> startKS >> endKS;
        file.ignore();

        Connection conn(connId, pipeId, startKS, endKS);
        network.addConnection(conn);
    }

    file.close();

    // Обновляем nextId
    int maxPipeId = 0, maxKSId = 0;
    for (const auto& item : pipes) {
        if (item.first > maxPipeId) maxPipeId = item.first;
    }
    for (const auto& item : kss) {
        if (item.first > maxKSId) maxKSId = item.first;
    }

    cout << "Данные загружены из файла: " << filename << endl;
}

void deleteObject(unordered_map<int, Pipe>& pipes, unordered_map<int, KS>& kss, GasNetwork& network) {
    cout << "Что вы хотите удалить?" << endl;
    cout << "1. Трубу" << endl;
    cout << "2. КС" << endl;
    cout << "3. Соединение" << endl;
    cout << "Выберите вариант: ";

    int choice = GetNumber<int>(1, 3);

    switch (choice) {
    case 1: {
        // Удаление трубы
        if (pipes.empty()) {
            cout << "Нет доступных труб для удаления!" << endl;
            return;
        }

        cout << "Доступные трубы:" << endl;
        for (const auto& item : pipes) {
            cout << "ID: " << item.first << " - " << item.second.getName() << endl;
            if (network.isPipeConnected(item.first)) {
                cout << "Данная труба включена в ГТС. Нельзя удалить!" << endl;
            }
        }

        cout << "Введите ID трубы для удаления: ";
        int id = GetNumber<int>(0);

        if (network.isPipeConnected(id)) {
            cout << "Труба включена в сеть! Сначала удалите соединение!" << endl;
            return;
        }

        auto pipeIt = pipes.find(id);
        if (pipeIt != pipes.end()) {
            pipes.erase(pipeIt);
            cout << "Труба удалена!" << endl;
        }
        else {
            cout << "Труба с ID " << id << " не найдена!" << endl;
        }
        break;
    }

    case 2: {
        // Удаление КС
        if (kss.empty()) {
            cout << "Нет доступных КС для удаления!" << endl;
            return;
        }

        cout << "Доступные КС:" << endl;
        for (const auto& item : kss) {
            cout << "ID: " << item.first << " - " << item.second.getName() << endl;
            if (network.isKSConnected(item.first)) {
                cout << "Данная КС включена в ГТС. Нельзя удалить!" << endl;
            }
        }

        cout << "Введите ID КС для удаления: ";
        int id = GetNumber<int>(0);

        if (network.isKSConnected(id)) {
            cout << "КС включена в сеть! Сначала удалите соединение!" << endl;
            return;
        }

        auto ksIt = kss.find(id);
        if (ksIt != kss.end()) {
            kss.erase(ksIt);
            cout << "КС удалена!" << endl;
        }
        else {
            cout << "КС с ID " << id << " не найдена!" << endl;
        }
        break;
    }

    case 3: {
        if (network.getConnections().empty()) {
            cout << "Нет доступных соединений для удаления!" << endl;
            return;
        }

        cout << "Доступные соединения:" << endl;
        network.showAllConnections();

        cout << "Введите ID соединения для удаления: ";
        int id = GetNumber<int>(0);

        network.disconnect(id);
        break;
    }
    }
}

void showAvailableKS(const unordered_map<int, KS>& kss) {
    if (kss.empty()) {
        cout << "Нет доступных КС!" << endl;
        return;
    }

    cout << "=== ДОСТУПНЫЕ КС ===" << endl;
    for (const auto& item : kss) {
        cout << "ID: " << item.first << " - " << item.second.getName() << endl;
    }
    cout << "=====================" << endl;
}

void showAvailablePipes(const unordered_map<int, Pipe>& pipes, const GasNetwork& network) {
    if (pipes.empty()) {
        cout << "Нет доступных труб!" << endl;
        return;
    }

    cout << "=== ДОСТУПНЫЕ ТРУБЫ ===" << endl;
    for (const auto& item : pipes) {
        cout << "ID: " << item.first << " - " << item.second.getName()
            << " (Диаметр: " << item.second.getDiameter() << " мм)";
        if (network.isPipeConnected(item.first)) {
            cout << " [ЗАНЯТА]";
        }
        else {
            cout << " [СВОБОДНА]";
        }
        cout << endl;
    }
    cout << "=======================" << endl;
}

int getValidKSId(const unordered_map<int, KS>& kss, const string& prompt) {
    int id;
    while (true) {
        cout << prompt;
        id = GetNumber<int>(0);

        if (kss.find(id) != kss.end()) {
            return id;
        }
        else {
            cout << "КС с ID " << id << " не найдена! ";
            showAvailableKS(kss);
            cout << "Пожалуйста, введите корректный ID: ";
        }
    }
}
