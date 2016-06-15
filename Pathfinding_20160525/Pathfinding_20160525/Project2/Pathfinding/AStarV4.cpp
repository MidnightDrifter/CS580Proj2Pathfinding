#include "DXUT.h"
#include "AStarV4.h"
#include "Source/global.h"
#include "Source/terrain.h"
#include "Source/singleton.h"
#undef max


 float AStarV4::SQRT2 = sqrtf(2);

AStarV4::AStarV4() : goalRow(-1), goalCol(-1), startRow(-1),startCol(-1), sizeOfOpenList(0), map(), openList()
{
	for(int i=0; i<g_terrain.GetWidth(); i++)
	{
		for (int j = 0; j < g_terrain.GetWidth(); j++)
		{
			map[i][j] = AStarNodeV3(i,j);
		}
	}

	for (int k = 0; k < CAPACITY_OF_OPEN_LIST; k++)
	{
		openList[k] = AStarNodeV3();
	}
}


int AStarV4::getGoalRow() const{return goalRow;}
int AStarV4::getGoalCol() const { return goalCol; }
int AStarV4::getStartRow() const { return startRow; }
int AStarV4::getStartCol() const { return startCol; }
int AStarV4::getOpenListSize() const { return sizeOfOpenList; }

float AStarV4::calculateHeuristicCost(int h, float weight, int nodeX, int nodeY, int goalX, int goalY) const
{
	if (nodeX == goalX && nodeY == goalY)
	{
		return 0.f;
	}

	else if (h == 0)
	{
		//Euclidean
		return weight*sqrtf(powf(nodeX - goalX, 2.f) + powf(nodeY - goalY, 2.f));
	}

	else if (h == 1)
	{
		//Octile
		return weight*(SQRT2*fmin(abs(nodeX - goalX), abs(nodeY - goalY))) + fmax(abs(nodeX - goalX), abs(nodeY - goalY)) - fmin(abs(nodeX - goalX), abs(nodeY - goalY));
	}

	else if (h == 2)
	{
		//Chebyshev
		return fmax(abs(nodeX - goalX), abs(nodeY - goalY))*weight;
	}

	else if (h == 3)
	{
		//Manhattan
		return weight*abs(nodeX - goalX) + abs(nodeY - goalY);
	}
}

bool AStarV4::isValidNode(int x, int y)
{
	return !(x < 0 || y < 0 || x >= g_terrain.GetWidth() || y >= g_terrain.GetWidth());
}

void AStarV4::setGoalRow(int x) { goalRow = x; }
void AStarV4::setGoalCol(int x) { goalCol = x; }
void AStarV4::setGoal(int x, int y) { goalRow = x; goalCol = y;}

void AStarV4::setStartRow(int x) { startRow = x; }
void AStarV4::setStartCol(int x) { startCol = x; }
void AStarV4::setStart(int x, int y) { startRow = x; startCol = y; }


void AStarV4::clearMap()
{
	for(int i=0; i<SIZE_OF_MAP;i++)
	{
		for (int j = 0; j < SIZE_OF_MAP; j++)
		{
			map[i][j].clear();
			
		}
	}
	
}

void AStarV4::clearOpenList()
{
	for(int i=0;i<sizeOfOpenList;i++)
	{
		openList[i].del();
	}
	sizeOfOpenList = 0;
}

void AStarV4::clear()
{
	this->clearMap();
	this->clearOpenList();
	this->setGoalRow(-1);
	this->setGoalCol(-1);
	this->setStart(-1, -1);
}

void AStarV4::pushOpen(AStarNodeV3 n)
{
	openList[sizeOfOpenList] = n;
	sizeOfOpenList++;
	g_terrain.SetColor(n.getX(), n.getY(), DEBUG_COLOR_PURPLE);
}

AStarNodeV3 AStarV4::popOpenMin()
{
	int index = 0;
	for (int i = 0; i < sizeOfOpenList; i++)
	{
		if(openList[i].getTotalCost() < openList[index].getTotalCost())
		{
			index = i;
		}

	}
	AStarNodeV3 temp(openList[index]);
	sizeOfOpenList--;
	openList[index] = openList[sizeOfOpenList];
	openList[sizeOfOpenList].clear();
	temp.setOpen(false);
	this->getMapNode(temp.getX(), temp.getY()).setOpen(false);
	return temp;
}


void AStarV4::removeNodeOpen(int x, int y)
{
	for (int i = 0; i < sizeOfOpenList; i++)
	{
		if (openList[i].getX() == x && openList[i].getY() == y)
		{
			sizeOfOpenList--;
			openList[i] = openList[sizeOfOpenList];
			openList[sizeOfOpenList].clear();

			//break;
		}
	}
}

AStarNodeV3 AStarV4::getMapNode(int i, int j)
{
	return map[i][j];
}



bool AStarV4::findPath(bool newRequest, bool isSingleStep, int heuristic, float hWeight, int startX, int startY, int goalX, int goalY)
{
	if (newRequest)
	{
		g_terrain.ResetColors();
		this->setGoal(goalX, goalY);
		this->setStart(startX, startY);
		this->map[startX][startY].setCost(0.f);
		this->map[startX][startY].setTotalCost(0.f);
		this->pushOpen(map[startX][startY]);

	}



	do
	{
		AStarNodeV3 n = this->popOpenMin();
		int currX = n.getX();
		int currY = n.getY();

		if (currX == this->getGoalRow() && currY == this->getGoalCol())
		{
			//Path found do stuff
			return true;
		}


		for (int i = currX - 1; i <= currX + 1; i++)
		{
			for (int j = currY - 1; j <= currY + 1; j++)
			{
				//Loop through neighbors


				if (this->isValidNode(i, j) && !(j == currY && i == currY) && !g_terrain.IsWall(i, j))
				{
					float hc = this->calculateHeuristicCost(heuristic, hWeight, i, j, this->getGoalRow(), this->getGoalCol());
					float gc = std::numeric_limits<float>::max();
					if (i == currX + 1 && j == currY + 1 && !g_terrain.IsWall(i, currY) && !g_terrain.IsWall(currX, j))
					{
						//diag

						gc = map[currX][currY].getCost() + SQRT2;
					}

					else if (i == currX + 1 && j == currY - 1 && !g_terrain.IsWall(i, currY) && !g_terrain.IsWall(currX, j))
					{
						//diag
						gc = map[currX][currY].getCost() + SQRT2;
					}

					else if (i == currX - 1 && j == currY + 1 && !g_terrain.IsWall(i, currY) && !g_terrain.IsWall(currX, j))
					{
						//diag
						gc = map[currX][currY].getCost() + SQRT2;
					}

					else if (i == currX - 1 && j == currY - 1 && !g_terrain.IsWall(i, currY) && !g_terrain.IsWall(currX, j))
					{
						//diag
						gc = map[currX][currY].getCost() + SQRT2;
					}

					else if ((i == currX && (j == currY + 1 || j == currY - 1)) || (j == currY && (i == currX + 1 || i == currX - 1)))
					{
						//horizontal
						gc = map[currX][currY].getCost() + 1;
					}


					if (!map[i][j].getOpen() && !map[i][j].getClosed())  //Not on either list, just push onto OL
					{
						map[i][j].setOpen(true);
						map[i][j].setParentX(currX);
						map[i][j].setParentY(currY);
						map[i][j].setCost(gc);
						map[i][j].setTotalCost(gc + hc);
						this->pushOpen(map[i][j]);

					}

					else
					{
						if ((gc + hc) < map[i][j].getTotalCost())
						{
							if (map[i][j].getOpen())
							{
								this->removeNodeOpen(i, j);
							}

							else
							{
								map[i][j].setClosed(false);
							}

							map[i][j].setOpen(true);
							map[i][j].setParentX(currX);
							map[i][j].setParentY(currY);
							map[i][j].setCost(gc);
							map[i][j].setTotalCost(gc + hc);
							this->pushOpen(map[i][j]);
						}
					}

				}
			}
		}
		map[currX][currY].setClosed(true);
		g_terrain.SetColor(currX, currY, DEBUG_COLOR_RED);
		if(isSingleStep)
		{
			break;
		}

	} while (sizeOfOpenList > 0);

	return false;

}




AStarV4::~AStarV4()
{
}
