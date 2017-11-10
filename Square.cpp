#include "Square.h"

std::ostream& operator<<(std::ostream &os, const Square& sq)
{
	os << sq.toChar();
	return os;
}

void Square::reset()
{
	isVisible = false;
	isHill = false;
	isFood = false;
	ant = hillPlayer = -1;
	deadAnts.clear();
}

bool Square::IsFogged() const
{
	return !(isWater || isLand);
}

char Square::toChar() const
{
	if(isWater)
		return '%';
	else if(isFood)
		return '*';
	else if(isHill)
		return (char)('A' + hillPlayer);
	else if(ant >= 0)
		return (char)('a' + ant);
	else if(isVisible)
		return '.';
	else
		return '?';
}

Square::Square()
{
	isVisible = isWater = isHill = isFood = isLand = false;
	ant = hillPlayer = -1;
	pathScore = PATHSCORE_UNKNOWN;
	pathScoreComplete = false;
}
