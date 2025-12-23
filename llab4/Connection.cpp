#include "Connection.h"
#include <iostream>

using namespace std;

Connection::Connection(int id, int pipeId, int startKS, int endKS)
    : id(id), pipeId(pipeId), startKS(startKS), endKS(endKS) {
}

void Connection::show() const {
    cout << "=== Соединение ID: " << id << " ===" << endl;
    cout << "Труба ID: " << pipeId << endl;
    cout << "КС входа: " << startKS << endl;
    cout << "КС выхода: " << endKS << endl;
    cout << "=========================" << endl;
}