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

Order Ant::getLastOrder() const
{
	if(m_lastOrders.empty())
	{
		return Order();
	}
	else
	{
		return *m_lastOrders.rbegin();
	}

}

bool Ant::getPreviousMove(AntDirection& dir) const
{
	for(std::vector<Order>::const_reverse_iterator it = m_lastOrders.rbegin(); it != m_lastOrders.rend(); ++it)
	{
		const Order& previousOrder = *it;
		if(previousOrder.getOrderType() == Order::OrderType::Idle)
		{
			continue;
		}
		else
		{
			dir = previousOrder.getMove();
			return true;
		}
	}

	return false;
}

bool Ant::operator==(const Ant& ant) const
{
	return this->getLocation() == ant.getLocation();
}
