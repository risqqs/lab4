#pragma once
#include <iostream>

class Connection
{
private:
	int id;
	int pipeId;
	int startKS;
	int endKS;
public:
	Connection() = default;
	Connection(int id, int pipeid, int startKS, int endKS);

	int getId() const { return id; }
	int getPipeId() const { return pipeId; }
	int getStartKS() const { return startKS; }
	int getEndKS() const { return endKS; }

	void show() const;
};

