
#include "DXUT.h"
#include "AStar.h"
#include "Source/singleton.h"
#include "Source/terrain.h"


//std::vector<AStarNode>* AStar::openList = new  std::vector<AStarNode>;
//std::vector<AStarNode>* AStar::closedList = new std::vector<AStarNode>;
//AStarNode* AStar::goalNode = new AStarNode();
//std::vector<std::vector<AStarNode*>*>* AStar::map = new std::vector<std::vector<AStarNode*>*>(40, new std::vector<AStarNode*>(40, new AStarNode()));

//void AStar::initialize() {
//	for (int i = 0; i < numRows; i++)
//	{
//		//AStar::map->at(i) = new std::vector<AStarNode*>;
//		for (int j = 0; j < numCols; j++)
//		{
//		
//			//AStar::map->at(i)->at(j) = new AStarNode(i, j, b);
//			this->editNode(i, j)->setXCoord(i);
//			this->editNode(i, j)->setYCoord(j);
//			this->editNode(i, j)->setWall(g_terrain.IsWall(i,j));
//		}
//	}
//
//}
bool AStar::isValidNode(int x, int y) const
{
	if(x < 0 || y < 0 || x >= this->getRowCount() || y>= this->getColCount())
	{
		return false;
	}
	return true;
}


std::vector<AStarNode> const * const AStar::getOpenList() const
{
	return this->openList;
}

std::vector<AStarNode> const * const AStar::getClosedList() const
{
	return this->closedList;
}

void AStar::setStartingNode(int x, int y)  //Assumes grid has already been initialized
{
	map->at(x)->at(y)->setCostToGetToThisNode(0.f);
	map->at(x)->at(y)->setTotalCost(0.f);
}


AStar::AStar() : numRows(40), numCols(40)
{
	map = new std::vector<std::vector<AStarNode*>*>(40);
	for (int i = 0; i < numRows; i++)
	{
		map->at(i) = new std::vector<AStarNode*>(40);
		for (int j = 0; j < numCols; j++)
		{
			bool b = true;
			if (!(g_terrain.IsWall(i, j)))
			{
				b = false;
			}
			map->at(i)->at(j) = new AStarNode(i, j, b);
			//this->editNode(i, j)->setXCoord(i);
			//this->editNode(i, j)->setYCoord(j);
			//this->editNode(i, j)->setWall(b);
		}
	}
	openList = new  std::vector<AStarNode>;
	closedList = new std::vector<AStarNode>;
	goalNode = new AStarNode();

}






AStar::AStar(int r, int c) : numRows(r), numCols(c)
{
	map = new std::vector<std::vector<AStarNode*>*>(r);
	for (int i = 0; i < numRows; i++)
	{
		map->at(i) = new std::vector<AStarNode*>(c);
		for (int j = 0; j < numCols; j++)
		{
			bool b = true;
			if (!(g_terrain.IsWall(i, j)))
			{
				b = false;
			}
			map->at(i)->at(j) = new AStarNode(i, j, b);
		}
	}
	openList = new  std::vector<AStarNode>;
	closedList = new std::vector<AStarNode>;
	goalNode = new AStarNode();

}

AStar::AStar(const AStar& other) : numRows(other.getRowCount()), numCols(other.getColCount())
{
	map = new std::vector<std::vector<AStarNode*>*>(numRows);
	for(int i=0;i<numRows;i++)
	{
		map->at(i) = new std::vector<AStarNode*>(numCols);
		for (int j = 0; j < numCols; j++)
		{
			*(map->at(i)->at(j)) = *(other.map->at(i)->at(j));  //Double check this!!!!
																
		}
	}
	openList = new std::vector<AStarNode>(*other.getOpenList());
	closedList = new std::vector<AStarNode>(*other.getClosedList());
	goalNode = new AStarNode(other.getGoalNode());
	
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
		(*map) = *(rhs.map);
		(*openList) = *(rhs.openList);
		(*closedList) = *(rhs.closedList);
		(*goalNode) = *(rhs.goalNode);

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
	return (map->at(i))->at(j);
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
	auto i = map->begin();
	while (i != map->end())
	{
		
		
		(*i)->erase((*i)->begin(), (*i)->end());
		++i;
	}
	map->erase(map->begin(), map->end());

	openList->erase(openList->begin(), openList->end());
	closedList->erase(closedList->begin(), closedList->end());

	/*for (int i = 0; i < map->size(); i++)
	{
		for (int j = 0; j < map->size(); j++)
		{
			delete((map->at(i)->at(j)));
		}
		delete((map->at(i)));
	}
	delete(map);*/
}

void AStar::setGoalNode(AStarNode* g)
{
goalNode = g;
}

void AStar::setGoalNode(int x, int y)
{
	goalNode = this->editNode(x, y);
}


void AStar::pushOpen(AStarNode* p)
{
	p->setOpen(true);
	this->editNode(p->getXCoord(), p->getYCoord())->setOpen(true);
	openList->push_back(*p);
}

void AStar::pushClosed(AStarNode* p)
{
	p->setClosed(true);
	this->editNode(p->getXCoord(), p->getYCoord())->setClosed(true);
	closedList->push_back(*p);
}

AStarNode AStar::popOpenMin()
{
	//if()
	auto i = std::min_element(openList->begin(), openList->end());
	AStarNode temp = *i;
	openList->erase(i);
	
	//openList->erase(std::min_element(openList->begin(), openList->end()));
	temp.setOpen(false);
	return temp;
}

std::vector<AStarNode>*const AStar::editOpenList()
{
	return openList;
}

//AStarNode* AStar::popOpen(AStarNode* a)
//{
//	if (openList->size() >= 1)
//	{
//		std::vector<AStarNode>::iterator it = openList->find(*a);
//		AStarNode* temp = new AStarNode();
//		if (it != openList->end())
//		{
//			*temp = *it;
//			openList->erase(it);
//			return temp;
//		}
//		return NULL;
//	}
//	else
//	{
//		return NULL;
//	}
//}


void AStar::updateOpen(AStarNode* a)
{
	bool isNewElement = true;
	if (a)
	{
		for (int i = 0; i < openList->size(); i++)
		{
			if ((*openList)[i].getXCoord() == a->getXCoord() && (*openList)[i].getYCoord() == a->getYCoord())
			{
				if (a->getTotalCost() < (*openList)[i].getTotalCost())
				{
					(*openList)[i] = *a;
					//this->editNode(a->getXCoord(), a->getYCoord())->calculateTotalCost();
				}
				isNewElement = false;
				break;
			}
		}
	if(isNewElement)
	{
		openList->push_back(*a);
		a->setOpen(true);
	}

	}



	//std::for_each()
	//	this->openList->for_each
	//	for_each(openList->)
	//	if (temp)
	//	{
	//		if (temp->getTotalCost() < a->getTotalCost())
	//		{
	//			this->pushOpen(temp);
	//		}
	//		else
	//		{
	//			this->pushOpen(a);
	//		}
	//	}
	//	else
	//	{
	//		this->pushOpen(a);
	//	}
	//}
}

void AStar::updateOpen(int x, int y)
{
	
	this->updateOpen(this->editNode(x, y));// , f);
}

bool AStar::canMoveDiagonal(const AStarNode& current, const AStarNode& destination) const   //yeah double check this
{
	if (this->isValidNode(destination.getXCoord(), destination.getYCoord()))
	{
		//upper left  & upper right diags
		if (destination.getXCoord() == current.getXCoord() - 1)
		{
			//upper left
			if ((this->isValidNode(destination.getXCoord()-1, destination.getYCoord())&&(!this->getNode(destination.getXCoord() - 1, destination.getYCoord())->getWall())))
			{
				if (destination.getYCoord() == current.getYCoord() - 1)
				{
					return (this->isValidNode(current.getXCoord()-1, current.getYCoord()-1) && (this->getNode(current.getXCoord() - 1, current.getYCoord() - 1)->getWall()));
				}

				else if (destination.getYCoord() == current.getYCoord() + 1)
				{
					return (this->isValidNode(current.getXCoord() - 1, current.getYCoord() + 1) && (this->getNode(current.getXCoord() - 1, current.getYCoord() + 1)->getWall()));
				}
				else
				{
					return false;

				}
			}
			else
			{
				return false;
			}
		}
		//lower left & lower right diags
		else if (destination.getXCoord() == current.getXCoord() + 1)
		{
			if ((this->isValidNode(destination.getXCoord() + 1, destination.getYCoord()) && !this->getNode(destination.getXCoord() + 1, destination.getYCoord())->getWall()))
			{
				if (destination.getYCoord() == current.getYCoord() - 1)
				{
					return (this->isValidNode(current.getXCoord() + 1, current.getYCoord() - 1) && this->getNode(current.getXCoord() + 1, current.getYCoord() - 1));
				}
				else if (destination.getYCoord() == current.getYCoord() + 1)
				{
					return (this->isValidNode(current.getXCoord() + 1, current.getYCoord() + 1) && this->getNode(current.getXCoord() + 1, current.getYCoord() + 1));

				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}


bool AStar::canMoveHorizontal(const AStarNode& current, const AStarNode& destination) const
{
	if (this->isValidNode(destination.getXCoord(), destination.getYCoord()) && !destination.getWall())
	{
		return true;
	}

	else
	{
		return false;
	}
}



AStarNode const & AStar::getGoalNode() const
{
	return *(this->goalNode);
}


AStarNode AStar::popClosed()
{
	auto i = std::min_element(closedList->begin(), closedList->end());
	AStarNode temp = *i;
	closedList->erase(i);
	temp.setClosed(false);
	return temp;
}


void AStar::calculateTotalCost(AStarNode* current, int i)
{
	current->calculateTotalCost(i, *goalNode);
}

void AStar::calculateTotalCost(int x, int y, int i)
{
	this->editNode(x, y)->calculateTotalCost(i, *goalNode);
}
	//AStarNode::AStarNode() : xCoord(-1), yCoord(-1), isWall(false), isOpen(false), isClosed(false), totalCost(std::numeric_limits<float>::max()), costToGetToThisNode(std::numeric_limits<float>::max()), parent(NULL) {}
void AStar::clean()
{
	
	for (int i = 0; i < map->size(); i++)
	{
		for (int j = 0; j < map->size(); j++)
		{
			map->at(i)->at(j)->clearNode();
			if(!g_terrain.IsWall(i,j))
			{
				map->at(i)->at(j)->setWall(false);
			}

			else
			{
				map->at(i)->at(j)->setWall(true);
			}
		}
	}
}