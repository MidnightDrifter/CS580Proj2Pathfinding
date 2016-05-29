#include "DXUT.h"
#include "AStarNode.h"
#include <math.h>
#define SQRT2 1.41421356237  //A truncation of sqrt(2) to cut down on computation--can always replace with actual sqrt(2)
AStarNode::AStarNode() : xCoord(-1), yCoord(-1), isWall(false) {}
AStarNode::AStarNode(int x, int y) : xCoord(x), yCoord(y), isWall(false) {}
AStarNode::AStarNode(int x, int y, bool w) : xCoord(x), yCoord(y), isWall(w) {}
AStarNode::AStarNode(const AStarNode& other) : xCoord(other.getXCoord()), yCoord(other.getYCoord()), isWall(other.getWall()) {}


AStarNode::~AStarNode()  {}

void AStarNode::setXCoord(int x)
{
	this->xCoord = x;
}

void AStarNode::setYCoord(int y)
{
	this->yCoord = y;
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
	if (this->getXCoord() != rhs.getXCoord() && this->getYCoord() != rhs.getYCoord())
	{
		this->setXCoord(rhs.getXCoord());
		this->setYCoord(rhs.getYCoord());
	}

	return *this;
}