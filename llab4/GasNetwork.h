#pragma once
#include <iostream>
#include <unordered_map>
#include <vector>
#include "Connection.h"
#include "Pipe.h"
#include "Ks.h"

using namespace std;

class GasNetwork
{
private:
	unordered_map<int, Connection> connections;
	int nextConnectionId;

	bool bfsForMaxFlow(int source, int sink, const unordered_map<int, vector<pair<int, double>>>& adj, unordered_map<int, int>& parent) const;
	vector<int> dijkstra(int start, int end, const unordered_map<int, vector<pair<int, double>>>& weightedAdj, unordered_map<int, double>& distances, unordered_map<int, int>& predecoders) const;
public:
	GasNetwork();

	bool connectKS(unordered_map<int, Pipe>& pipes, const unordered_map<int, KS>& kss, int startKS, int endKS, int diameter);
	void showAllConnections() const;
	bool disconnect(int ConnectionId);
	bool disconnectAll();

	vector<int> topologicalSort(const unordered_map<int, KS>& kss) const;
	void showTopologicalOrder(const unordered_map<int, KS>& kss) const;

	bool isKSConnected(int ksId) const;
	bool isPipeConnected(int pipeID) const;
	int findFreePipeByDiameter(const unordered_map<int, Pipe>& pipes, int diameter) const;

	void addConnection(const Connection& connection);
	void clear();

	bool connectionExists(int startKS, int endKS) const;

	const unordered_map<int, Connection>& getConnections() const { return connections; }

	double calculateMaxFlow(int sourceKS, int sinkKS, const unordered_map<int, Pipe>& pipes) const;
	vector<int> findShortestPath(int startKS, int endKS, const unordered_map<int, Pipe>& pipes) const;
	void showMaxFlowInfo(int sourceKS, int sinkKS, const unordered_map<int, Pipe>& pipes, const unordered_map<int, KS>& kss) const;
	void showShortestPathInfo(int startKS, int endKS, const unordered_map<int, Pipe>& pipes, const unordered_map<int, KS>& kss) const;
	unordered_map<int, vector<pair<int, double>>> getWeightedGraph(const unordered_map<int, Pipe>& pipes, bool useCapacity = false) const;
};

