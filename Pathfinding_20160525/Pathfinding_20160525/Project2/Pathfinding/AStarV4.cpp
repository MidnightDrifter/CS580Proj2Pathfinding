#include "DXUT.h"
#include "AStarV4.h"
#include "Source/global.h"
#include "Source/terrain.h"
#include "Source/singleton.h"
#undef max


 float AStarV4::SQRT2 = sqrtf(2);

AStarV4::AStarV4() : goalRow(-1), goalCol(-1), startRow(-1),startCol(-1), sizeOfOpenList(0), map(), openList(), rubberbandList(), fogOfWarMap()
{
	for(int i=0; i<g_terrain.GetWidth(); i++)
	{
		for (int j = 0; j < g_terrain.GetWidth(); j++)
		{
			map[i][j] = AStarNodeV3(i,j);
			fogOfWarMap[i][j] = -1.f;
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
std::list<AStarNodeV3>& AStarV4::getRubberbandList() { return rubberbandList; }
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

int AStarV4::getSizeOfOpenList() const
{
	return sizeOfOpenList;
}

void AStarV4::setGoalRow(int x) { goalRow = x; }
void AStarV4::setGoalCol(int x) { goalCol = x; }
void AStarV4::setGoal(int x, int y) { goalRow = x; goalCol = y;}

void AStarV4::setStartRow(int x) { startRow = x; }
void AStarV4::setStartCol(int x) { startCol = x; }
void AStarV4::setStart(int x, int y) { startRow = x; startCol = y; }


void AStarV4::clearMap()
{
	for(int i=0; i<g_terrain.GetWidth();i++)
	{
		for (int j = 0; j < g_terrain.GetWidth(); j++)
		{
			map[i][j].clear();
			fogOfWarMap[i][j] = -1.f;
			
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

	
	this->rubberbandList.clear();
	//g_terrain.ResetColors();
	

}

void AStarV4::pushOpen(AStarNodeV3 n)
{
	openList[sizeOfOpenList] = n;
	sizeOfOpenList++;
	g_terrain.SetColor(n.getX(), n.getY(), DEBUG_COLOR_BLUE);
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



bool AStarV4::findPath(bool newRequest, bool isSingleStep, int heuristic, float hWeight, int startX, int startY, int goalX, int goalY, bool useAnalysis, bool fogOfWar)
{
//	bool isFirstPass = false;
	if (newRequest)
	{
		this->clear();
		//g_terrain.ResetColors();
		this->setGoal(goalX, goalY);
		this->setStart(startX, startY);
		this->map[startX][startY].setCost(0.f);
		this->map[startX][startY].setTotalCost(0.f);
		this->pushOpen(map[startX][startY]);


		if (fogOfWar)
		{
			g_terrain.InitFogOfWar();
			for (int aaa = 0; aaa < g_terrain.GetWidth(); aaa++)
			{
				for (int bbb = 0; bbb < g_terrain.GetWidth(); bbb++)
				{
					fogOfWarMap[aaa][bbb] = g_terrain.GetInfluenceMapValue(aaa, bbb);
				}
			}
		}


	}

	

	//	isFirstPass = true;
		

	/*	if (((goalRow == startRow + 1) || (goalRow == startRow - 1) || goalRow == startRow) && (goalCol == startCol + 1 || goalCol == startCol - 1 || goalCol == startCol) && !(goalRow == startRow && goalCol == startCol))
		{
			this->map[goalRow][goalCol].setParentX(startRow);
			this->map[goalRow][goalCol].setParentY(startCol);
			return true;
		}*/

	


	//if(((goalRow == startRow+1) ||(goalRow ==startRow-1) || goalRow==startRow ) && (goalCol == startCol+1 || goalCol==startCol-1 || goalCol==startCol) )



	do
	{
		if(fogOfWar)
		{
			g_terrain.AnalyzeVisibleToPlayer();

			for (int a = 0; a < g_terrain.GetWidth(); a++)
			{
				for (int b = 0; b < g_terrain.GetWidth(); b++)
				{
					if (g_terrain.GetInfluenceMapValue(a,b) >0 && fogOfWarMap[a][b] == TILE_WALL_INVISIBLE)
					{
						fogOfWarMap[a][b] == TILE_WALL;
					}
				}
			}
		}

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

				bool isWall=true;
				bool isValidNode = this->isValidNode(i, j);
				if(isValidNode)
				{
					if (fogOfWar)
					{
						isWall = g_terrain.isWallPlusInvisible(i,j);
					}

					else
					{
						isWall = g_terrain.IsWall(i, j);
					}
				}


				if (isValidNode && !(j == currY && i == currX) && !isWall)
				{
					bool isNotWall = false;
					if (fogOfWar)
					{
						isNotWall = !g_terrain.isWallPlusInvisible(i, currY) && !g_terrain.isWallPlusInvisible(currX, j);
					}

					else
					{
						isNotWall = !g_terrain.IsWall(i, currY) && !g_terrain.IsWall(currX, j);
					}

					float hc = this->calculateHeuristicCost(heuristic, hWeight, i, j, this->getGoalRow(), this->getGoalCol());
					float gc = std::numeric_limits<float>::max();
					if (i == currX + 1 && j == currY + 1 && isNotWall)
					{
						//diag

						gc = map[currX][currY].getCost() + SQRT2;

						if (useAnalysis)
						{
							gc += 20.f* g_terrain.GetInfluenceMapValue(i, j); //Maybe change to curX & curY?
						}

					}

					else if (i == currX + 1 && j == currY - 1 && isNotWall)
					{
						//diag
						gc = map[currX][currY].getCost() + SQRT2;

						if (useAnalysis)
						{
							gc += 20.f* g_terrain.GetInfluenceMapValue(i, j); //Maybe change to curX & curY?
						}
					}

					else if (i == currX - 1 && j == currY + 1 && isNotWall)
					{
						//diag
						gc = map[currX][currY].getCost() + SQRT2;

						if (useAnalysis)
						{
							gc += 20.f* g_terrain.GetInfluenceMapValue(i, j); //Maybe change to curX & curY?
						}
					}

					else if (i == currX - 1 && j == currY - 1 && isNotWall)
					{
						//diag
						gc = map[currX][currY].getCost() + SQRT2;

						if (useAnalysis)
						{
							gc += 20.f* g_terrain.GetInfluenceMapValue(i, j); //Maybe change to curX & curY?
						}
					}

					else if ((i == currX && (j == currY + 1 || j == currY - 1)) || (j == currY && (i == currX + 1 || i == currX - 1)))
					{
						//horizontal
						gc = map[currX][currY].getCost() + 1;

						if (useAnalysis)
						{
							gc += 20.f* g_terrain.GetInfluenceMapValue(i, j); //Maybe change to curX & curY?
						}
					}


					if (!map[i][j].getOpen() && !map[i][j].getClosed() && gc < std::numeric_limits<float>::max())  //Not on either list, just push onto OL
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
		g_terrain.SetColor(currX, currY, DEBUG_COLOR_YELLOW);

		if(isSingleStep)
		{
			break;
		}

	} while (sizeOfOpenList > 0);



	return false;

}

//bool AStarV4::findPathFOW(bool newRequest, bool isSingleStep, int heuristic, float hWeight, int startX)

void AStarV4::rubberband()
{
	AStarNodeV3 goal = map[goalRow][goalCol];
	while (!(goal.getX() == startRow && goal.getY() == startCol))
	{
		rubberbandList.push_front(goal);
		goal = this->getMapNode(goal.getParentX(), goal.getParentY());
	}
	rubberbandList.push_front(map[startRow][startCol]);

	if (rubberbandList.size() >= 3)
	{
		std::list<AStarNodeV3>::iterator it = rubberbandList.begin();
		std::list<AStarNodeV3>::iterator stop = rubberbandList.end();
		--stop;
		--stop;
		AStarNodeV3 a, b, c;
		bool startOver = true;

		do
		{
			if (startOver)
			{
				a = *it;
				++it;
				//m_waypointList.pop_front();
				if (it != rubberbandList.end())
				{
					b = *it;
					++it;
				}

				//m_waypointList.pop_front();
				if (it != rubberbandList.end())
				{
					c = *it;
				}
				startOver = false;
			}
			if (it != rubberbandList.end())
			{
				bool bb = true;
			for (int i = min(a.getX(), c.getX()); i <= (a.getX() > c.getX() ? a.getX() : c.getX()); ++i)
			{
				for (int j = min(a.getY(), c.getY()); j <= (a.getY() > c.getY() ? a.getY() : c.getY()); j++)
				{
					if (g_terrain.IsWall(i, j))
					{
						bb = false;
						break;
					}
				}
			}

			if (bb)
			{
				//	m_waypointList.push_front(c);
				//	m_waypointList.push_back(a);
				rubberbandList.remove(b);
				it = rubberbandList.begin();
				startOver = true;
			}
			else
			{
				a = b;
				b = c;
				++it;
				if (it != rubberbandList.end())
				{
					c = *it;
				}
			}


		}



		} while (it != rubberbandList.end());//  && it != stop);

		

	}

	else
	{
		//return false;
	}


}



AStarV4::~AStarV4()
{
	delete[] map;
	delete[] openList;
	rubberbandList.clear();
}
