#include "Orders.h"

#include "Location.h"

Orders::Orders()
{

}

void Orders::setCount(size_t antCount)
{
	m_vOrders.resize(antCount);
}

Orders::~Orders()
{
	m_vOrders.clear();
}

Order Orders::getOrder(const size_t antIndex)
{
	return m_vOrders.at(antIndex);
}

const Order& Orders::getOrder(const size_t antIndex) const
{
	return m_vOrders.at(antIndex);
}

void Orders::setOrder(size_t antIndex, Order order)
{
	m_vOrders.at(antIndex) = order;
}

void Orders::resetAll()
{
	//	for now: reset every order; later keep Orders and update them
	for(auto o : m_vOrders)
	{
		o.reset();
	}
}

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

Order::OrderType Order::GetOrderType() const
{
	return m_OrderType;
}
