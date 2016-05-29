#pragma once
#include "AStarNode.h"
#include "Source/Terrain.h"
#include <math.h>
#include <vector>
#define HORIZONTAL_VERTICAL_DISTANCE = 1
#define float DIAGONAL_DISTANCE = sqrtf(2)
class AStar
{



public:
	AStar();
	AStar(int rows, int cols);
	~AStar();

	int getRows() const;
	int getCols() const;
	void setRows(int r);
	void setCols(int c);

	void initalize();

private:
	int numRows;
	int numCols;
	std::vector<std::vector<AStarNode>>  map;
	
};

