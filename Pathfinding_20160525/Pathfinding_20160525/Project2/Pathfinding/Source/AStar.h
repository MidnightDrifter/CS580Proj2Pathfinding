#pragma once
#include "AStarNode.h"
#include <math.h>
#include <vector>
#define HORIZONTAL_VERTICAL_DISTANCE = 1
#define float DIAGONAL_DISTANCE = sqrtf(2)
class AStar
{



public:
	AStar();
	~AStar();

private:
	int numRows;
	int numCols;
	std::vector<AStarNode>  map;
	
};

