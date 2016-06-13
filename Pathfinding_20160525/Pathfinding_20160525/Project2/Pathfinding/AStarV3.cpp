#include "DXUT.h"
#include "AStarV3.h"

//AStarV3::AStarV3() {}
float AStarV3::SQRT2 = sqrtf(2);
AStarV3::AStarV3() : goalRow(-1), goalCol(-1), openListSize(0)
{
	for (int i = 0; i < SIZE_OF_MAP; i++)
	{
		for (int j = 0; j < SIZE_OF_MAP; j++)
		{
			map[i][j] = new AStarNodev2(i, j);
		}
	}

	for (int k = 0; k < CAPACITY_OF_OPEN_LIST; k++)
	{
		openList[k] = AStarNodev2();
	}
}


AStarV3::~AStarV3()
{}


int AStarV3::getGoalRow() const
{
	return goalRow;
}

int AStarV3::getGoalCol() const
{
	return goalCol;
}

int AStarV3::getOpenListSize() const
{
	return openListSize;
}

AStarNodev2* AStarV3::editMap(int i, int j)
{
	return map[i][j];
}

//
//int AStarNode::getChebyshevDistance(const AStarNode& other) const
//{
//	return static_cast<int>(fmax(abs(this->getXCoord() - other.getXCoord()), abs(this->getYCoord() - other.getYCoord())));
//}
//
//int AStarNode::getManhattanDistance(const AStarNode& other) const
//{
//	return static_cast<int>(abs(this->getXCoord() - other.getXCoord()) + abs(this->getYCoord() - other.getYCoord()));
//}
//
//float AStarNode::getEuclideanDistance(const AStarNode& other) const
//{
//	return sqrtf(powf(this->getXCoord() - other.getXCoord(), 2.f) + powf(this->getYCoord() - other.getYCoord(), 2.f));
//}
//
//float AStarNode::getOctileDistance(const AStarNode& other) const
//{
//	return (SQRT2*fmin(abs(this->getXCoord() - other.getXCoord()), abs(this->getYCoord() - other.getYCoord()))) + fmax(abs(this->getXCoord() - other.getXCoord()), abs(this->getYCoord() - other.getYCoord())) - fmin(abs(this->getXCoord() - other.getXCoord()), abs(this->getYCoord() - other.getYCoord()));
//}



float AStarV3::calculateHeuristicCost(int h, float weight, int nodeX, int nodeY, int goalX, int goalY) const
{
	if(nodeX == goalX && nodeY == goalY)
	{
		return 0.f;
	}

	else if (h == 1)
	{
		//Euclidean
		return weight*sqrtf(powf(nodeX - goalX, 2.f) + powf(nodeY - goalY, 2.f));
	}

	else if (h == 2)
	{
		//Octile
		return weight*(SQRT2*fmin(abs(nodeX - goalX), abs(nodeY - goalY))) + fmax(abs(nodeX - goalX), abs(nodeY - goalY)) - fmin(abs(nodeX - goalX), abs(nodeY - goalY));
	}

	else if (h == 3)
	{
		//Chebyshev
		return fmax(abs(nodeX - goalX), abs(nodeY - goalY))*weight;
	}

	else if (h == 4)
	{
		//Manhattan
		return weight*abs(nodeX - goalX) + abs(nodeY - goalY);
	}
}


void AStarV3::setGoal(int i, int j)
{
	goalRow = i;
	goalCol = j;
}

void AStarV3::setGoalCol(int i)
{
	goalCol = i;
}

void AStarV3::setGoalRow(int i)
{
	goalRow = i;
}

void AStarV3::clearMap()
{
	for(int i=0; i<SIZE_OF_MAP;i++)
	{
		for (int j = 0; j < SIZE_OF_MAP; j++)
		{
			map[i][j]->clearNode();
		}
	}
}

void AStarV3::clearOpenList()
{
	for (int i = 0; i < CAPACITY_OF_OPEN_LIST; i++)
	{
		openList[i].clearNode();
	}
}


void AStarV3::pushOpen(int i, int j)
{/*
	openList[openListSize] = *map[i][j];
	openListSize++;
	map[i][j]->setOpen(true);*/
	if (openListSize == 0)
	{
		openList[0] = *(this->editMap(i, j));
		openListSize++;
	}

	else {
	
		for (int k = 0; k < openListSize; k++)
		{
			if (i == openList[k].getX() && i == openList[k].getY())
			{
				if (openList[k].getTotalCost() < this->map[i][j]->getTotalCost())
				{
					this->clearOpenNode(i, j);
					openList[openListSize] = *(this->editMap(i, j));
					this->editMap(i, j)->setOpen(true);
					this->editMap(i, j)->setClosed(false);
					openListSize++;
					return;

				}
				
			}
		}
		openList[openListSize] = *(this->editMap(i, j));
		this->editMap(i, j)->setOpen(true);
		openListSize++;
	}
}

void AStarV3::pushClosed(int i, int j)
{
	map[i][j]->setClosed(true);
}

void AStarV3::clearOpenNode(int i, int j)
{
	for (int k = 0; k < openListSize; k++)
	{
		if(openList[k].getX() ==i  && openList[k].getY() ==j)
		{

			openListSize--;
			openList[k] = openList[openListSize];
			openList[openListSize].clearNode();
			this->editMap(i, j)->setOpen(false);
		}
	}
}

AStarNodev2 AStarV3::popOpenMin()
{
	int index = 0;
	for(int i=0; i<openListSize;i++)
	{if(openList[i].getCost() < openList[index].getCost())
	{
		index = i;
	}
	
	}
	AStarNodev2 temp(openList[index]);
	//openListSize;
	openListSize--;
	openList[index] = openList[openListSize];
	openList[openListSize].clearNode();
	return temp;
}


bool AStarV3::isValidNode(int x, int y, int mapX, int mapY) const
{
	return !(x < 0 || y < 0 || x >= mapX || y >= mapY);
}


bool AStarV3::findPath(bool newRequest, bool isSingleStep, int heuristic, float hWeight, int startX, int startY, int goalX, int goalY)
{
	if(newRequest)
	{
		this->setGoal(goalX, goalY);
		this->clearOpenList();
		this->pushOpen(startX, startY);
		this->editMap(startX, startY)->setCost(0.f);
		this->editMap(startX, startY)->setTotalCost(0.f);
		this->pushOpen(startX, startY);
	
	}

	AStarNodev2 currentNode = this->popOpenMin();

	if (currentNode.getX() == goalRow && currentNode.getY() == goalCol)
	{
		//Found path, do stuff

		return true;
	}
	int mapX = g_terrain.GetWidth();

	do
	{
		int cX = currentNode.getX();
		int cY = currentNode.getY();
		for (int i = cX- 1; i <= cX+1; i++)
		{
			for (int j = cY - 1; j <= cY+ 1; j++)
			{
				//Loop through neighbors
				float tCost;
				if (this->isValidNode(i, j, mapX, mapX) && !(j == cY && i == cY) && !g_terrain.IsWall(i, j))
				{
					if (i == cX + 1 && j == cY + 1 && !g_terrain.IsWall(i, cY) && !g_terrain.IsWall(cX, j))
					{
						//diag
						this->editMap(i, j)->setParent(this->editMap(cX, cY));
						this->editMap(i, j)->setCost(this->editMap(cX, cY)->getCost() + SQRT2);
						this->editMap(i, j)->setTotalCost(this->editMap(i, i)->getCost() + this->calculateHeuristicCost(heuristic, hWeight, i, j, goalRow, goalCol));
						//tCost = this->editMap(i, j)->getCost() + SQRT2 + this->calculateHeuristicCost(heuristic, hWeight, cX, cY, goalRow, goalCol);
						
					}

					else if (i == cX + 1 && j == cY - 1 && !g_terrain.IsWall(i, cY) && !g_terrain.IsWall(cX, j))
					{
						//tCost = this->editMap(cX, cY)->getCost() + SQRT2 + this->calculateHeuristicCost(heuristic, hWeight, cX, cY, goalRow, goalCol);
						//diag
						//this->editMap(cX, cY)->setParent(this->editMap(i, j));
						//this->editMap(cX, cY)->setCost(this->editMap(cX, cY)->getCost() + SQRT2);
						//this->editMap(cX, cY)->setTotalCost(this->editMap(cX, cY)->getCost() + this->calculateHeuristicCost(heuristic, hWeight, cX, cY, goalRow, goalCol));
					
						this->editMap(i, j)->setParent(this->editMap(cX, cY));
						this->editMap(i, j)->setCost(this->editMap(cX, cY)->getCost() + SQRT2);
						this->editMap(i, j)->setTotalCost(this->editMap(i, i)->getCost() + this->calculateHeuristicCost(heuristic, hWeight, i, j, goalRow, goalCol));
					
					}

					else if (i == cX - 1 && j == cY + 1 && !g_terrain.IsWall(i, cY) && !g_terrain.IsWall(cX, j))
					{
						//tCost = this->editMap(cX, cY)->getCost() + SQRT2 + this->calculateHeuristicCost(heuristic, hWeight, cX, cY, goalRow, goalCol);
						//diag
						//this->editMap(cX, cY)->setParent(this->editMap(i, j));
						//this->editMap(cX, cY)->setCost(this->editMap(cX, cY)->getCost() + SQRT2);
						//this->editMap(cX, cY)->setTotalCost(this->editMap(cX, cY)->getCost() + this->calculateHeuristicCost(heuristic, hWeight, cX, cY, goalRow, goalCol));

						this->editMap(i, j)->setParent(this->editMap(cX, cY));
						this->editMap(i, j)->setCost(this->editMap(cX, cY)->getCost() + SQRT2);
						this->editMap(i, j)->setTotalCost(this->editMap(i, i)->getCost() + this->calculateHeuristicCost(heuristic, hWeight, i, j, goalRow, goalCol));
					}

					else if (i == cX - 1 && j == cY - 1 && !g_terrain.IsWall(i, cY) && !g_terrain.IsWall(cX, j))
					{
						tCost = this->editMap(cX, cY)->getCost() + SQRT2 + this->calculateHeuristicCost(heuristic, hWeight, cX, cY, goalRow, goalCol);
						//diag
						this->editMap(cX, cY)->setParent(this->editMap(i, j));
					//	this->editMap(cX, cY)->setCost(this->editMap(cX, cY)->getCost() + SQRT2);
						//this->editMap(cX, cY)->setTotalCost(this->editMap(cX, cY)->getCost() + this->calculateHeuristicCost(heuristic, hWeight, cX, cY, goalRow, goalCol));
						this->editMap(i, j)->setParent(this->editMap(cX, cY));
						this->editMap(i, j)->setCost(this->editMap(cX, cY)->getCost() + SQRT2);
						this->editMap(i, j)->setTotalCost(this->editMap(i, i)->getCost() + this->calculateHeuristicCost(heuristic, hWeight, i, j, goalRow, goalCol));
					}

					else if ((i == cX && (j == cY + 1 || j == cY - 1)) || (j == cY && (i == cX + 1 || j == cX - 1)))
					{
						//tCost = this->editMap(cX, cY)->getCost() + 1.f + this->calculateHeuristicCost(heuristic, hWeight, cX, cY, goalRow, goalCol);
						//horizontal
						//this->editMap(cX, cY)->setParent(this->editMap(i, j));
					//	this->editMap(cX, cY)->setCost(this->editMap(cX, cY)->getCost() + 1.f);
					//	this->editMap(cX, cY)->setTotalCost(this->editMap(cX, cY)->getCost() + this->calculateHeuristicCost(heuristic, hWeight, cX, cY, goalRow, goalCol));

						this->editMap(i, j)->setParent(this->editMap(cX, cY));
						this->editMap(i, j)->setCost(this->editMap(cX, cY)->getCost() + 1);
						this->editMap(i, j)->setTotalCost(this->editMap(i, i)->getCost() + this->calculateHeuristicCost(heuristic, hWeight, i, j, goalRow, goalCol));
					}
					this->pushOpen(i, j);


					/*if (!map[i][j]->getOpen() && !map[i][j]->getClosed())
					{
						this->editMap(
						this->pushOpen(i, j);
					}*/

				}


			}
		}
		this->pushClosed(cX, cY);

		if(isSingleStep)
		{
			break;
		}
	} while (openListSize > 0);

	return false;
}