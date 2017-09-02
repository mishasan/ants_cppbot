#pragma once

#include "Location.h"
#include "Orders.h"

class Ant
{
public:
	Ant(const Location& loc);
	~Ant();
	
	void setValid(bool bValid = true) {m_bValid = bValid;}
	bool isValid() const {return m_bValid;}

	const Location& getLocation() const {return m_loc;}
	void setLocation(const Location& locAnt);
	Location getNewLocation() const;
	void moveByOrder();

	void setOrder(const Order& order);
	const Order& getOrder() const;
	void saveLastOrder();
	bool getPreviousMove(AntDirection& dir) const;
private:
	Location m_loc;
	bool m_bValid;

	Order m_order;
	std::vector<Order> m_lastOrders;
};

