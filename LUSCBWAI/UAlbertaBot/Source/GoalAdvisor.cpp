#include "GoalAdvisor.h"

class ColorNode;

int getSmallest(std::list<ColorNode*>,std::map<int,double>);
double distanceBetweenCenters(int,int);
int smallestNonGreenNode(std::map<int,double>);

int GoalAdvisor::getGoalRegion() {
	return goalNode;
}

GoalAdvisor::GoalAdvisor() {
	BWAPI::Player* p = BWAPI::Broodwar->self();
	BWAPI::Position startLocation = BWAPI::Position(p->getStartLocation());
	originNode = ColorGraph::Instance().getNodeAtPosition(startLocation);
}

GoalAdvisor::~GoalAdvisor(void)
{
}

void GoalAdvisor::update() {

	//calculate distance w/ dijkstra
	int size = ColorGraph::Instance().size();
	std::map<int,double> distance_map;

	std::list<int> q;

	distance_map[originNode]=0;
	q.push_back(originNode);

	while(!q.empty()) {
		int current = getSmallest(q, distance_map);
		q.remove(current);

		std::list<int> neighbors = ColorGraph::Instance().getNodeNeighbors(current);
		for each (int neighbor in neighbors)
		{
			if (distance_map.find(neighbor)==distance_map.end())
			{
				distance_map[neighbor] = DBL_MAX;
			}
			double alt = distance_map[current] + distanceBetweenCenters(current, neighbor);
			
			if (alt < distance_map[neighbor])
			{
				distance_map[neighbor] = alt;
				q.push_back(neighbor);
			}
		}
	}

	goalNode = smallestNonGreenNode(distance_map);
}

int GoalAdvisor::getSmallest(std::list<int> list, std::map<int,double> map) {
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

double GoalAdvisor::distanceBetweenCenters(int one, int two) {
	BWAPI::Position oneCenter = ColorGraph::Instance().getNodeCenter(one);
	BWAPI::Position twoCenter = ColorGraph::Instance().getNodeCenter(two);

	return (pow((double)(twoCenter.y() - oneCenter.x()), 2) + pow((double)(twoCenter.y() - oneCenter.y()), 2));
}

int GoalAdvisor::smallestNonGreenNode(std::map<int,double> map) {
	std::list<int> nonGreenNodes;

	std::map<int,double>::const_iterator it;
	
	for(it = map.begin(); it!=map.end(); ++it) {
		int current = it->first; //std::pair is (not so) weird
		if (ColorGraph::Instance().getNodeColor(current)!=NodeColor::GREEN) {
			nonGreenNodes.push_back(current);
		}
	}

	//instead of directly returning, just place it in a variable so we can clear the memory used for nonGreenNodes
	int result = getSmallest(nonGreenNodes,map);
	return result;
}

GoalAdvisor & GoalAdvisor::Instance() 
{
	static GoalAdvisor instance;
	return instance;
}
