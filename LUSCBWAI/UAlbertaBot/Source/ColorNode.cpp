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
	std::list<int> temp = std::list<int>(neighbors);
	return temp;
}
	
BWAPI::Position ColorNode::getCenter()
{
	return center;
}

double ColorNode::getImportance()
{
	return importance;
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
	center = BWAPI::Position(regionCenter.x(), regionCenter.y());
	updateAge();
}

void ColorNode::updateAge()
{
	lastUpdated = BWAPI::Broodwar->getFrameCount();
}

void ColorNode::setImportance(double imp)
{
	importance = imp;
	updateAge();
}

ColorNode::~ColorNode(void)
{
}
