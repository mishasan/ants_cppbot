#pragma once

#include <vector>

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
	AntDirection getMove() const;
	OrderType getOrderType() const;

private:
	OrderType m_OrderType;
	AntDirection m_dirGoTo;
};