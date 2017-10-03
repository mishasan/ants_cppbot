#include "Bug.h"

Bug& Bug::bug()
{
	static Bug s_BugInstance;
	return s_BugInstance;
}

void Bug::close()
{
#ifdef _DEBUG
	file.close();
#endif
}

void Bug::open(const std::string &filename)
{
#ifdef _DEBUG
	file.open(filename.c_str());
#endif
}
