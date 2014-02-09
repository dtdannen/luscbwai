#pragma once
#include <Common.h>
enum NodeColor { GREEN, ORANGE, RED, BLACK }; // No more yellow!

class ColorNode
{

	int id;
	int lastUpdated;
	double importance;
	NodeColor color;
	std::list<int> neighbors;
	BWAPI::Position center;

	void updateAge();

public:
	ColorNode(int Id);
	~ColorNode(void);

	// getters
	int getId();
	int getLastFrameUpdated();
	double getImportance();
	NodeColor getColor();
	std::list<int> getNeighbors();
	BWAPI::Position getCenter();

	// setters
	void setColor(NodeColor nodeColor);
	void addNeighbor(int neighborId);
	void setCenter(BWAPI::Position regionCenter);
	void setImportance(double imp);
};

