#pragma once
#include "AStarNode.h"
class AStarOpenListFunctor
{
public:
	AStarOpenListFunctor();
	AStarOpenListFunctor(int x, int y, float f);
	AStarOpenListFunctor(const AStarNode& a);
	~AStarOpenListFunctor();
	int getX() const;
	int getY() const;
	AStarNode* getParent() const;
	float getCost() const;

		void setParent(AStarNode* a);
	void setX(int x);
	void setY(int x);
	void setCost(float x);

	void setFunctor(AStarNode* a);


	int operator() (AStarNode* a);

private:
	int x;
	int y;
	float cost;
	AStarNode* parent;
};

