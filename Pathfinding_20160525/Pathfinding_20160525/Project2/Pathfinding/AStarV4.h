#pragma once
#include "AStarNodeV3.h"
#include <math.h>
#include <list>
#define SIZE_OF_MAP 40
#define CAPACITY_OF_OPEN_LIST 256
class AStarV4
{
public:
	AStarV4();
	~AStarV4();


	int getGoalRow() const;
	int getGoalCol() const;
	int getStartRow() const;
	int getStartCol() const;
	int getOpenListSize() const;
	float calculateHeuristicCost(int h, float weight, int nodeX, int nodeY, int goalX, int goalY) const;
	bool isValidNode(int i, int j);
	bool findPath(bool newRequest, bool isSingleStep, int heuristic, float hWeight, int startX, int startY, int goalX, int goalY);
	AStarNodeV3 getMapNode(int i, int j);
	
	int getSizeOfOpenList() const;
	std::list<AStarNodeV3>& getRubberbandList();

	void setGoalRow(int x);
	void setGoalCol(int x);
	void setGoal(int x, int y);
	void setStartRow(int x);
	void setStartCol(int y);
	void setStart(int x, int y);
	void clearMap();
	void clearOpenList();
	void clear();

	void pushOpen(AStarNodeV3 n);
	AStarNodeV3 popOpenMin();
	void removeNodeOpen(int x, int y);

	void rubberband();




private:
	int goalRow;
	int goalCol;

	int startRow;
	int startCol;
	
	int sizeOfOpenList;
	AStarNodeV3 map[SIZE_OF_MAP][SIZE_OF_MAP];
	AStarNodeV3 openList[CAPACITY_OF_OPEN_LIST];
	std::list<AStarNodeV3> rubberbandList;
	static float SQRT2;

};

