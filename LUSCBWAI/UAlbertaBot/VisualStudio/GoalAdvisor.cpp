#include "GoalAdvisor.h"

class ColorNode;

ColorNode* getSmallest(std::list<ColorNode*>,std::map<int,double>);
double distanceBetweenCenters(int,int);
ColorNode* smallestNonGreenNode(std::map<int,double>);

ColorNode* GoalAdvisor::getGoalRegion() {
	return goalNode;
}

GoalAdvisor::GoalAdvisor() {
	BWAPI::Player* p = BWAPI::Game::self();
	BWAPI::Position* startLocation = p.getStartLocation();
	originNode = ColorGraph::Instance().getNodeById(ColorGraph::Instance().getNodeAtPosition(startLocation));
	GoalAdvisor::update();
}

void GoalAdvisor::update() {

	//calculate distance w/ dijkstra
	int size = ColorGraph::Instance().size();
	std::map<int,double> distance_map = * new std::map<int,double>();
	std::map<int,bool> visited_map = * new std::map<int,bool>();

	std::list<ColorNode*> q = *new std::list<ColorNode*>;

	distance_map[originNode->getId()]=0;
	q.push_back(originNode);

	while(!q.empty()) {
		ColorNode * current = getSmallest(q,distance_map);
		q.remove(current);
		visited_map[current]=true;

		std::list<int> neighbors = current->getNeighbors();
		for (int i = 0; i < neighbors.size(); i++) {
			double alt = distance_map[current] + distanceBetweenCenters(current->getId(),neighbors[i]);
			//if distance map contains i and is bigger than the current calculated distance, and we have not visited i yet
			if (distance_map.find(i)!=distance_map.end() && alt < distance_map[i] && !visited_map[i]) {
				distance_map[i] = alt;
				q.push_back(ColorGraph::Instance().getNodeById(neighbors[i]));
			}
		}

	}

	goalNode = smallestNonGreenNode(distance_map);

	delete &distance_map;
	delete &visited_map;
	delete &q;
}

ColorNode* getSmallest(std::list<ColorNode*> list, std::map<int,double> map) {
	std::list<ColorNode*>::const_iterator it;

	double smallest_distance = DBL_MAX;
	ColorNode* smallest = NULL;

	for (it = list.begin(); it!=list.end(); ++it) {
		int id = (*it)->getId();
		double dist = map[id];
		if (dist<smallest_distance) {
			smallest_distance=dist;
			smallest = *it;
		}

	}

	return smallest;
}

double distanceBetweenCenters(int one, int two) {
	ColorNode* one = ColorGraph::Instance().getNodeById(one);
	ColorNode* two = ColorGraph::Instance().getNodeById(two);

	BWAPI::Position oneCenter = one.getCenter();
	BWAPI::Position twoCenter = two.getCenter();

	int* oneCoords = {oneCenter.x(), oneCenter.y()};
	int* twoCoords = {twoCenter.x(), twoCenter.y()};

	return sqrt(pow(twoCoords[0]-oneCoords[0],2)+pow(twoCoords[1]-oneCoords[1],2));
}

ColorNode* smallestNonGreenNode(std::map<int,double> map) {
	std::list<ColorNode*> nonGreenNodes = * new std::list<ColorNode*>();

	std::map<int,double>::const_iterator it;
	
	for(it = map.begin(), it!=map.end(); ++it) {
		ColorNode* current = ColorGraph::Instance().getNodeById(std::get<0>(*it)); //std::pair is weird
		if (current->getColor()!=NodeColor::GREEN) {
			nonGreenNodes.push_back(current);
		}
	}

	//instead of directly returning, just place it in a variable so we can clear the memory used for nonGreenNodes
	ColorNode* result = getSmallest(nonGreenNodes,map);
	delete &nonGreenNodes;
	return result;
}