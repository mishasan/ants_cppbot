#include "Map.h"

Map::Map()
{

}

Map::~Map()
{

}

Map& Map::map()
{
	static Map mapInstance;
	return mapInstance;
}
