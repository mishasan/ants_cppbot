#ifndef BUG_H_
#define BUG_H_

#include <fstream>

/*
    struct for debugging - this is gross but can be used pretty much like an ofstream,
                           except the debug messages are stripped while compiling if
                           DEBUG is not defined.
    example:
        Bug bug;
        bug.open("./debug.txt");
        bug << state << endl;
        bug << "testing" << 2.0 << '%' << endl;
        bug.close();
*/
struct Bug
{
    std::ofstream file;

    Bug()
    {

    };

    //opens the specified file
    inline void open(const std::string &filename)
    {
        #ifdef _DEBUG
            file.open(filename.c_str());
        #endif
    };

    //closes the ofstream
    inline void close()
    {
        #ifdef _DEBUG
            file.close();
        #endif
    };
};

//output function for endl
inline Bug& operator<<(Bug &bug, std::ostream& (*manipulator)(std::ostream&))
{
    #ifdef _DEBUG
        bug.file << manipulator;
    #endif

    return bug;
};

//output function
template <class T>
inline Bug& operator<<(Bug &bug, const T &t)
{
    #ifdef _DEBUG
        bug.file << t;
    #endif

    return bug;
};

/*
DeBug: a way to hold the bot to wait for attaching the Debugger to the process.
Needs a file DebugWait.txt with one value. 
When 1 the bot waits, when set to 0 the bot goes on. 
Set --turnlimit 50000 or something, set to 1, while starting the bot, attach to process, set breakpoints, set to 0.
*/
struct DeBug
{
	std::ifstream file;

	DeBug()
	{

	};

	//opens the specified file
	void open(const std::string &filename)
	{
#ifdef _DEBUG
		file.open(filename.c_str());
#endif
	};

	void wait()
	{
#ifdef _DEBUG
		//Timer t;
		//t.start();
		//while(true)
		//{
		//	bool stopAndRead = ((int)t.getTime() % 2000) < 50;
		//	open("DebugWait.txt");
		//	bool bWaitForDebugging = false;
		//	file >> bWaitForDebugging;
		//	close();
		//	if(!bWaitForDebugging)
		//		break;
		//}
#endif
	}

	//closes the ofstream
	inline void close()
	{
#ifdef _DEBUG
		file.close();
#endif
	};
};

#endif //BUG_H_
