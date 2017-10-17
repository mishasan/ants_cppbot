#include "stdafx.h"

#include <vector>
#include <fstream>

#include "CppUnitTest.h"
#include "PathFinder.h"
#include "Location.h"
#include "State.h"
#include "Map.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace U1_TestPathFinder
{		
	TEST_CLASS(TestPathFinder)
	{
	private:

		void prepareMap_Empty()
		{
			Map& m = Map::map();
			unsigned int nMapSize = 20;
			m.setDimensions(nMapSize, nMapSize);
			for(unsigned int x = 0; x < nMapSize; x++)
			{
				for(unsigned int y = 0; y < nMapSize; y++)
				{
					m[x][y].SetToLand();
					m[x][y].SetVisible();
				}
			}
		}

		void prepareMap_Cross()
		{
			Map& m = Map::map();
			unsigned int nMapSize = 20;
			m.setDimensions(nMapSize, nMapSize);
			for(unsigned int isWater = 2; isWater < nMapSize - 2; isWater++)
			{
				m[isWater][nMapSize/2].SetToWater();
				m[nMapSize/2][isWater].SetToWater();
			}
		}

	public:
		TEST_METHOD(StraightLines)
		{
			Bug::bug().open("Test_Debug_StraightLines.txt");
			
			prepareMap_Empty();

			PathFinder pf;
			pf.Init();

			Map::map().printKnownMap(Bug::bug().getStream());

			//	go straight in all directions
			Location locFrom(5, 5);
			for(unsigned int i = 0; i < AllAntDirectionsVec.size(); ++i)
			{
				auto dVec = AllAntDirectionsVec[i];
				int rowTo = locFrom.row + dVec.first * 4;
				int colTo = locFrom.col + dVec.second * 4;
				const Location locTo(Location::getLocation(rowTo, colTo));	//wrap edges

				std::vector<AntDirection> path;
				bool bFoundPath = pf.findPath(locFrom, locTo, path);
				Assert::IsTrue(bFoundPath);

				Location locTestRun(locFrom);
				for(auto d : path)
				{
					locTestRun = Location::getLocation(locTestRun, d);
				}
				Assert::IsTrue(locTestRun == locTo);

				AntDirection dCorAns = AllAntDirections[i];
				Assert::IsTrue(std::all_of(path.begin(), path.end(), [dCorAns](AntDirection a) {return a == dCorAns;}));

				Bug::bug() << "Straight line from " << locFrom << " to " << locTo << std::endl;
				pf.printPathAndMap(locFrom, path);
			}
		}

		TEST_METHOD(MapOperators)
		{
			Map& m = Map::map();
			m.setDimensions(5, 5);

			//	check if Map-Operators work
			Location locTestWater(1,2);

			Square& sq = Map::map()(locTestWater);
			sq.SetToWater();
			Square& sqSame = Map::map()[1][2];
			Assert::IsTrue(&sq == &sqSame);
		}

		TEST_METHOD(AroundCornerWalking)
		{
			Bug::bug().open("Test_Debug_AroundCornerWalking.txt");

			//	TODO: needs a save and load map
			prepareMap_Cross();

			PathFinder pf;
			pf.Init();
			
			//	check pathfind-algorithm
			{
				Location locFrom(9, 9), locTo(14, 15);
				Assert::IsFalse(Map::map()(locFrom).IsWater());
				Assert::IsFalse(Map::map()(locTo).IsWater());
				std::vector<AntDirection> path;
				bool bFoundPath = pf.findPath(locFrom, locTo, path);
				Assert::IsTrue(bFoundPath);
				Bug::bug() << "Testcase: Easy around an edge. From " << locFrom << " to " << locTo << std::endl;
				Location locTestRun(locFrom);
				std::stringstream ss;
				for(auto d : path)
				{
					Assert::IsFalse(Map::map()(locTestRun).IsWater());
					Bug::bug() << locTestRun << " -> ";
					locTestRun = Location::getLocation(locTestRun, d);
					ss << d;
				}
				Bug::bug() << std::endl;
				Assert::IsTrue(locTestRun == locTo);

				pf.printPathAndMap(locFrom, path);
				Bug::bug() << "Path (From Begin to End): " << ss.str() << std::endl;
			}
		}

		TEST_METHOD(TestUnknownMapParts)
		{
			Bug::bug().open("Test_Debug_TestUnknownMapParts.txt");

			prepareMap_Empty();

			//	surround Startposition with water
			Location locFrom(2, 2);
			Map::map()[1][1].SetToWater();
			Map::map()[1][2].SetToWater();
			Map::map()[1][3].SetToWater();
			
			Map::map()[3][1].SetToWater();
			Map::map()[3][2].SetToWater();
			Map::map()[3][3].SetToWater();

			Map::map()[2][1].SetToWater();
			Map::map()[2][3].SetToWater();

			PathFinder pf;
			pf.Init();

			//	check pathfind-algorithm
			{
				Location locTo(5, 5);
				Assert::IsFalse(Map::map()(locFrom).IsWater());
				Assert::IsFalse(Map::map()(locTo).IsWater());
				std::vector<AntDirection> path;
				bool bFoundPath = pf.findPath(locFrom, locTo, path);
				Assert::IsFalse(bFoundPath);
				Bug::bug() << "Testcase: Surrounded by Water. From " << locFrom << " to " << locTo << std::endl;
				pf.printPathAndMap(locFrom, path);
			}
		}

	};
}