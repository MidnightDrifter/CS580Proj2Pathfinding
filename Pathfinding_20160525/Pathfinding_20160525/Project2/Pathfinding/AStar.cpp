
#include "DXUT.h"
#include "AStar.h"




void AStar::setStartingNode(int x, int y)  //Assumes grid has already been initialized
{
	map->at(x)->at(y)->setCostToGetToThisNode(0.f);
}


AStar::AStar() : numRows(40), numCols(40))
{
	for (int i = 0; i < numRows; i++)
	{
		map->at(i) = new std::vector<AStarNode*>;
		for (int j = 0; j < numCols; j++)
		{
			bool b = true;
			if (!(g_terrain.isWall(i, j)))
			{
				b = false;
			}
			map->at(i)->at(j) = new AStarNode(i, j, b);
		}
	}


}

AStar::AStar(int r, int c) : numRows(r), numCols(c))
{
	for (int i = 0; i < numRows; i++)
	{
		map->at(i) = new std::vector<AStarNode*>;
		for (int j = 0; j < numCols; j++)
		{
			bool b = true;
			if (!(g_terrain.isWall(i, j)))
			{
				b = false;
			}
			map->at(i)->at(j) = new AStarNode(i, j, b);
		}
	}
}

AStar::AStar(const AStar& other) : numRows(other.getRowCount()), numCols(other.getColCount()))
{
	for(int i=0;i<numRows;i++)
	{
		map->at(i) = new std::vector<AStarNode*>;
		for (int j = 0; j < numCols; j++)
		{
			*(map->at(i)->at(j)) = *(other.map->at(i)->at(j));  //Double check this!!!!
																
		}
	}
}

const AStar&  AStar::operator=(const AStar& rhs)  //Assumes both AStar objects have same size
{

	if (this->getRowCount() == rhs.getRowCount() || this->getColCount() == rhs.getColCount())
	{
		for(int i=0; i<this->getRowCount(); i++)
		{
			for (int j = 0; j < this->getColCount(); j++)
			{
				(this)->editNode(i, j)->setXCoord( (rhs).getNode(i, j)->getXCoord());
				this->editNode(i, j)->setYCoord(rhs.getNode(i, j)->getYCoord());
				this->editNode(i, j)->setWall(rhs.getNode(i, j)->getWall());
				this->editNode(i, j)->setClosed(rhs.getNode(i, j)->getClosed());
				this->editNode(i, j)->setOpen(rhs.getNode(i, j)->getOpen());
				this->editNode(i, j)->setTotalCost(rhs.getNode(i, j)->getTotalCost());
				this->editNode(i, j)->setCostToGetToThisNode(rhs.getNode(i, j)->getCostToGetToThisNode());
			}
		}


	}
	return *this;
}


int AStar::getRowCount() const
{
	return this->numRows;
}

int AStar::getColCount() const
{
	return this->numCols;
}

std::vector<AStarNode*>* AStar::getRow(int i)
{
	return map->at(i);
}

AStarNode* AStar::editNode(int i, int j)
{
	return map->at(i)->at(j);
}

const AStarNode* AStar::getNode(int i, int j) const
{
	return map->at(i)->at(j);
}


void AStar::setRowCount(int r)
{
	this->numRows = r;
}

void AStar::setColCount(int c)
{
	this->numCols = c;
}

AStar::~AStar()
{
	for (int i = 0; i < map->size(); i++)
	{
		for (int j = 0; j < map->size(); j++)
		{
			delete((map->at(i)->at(j)));
		}
		delete((map->at(i)));
	}
	delete(map);
}



void AStar::pushOpen(AStarNode* p)
{
	p->setOpen(true);
	openList.insert(*p);
}

void AStar::pushClosed(AStarNode* p)
{
	p->setClosed(true);
	closedList.insert(*p);
}

AStarNode AStar::popOpen()
{
	
	AStarNode temp = *(std::min_element(openList.begin(), openList.end()));
	openList.erase(temp);
	temp.setOpen(false);
	return temp;
 }


AStarNode AStar::popClosed()
{

	AStarNode temp = *(std::min_element(closedList.begin(), closedList.end()));
	closedList.erase(temp);
	temp.setClosed(false);
	return temp;
}


	