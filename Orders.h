#pragma once

#include <vector>
#include "Location.h"

enum class AntDirection;

class Order
{
public:
	Order();
	~Order();
	
	enum class OrderType {Idle, IdleActive, Food, Explore, Attack, AttackHill};

	void reset();
	void setOrderType(OrderType ordertype);
	void setMove(AntDirection antDir);
	void setTarget(const Location& locTarget);
	AntDirection getMove() const;
	OrderType getOrderType() const;
	const Location& getTarget() const;

#if _DEBUG
	std::string print() const;
#endif

private:
	OrderType m_OrderType;
	AntDirection m_dirGoTo;
	Location m_Target;
};

std::ostream& operator<<(std::ostream &os, const Order::OrderType& loc);