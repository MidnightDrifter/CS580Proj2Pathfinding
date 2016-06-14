#include "DXUT.h"
#include "AStarV3.h"
#undef max

float AStarV3::SQRT2 = sqrtf(2);

AStarV3::AStarV3() : openListSize(0), goalRow(-1), goalCol(-1)
{
	for(int i=0; i<SIZE_OF_MAP;i++)
	{
		for (int j = 0; j < SIZE_OF_MAP; j++)
		{
			//map[i][j]= new AStarNodev2(((SIZE_OF_MAP-1)-i),j);
			map[i][j] = new AStarNodev2(i, j);
		}
	}

	for (int k = 0; k < CAPACITY_OF_OPEN_LIST;k++)
	{
		openList[k]= new AStarNodev2();
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
		openList[i]->clearNode();
	}
	openListSize = 0;
}


void AStarV3::pushOpen(int i, int j)
{
	openList[openListSize] = (map[i][j]);
	openListSize++;
	map[i][j]->setOpen(true);
	g_terrain.SetColor(i, j, DEBUG_COLOR_PURPLE);
}

void AStarV3::pushClosed(int i, int j)
{
	map[i][j]->setClosed(true);
	g_terrain.SetColor(i, j, DEBUG_COLOR_RED);
}

AStarNodev2* AStarV3::popOpen(int i, int j)
{
	//int index = 0;
	AStarNodev2* temp=NULL;
	for (int k = 0; k < openListSize; k++)
	{
		if (openList[k]->getX() == i && openList[k]->getY() == j)
		{
			temp = openList[k];
			openListSize--;
			openList[k] = openList[openListSize];
				openList[openListSize]->clearNode();
				break;
		}
	}
	if (temp)
	{
		temp->setOpen(false);
		if (temp->getX() > 0)
		{
			this->editMap(temp->getX(), temp->getY())->setOpen(false);
		}
	}
	return temp;
}


AStarNodev2* AStarV3::popOpenMin()
{
	int index = 0;
	for(int i=0; i<openListSize;i++)
	{if(openList[i]->getCost() < openList[index]->getCost())
	{
		index = i;
	}
	
	}
	AStarNodev2* temp =openList[index];
	openListSize--;
	openList[index] = openList[openListSize];
	openList[openListSize - 1]->clearNode();
	return temp;
}


bool AStarV3::isValidNode(int x, int y) const
{
	return !(x < 0 || y < 0 || x >= g_terrain.GetWidth() || y >= g_terrain.GetWidth());
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
	
	}

	AStarNodev2* currentNode = this->popOpenMin();

	if (currentNode->getX() == goalRow && currentNode->getY() == goalCol)
	{
		//Found path, do stuff

		return true;
	}


	do
	{
		int cX = currentNode->getX();
		int cY = currentNode->getY();
		for (int i = cX- 1; i <= cX+1; i++)
		{
			for (int j = cY - 1; j <= cY+ 1; j++)
			{
				//Loop through neighbors
				

				if (this->isValidNode(i, j) && !(j == cY && i == cY) && !g_terrain.IsWall(i, j))
				{
					float c = std::numeric_limits<float>::max();
					if (i == cX + 1 && j == cY + 1 && !g_terrain.IsWall(i, cY) && !g_terrain.IsWall(cX, j))
					{
						//diag
						c = currentNode->getCost() + SQRT2;
					}

					else if (i == cX + 1 && j == cY - 1 && !g_terrain.IsWall(i, cY) && !g_terrain.IsWall(cX, j))
					{
						//diag
						c = currentNode->getCost() + SQRT2;
					}

					else if (i == cX - 1 && j == cY + 1 && !g_terrain.IsWall(i, cY) && !g_terrain.IsWall(cX, j))
					{
						//diag
						c = currentNode->getCost() + SQRT2;
					}

					else if (i == cX - 1 && j == cY - 1 && !g_terrain.IsWall(i, cY) && !g_terrain.IsWall(cX, j))
					{
						//diag
						c = currentNode->getCost() + SQRT2;
					}

					else if ((i == cX && (j == cY + 1 || j == cY - 1)) || (j == cY && (i == cX + 1 || j == cX - 1)))
					{
						//horizontal
						c = currentNode->getCost() + 1;
					}



					if (!this->editMap(i, j)->getOpen() && !this->editMap(i, j)->getClosed())
					{
						this->editMap(i, j)->setCost(c);
						this->editMap(i, j)->setTotalCost(this->calculateHeuristicCost(heuristic, hWeight, i, j, this->getGoalRow(), this->getGoalCol()) + c);
						this->editMap(i, j)->setParent(this->editMap(cX, cY));
						this->pushOpen(i, j);
					}

					else {
						AStarNodev2* t = this->popOpen(i, j);
						//Know that temp will be a default constructed node if it's not popped off of the open list, so can simply check for default values
						if (t)  // on list, check cost
						{
							if ( t->getTotalCost() > (c + this->calculateHeuristicCost(heuristic, hWeight, i, j, this->getGoalRow(), this->getGoalCol())))  //New path is better, update node push it on
							{
								this->editMap(i, j)->setCost(c);
								this->editMap(i, j)->setTotalCost(this->calculateHeuristicCost(heuristic, hWeight, i, j, this->getGoalRow(), this->getGoalCol()) + c);
								this->editMap(i, j)->setClosed(false);
								this->editMap(i, j)->setParent(this->editMap(cX, cY));
								this->pushOpen(i, j);

							}

							else   //Old path is better, push it back on
							{
								this->pushOpen(t->getX(), t->getY());
							}

						}
						else //is on closed list
						{
							if ( this->editMap(i, j)->getTotalCost() > (c + this->calculateHeuristicCost(heuristic, hWeight, i, j, this->getGoalRow(), this->getGoalCol())))  //Node on closed list but new path through it is better, update and push it on
							{
								this->editMap(i, j)->setCost(c);
								this->editMap(i, j)->setTotalCost(this->calculateHeuristicCost(heuristic, hWeight, i, j, this->getGoalRow(), this->getGoalCol()) + c);
								this->editMap(i, j)->setClosed(false);
								this->editMap(i, j)->setParent(this->editMap(cX, cY));
								this->pushOpen(i, j);
							}


						}
						
					}


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