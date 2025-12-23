#include "GasNetwork.h"
#include "Utils.h"
#include <algorithm>
#include <iostream>
#include "Connection.h"
#include <queue>
#include <unordered_set>


using namespace std;

GasNetwork::GasNetwork() : nextConnectionId(0) {}

bool GasNetwork::connectKS(unordered_map<int, Pipe>& pipes, const unordered_map<int, KS>& kss, int startKS, int endKS, int diameter){

	if (kss.find(startKS) == kss.end()) {
		cout << "КС входа с ID " << startKS << " не найдена!" << endl;
		showAvailableKS(kss);
		return false;
	}
	if (kss.find(endKS) == kss.end()) {
		cout << "КС выхода с ID " << endKS << " не найдена!" << endl;
		showAvailableKS(kss);
		return false;
	}
	if (startKS == endKS) {
		cout << "Нельзя соединять КС саму с собой!" << endl;
		return false;
	}

	// Проверяем, нет ли уже соединения между этими КС
	if (connectionExists(startKS, endKS)) {
		cout << "Соединение между КС " << startKS << " и КС " << endKS << " уже существует!" << endl;
		cout << "Нельзя создавать параллельные или обратные соединения!" << endl;
		return false;
	}

	// Проверяем стандартные диаметры
	vector<int> allowedDiameters = { 500, 700, 1000, 1400 };
	bool diameterValid = false;
	for (int d : allowedDiameters) {
		if (d == diameter) {
			diameterValid = true;
			break;
		}
	}

	if (!diameterValid) {
		cout << "Диаметр " << diameter << " не поддерживается! Используйте: 500, 700, 1000, 1400 мм" << endl;
		return false;
	}

	int pipeId = findFreePipeByDiameter(pipes, diameter);

	if (pipeId == -1) {
		cout << "Свободной трубы с диаметром " << diameter << " мм не найдено!" << endl;
		cout << "Пожалуйста создайте такую трубу самостоятельно!" << endl;

		cout << "Хотите создать новую трубу ЭТОГО диаметра? (1-да, 0-нет): ";
		int createNew = GetNumber(0, 1);
		if (createNew == 1) {
			Pipe newPipe;
			cout << "Создание новой трубы диаметром " << diameter << " мм:" << endl;

			cout << "Введите название Трубы: ";
			newPipe.setName(GetName());
			cout << "Введите длину трубы в км: ";
			newPipe.setLength(GetNumber(0.01));
			newPipe.setDiameter(diameter);  // Устанавливаем наш диаметр
			cout << "Труба в ремонте (1 - ДА | 0 - НЕТ): ";
			newPipe.setRepair(GetNumber(0, 1));

			pipes.insert({ newPipe.getId(), newPipe });
			pipeId = newPipe.getId();
			cout << "Новая труба создана! ID: " << pipeId << endl;
		}
		else {
			cout << "Соединение не создано." << endl;
			return false;
		}
	}

	if (isPipeConnected(pipeId)) {
		cout << "Ошибка: труба " << pipeId << " уже используется в другом соединении!" << endl;
		return false;
	}

	Connection newConnection(nextConnectionId++, pipeId, startKS, endKS);
	connections[newConnection.getId()] = newConnection;

	cout << "Соединение создано! ID = " << newConnection.getId() << endl;
	cout << "КС входа " << startKS << " с трубой " << pipeId << " и с КС выхода" << endKS << endl;
	return true;
}

int GasNetwork::findFreePipeByDiameter(const unordered_map<int, Pipe>& pipes, int diameter) const {
	vector<int> allowedDiameters = { 500, 700, 1000, 1400 };
	bool diameterValid = false;
	for (int d : allowedDiameters) {
		if (d == diameter) {
			diameterValid = true;
			break;
		}
	}

	if (!diameterValid) {
		cout << "Диаметр " << diameter << " не поддерживается! Используйте: 500, 700, 1000, 1400 мм" << endl;
		return -1;
	}

	for (const auto& item : pipes) {
		const Pipe& pipe = item.second;
		if (pipe.getDiameter() == diameter && !isPipeConnected(pipe.getId()) && !pipe.isInRepair()) {
			cout << "Выбрали трубу с id = " << pipe.getId() << endl;
			return pipe.getId();
		}
	}
	return -1;
}

void GasNetwork::showAllConnections() const {
	if (connections.empty()) {
		cout << "Соединения отсутствуют" << endl;
		return;
	}

	cout << "---------------СОЕДИНЕНИЯ---------------" << endl;
	for (const auto& connection : connections) {
		connection.second.show();
	}
}

bool GasNetwork::disconnect(int connectionId) {
	auto it = connections.find(connectionId);
	if (it != connections.end()) {
		connections.erase(it);
		cout << "Соединение " << connectionId << " удалено!" << endl;
		return true;
	}

	cout << "Соединение с ID = " << connectionId << " не найдено!" << endl;
	return false;
}

bool GasNetwork::isKSConnected(int ksId) const {
	for (const auto& connection : connections) {
		const Connection& conn = connection.second;
		if (conn.getStartKS() == ksId || conn.getEndKS() == ksId) {
			return true;
		}
	}
	return false;
}

bool GasNetwork::isPipeConnected(int pipeId) const {
	for (const auto& connection : connections) {
		if (connection.second.getPipeId() == pipeId) {
			return true;
		}
	}
	return false;
}

vector<int> GasNetwork::topologicalSort(const unordered_map<int, KS>& kss) const {
	vector<int> res;
	
	if (connections.empty()) {
		cout << "В сети нет соединений!" << endl;
		for (const auto& ks : kss) {
			res.push_back(ks.first);
		}
		return res;
	}

	// Строим списки смежности и считаем полустепени захода
	unordered_map<int, vector<int>> adjList;
	unordered_map<int, int> inDegree;

	// Инициализируем наши словари для всех КС
	for (const auto& ksPair : kss) {
		int ksId = ksPair.first;
		adjList[ksId] = vector<int>();
		inDegree[ksId] = 0;
	}

	// Заполнение графа из соединений
	for (const auto& connectionPair : connections) {
		const Connection& conn = connectionPair.second;
		int start = conn.getStartKS();
		int end = conn.getEndKS();

		// Добавляем направленное ребро start -> end
		adjList[start].push_back(end);
		inDegree[end]++;
	}

	// Алгоритм Кана (топологическая сортировка)
	queue<int> q;

	// Находим все вершины с нулевой полустепенью захода
	for (const auto& degreePair : inDegree) {
		if (degreePair.second == 0) {
			q.push(degreePair.first);
		}
	}

	// Обрабатываем вершины
	while (!q.empty()) {
		int current = q.front(); // фронт - получаем без удаления элемент, поп - удаляем первый в очереди, пуш - добавляем
		q.pop();
		res.push_back(current);

		// Уменьшаем полустепень захода для всех соседей
		for (int neighbor : adjList[current]) {
			inDegree[neighbor]--;
			if (inDegree[neighbor] == 0) {
				q.push(neighbor);
			}
		}
	}
	// Проверка на циклы
	if (res.size() != kss.size()) {
		cout << "Обнаружен цикл в газотранспортной сети! Топологическая сортировка невозможна." << endl;
		return vector<int>(); // Возвращаем пустой вектор
	}

	return res;
}

void GasNetwork::showTopologicalOrder(const unordered_map<int, KS>& kss) const {
	cout << "------------ТОПОЛОГИЧЕСКАЯ СОРТИРОВКА ГАЗОТРАНСПОРТНОЙ СЕТИ------------" << endl;

	vector<int> order = topologicalSort(kss);

	if (order.empty()) {
		cout << "Невозможно выполнить топологическую сортировку из-за циклов в сети." << endl;
		return;
	}

	cout << "КС в порядке топологической сортировки (от источников к стокам):" << endl;

	for (size_t i = 0; i < order.size(); ++i) {
		auto it = kss.find(order[i]);
		if (it != kss.end()) {
			cout << i + 1 << ". " << it->second.getName() << " (ID: " << order[i] << ")";

			// Показываем информацию о соединениях для этой КС
			bool hasOutgoing = false;
			bool hasIncoming = false;

			for (const auto& conn : connections) {
				if (conn.second.getStartKS() == order[i]) {
					hasOutgoing = true;
				}
				if (conn.second.getEndKS() == order[i]) {
					hasIncoming = true;
				}
			}

			if (!hasIncoming && hasOutgoing) {
				cout << " [ИСТОЧНИК]";
			}
			else if (hasIncoming && !hasOutgoing) {
				cout << " [СТОК]";
			}
			else if (!hasIncoming && !hasOutgoing) {
				cout << " [ИЗОЛИРОВАНА]";
			}

			cout << endl;
		}
	}

	// Дополнительная информация о сети
	cout << "--- Статистика сети ---" << endl;
	cout << "Всего КС: " << kss.size() << endl;
	cout << "Всего соединений: " << connections.size() << endl;

	// Находим источники и стоки
	vector<int> sources, sinks;
	for (const auto& ksPair : kss) {
		int ksId = ksPair.first;
		bool hasIncoming = false;
		bool hasOutgoing = false;

		for (const auto& conn : connections) {
			if (conn.second.getStartKS() == ksId) hasOutgoing = true;
			if (conn.second.getEndKS() == ksId) hasIncoming = true;
		}

		if (!hasIncoming && hasOutgoing) sources.push_back(ksId);
		if (hasIncoming && !hasOutgoing) sinks.push_back(ksId);
	}

	if (!sources.empty()) {
		cout << "Источники (КС без входящих соединений): ";
		for (int src : sources) cout << src << " ";
		cout << endl;
	}

	if (!sinks.empty()) {
		cout << "Стоки (КС без исходящих соединений): ";
		for (int sink : sinks) cout << sink << " ";
		cout << endl;
	}
}

void GasNetwork::addConnection(const Connection& connection) {
	connections[connection.getId()] = connection;
	if (connection.getId() >= nextConnectionId) {
		nextConnectionId = connection.getId() + 1;
	}
}

void GasNetwork::clear() {
	connections.clear();
	nextConnectionId = 0;
}

bool GasNetwork::connectionExists(int startKS, int endKS) const {
	for (const auto& connection : connections) {
		const Connection& conn = connection.second;
		if ((conn.getStartKS() == startKS && conn.getEndKS() == endKS) or (conn.getStartKS() == endKS && conn.getEndKS() == startKS)) {
			return true;
		}
	}
	return false;
}
bool GasNetwork::disconnectAll() {
	if (connections.empty()) {
		cout << "В сети нет соединений для удаления!" << endl;
		return false;
	}

	int connectionCount = connections.size();

	cout << "=== УДАЛЕНИЕ ВСЕХ СОЕДИНЕНИЙ ===" << endl;
	cout << "Количество соединений для удаления: " << connectionCount << endl;

	// Запрашиваем подтверждение
	cout << "Вы уверены, что хотите удалить ВСЕ " << connectionCount << " соединений?" << endl;
	cout << "Это действие нельзя отменить! (1-да, 0-нет): ";
	int confirm = GetNumber(0, 1);

	if (confirm == 1) {
		connections.clear();
		cout << "Все " << connectionCount << " соединений успешно удалены!" << endl;
		cout << "Сеть очищена." << endl;
		return true;
	}
	else {
		cout << "Удаление отменено. Соединения сохранены." << endl;
		return false;
	}
}

// Получение взвешенного графа
unordered_map<int, vector<pair<int, double>>>
GasNetwork::getWeightedGraph(const unordered_map<int, Pipe>& pipes, bool useCapacity) const {
	unordered_map<int, vector<pair<int, double>>> graph;

	for (const auto& connection : connections) {
		const Connection& conn = connection.second;
		int from = conn.getStartKS();
		int to = conn.getEndKS();
		int pipeId = conn.getPipeId();

		// Ищем трубу
		auto pipeIt = pipes.find(pipeId);
		if (pipeIt == pipes.end()) {
			// Труба не найдена - пропускаем это соединение
			cout << "Предупреждение: труба ID " << pipeId << " не найдена!" << endl;
			continue;
		}

		const Pipe& pipe = pipeIt->second;
		double weight;

		if (useCapacity) {
			// Для максимального потока используем пропускную способность
			weight = pipe.getCapacity();
		}
		else {
			// Для кратчайшего пути используем вес (длину)
			weight = pipe.getWeight();
		}

		// Добавляем ребро в граф
		graph[from].push_back({ to, weight });

		// Для алгоритма Форда-Фалкерсона нам также нужно обратное ребро с 0 пропускной способностью
		// Оно будет добавлено при построении остаточной сети
	}

	return graph;
}

bool GasNetwork::bfsForMaxFlow(int source, int sink, const unordered_map<int, vector<pair<int, double>>>& residualGraph, unordered_map<int, int>& parent) const {
	// parent - для восстановления пути: parent[vertex] = предыдущая вершина

	unordered_set<int> visited;  // Посещенные вершины
	queue<int> q;

	// Начинаем с источника
	q.push(source);
	visited.insert(source);
	parent[source] = -1;  // У источника нет родителя

	while (!q.empty()) {
		int current = q.front();
		q.pop();

		// Ищем всех соседей в остаточном графе
		auto it = residualGraph.find(current);
		if (it == residualGraph.end()) {
			continue;  // Нет исходящих ребер
		}

		// Проходим по всем соседям
		for (const auto& neighbor : it->second) {
			int next = neighbor.first;
			double capacity = neighbor.second;

			// Если еще не посещали и есть остаточная пропускная способность
			if (visited.find(next) == visited.end() && capacity > 0.0001) {
				// Малое число вместо 0 для учета погрешности с double
				parent[next] = current;

				if (next == sink) {
					return true;  // Нашли путь до стока
				}

				q.push(next);
				visited.insert(next);
			}
		}
	}

	return false;  // Путь до стока не найден
}

vector<int> GasNetwork::dijkstra(int start, int end,
	const unordered_map<int, vector<pair<int, double>>>& weightedAdj, unordered_map<int, double>& distances, unordered_map<int, int>& predecessors) const {
	// Используем приоритетную очередь для эффективного выбора следующей вершины
	// pair<расстояние, вершина>, сортируем по расстоянию (min-heap)
	priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

	// Инициализация
	for (const auto& node : weightedAdj) {
		distances[node.first] = numeric_limits<double>::infinity();
		predecessors[node.first] = -1;  // -1 означает "нет предшественника"
	}

	distances[start] = 0.0;
	pq.push({ 0.0, start });

	while (!pq.empty()) {
		// Берем вершину с минимальным расстоянием
		double currentDist = pq.top().first;
		int current = pq.top().second;
		pq.pop();

		// Если это конечная вершина - можно остановиться
		if (current == end) {
			break;
		}

		// Пропускаем если нашли лучший путь до этой вершины
		if (currentDist > distances[current] + 0.0001) {
			continue;
		}

		// Проверяем, есть ли у текущей вершины соседи
		auto it = weightedAdj.find(current);
		if (it == weightedAdj.end()) {
			continue;
		}

		// Обновляем расстояния до всех соседей
		for (const auto& neighbor : it->second) {
			int next = neighbor.first;
			double weight = neighbor.second;

			// Пропускаем ребра с бесконечным весом (трубы в ремонте)
			if (isinf(weight)) {
				continue;
			}

			double newDist = distances[current] + weight;

			// Если нашли более короткий путь
			if (newDist < distances[next] - 0.0001) {  // Учет погрешности для double
				distances[next] = newDist;
				predecessors[next] = current;
				pq.push({ newDist, next });
			}
		}
	}

	// Восстанавливаем путь от end к start
	vector<int> path;
	if (isinf(distances[end])) {
		// Путь не найден
		return path;
	}

	// Восстанавливаем путь от конца к началу
	for (int v = end; v != -1; v = predecessors[v]) {
		path.push_back(v);
	}

	// Разворачиваем путь чтобы получить от start к end
	reverse(path.begin(), path.end());

	return path;
}

double GasNetwork::calculateMaxFlow(int sourceKS, int sinkKS,
	const unordered_map<int, Pipe>& pipes) const {
	// Проверяем, что source и sink существуют в графе
	if (sourceKS == sinkKS) {
		cout << "Источник и сток не могут совпадать!" << endl;
		return 0.0;
	}

	// Строим исходный граф с пропускными способностями
	unordered_map<int, vector<pair<int, double>>> originalGraph = getWeightedGraph(pipes, true);

	// Создаем остаточную сеть
	// В остаточной сети для каждого ребра (u,v) с capacity c:
	// - forward edge: остаточная пропускная способность = c - flow
	// - backward edge: остаточная пропускная способность = flow

	// Для простоты создаем полную остаточную сеть
	unordered_map<int, vector<pair<int, double>>> residualGraph;

	// Инициализируем остаточную сеть исходными пропускными способностями
	for (const auto& node : originalGraph) {
		int u = node.first;
		for (const auto& edge : node.second) {
			int v = edge.first;
			double capacity = edge.second;

			// Прямое ребро с исходной пропускной способностью
			residualGraph[u].push_back({ v, capacity });
			// Обратное ребро с 0 пропускной способностью (пока)
			residualGraph[v].push_back({ u, 0.0 });
		}
	}

	// Добавляем изолированные вершины (КС без исходящих соединений)
	// чтобы они были в residualGraph
	for (const auto& connection : connections) {
		const Connection& conn = connection.second;
		int from = conn.getStartKS();
		int to = conn.getEndKS();

		if (residualGraph.find(from) == residualGraph.end()) {
			residualGraph[from] = vector<pair<int, double>>();
		}
		if (residualGraph.find(to) == residualGraph.end()) {
			residualGraph[to] = vector<pair<int, double>>();
		}
	}

	// Массив для восстановления пути
	unordered_map<int, int> parent;
	double maxFlow = 0.0;

	// Основной цикл алгоритма Форда-Фалкерсона
	int iteration = 0;
	const int MAX_ITERATIONS = 1000;  // Защита от бесконечного цикла

	while (bfsForMaxFlow(sourceKS, sinkKS, residualGraph, parent) && iteration < MAX_ITERATIONS) {
		iteration++;

		// 1. Находим минимальную пропускную способность на найденном пути
		double pathFlow = numeric_limits<double>::infinity();

		// Идем от стока к источнику по parent
		for (int v = sinkKS; v != sourceKS; v = parent[v]) {
			int u = parent[v];

			// Ищем ребро u->v в остаточном графе
			bool found = false;
			auto& edges = residualGraph[u];
			for (auto& edge : edges) {
				if (edge.first == v) {
					pathFlow = min(pathFlow, edge.second);
					found = true;
					break;
				}
			}

			if (!found) {
				pathFlow = 0.0;
				break;
			}
		}

		if (pathFlow < 0.0001) {  // Если поток слишком маленький
			break;
		}

		// 2. Обновляем остаточные пропускные способности
		for (int v = sinkKS; v != sourceKS; v = parent[v]) {
			int u = parent[v];

			// Уменьшаем пропускную способность на прямом ребре
			auto& forwardEdges = residualGraph[u];
			for (auto& edge : forwardEdges) {
				if (edge.first == v) {
					edge.second -= pathFlow;
					break;
				}
			}

			// Увеличиваем пропускную способность на обратном ребре
			auto& backwardEdges = residualGraph[v];
			bool backwardFound = false;
			for (auto& edge : backwardEdges) {
				if (edge.first == u) {
					edge.second += pathFlow;
					backwardFound = true;
					break;
				}
			}

			// Если обратное ребро не нашлось - создаем его
			if (!backwardFound) {
				residualGraph[v].push_back({ u, pathFlow });
			}
		}

		// 3. Добавляем поток пути к общему потоку
		maxFlow += pathFlow;

		// Очищаем parent для следующей итерации
		parent.clear();
	}

	if (iteration >= MAX_ITERATIONS) {
		cout << "Предупреждение: достигнуто максимальное количество итераций!" << endl;
	}

	return maxFlow;
}

vector<int> GasNetwork::findShortestPath(int startKS, int endKS,
	const unordered_map<int, Pipe>& pipes) const {
	// Проверка базовых условий
	if (startKS == endKS) {
		cout << "Начальная и конечная КС совпадают!" << endl;
		return vector<int>{startKS};
	}

	// Проверяем, что КС вообще есть в соединениях
	bool startExists = false, endExists = false;
	for (const auto& conn : connections) {
		if (conn.second.getStartKS() == startKS || conn.second.getEndKS() == startKS) {
			startExists = true;
		}
		if (conn.second.getStartKS() == endKS || conn.second.getEndKS() == endKS) {
			endExists = true;
		}
	}

	if (!startExists || !endExists) {
		cout << "Одна или обе КС не подключены к сети!" << endl;
		return vector<int>();
	}

	// Строим взвешенный граф (вес = длина трубы)
	unordered_map<int, vector<pair<int, double>>> weightedGraph = getWeightedGraph(pipes, false);

	// Проверяем, что start и end есть в графе
	if (weightedGraph.find(startKS) == weightedGraph.end()) {
		// Добавляем start как изолированную вершину
		weightedGraph[startKS] = vector<pair<int, double>>();
	}
	if (weightedGraph.find(endKS) == weightedGraph.end()) {
		// Добавляем end как изолированную вершину
		weightedGraph[endKS] = vector<pair<int, double>>();
	}

	// Массивы для расстояний и предшественников
	unordered_map<int, double> distances;
	unordered_map<int, int> predecessors;

	// Запускаем алгоритм Дейкстры
	vector<int> path = dijkstra(startKS, endKS, weightedGraph, distances, predecessors);

	return path;
}

void GasNetwork::showMaxFlowInfo(int sourceKS, int sinkKS,
	const unordered_map<int, Pipe>& pipes,
	const unordered_map<int, KS>& kss) const {
	cout << "\n------РАСЧЕТ МАКСИМАЛЬНОГО ПОТОКА---------" << endl;

	// Проверяем существование КС
	if (kss.find(sourceKS) == kss.end()) {
		cout << "Ошибка: КС-источник с ID " << sourceKS << " не найдена!" << endl;
		return;
	}
	if (kss.find(sinkKS) == kss.end()) {
		cout << "Ошибка: КС-сток с ID " << sinkKS << " не найдена!" << endl;
		return;
	}

	// Проверяем, что есть соединения
	if (connections.empty()) {
		cout << "В сети нет соединений! Максимальный поток = 0" << endl;
		return;
	}

	// Проверяем, что source и sink не совпадают
	if (sourceKS == sinkKS) {
		cout << "Ошибка: источник и сток не могут совпадать!" << endl;
		return;
	}

	// Информация о КС
	cout << "Источник: КС " << sourceKS;
	auto sourceIt = kss.find(sourceKS);
	if (sourceIt != kss.end()) {
		cout << " \"" << sourceIt->second.getName() << "\"";
	}
	cout << endl;

	cout << "Сток: КС " << sinkKS;
	auto sinkIt = kss.find(sinkKS);
	if (sinkIt != kss.end()) {
		cout << " \"" << sinkIt->second.getName() << "\"";
	}
	cout << endl;

	// Показываем информацию о трубах на пути
	cout << "\n--- Информация о трубах в сети ---" << endl;
	bool hasValidPipes = false;
	for (const auto& connection : connections) {
		const Connection& conn = connection.second;
		int pipeId = conn.getPipeId();

		auto pipeIt = pipes.find(pipeId);
		if (pipeIt != pipes.end()) {
			const Pipe& pipe = pipeIt->second;
			cout << "Труба ID " << pipeId << ": ";
			cout << "КС " << conn.getStartKS() << " -> КС " << conn.getEndKS();
			cout << " | Пропускная способность: " << pipe.getCapacity() << " усл. ед.";
			if (pipe.isInRepair()) {
				cout << " [В РЕМОНТЕ]";
			}
			cout << endl;
			hasValidPipes = true;
		}
	}

	if (!hasValidPipes) {
		cout << "Нет доступных труб для транспортировки!" << endl;
		return;
	}

	cout << "\n--- Расчет максимального потока ---" << endl;

	// Рассчитываем максимальный поток
	double maxFlow = calculateMaxFlow(sourceKS, sinkKS, pipes);

	// Интерпретация результата
	cout << "\nРЕЗУЛЬТАТ:" << endl;
	cout << "Максимальный поток: " << maxFlow << " усл. ед." << endl;

	if (maxFlow < 0.0001) {
		cout << "Статус: НЕТ ПУТИ - невозможно доставить газ от источника к стоку" << endl;
		cout << "Возможные причины:" << endl;
		cout << "1. Нет соединяющих путей между КС " << sourceKS << " и КС " << sinkKS << endl;
		cout << "2. Все трубы на пути в ремонте" << endl;
		cout << "3. Пропускная способность всех труб = 0" << endl;
	}
	else if (maxFlow < 1.0) {
		cout << "Статус: МАЛЫЙ ПОТОК - ограниченная пропускная способность" << endl;
	}
	else {
		cout << "Статус: ПУТЬ СУЩЕСТВУЕТ - газ может транспортироваться" << endl;
	}

	cout << "\n==================================" << endl;
}

void GasNetwork::showShortestPathInfo(int startKS, int endKS,
	const unordered_map<int, Pipe>& pipes,
	const unordered_map<int, KS>& kss) const {
	cout << "\n=== КРАТЧАЙШИЙ ПУТЬ ===" << endl;

	// Поиск пути
	vector<int> path = findShortestPath(startKS, endKS, pipes);

	if (path.empty()) {
		cout << "Путь не найден" << endl;
		return;
	}

	// Расчет длины
	double totalLength = 0.0;
	for (size_t i = 0; i < path.size() - 1; ++i) {
		for (const auto& conn : connections) {
			if (conn.second.getStartKS() == path[i] &&
				conn.second.getEndKS() == path[i + 1]) {
				auto pipeIt = pipes.find(conn.second.getPipeId());
				if (pipeIt != pipes.end() && !pipeIt->second.isInRepair()) {
					totalLength += pipeIt->second.getLength();
				}
				break;
			}
		}
	}

	// Вывод
	cout << "Длина: " << totalLength << " км" << endl;
	cout << "Труб: " << path.size() - 1 << endl;
	cout << "\nМаршрут:" << endl;

	for (size_t i = 0; i < path.size(); ++i) {
		cout << "КС" << path[i];
		if (i < path.size() - 1) {
			cout << " -> ";
		}
	}
	cout << endl;
}