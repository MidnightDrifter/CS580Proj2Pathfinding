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
	AStarNode popOpenMin();
	//AStarNode* popOpen(AStarNode* p);
	void pushClosed(AStarNode* p);
	AStarNode popClosed();

	void setStartingNode(int x, int y);

	//void initalize();

	const AStar& operator=(const AStar& rhs);

	 void calculateTotalCost(AStarNode* current, int i);
	 void calculateTotalCost(int x, int y, int i);

	 void updateOpen(AStarNode* a);// , AStarOpenListFunctor* f);
	 void updateOpen(int x, int y);// , AStarOpenListFunctor* f);

	 void setGoalNode(AStarNode* goalNode);
	 void setGoalNode(int x, int y);

	 AStarNode const & getGoalNode() const;

	 bool canMoveDiagonal(const AStarNode& current, const AStarNode& destination) const;
	 bool canMoveHorizontal(const AStarNode& current, const AStarNode& destination) const;
	//static std::vector<std::vector<AStarNode*>*>* editMap();
	//static std::set<AStarNode>* editOpenList();
	//static std::set<AStarNode>* editClosedList();
	//static AStarNode* editGoalNode();

	// void initialize();
	 void clean();

	 std::vector<AStarNode>*const editOpenList();
	 std::vector<AStarNode> const * const getOpenList() const;
	 std::vector<AStarNode> const* const getClosedList() const;
	 bool isValidNode(int x, int y) const;
		
private:
	int numRows;
	int numCols;
	 std::vector<std::vector<AStarNode*>*>*  map;
	 std::vector<AStarNode> * openList;
	 std::vector<AStarNode> * closedList;
	 AStarNode* goalNode;
	//Probably just replace sets with unordered list for ease of use because ow my brain
	
	
};

