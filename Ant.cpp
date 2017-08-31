#include "Ant.h"

Ant::Ant(const Location& loc)
{
	m_loc = loc;
	setValid();
	m_order.reset();
}


Ant::~Ant()
{
	setValid(false);
}

void Ant::setLocation(const Location& locAnt)
{
	m_loc = locAnt;
}
