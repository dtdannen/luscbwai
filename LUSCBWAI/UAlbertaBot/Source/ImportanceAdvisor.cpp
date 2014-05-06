#include "ImportanceAdvisor.h"
#include <queue>
#include <boost/foreach.hpp>

int* centroid(std::vector<int>);
std::vector<int> getNodesWithColor(NodeColor);
double distance(int*, int*);
double dijkstra(int,int);
int min(std::vector<int>,double*);

ImportanceAdvisor & ImportanceAdvisor::Instance() {
	static ImportanceAdvisor instance;
	return instance;
}

ImportanceAdvisor::ImportanceAdvisor(void) {
	update();
}

void ImportanceAdvisor::update() {
	std::vector<int> ourBases = getNodesWithColor(NodeColor::GREEN);
	std::vector<int> theirBases = getNodesWithColor(NodeColor::RED);

	assert(ourBases.size()>0);

	ourCentroid = centroid(ourBases);
	if (theirBases.size()>0) {
		theirCentroid = centroid(theirBases);
	}
	else {
		theirCentroid = new int[2];
		theirCentroid[0] = 0;
		theirCentroid[1] = 1;
	}

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
	//double nodeDistance = 1.0 / dijkstra(nodeId,ColorGraph::Instance().getNodeAtPosition(BWAPI::Position(theirCentroid[0],theirCentroid[1])));
	ColorGraph::Instance().setNodeImportance(nodeId, triangleDistance);
	BWAPI::Broodwar->printf("Setting %d to %f",nodeId,triangleDistance);
}

int* centroid(std::vector<int> bases) {
	int totalx;
	int totaly;
	int *result = new int[2];

	if (bases.size() == 0) {
		result[0] = 0;
		result[1] = 0;
		return result;
	}

	for (std::vector<int>::iterator it=bases.begin(); it != bases.end(); ++it) {
		totalx += ColorGraph::Instance().getNodeCenter(*it).x();
		totaly += ColorGraph::Instance().getNodeCenter(*it).y();
	}
	result[0] = totalx/bases.size();
	result[1] = totaly/bases.size();
	return result;
}

std::vector<int> getNodesWithColor(NodeColor c) {
	std::vector<int> result = *new std::vector<int>();
	for (int i = 0; i < ColorGraph::Instance().size(); i++) {
		if (ColorGraph::Instance().getNodeColor(i) == c) {
			result.push_back(i);
		}
	}
	return result;
}

double distance(int* a, int* b) {
	return sqrt( pow((double) b[0] - a[0],2) + pow((double) b[1]-a[1],2));
}

double dijkstra(int startNode, int endNode) {
	int graphSize = ColorGraph::Instance().size();
	double* dist = new double[graphSize];
	for (int i = 0; i < graphSize; i++) {
		if (i = startNode) {
			dist[i] = 0;
		}
		else {
			dist[i] = DBL_MAX;
		}
	}
	std::vector<int> q = *new std::vector<int>();

	for (int i = 0; i < graphSize; i++) {
		q.push_back(i);
	}

	while (!q.empty()) {
		int minNode = min(q,dist);
		q.erase(q.begin() + minNode);
		if (dist[minNode] == DBL_MAX) {
			break;
		}

		BOOST_FOREACH(int i, ColorGraph::Instance().getNodeNeighbors(minNode)) {
			int minCenter[2];
			int neighborCenter[2];

			minCenter[0] = ColorGraph::Instance().getNodeCenter(minNode).x();
			minCenter[1] = ColorGraph::Instance().getNodeCenter(minNode).y();
			neighborCenter[0] = ColorGraph::Instance().getNodeCenter(i).x();
			neighborCenter[1] = ColorGraph::Instance().getNodeCenter(i).y();

			double alt = dist[minNode] + distance(minCenter,neighborCenter);
			if (alt < dist[i]) {
				dist[i] = alt;
			}
		}
	}

	return dist[endNode];
}

int min(std::vector<int> q, double* dist) {
	int min = q[0];
	double minDist = dist[min];

	for (int i = 1; i < q.size(); i++) {
		if (dist[q[i]] < minDist) {
			min = i;
			minDist = dist[q[i]];
		}
	}

	return min;
}