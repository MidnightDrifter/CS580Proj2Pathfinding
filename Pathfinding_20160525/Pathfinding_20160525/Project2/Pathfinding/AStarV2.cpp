#include "DXUT.h"
#include "AStarV2.h"
#include "Source/singleton.h"
#include "Source/terrain.h"

AStarV2::AStarV2() : goalRow(-1), goalCol(-1), map(), openList(), sizeOfOpenList(0), goalPath()
{

}

AStarV2::AStarV2(int r, int c) : goalRow(r), goalCol(c), map(), openList(), sizeOfOpenList(0), goalPath()
{

}

AStarV2::AStarV2(const AStarNode& goal) : goalRow(goal.getXCoord()), goalCol(goal.getYCoord()), map(), openList(), sizeOfOpenList(), goalPath() {}


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

 AStarNode& AStarV2::popOpenMin()
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
	 openList[sizeOfOpenList - 1].clearNode();
	 sizeOfOpenList--;
	 map[temp.getXCoord()][temp.getYCoord()].setOpen(false);
	 return temp;
 }

 const AStarNode& AStarV2::getGoalPath()
 {
	 return goalPath;
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
 }

 void AStarV2::setGoal(int x, int y)
 {
	 goalRow = x;
	 goalCol = y;
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


 bool AStarV2::findPath(bool newRequest, bool isSingleStep, int heuristic, int goalR, int goalC, int startR, int startC)
 {
	 if (newRequest)
	 {
		 this->setGoal(goalR, goalC);
		 this->clearOpenList();
		 this->editMap(startR, startC).setCostToGetToThisNode(0.f);
		 this->editMap(startR, startC).setTotalCost(0.f);
		 this->pushOpen(this->editMap(startR, startC));
	 }

	 AStarNode currentNode = this->popOpenMin();

	 for(int i=currentNode.getXCoord()-1; i<currentNode.getXCoord()+1; i++)
	 {
		 for (int j = currentNode.getYCoord() - 1; j < currentNode.getYCoord() + 1; j++)
		 {
			 if (this->isValidNode(i, j) && !(currentNode.getXCoord() != i  && currentNode.getYCoord() != j) && !g_terrain.IsWall(i, j))
			 {
				 //horizontal-vertical check 
				 //{}

				 //diagonal check
			 }
		 }
	 }

 }




AStarV2::~AStarV2()
{

}



