#include "DXUT.h"
#include "AStarNode.h"
#include <math.h>
#include <limits>
#undef max
#define SQRT2 1.41421356237  //A truncation of sqrt(2) to cut down on computation--can always replace with actual sqrt(2)


AStarNode::AStarNode() : xCoord(-1), yCoord(-1), isWall(false), isOpen(false), isClosed(false), totalCost(std::numeric_limits<float>::max()), costToGetToThisNode(std::numeric_limits<float>::max()), parent(NULL) {}
AStarNode::AStarNode(int x, int y) : xCoord(x), yCoord(y), isWall(false), isOpen(false), isClosed(false), totalCost(std::numeric_limits<float>::max()), costToGetToThisNode(std::numeric_limits<float>::max()), parent(NULL) {}
AStarNode::AStarNode(int x, int y, bool w) : xCoord(x), yCoord(y), isWall(w), isOpen(false), isClosed(false), totalCost(std::numeric_limits<float>::max()), costToGetToThisNode(std::numeric_limits<float>::max()), parent(NULL) {}
AStarNode::AStarNode(int x, int y, bool a, bool b, bool c) : xCoord(x), yCoord(y), isWall(a), isOpen(b), isClosed(c), totalCost(std::numeric_limits<float>::max()), costToGetToThisNode(std::numeric_limits<float>::max()), parent(NULL) {}
AStarNode::AStarNode(int x, int y, bool a, bool b, bool c, float h, float t) : xCoord(x), yCoord(y), isWall(a), isOpen(b), isClosed(c), totalCost(h), costToGetToThisNode(t), parent(NULL) {}
AStarNode::AStarNode(int x, int y, bool a, bool b, bool c, float h, float t, AStarNode* p) : xCoord(x), yCoord(y), isWall(a), isOpen(b), isClosed(c), totalCost(h), costToGetToThisNode(t), parent(p) {}
AStarNode::AStarNode(const AStarNode& other) : xCoord(other.getXCoord()), yCoord(other.getYCoord()), isWall(other.getWall()), isOpen(other.getOpen()), isClosed(other.getClosed()), totalCost(other.getHeuristicCost()), costToGetToThisNode(other.getCostToGetToThisNode()), parent(other.getParent()) {}


AStarNode::~AStarNode()  {}

//AStarNode::AStarNode() : xCoord(-1), yCoord(-1), isWall(false), isOpen(false), isClosed(false), totalCost(std::numeric_limits<float>::max()), costToGetToThisNode(std::numeric_limits<float>::max()), parent(NULL) {}
void AStarNode::clearNode()
{
	this->setWall(false);
	this->setOpen(false);
	this->setClosed(false);
	this->setCostToGetToThisNode(std::numeric_limits<float>::max());
	this->setTotalCost(std::numeric_limits<float>::max());
	this->setParent(NULL);
}

bool operator<(const AStarNode& lhs, const AStarNode& rhs) 
{
	return (lhs.totalCost < rhs.totalCost);
}


//void AStarNode::intializeStartingCost()
//{
//	startingCost = 0.f;
//}
//
//void AStarNode::setStartingCost(float f)
//{
//	startingCost = f;
//}
//
//void AStarNode::addToStartingCost(float f)
//{
//	startingCost += f;
//}
//
//void AStarNode::subtractFromStartingCost(float f)
//{
//	startingCost -= f;
//}

void AStarNode::setXCoord(int x)
{
	this->xCoord = x;
}

void AStarNode::setYCoord(int y)
{
	this->yCoord = y;
}

void AStarNode::setOpen(bool x)
{
	this->isOpen = x;
}

void AStarNode::setClosed(bool x)
{
	this->isClosed = x;

}

void AStarNode::setTotalCost(float f)
{
	this->totalCost = f;
}

bool AStarNode::operator==(const AStarNode& r) const
{
	if(r.getXCoord() == this->getXCoord() && r.getYCoord() == this->getYCoord())
	{
		return true;
	}

	return false;
}

bool operator==(const AStarNode& rhs, const AStarNode& lhs)
{
	if(rhs.getXCoord() == lhs.getXCoord() && rhs.getYCoord() == lhs.getYCoord())
	{
		return true;
	}
	return false;
}

void AStarNode::calculateTotalCost( int i, AStarNode& goal)
{
	if (i == AStarNode::EUCLIDEAN)
	{
		this->setTotalCost(this->costToGetToThisNode + this->getEuclideanDistance(goal));
	}
	else if (i == AStarNode::OCTILE)
	{
		this->setTotalCost(this->costToGetToThisNode + this->getOctileDistance(goal));
	}
	else if (i == AStarNode::CHEBYSHEV)
	{
		this->setTotalCost(this->costToGetToThisNode + this->getChebyshevDistance(goal));
	}

	else if (i == AStarNode::MANHATTAN)
	{
		this->setTotalCost(this->costToGetToThisNode + this->getManhattanDistance(goal));
	}
}


void AStarNode::setCostToGetToThisNode(float f)
{
	this->costToGetToThisNode = f;
}

bool AStarNode::getClosed() const
{
	return this->isClosed;
}

bool AStarNode::getOpen() const
{
	return this->isOpen;
 }


int AStarNode::getXCoord() const
{
	return this->xCoord;
}

int AStarNode::getYCoord() const
{
	return this->yCoord;
}

bool AStarNode::getWall() const
{
	return this->isWall;
}

void AStarNode::setWall(bool w)
{
	this->isWall = w;
}

float AStarNode::getTotalCost() const
{
	return (this->totalCost);
}

float AStarNode::getHeuristicCost() const
{
	return (this->totalCost - this->costToGetToThisNode);
}

float AStarNode::getCostToGetToThisNode() const
{
	return this->costToGetToThisNode;
}

int AStarNode::getChebyshevDistance( AStarNode& other) const
{
	return static_cast<int>(fmax(abs(this->getXCoord() - other.getXCoord()), abs(this->getYCoord() - other.getYCoord())));
}

int AStarNode::getManhattanDistance(AStarNode& other) const
{
	return static_cast<int>(abs(this->getXCoord() - other.getXCoord()) + abs(this->getYCoord() - other.getYCoord()));
}

float AStarNode::getEuclideanDistance(AStarNode& other) const
{
	return sqrtf(powf(this->getXCoord() - other.getXCoord(), 2.f) + powf(this->getYCoord() - other.getYCoord(), 2.f));
}

float AStarNode::getOctileDistance(AStarNode& other) const
{
	return (SQRT2*fmin(abs(this->getXCoord() - other.getXCoord()), abs(this->getYCoord() - other.getYCoord()))) + fmax(abs(this->getXCoord() - other.getXCoord()), abs(this->getYCoord() - other.getYCoord())) - fmin(abs(this->getXCoord() - other.getXCoord()), abs(this->getYCoord() - other.getYCoord()));
}


AStarNode* AStarNode::getParent() const
{
	return this->parent;
}

void AStarNode::setParent(AStarNode* p)
{
	this->parent = p;
}


const AStarNode& AStarNode::operator=(const AStarNode& rhs) 
{
	if (!this || (this->getXCoord() != rhs.getXCoord() && this->getYCoord() != rhs.getYCoord() && this->getWall() != rhs.getWall() && this->getOpen() != rhs.getOpen() && this->getClosed() != rhs.getClosed() ))
	{
		this->setXCoord(rhs.getXCoord());
		this->setYCoord(rhs.getYCoord());
		this->setWall(rhs.getWall());
		this->setOpen(rhs.getOpen());
		this->setClosed(rhs.getClosed());
		this->setTotalCost(rhs.getTotalCost());
		this->setCostToGetToThisNode(rhs.getCostToGetToThisNode());
	}

	return *this;
}


bool AStarNode::updateCostToGetToThisNode(float newCost, AStarNode* newParent)  //Note:  this assumes that there will be few / no cases where a path with a lower cost will be found going through the same parent node
{
	if (newCost < this->getCostToGetToThisNode())
	{
		this->setTotalCost(this->getTotalCost() - (this->getCostToGetToThisNode() - newCost));
		this->setCostToGetToThisNode(newCost);
		this->setParent(newParent);
		this->setOpen(true);
		this->setClosed(false);

		return true;
	}

	return false;
}