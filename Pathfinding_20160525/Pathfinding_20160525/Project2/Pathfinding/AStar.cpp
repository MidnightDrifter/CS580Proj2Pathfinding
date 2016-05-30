
#include "DXUT.h"
#include "AStar.h"






AStar::AStar() : numRows(40), numCols(40), map(new std::vector<std::vector<AStarNode*>*>)
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

AStar::AStar(int r, int c) : numRows(r), numCols(c), map(new std::vector<std::vector<AStarNode*>*>)
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

AStar::AStar(const AStar& other) : numRows(other.getRowCount()), numCols(other.getColCount()), map(new std::vector<std::vector<AStarNode*>*>)
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
				this->editNode(i, j)->setHeuristicCost(rhs.getNode(i, j)->getHeuristicCost());
				this->editNode(i, j)->setTrueCost(rhs.getNode(i, j)->getTrueCost());
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
	return this->map->at(i);
}

AStarNode* AStar::editNode(int i, int j)
{
	return this->map->at(i)->at(j);
}

const AStarNode* AStar::getNode(int i, int j) const
{
	return this->map->at(i)->at(j);
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



	