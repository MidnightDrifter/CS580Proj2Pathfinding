#pragma once
#include "Source/singleton.h"
#include "AStarNode.h"
#include "Source/terrain.h"
#include "Source/global.h"

#define SIZE_OF_MAP	40
#define CAPACITY_OF_OPEN_LIST 100
class AStarV2
{
public:
	AStarV2();
	AStarV2(int r, int c);
	AStarV2(const AStarNode& goal);
	~AStarV2();

	int getGoalRow() const;
	int getGoalCol() const;
	bool isValidNode(int a, int b) const;
	 AStarNode& editMap(int x, int y);
	 AStarNode& popOpenMin();
	 const AStarNode& getGoalPath();

	void pushClosed(int x, int y);
	void setGoalRow(int x);
	void setGoalCol(int x);
	void setGoal(int x, int y);
	void setGoal(const AStarNode& goal);
	void pushOpen(const AStarNode& n);
	void clearOpenList();

	bool findPath(bool newRequest, bool singleStep, int heuristic, int goalR, int goalC, int startR, int startC);
	

private:
	AStarNode map[SIZE_OF_MAP][SIZE_OF_MAP];
	AStarNode openList[CAPACITY_OF_OPEN_LIST];
	int sizeOfOpenList=0;
	int goalRow;
	int goalCol;
	AStarNode goalPath;

};

