#pragma once
#include "AStarNode.h"
#include "Source/global.h"
#include "Source/Map.h"
#include <math.h>
#include <vector>
#include <set>
#include <algorithm>
#define HORIZONTAL_VERTICAL_DISTANCE 1
#define DIAGONAL_DISTANCE  1.41421356237  //A truncation of sqrt(2) to cut down on computation--can always replace with actual sqrt(2)
class AStar
{



public:
	AStar();
	AStar(int rows, int cols);
	AStar(const AStar& other);
	~AStar();

	int getRowCount() const;
	int getColCount() const;
	std::vector<AStarNode*>* getRow(int i);
	AStarNode* editNode(int i, int j);
	 const AStarNode* getNode(int i, int j) const;
	void setRowCount(int r);
	void setColCount(int c);

	void pushOpen(AStarNode* p);
	AStarNode popOpen();
	void pushClosed(AStarNode* p);
	AStarNode popClosed();

	void setStartingNode(int x, int y);

	//void initalize();

	const AStar& operator=(const AStar& rhs);

	 void calculateTotalCost(AStarNode* current, int i);
	 void calculateTotalCost(int x, int y, int i);

	

	 void setGoalNode(AStarNode* goalNode);
	 void setGoalNode(int x, int y);

	 void clean();
		
private:
	int numRows;
	int numCols;
	static std::vector<std::vector<AStarNode*>*>*  map;
	static std::set<AStarNode> * openList;
	static std::set<AStarNode> * closedList;
	static AStarNode* goalNode;
	
	
	
};

