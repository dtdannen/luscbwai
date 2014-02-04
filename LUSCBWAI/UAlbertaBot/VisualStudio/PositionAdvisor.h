#include "BWTA/Region.h"
#include "BWAPI.h"
#include <math.h>

class PositionAdvisor {
	public:
		const int RANGE_FUDGE_FACTOR = 2;
		/**
		* Author: Steven Stinson
		* Description: Returns the best scoring position on an arc with radius slightly smaller than unit's range
		*              in region to defend defendedPosition. "Defend" can also mean seige, etc. Basically when you
					   want to point tanks at something this is what you'd use.
		*/
		static BWAPI::Position getPosition(BWTA::Region* region, BWAPI::Unit* unit, BWAPI::Position defendedPosition);

	private:
		/*
		* Author: Steven Stinson
		* Description: Returns an array of TilePositions (i.e. Tiles) in an arc distance away from point in region, also feeds out the number
		*              of tiles into numberOfTiles (necessary for usage of the array in C++).
		*/
		static std::vector<BWAPI::Position> getTileArc(const BWTA::Region* region, const BWAPI::Position point, const int distance);

		/*
		* Author: Steven Stinson
		* Description: Returns an integer representing the "score" of the given tile. This score is very modular, and the factors feeding into
		*              it can vary greatly and can be recalculated with machine learning potentially.
		*/
		static int evaluateScore(const BWAPI::Position tile);

		/*
		* Author: Steven Stinosn
		* Description: Returns the distance between two BWAPI Position objects.
		*/
		static double getDistance(BWAPI::Position x, BWAPI::Position y);

		/**
		* Author: Steven Stinson
		* Description: Returns a vector of positions representing a circle of radius radius from Position position.
		*              Algorithm from http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
		*/
		static std::vector<BWAPI::Position> getCircle(const BWAPI::Position point, int radius);

		/**
		* Author: Steven Stinson
		* Description: Returns an int calculated based on the distance of the nearest attacking unit to the
		*              current unit. This is represented as a piecewise function:
		*              >500px: 0
		*			   0<x<500px: score=(-1/5)*dist+100
		*/
		static int distanceFromNearestUnitScore(const BWAPI::Position point);
};