#include "ImportanceAdvisor.h"

int* centroid();
std::list<int> getNodesWithColor(NodeColor);
int distance(int*, int*);

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

	double imp = 1.0 / ( (distance(center,theirCentroid) + distance(center,ourCentroid)) / maxDistance);
	ColorGraph::Instance().setNodeImportance(nodeId,imp);
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