#include "DXUT.h"
#include "AStarV2.h"
#include "Source/singleton.h"
#include "Source/terrain.h"

float AStarV2::DIAG_DISTANCE = sqrtf(2);
int AStarV2::HV_DISTANCE = 1;

AStarV2::AStarV2() : goalRow(-1), goalCol(-1), map(), openList(), sizeOfOpenList(0), goalPath()
{
	for (int i = 0; i < SIZE_OF_MAP;i++)
	{
		for (int j = 0; j < SIZE_OF_MAP;j++)
		{
		map[i][j] = AStarNode(i,j);
		
		}
	}

	for (int k = 0; k < CAPACITY_OF_OPEN_LIST; k++)
	{
		openList[k] = AStarNode();
	}
}

AStarV2::AStarV2(int r, int c) : goalRow(r), goalCol(c), map(), openList(), sizeOfOpenList(0), goalPath()
{
	for (int i = 0; i < SIZE_OF_MAP; i++)
	{
		for (int j = 0; j < SIZE_OF_MAP; j++)
		{
			map[i][j] = AStarNode(i, j);

		}
	}

	for (int k = 0; k < CAPACITY_OF_OPEN_LIST; k++)
	{
		openList[k] = AStarNode();
	}
}

AStarV2::AStarV2(const AStarNode& goal) : goalRow(goal.getXCoord()), goalCol(goal.getYCoord()), map(), openList(), sizeOfOpenList(), goalPath() {
	for (int i = 0; i < SIZE_OF_MAP; i++)
	{
		for (int j = 0; j < SIZE_OF_MAP; j++)
		{
			map[i][j] = AStarNode(i, j);

		}
	}

	for (int k = 0; k < CAPACITY_OF_OPEN_LIST; k++)
	{
		openList[k] = AStarNode();
	}
}


int AStarV2::getGoalCol() const
{
	return goalCol;
}

int AStarV2::getGoalRow() const
{
	return goalRow;
}

bool AStarV2::isValidNode(int x, int y) const
{
	return !(x < 0 || y < 0 || x >= 40 || y >= 40);
}

 AStarNode& AStarV2::editMap(int i, int j)
{
	return map[i][j];
}

 const AStarNode& AStarV2::getMap(int i, int j)
 {
	 return map[i][j];
 }

 AStarNode AStarV2::popOpenMin()
 {
	 int index = 0;
	 for (int i = 0; i < sizeOfOpenList; i++)
	 {
		 if (openList[i].getTotalCost() < openList[index].getTotalCost())
		 {
			 index = i;
		 }
	 }

	 AStarNode temp(openList[index]);
	 openList[index] = openList[sizeOfOpenList - 1];
	 openList[sizeOfOpenList - 1].deleteNode();
	 sizeOfOpenList--;
	 map[temp.getXCoord()][temp.getYCoord()].setOpen(false);
	 return temp;
 }

 const AStarNode& AStarV2::getGoalPath()
 {
	 return goalPath;
 }


 AStarNode* AStarV2::getGoalNode()
 {
	 return &map[goalRow][goalCol];
 }

 void AStarV2::pushClosed(int x, int y)
 {
	 map[x][y].setClosed(true);
 }

 void AStarV2::pushOpen(const AStarNode& n)
 {
	 openList[sizeOfOpenList] = n;
	 sizeOfOpenList++;
	 map[n.getXCoord()][n.getYCoord()].setOpen(true);
 }

 void AStarV2::setGoal(const AStarNode& n)
 {
	 goalRow = n.getXCoord();
	 goalCol = n.getYCoord();
	// goalPath = n;
 }

 void AStarV2::setGoal(int x, int y)
 {
	 goalRow = x;
	 goalCol = y;
	// goalPath = AStarNode(x, y);
 }

 void AStarV2::setGoalRow(int x)
 {
	 goalRow = x;
 }

 void AStarV2::setGoalCol(int x)
 {
	 goalCol = x;
 }

 void AStarV2::clearOpenList()
 {
	 this->sizeOfOpenList = 0;
 }

 void AStarV2::clearMap()
 {
	 for(int i=0; i<SIZE_OF_MAP; i++)
	 {
		 for (int j = 0; j<SIZE_OF_MAP; j++)
		 {
			 map[i][j].clearNode();
		 }
	 }
 }



 bool AStarV2::findPath(bool newRequest, bool isSingleStep, int heuristic, int goalR, int goalC, int startR, int startC, float hWeight)
 {
	 if (newRequest)
	 {
		 this->setGoal(goalR, goalC);
		 this->clearOpenList();
		 this->editMap(startR, startC).setCostToGetToThisNode(0.f);
		 this->editMap(startR, startC).setTotalCost(0.f);
		 this->pushOpen(this->editMap(startR, startC));
	 }

	 do{
	 AStarNode currentNode = this->popOpenMin();
	 if (currentNode.getXCoord() == this->getGoalRow() && currentNode.getYCoord() == this->getGoalCol())
	 {
		 this->setGoal(currentNode);
		 return true;
	 }

	 for(int i=currentNode.getXCoord()-1; i<=currentNode.getXCoord()+1; i++)
	 {
		 for (int j = currentNode.getYCoord() - 1; j <= currentNode.getYCoord() + 1; j++)
		 {
			 bool update = false;
			 if (this->isValidNode(i, j) && !(currentNode.getXCoord() == i  && currentNode.getYCoord() == j) && !g_terrain.IsWall(i, j) && !this->editMap(i,j).getClosed())
			 {
				 if (i == currentNode.getXCoord() + 1 && j == currentNode.getYCoord() + 1 && !g_terrain.IsWall(currentNode.getXCoord(), j) && !g_terrain.IsWall(i, currentNode.getYCoord()))
				 {
					 //is legal diag
					 update = this->editMap(i, j).updateCostToGetToThisNode(currentNode.getCostToGetToThisNode() + DIAG_DISTANCE, &currentNode, hWeight, heuristic);
					
				 }

				 else if (i == currentNode.getXCoord() - 1 && j == currentNode.getYCoord() + 1 && !g_terrain.IsWall(i, currentNode.getYCoord()) && !g_terrain.IsWall(currentNode.getXCoord(), j))
				 {
					 //is legal diag
					 update = this->editMap(i, j).updateCostToGetToThisNode(currentNode.getCostToGetToThisNode() + DIAG_DISTANCE, &currentNode, hWeight, heuristic);
				 }

				 else if (i == currentNode.getXCoord() - 1 && j == currentNode.getYCoord() - 1 && !g_terrain.IsWall(i, currentNode.getYCoord()) && !g_terrain.IsWall(currentNode.getXCoord(), j))
				 {
					 //is legal diag
					 update = this->editMap(i, j).updateCostToGetToThisNode(currentNode.getCostToGetToThisNode() + DIAG_DISTANCE, &currentNode, hWeight, heuristic);
				 }

				 else if (i == currentNode.getXCoord() + 1 && j == currentNode.getYCoord() - 1 && !g_terrain.IsWall(i, currentNode.getYCoord()) && !g_terrain.IsWall(currentNode.getXCoord(), j))
				 {
					 //is legal diag
					 update = this->editMap(i, j).updateCostToGetToThisNode(currentNode.getCostToGetToThisNode() + DIAG_DISTANCE, &currentNode, hWeight, heuristic);
				 }

				 else if (!g_terrain.IsWall(i,j) && (i == currentNode.getXCoord() && (j == currentNode.getYCoord() - 1 || j == currentNode.getYCoord() + 1)) || (j == currentNode.getYCoord() && (i == currentNode.getXCoord() + 1 || i == currentNode.getYCoord() - 1)))
				 {
					 //is legal horizontal-vertical
					 update = this->editMap(i, j).updateCostToGetToThisNode(currentNode.getCostToGetToThisNode() + HV_DISTANCE, &currentNode, hWeight, heuristic);
				 }

				 if (update)
				 {
					 this->pushOpen(this->getMap(i, j));
					 g_terrain.SetColor(i, j, DEBUG_COLOR_PURPLE);
				 }

				 
			 }
		 }
		
	 }

	 this->pushClosed(currentNode.getXCoord(), currentNode.getYCoord());
	 g_terrain.SetColor(currentNode.getXCoord(), currentNode.getYCoord(), DEBUG_COLOR_RED);

	 if (isSingleStep)
	 {
		 break;
	 }
	 } while (sizeOfOpenList > 0);
	
	 return false;
 }




AStarV2::~AStarV2()
{

}


