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

Location Ant::getNewLocation() const
{
	if(m_order.getOrderType() == Order::OrderType::Idle)
	{
		return m_loc;
	}

	AntDirection dir = m_order.getMove();
	
	return Location::getLocation(m_loc, dir);
}

void Ant::moveByOrder()
{
	setLocation(getNewLocation());
}

void Ant::setOrder(const Order& order)
{
	m_order = order;
}

const Order& Ant::getOrder() const
{

	return m_order;
}

void Ant::saveLastOrder()
{
	m_lastOrders.push_back(m_order);
	m_order.reset();
}
