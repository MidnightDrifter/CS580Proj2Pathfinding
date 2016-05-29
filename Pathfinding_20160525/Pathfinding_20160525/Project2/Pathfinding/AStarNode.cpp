#include "DXUT.h"
#include "AStarNode.h"
#include <math.h>
#define SQRT2 1.41421356237  //A truncation of sqrt(2) to cut down on computation--can always replace with actual sqrt(2)
AStarNode::AStarNode() : xCoord(-1), yCoord(-1), isWall(false), isOpen(false), isClosed(false) {}
AStarNode::AStarNode(int x, int y) : xCoord(x), yCoord(y), isWall(false), isOpen(false), isClosed(false) {}
AStarNode::AStarNode(int x, int y, bool w) : xCoord(x), yCoord(y), isWall(w), isOpen(false), isClosed(false) {}
AStarNode::AStarNode(int x, int y, bool a, bool b, bool c) : xCoord(x), yCoord(y), isWall(a), isOpen(b), isClosed(c) {}
AStarNode::AStarNode(const AStarNode& other) : xCoord(other.getXCoord()), yCoord(other.getYCoord()), isWall(other.getWall()), isOpen(other.getOpen()), isClosed(other.getClosed()) {}


AStarNode::~AStarNode()  {}

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

bool AStarNode::getClosed() const
{
	return this->isClosed;
}

bool AStarNode::getClosed() const
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


const AStarNode& AStarNode::operator=( AStarNode& rhs)
{
	if (this->getXCoord() != rhs.getXCoord() && this->getYCoord() != rhs.getYCoord() && this->getWall() != rhs.getWall() && this->getOpen() != rhs.getOpen() && this->getClosed() != rhs.getClosed())
	{
		this->setXCoord(rhs.getXCoord());
		this->setYCoord(rhs.getYCoord());
		this->setWall(rhs.getWall());
		this->setOpen(rhs.getOpen());
		this->setClosed(rhs.getClosed());
	}

	return *this;
}