#pragma once
#include <Common.h>
enum NodeColor { GREEN, YELLOW, RED, BLACK };

class ColorNode
{

	int id;
	int lastUpdated;
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
	NodeColor getColor();
	std::list<int> getNeighbors();
	BWAPI::Position getCenter();

	// setters
	void setColor(NodeColor nodeColor);
	void addNeighbor(int neighborId);
	void setCenter(BWAPI::Position regionCenter);
};

