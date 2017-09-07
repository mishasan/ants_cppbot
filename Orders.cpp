#include "Orders.h"

#ifdef _DEBUG
#include <sstream>
#endif // DEBUG

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

const Location& Order::getTarget() const
{
	return m_Target;
}

void Order::setTarget(const Location& locTarget)
{
	m_Target = locTarget;
}

#ifdef _DEBUG
std::string Order::print() const
{
	std::stringstream s;
	s << "DirGoTo: " << m_dirGoTo << " Target: " << m_Target << " OT: " << (int)m_OrderType;
	return s.str();
}
#endif // _DEBUG
