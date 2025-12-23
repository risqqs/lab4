#pragma once
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;


class KS {
private:
    int id;
    string name;
    int countWorkshop;
    int countWorkshopInWork;
    string other;
public:
    static int nextId;

    KS();

    void show() const;

    // Геттеры, чтобы получать значение, без доступа
    int getId() const { return id; }
    string getName() const { return name; }
    int getCountWorkshop() const { return countWorkshop; }
    int getCountWorkshopInWork() const { return countWorkshopInWork; }
    

    void startWorkshops(int count);
    void stopWorkshops(int count);

    double getUnusedPercent() const;

    friend ostream& operator << (ostream& out, const KS& ks);
    friend istream& operator >> (istream& in, KS& ks);
    friend ofstream& operator << (ofstream& out, const KS& ks);
    friend ifstream& operator >> (ifstream& in, KS& ks);
};

// Функции для работы с КС
void addKS(unordered_map<int, KS>& kss);
void showAllKS(const unordered_map<int, KS>& kss);
void searchKS(const unordered_map<int, KS>& kss);
void editKS(unordered_map<int, KS>& kss);