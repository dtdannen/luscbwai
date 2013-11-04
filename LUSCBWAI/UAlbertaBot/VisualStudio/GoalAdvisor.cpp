#include "GoalAdvisor.h"

class ColorNode;

int getSmallest(std::list<ColorNode*>,std::map<int,double>);
double distanceBetweenCenters(int,int);
int smallestNonGreenNode(std::map<int,double>);

int GoalAdvisor::getGoalRegion() {
	return goalNode;
}

GoalAdvisor::GoalAdvisor() {
	BWAPI::Player* p = BWAPI::Game::self();
	BWAPI::Position* startLocation = p.getStartLocation();
	originNode = ColorGraph::Instance().getNodeAtPosition(startLocation);
	GoalAdvisor::update();
}

void GoalAdvisor::update() {

	//calculate distance w/ dijkstra
	int size = ColorGraph::Instance().size();
	std::map<int,double> distance_map = * new std::map<int,double>();
	std::map<int,bool> visited_map = * new std::map<int,bool>();

	std::list<int> q = *new std::list<int>;

	distance_map[originNode->getId()]=0;
	q.push_back(originNode);

	while(!q.empty()) {
		int current = getSmallest(q,distance_map);
		q.remove(current);
		visited_map[current]=true;

		std::list<int> neighbors = ColroGraph::Instance().getNodeNeighbors(current);
		for (int i = 0; i < neighbors.size(); i++) {
			double alt = distance_map[current] + distanceBetweenCenters(current,neighbors[i]);
			//if distance map contains i and is bigger than the current calculated distance, and we have not visited i yet
			if (distance_map.find(i)!=distance_map.end() && alt < distance_map[i] && !visited_map[i]) {
				distance_map[i] = alt;
				q.push_back(neighbors[i]);
			}
		}

	}

	goalNode = smallestNonGreenNode(distance_map);

	delete &distance_map;
	delete &visited_map;
	delete &q;
}

int getSmallest(std::list<int> list, std::map<int,double> map) {
	std::list<int>::const_iterator it;

	double smallest_distance = DBL_MAX;
	int smallest = -1;

	for (it = list.begin(); it!=list.end(); ++it) {
		int id = *it;
		double dist = map[id];
		if (dist<smallest_distance) {
			smallest_distance=dist;
			smallest = id;
		}

	}

	return smallest;
}

double distanceBetweenCenters(int one, int two) {
	BWAPI::Position oneCenter = ColorGraph::Instance().getNodeCenter(one);
	BWAPI::Position twoCenter = ColorGraph::Instance().getNodeCenter(two);

	int* oneCoords = {oneCenter.x(), oneCenter.y()};
	int* twoCoords = {twoCenter.x(), twoCenter.y()};

	return sqrt(pow(twoCoords[0]-oneCoords[0],2)+pow(twoCoords[1]-oneCoords[1],2));
}

int smallestNonGreenNode(std::map<int,double> map) {
	std::list<int> nonGreenNodes = * new std::list<int>();

	std::map<int,double>::const_iterator it;
	
	for(it = map.begin(), it!=map.end(); ++it) {
		int current = std::get<0>(*it); //std::pair is weird
		if (ColorGraph::Instance().getNodeColor(current)!=NodeColor::GREEN) {
			nonGreenNodes.push_back(current);
		}
	}

	//instead of directly returning, just place it in a variable so we can clear the memory used for nonGreenNodes
	int result = getSmallest(nonGreenNodes,map);
	delete &nonGreenNodes;
	return result;
}
