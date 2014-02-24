//Author: Steven Stinson
//Description: Imagine a triangle with the three corners being the centroids of each player's base, and the third point being the center of a node.
//			   The importance of the initial runthrough is the normalized sum of the two lines from each centroid to the center of the node. This is
//             then adjusted based on game information (i.e. if something becomes contested, it is more important, and if it is less contested, then
//			   we decrease the importance by some constant).

#include "ColorGraph.h"

#pragma once
class ImportanceAdvisor {
private:
	static const double IMOPRTANCE_ADJUSTMENT = 0.05;
	ImportanceAdvisor();
	static ImportanceAdvisor* instance;
	int* ourCentroid;
	int* theirCentroid;
	int maxDistance;

public:
	static ImportanceAdvisor Instance();
	void update(int nodeId);
	void update();
};