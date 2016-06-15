#include "DXUT.h"
#include "AStarNodeV3.h"
#include <limits>
#undef max



AStarNodeV3::AStarNodeV3() : myX(-1), myY(-1), parentX(-1), parentY(-1), open(false), closed(false), cost(std::numeric_limits<float>::max()), totalCost(cost){}
AStarNodeV3::AStarNodeV3(int x, int y) : myX(x), myY(y), parentX(-1), parentY(-1), open(false), closed(false), cost(std::numeric_limits<float>::max()-100), totalCost(cost) {}
AStarNodeV3::AStarNodeV3(int x, int y, int a, int b) : myX(x), myY(y), parentX(a), parentY(b), open(false), closed(false), cost(std::numeric_limits<float>::max()-100), totalCost(std::numeric_limits<float>::max()-100){}
AStarNodeV3::AStarNodeV3(int x, int y, bool o, bool c) : myX(x), myY(y), parentX(-1), parentY(-1), open(o), closed(c), cost(std::numeric_limits<float>::max()), totalCost(cost) {}
AStarNodeV3::AStarNodeV3(int x, int y, bool o, bool c, float co, float tc) : myX(x), myY(y), parentX(-1), parentY(-1), open(o), closed(c), cost(co), totalCost(tc) {}
AStarNodeV3::AStarNodeV3(const AStarNodeV3& rhs) :myX(rhs.getX()), myY(rhs.getY()), parentX(rhs.getParentX()), parentY(rhs.getParentY()), open(rhs.getOpen()), closed(rhs.getClosed()), cost(rhs.getCost()), totalCost(rhs.getTotalCost()) {}


const AStarNodeV3& AStarNodeV3::operator=(const AStarNodeV3& r)
{
	if (this != &r)
	{
		this->setX(r.getX());
		this->setY(r.getY());
		this->setOpen(r.getOpen());
		this->setClosed(r.getClosed());
		this->setCost(r.getCost());
		this->setTotalCost(r.getTotalCost());
		this->setParentX(r.getParentX());
		this->setParentY(r.getParentY());
	}
	return *this;
}


int AStarNodeV3::getX() const { return myX; }

int AStarNodeV3::getY() const { return myY; }

int AStarNodeV3::getParentX() const { return parentX; }

int AStarNodeV3::getParentY() const { return parentY; }

bool AStarNodeV3::getOpen() const { return open; }


bool AStarNodeV3::getClosed() const { return closed; }


float AStarNodeV3::getCost() const { return cost; }


float AStarNodeV3::getTotalCost() const { return totalCost; }

void AStarNodeV3::setX(int x) { myX = x; }
void AStarNodeV3::setY(int x) { myY = x; }
void AStarNodeV3::setParentX(int x) { parentX = x; }
void AStarNodeV3::setParentY(int x) { parentY = x; }
void AStarNodeV3::setOpen(bool x) { open = x; }
void AStarNodeV3::setClosed(bool x) { closed = x; }
void AStarNodeV3::setCost(float x) { cost = x; }
void AStarNodeV3::setTotalCost(float x) { totalCost = x; }

void AStarNodeV3::clear()
{
	//this->setX(-1);
	//this->setY(-1);
	this->setParentX(-1);
	this->setParentY(-1);
	this->setOpen(false);
	this->setClosed(false);
	this->setCost(std::numeric_limits<float>::max()-100);
	this->setTotalCost(std::numeric_limits<float>::max()-100);
}

void AStarNodeV3::del()  //removes (x,y) coords
{
	this->setX(-1);
	this->setY(-1);
	this->clear();
}

AStarNodeV3::~AStarNodeV3()
{
}
