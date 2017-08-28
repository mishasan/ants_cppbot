#pragma once

#include <vector>

class Order;
enum class AntDirection;

class Orders
{
public:
	Orders();
	~Orders();

	void setCount(size_t antCount);
	Order getOrder(const size_t antIndex);
	const Order& getOrder(const size_t antIndex) const;
	void setOrder(size_t antIndex, Order order);

	void resetAll();

private:
	std::vector<Order> m_vOrders;

};

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
	OrderType GetOrderType() const;

private:
	OrderType m_OrderType;
	AntDirection m_dirGoTo;
};