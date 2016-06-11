#pragma once
#include "AStarNodev2.h"
#include "Source/singleton.h"
#include "Source/global.h"
#include "Source/terrain.h"
#define SIZE_OF_MAP 40
#define CAPACITY_OF_OPEN_LIST 150
class AStarV3
{
public:
	AStarV3();
	~AStarV3();

	int getGoalRow() const;
	int getGoalCol() const;
	int getOpenListSize() const;
	float calculateHeuristicCost(int h, float weight, int nodeX, int nodeY, int goalX, int goalY) const;
	
	AStarNodev2* editMap(int i, int j);
	void setGoal(int x, int y);
	void setGoalRow(int x);
	void setGoalCol(int y);
	void clearOpenList();
	void clearMap();

	bool findPath(bool newRequest, bool isSingleStep, int heuristic, float hWeight, int startX, int startY, int goalX, int goalY);
	bool isValidNode(int i, int j) const;

	void pushOpen(int i, int j);
	void pushClosed(int i, int j);
	void popOpen(int i, int j);  //Pop a specified node off the open list
	AStarNodev2* popOpenMin();

private:
	AStarNodev2* map[SIZE_OF_MAP][SIZE_OF_MAP];
	AStarNodev2 openList[CAPACITY_OF_OPEN_LIST];
	int goalRow;
	int goalCol;
	int openListSize;// = 0;
	static float SQRT2;
	

};

