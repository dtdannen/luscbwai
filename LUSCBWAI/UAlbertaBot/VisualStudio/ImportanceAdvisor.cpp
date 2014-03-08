#include "ImportanceAdvisor.h"
#include <queue>
#include <boost/foreach.hpp>

int* centroid();
std::list<int> getNodesWithColor(NodeColor);
int distance(int*, int*);
int dijkstra(int,int);
double min(std::list<int>,double*);

ImportanceAdvisor ImportanceAdvisor::Instance() {
	if (instance == NULL) {
		instance = new ImportanceAdvisor();
	}
	return *instance;
}

ImportanceAdvisor::ImportanceAdvisor() {
	update();
}

void ImportanceAdvisor::update() {
	std::list<int> ourBases = getNodesWithColor(NodeColor::GREEN);
	std::list<int> theirBases = getNodesWithColor(NodeColor::RED);

	assert(ourBases.size()>0);

	ourCentroid = centroid(ourBases);
	theirCentroid = centroid(theirBases);

	for (int i = 0; i < ColorGraph::Instance().size(); i++) {
		update(i);
	}
}

void ImportanceAdvisor::update(int nodeId){
	BWAPI::Position node = ColorGraph::Instance().getNodeCenter(nodeId);
	int center[2];
	center[0] = node.x();
	center[1] = node.y();

	double triangleDistance = 1.0 / (distance(center,theirCentroid) + distance(center,ourCentroid));
	double nodeDistance = 1.0 / dijkstra(nodeId,ColorGraph::Instance().getNodeAtPosition(theirCentroid));
	ColorGraph::Instance().setNodeImportance(nodeId, triangleDistance+nodeDistance);
}

int* centroid(std::list<int> bases) {
	int totalx;
	int totaly;

	for (std::list<int>::iterator it=bases.begin(); it != bases.end(); ++it) {
		totalx += ColorGraph::Instance().getNodeCenter(*it).x();
		totaly += ColorGraph::Instance().getNodeCenter(*it).y();
	}

	int result[2];
	result[0] = totalx/bases.size();
	result[1] = totaly/bases.size();
	return result;
}

std::list<int> getNodesWithColor(NodeColor c) {
	std::list<int> result = *new std::list<int>();
	for (int i = 0; i < ColorGraph::Instance().size(); i++) {
		if (ColorGraph::Instance().getNodeColor(i) == c) {
			result.push_back(i);
		}
	}
	return result;
}

int distance(int* a, int* b) {
	return sqrt( pow((double) b[0] - a[0],2) + pow((double) b[1]-a[1],2));
}

int dijkstra(int startNode, int endNode) {
	int graphSize = ColorGraph::Instance().size();
	double* distance = new double[graphSize];
	for (int i = 0; i < graphSize; i++) {
		if (i = startNode) {
			distance[i] = 0;
		}
		else {
			distance[i] = DBL_MAX;
		}
	}
	std::list<int> q = *new std::list<int>();

	for (int i = 0; i < graphSize; i++) {
		q.push_back(i);
	}

	while (!q.empty()) {
		int minNode = min(q,distance);
		q.remove(minNode);
		if (distance[minNode] == DBL_MAX) {
			break;
		}

		BOOST_FOREACH(int i, ColorGraph::Instance().getNodeNeighbors(minNode)) {
			int minCenter[2];
			int neighborCenter[2];

			minCenter[0] = ColorGraph::Instance().getNodeCenter(minNode).x();
			minCenter[1] = ColorGraph::Instance().getNodeCenter(minNode).y();
			neighborCenter[0] = ColorGraph::Instance().getNodeCenter(i).x();
			neighborCenter[1] = ColorGraph::Instance().getNodeCenter(i).y();

			double alt = distance[minNode] + distance(minCenter,neighborCenter);
			if (alt < distance[i]) {
				distance[i] = alt;
			}
		}

		return distance[endNode];

}