#pragma once

#include <vector>
#include "Location.h"
#include <fstream>

class State;
enum class AntDirection;

class PathFinder
{
public:

	PathFinder();
	~PathFinder();

	bool Init();

	bool findPath(const Location& locFrom, const Location& locTo, std::vector<AntDirection>& path);
	
	void printDirMap() const;
	void printPathAndMap(const Location& locFrom, const std::vector<AntDirection>& vPath) const;

private:
	std::vector<int> m_closedNodesMap, m_openNodesMap;
	std::vector<AntDirection> m_dirMap;
	
	int m_rows, m_cols;

	inline int At(const Location& loc) const {return (loc.row * m_cols) + loc.col; }
};

class Node
{
private:
	Location m_Location;
	int m_TraveledDistance;
	int priority;  // traveled distance + estimated remaining distance -> smaller: higher priority

public:
	Node(const Location& loc, int travDist, int prio) 
		: m_Location(loc), m_TraveledDistance(travDist), priority(prio) {}

	const Location& getLocation() const { return m_Location; }
	int getTraveledDistance() const { return m_TraveledDistance; }
	int getPriority() const { return priority; }

	// necessary operator for priority queue: smallest is the one with highest priority
	bool operator<(const Node& n) const { return this->getPriority() > n.getPriority(); } 

	void updatePriority(const Location& loc)
	{
		priority = m_TraveledDistance + estimate(loc) * 10;
	}

	void increaseDistance()
	{
		m_TraveledDistance += 10;
	}

	// Estimates the remaining distance
	int estimate(const Location& loc) const
	{
		// for now: euclidean distance 
		return static_cast<int>(Location::distance(m_Location, loc));
	}
};