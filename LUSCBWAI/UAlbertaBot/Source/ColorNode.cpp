#include "ColorNode.h"

ColorNode::ColorNode(int Id)
{
	id = Id;
}

int ColorNode::getId()
{
	return id;
}

int ColorNode::getLastFrameUpdated()
{
	return lastUpdated;
}

NodeColor ColorNode::getColor()
{
	return color;
}

std::list<int> ColorNode::getNeighbors()
{
	std::list<int> temp = std::list<int>(neighbors); // WOOOOOO
	return temp;
}
	
BWAPI::Position ColorNode::getCenter()
{
	return center;
}

void ColorNode::setColor(NodeColor nodeColor)
{
	color = nodeColor;
	updateAge();
}

void ColorNode::addNeighbor(int neighborId)
{
	neighbors.push_back(neighborId);
	updateAge();
}

void ColorNode::setCenter(BWAPI::Position regionCenter)
{
	center = regionCenter;
	updateAge();
}

void ColorNode::updateAge()
{
	lastUpdated = BWAPI::Broodwar->getFrameCount();
}

ColorNode::~ColorNode(void)
{
}
