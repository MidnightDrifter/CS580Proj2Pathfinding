#pragma once
#include "AStarNode.h"
#include "Source/global.h"
#include "Source/Map.h"
#include <math.h>
#include <vector>
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
	 AStarNode* getNode(int i, int j) ;
	void setRowCount(int r);
	void setColCount(int c);

	void initalize();

	const AStar& operator=( AStar& rhs);

private:
	int numRows;
	int numCols;
	std::vector<std::vector<AStarNode*>*>*  map;
	
};

