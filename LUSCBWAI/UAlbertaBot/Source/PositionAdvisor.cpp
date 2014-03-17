#include "PositionAdvisor.h"

BWAPI::Position PositionAdvisor::getPosition(BWTA::Region* region, BWAPI::Unit* unit, BWAPI::Position defendedPosition) {
	//Step 1: get list of tiles

	std::vector<BWAPI::Position> tiles = PositionAdvisor::getTileArc(region, defendedPosition, unit->getInitialType().groundWeapon().maxRange()-RANGE_FUDGE_FACTOR);

	//Step 2: for the list of tiles, evaluate the score of each
	std::vector<double> scores = *(new std::vector<double>());
	for (unsigned int i = 0; i < tiles.size(); i++) {
		scores.push_back(PositionAdvisor::evaluateScore(tiles[i]));
	}

	//Step 3: Get the highest score:

	int max = 0;
	for (unsigned int i = 1; i < tiles.size(); i++) {
		if (scores[i]>scores[max]) {
			max = i;
		}
	}

	return tiles[max];
}

std::vector<BWAPI::Position> PositionAdvisor::getTileArc(const BWTA::Region* region, const BWAPI::Position defendedPosition, const int distance) {

	std::vector<BWAPI::Position> circle = getCircle(defendedPosition,distance);
	std::vector<BWAPI::Position>* result = new std::vector<BWAPI::Position>();
	for (std::vector<BWAPI::Position>::iterator i = circle.begin(); i != circle.end(); ++i) {
		if ((*region).getPolygon().isInside(*i))
			result->push_back((*i));
	}

	return *result;
}

double PositionAdvisor::getDistance(BWAPI::Position x, BWAPI::Position y) {
	double x1 = x.x();
	double y1 = x.y();
	double x2 = y.x();
	double y2 = y.y();

	double root = sqrt( pow((x1-x2),2) + pow((y1-y2),2));
	return root;
}

std::vector<BWAPI::Position> PositionAdvisor::getCircle(const BWAPI::Position point, int radius) {
	int x0 = point.x();
	int y0 = point.y();
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x = 0;
	int y = radius;
	std::vector<BWAPI::Position>* result = new std::vector<BWAPI::Position>();
	
	result->push_back(*new BWAPI::Position(x0, y0 + radius));
	result->push_back(*new BWAPI::Position(x0, y0 - radius));
	result->push_back(*new BWAPI::Position(x0 + radius, y0));
	result->push_back(*new BWAPI::Position(x0 - radius, y0));
	
	while(x < y)
	{
		if(f >= 0) 
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x=x+1;
		ddF_x += 2;
		f += ddF_x;    
		result->push_back(*new BWAPI::Position(x0 + x, y0 + y));
		result->push_back(*new BWAPI::Position(x0 - x, y0 + y));
		result->push_back(*new BWAPI::Position(x0 + x, y0 - y));
		result->push_back(*new BWAPI::Position(x0 - x, y0 - y));
		result->push_back(*new BWAPI::Position(x0 + y, y0 + x));
		result->push_back(*new BWAPI::Position(x0 - y, y0 + x));
		result->push_back(*new BWAPI::Position(x0 + y, y0 - x));
		result->push_back(*new BWAPI::Position(x0 - y, y0 - x));
	}
}

double PositionAdvisor::evaluateScore(const BWAPI::Position tile) {
	double finalScore = 0;

	//IMPLEMENTED SCORING FACTORS:
	//1.Distance from nearest attacking unit
	finalScore+=distanceFromNearestUnitScore(tile);

	//TODO: More factors!
	return distanceFromNearestUnitScore(tile);
}
	
double PositionAdvisor::distanceFromNearestUnitScore(const BWAPI::Position point) {
	double dist = DBL_MAX;
	for(std::set<BWAPI::Unit*>::const_iterator i=BWAPI::Broodwar->self()->getUnits().begin();i!=BWAPI::Broodwar->self()->getUnits().end();i++) {
		BWAPI::UnitType t = (*i)->getType();
		if (t==BWAPI::UnitTypes::Terran_Firebat || t==BWAPI::UnitTypes::Terran_Ghost || t==BWAPI::UnitTypes::Terran_Goliath ||
			t==BWAPI::UnitTypes::Terran_Marine || t==BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode || t==BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode || 
			t==BWAPI::UnitTypes::Terran_Vulture) {
				BWAPI::Position unitPosition = (*i)->getPosition();
				double unitDist = getDistance(point,unitPosition);
				if (unitDist<dist)
					dist=unitDist;
		}
	}

	//Calculate Score. For equation see the comment in the header file
	if (dist>500)
		return 0;
	else
		return ceil(-(1.0/5.0)*dist+100);
}