#include <iostream>
#include <vector>
#include <limits>
#include <string>
#include <fstream>
#include "Pipe.h"
#include "Ks.h"
#include "Utils.h"
#include <unordered_map>
#include "GasNetwork.h"

using namespace std;

void menu(unordered_map<int, Pipe>& pipes, unordered_map<int, KS>& kss, GasNetwork& network) {
    int number;
    while (true)
    {
        cout << "1. Добавить трубу" << endl;
        cout << "2. Добавить КС" << endl;
        cout << "3. Просмотр всех труб" << endl;
        cout << "4. Просмотр всех КС" << endl;
        cout << "5. Поиск труб" << endl;
        cout << "6. Поиск КС" << endl;
        cout << "7. Пакетное редактирование труб" << endl;
        cout << "8. Редактировать КС" << endl;
        cout << "9. Удалить объект" << endl;
        cout << "10. Сохранить в файл" << endl;
        cout << "11. Загрузить из файла" << endl;
        cout << "12. Соединить КС с трубой" << endl;
        cout << "13. Просмотр всех соединений" << endl;
        cout << "14. Топологическая сортировка" << endl;
        cout << "15. Удалить соединение" << endl;
        cout << "16. Удалить все соединения" << endl;
        cout << "17. Расчет максимального потока" << endl;
        cout << "18. Поиск кратчайшего пути" << endl;
        cout << "19. Показать расширенную информацию о трубах" << endl;
        cout << "0. Выход" << endl;
        cout << "Выберите действие: ";

        number = GetNumber(0, 19);
        cout << endl;
        switch (number) {
        case 1:
        {
            addPipe(pipes);
            break;
        }
        case 2:
        {
            addKS(kss);
            break;
        }
        case 3:
        {
            showAllPipes(pipes);
            break;
        }
        case 4:
        {
            showAllKS(kss);
            break;
        }
        case 5:
        {
            searchPipes(pipes);
            break;
        }
        case 6:
        {
            searchKS(kss);
            break;
        }
        case 7:
        {
            batchEditPipes(pipes);
            break;
        }
        case 8:
        {
            editKS(kss);
            break;
        }
        case 9:
        {
            deleteObject(pipes, kss, network);
            break;
        }
        case 10:
        {
            saveToFile(pipes, kss, network);
            break;
        }
        case 11:
        {
            loadFromFile(pipes, kss, network);
            break;
        }
        case 12:
        {
            cout << "=== СОЕДИНЕНИЕ КС ТРУБОЙ ===" << endl;
            showAvailableKS(kss);

            int startKS = getValidKSId(kss, "Введите ID КС входа: ");

            int endKS;
            while (true) {
                endKS = getValidKSId(kss, "Введите ID КС выхода: ");
                if (endKS != startKS) {
                    break;
                }
                cout << "КС выхода не может совпадать с КС входа! ";
            }

            cout << "Введите диаметр трубы (500, 700, 1000, 1400): ";
            int diameter = GetNumber(0);
            network.connectKS(pipes, kss, startKS, endKS, diameter);
            break;
        }
        case 13:
        {
            network.showAllConnections();
            break;
        }
        case 14:
        {
            network.showTopologicalOrder(kss);
            break;
        }
        case 15:
        {
            cout << "-------------УДАЛЕНИЕ СОЕДИНЕНИЯ-------------" << endl;
            network.showAllConnections();
            cout << "Введите ID соединения для удаления: ";
            int connId = GetNumber(0);
            network.disconnect(connId);
            break;
        }
        case 16:
        {
            network.disconnectAll();
            break;
        }
        case 17:
        {
            cout << "----РАСЧЕТ МАКСИМАЛЬНОГО ПОТОКА----" << endl;
            showAvailableKS(kss);

            cout << "Введите ID КС-источника: ";
            int source = getValidKSId(kss, "ID источника: ");

            cout << "Введите ID КС-стока: ";
            int sink = getValidKSId(kss, "ID стока: ");

            if (source == sink) {
                cout << "Источник и сток не могут совпадать!" << endl;
                break;
            }

            network.showMaxFlowInfo(source, sink, pipes, kss);
            break;
        }

        case 18:
        {
            cout << "----ПОИСК КРАТЧАЙШЕГО ПУТИ----" << endl;
            showAvailableKS(kss);

            cout << "Введите ID начальной КС: ";
            int start = getValidKSId(kss, "ID начала: ");

            cout << "Введите ID конечной КС: ";
            int end = getValidKSId(kss, "ID конца: ");

            if (start == end) {
                cout << "Начальная и конечная КС не могут совпадать!" << endl;
                break;
            }

            network.showShortestPathInfo(start, end, pipes, kss);
            break;
        }

        case 19:
        {
            cout << "РАСШИРЕННАЯ ИНФОРМАЦИЯ О ТРУБАХ" << endl;
            if (pipes.empty()) {
                cout << "Трубы не найдены!" << endl;
                break;
            }

            for (const auto& item : pipes) {
                item.second.showExtended();
            }
            break;
        }
        case 0:
        {
            cout << "Выход из программы!\n";
            return;
        }
        default:
            cout << "Попробуйте ввести ещё раз, такого числа нет!\n" << endl;
            break;
        };
    };
};

int main()
{
    system("chcp 1251 > nul");
    setlocale(LC_ALL, "Russian");

    // ПРИНУДИТЕЛЬНЫЙ СБРОС ПОТОКА ВВОДА
    std::cin.clear(); // Сбрасываем флаги ошибок (включая EOF)
    std::cin.seekg(0, std::ios::beg); // Перемещаемся в начало 
    Logger::getInstance().init("session.txt");

    unordered_map<int, Pipe> pipes;
    unordered_map<int, KS> kss;
    GasNetwork network;

    menu(pipes, kss, network);
    return 0;
}
