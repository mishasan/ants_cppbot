#include "Orders.h"

#include "Location.h"

Order::Order()
{
	m_OrderType = OrderType::Idle;
}

Order::~Order()
{
	m_OrderType = OrderType::Idle;
}

void Order::setOrderType(OrderType ordertype)
{
	m_OrderType = ordertype;
}

void Order::setMove(AntDirection antDir)
{
	m_dirGoTo = antDir;
}

void Order::reset()
{
	m_OrderType = OrderType::Idle;
	m_dirGoTo = AntDirection::N;
}

AntDirection Order::getMove() const
{
	return m_dirGoTo;
}

Order::OrderType Order::getOrderType() const
{
	return m_OrderType;
}
