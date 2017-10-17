#include "PathFinder.h"

#include <queue>

#include "Location.h"
#include "State.h"
#include "Map.h"
#include <iomanip>

using namespace std;

PathFinder::PathFinder()
{

}


PathFinder::~PathFinder()
{

}

bool PathFinder::Init()
{
	m_rows = Map::map().rows();
	m_cols = Map::map().cols();
	int gridElements = m_rows * m_cols;

	try
	{
		m_openNodesMap.resize(gridElements);
		m_closedNodesMap.resize(gridElements);
		m_dirMap.resize(gridElements);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

bool PathFinder::findPath(const Location& locFrom, const Location& locTo, std::vector<AntDirection>& path)
{
	priority_queue<Node> pq[2]; // list of open (not-yet-tried) nodes
	unsigned int pqi = 0; // pq index to switch between working queue's, for shifting and removing nodes

	// reset the node maps
	std::fill(m_openNodesMap.begin(), m_openNodesMap.end(), 0);
	std::fill(m_closedNodesMap.begin(), m_closedNodesMap.end(), 0);

	// create start node and push into list of open nodes
	Node nodeStart(locFrom, 0, 0);
	nodeStart.updatePriority(locTo);
	pq[pqi].push(nodeStart);
	m_openNodesMap[At(locFrom)] = nodeStart.getPriority(); // mark it on the open nodes map

	// A* search
	while(!pq[pqi].empty())
	{
		// get node with highest priority from the list of open nodes
		const Node nodeParent(pq[pqi].top());
		pq[pqi].pop(); // remove the node from the open list

		m_openNodesMap[At(nodeParent.getLocation())] = 0;

		// put it on the closed nodes map
		m_closedNodesMap[At(nodeParent.getLocation())] = 1;

		// quit searching when the goal position is reached
		if(nodeParent.getLocation() == locTo) 
		{
			// generate the path from finish to start by following the directions
			Location p(locTo);
			while(p != locFrom)
			{
				AntDirection d = m_dirMap[At(p)];
				path.push_back(d);

				//	Directions are saved from Start to End, but to find the best path, go from End to Start, so invert saved directions
				AntDirection dCounter = Location::getCounterDirection(d);
				p = Location::getLocation(p, dCounter);
			}
			reverse(path.begin(), path.end()); //	reverse because directions are added to end of path vector

			// empty the leftover nodes
			while(!pq[pqi].empty())
			{
				pq[pqi].pop();
			}

			break;
		}

		// generate moves (child nodes) in all possible directions
		for(auto d : AllAntDirections)
		{
			Location locChild(Location::getLocation(nodeParent.getLocation(), d));
			if(Map::map()(locChild).IsLand() && m_closedNodesMap[At(locChild)] != 1)
			{
				// generate a child node
				Node nodeChild(locChild, nodeParent.getTraveledDistance(), nodeParent.getPriority());
				nodeChild.increaseDistance();
				nodeChild.updatePriority(locTo);

				// if it is not in the open list then add into that
				if(m_openNodesMap[At(locChild)] == 0)
				{
					m_openNodesMap[At(locChild)] = nodeChild.getPriority();
					pq[pqi].push(nodeChild);

					// remember where this comes from
					m_dirMap[At(locChild)] = d;
				}
				//	if this position is cheaper to get to, update position
				else if(m_openNodesMap[At(locChild)] > nodeChild.getPriority())
				{
					// update cheaper priority
					m_openNodesMap[At(locChild)] = nodeChild.getPriority();

					// remember where this comes from
					m_dirMap[At(locChild)] = d;
					
					//	TODO: still gross, juggling items from one pq to another. No need to sort in second pq again, could just be a vector
					// remove node from pq, by shifting everything else atop of it to second priority queue
					while(pq[pqi].top().getLocation() != locChild)
					{                
						pq[1-pqi].push(pq[pqi].top());
						pq[pqi].pop();       
					}
					pq[pqi].pop(); // remove wanted node

					// empty the larger size pq to the smaller one
					if(pq[pqi].size() > pq[1-pqi].size())
					{
						pqi = 1 - pqi;
					}
					while(!pq[pqi].empty())
					{                
						pq[1-pqi].push(pq[pqi].top());
						pq[pqi].pop();       
					}
					pqi = 1 - pqi;

					pq[pqi].push(nodeChild); // add the better node instead
				}
			}
		}
	}

	return !path.empty();
}

//	TODO: there is no NIL AntDirection, default is N, makes it hard to tell which position really was visited
void PathFinder::printDirMap() const
{
#ifdef _DEBUG
	Bug::bug() << "Dir Map:" << std::endl;
	for(int r = 0; r < m_rows; ++r)
	{
		Bug::bug() << std::setw(3) << std::fixed << r << "   ";
		for (int c = 0; c < m_cols; c++)
		{
			const bool bIsWater = Map::map()[r][c].IsWater();
			if(bIsWater)
				Bug::bug() << "%";
			else
			{
				const AntDirection& d = m_dirMap[At(Location(r, c))];
				Bug::bug() << d;
			}
		}
		Bug::bug() << std::endl;
	}
	Bug::bug() << std::endl;
#endif // _DEBUG
}

void PathFinder::printPathAndMap(const Location& locFrom, const vector<AntDirection>& vPath) const
{
#ifdef _DEBUG
	vector<char> vMap;
	vMap.resize(m_rows * m_cols);

	//	save Map related information first 
	for(int r = 0; r < m_rows; ++r)
	{
		for (int c = 0; c < m_cols; c++)
		{
			vMap[At(Location(r, c))] = Map::map()[r][c].toChar();
		}
	}

	//	run Path from beginning to end and overwrite Map Information
	Location locRun(locFrom);
	vMap[At(locRun)] = 'B';	// Beginning : B
	for(AntDirection d : vPath)
	{
		locRun = Location::getLocation(locRun, d);
		vMap[At(locRun)] = 'X';
	}
	vMap[At(locRun)] = 'F';	// Finish : F

	//	print Map and Path
	Bug::bug() << "Map and Path:" << std::endl;
	for(int r = 0; r < m_rows; ++r)
	{
		Bug::bug() << std::setw(3) << std::fixed << r << "   ";
		for (int c = 0; c < m_cols; c++)
		{
			Bug::bug() << vMap[At(Location(r, c))];
		}
		Bug::bug() << std::endl;
	}
	Bug::bug() << std::endl;
#endif // _DEBUG
}
