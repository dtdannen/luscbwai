#include "ChokePriority.h"

ChokePriority::ChokePriority(BWTA::Chokepoint* chokepoint, int priority, ChokePointAdvisor* advisor) {
	this->chokepoint = chokepoint;
	this->priority = priority;
	this->squad = new Squad(std::map<BWAPI::UnitType*, int>(), advisor);
	this->advisor = advisor;
}

ChokePointAdvisor* ChokePriority::getAdvisor() {
	return this->advisor;
}

void ChokePriority::setSquad(Squad* squad) {
	this->squad = squad;
}

BWTA::Chokepoint* ChokePriority::getChokepoint() {
	return this->chokepoint;
}

int ChokePriority::getPriority() {
	return this->priority;
}

Squad* ChokePriority::getSquad() {
	return this->squad;
}