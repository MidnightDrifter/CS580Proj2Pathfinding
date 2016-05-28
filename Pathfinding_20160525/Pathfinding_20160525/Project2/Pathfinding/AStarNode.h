#pragma once
class AStarNode
{
public:
	AStarNode();
	AStarNode(int x, int y);
	AStarNode(const AStarNode& r);
	~AStarNode();

	int getXCoord() const;
	int getYCoord() const;
	void setXCoord(int x);
	void setYCoord(int y);
	float getEuclideanDistance(AStarNode& other) const;
	int getManhattanDistance(AStarNode& other) const;
	float getOctileDistance(AStarNode& other) const;
	int getChebyshevDistance(AStarNode& other)const;

	const AStarNode& operator=(AStarNode& rhs);
private:
	int xCoord;
	int yCoord;
};

