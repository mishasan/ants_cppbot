#pragma once

class Map
{
public:
	static Map& map();

	void setDimensions(unsigned int rows, unsigned int cols) { map().m_rows = rows; map().m_cols = cols; }

	unsigned int rows() const { return map().m_rows; }
	unsigned int cols() const { return map().m_cols; }

private:
	Map();
	~Map();
	Map(const Map& map);

	unsigned int m_rows, m_cols;
};
