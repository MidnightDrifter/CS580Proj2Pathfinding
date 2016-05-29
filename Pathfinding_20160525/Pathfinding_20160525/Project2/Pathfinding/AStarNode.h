#pragma once
class AStarNode
{
public:
	AStarNode();
	AStarNode(int x, int y);
	AStarNode(int x, int y, bool w);
	AStarNode(int x, int y, bool w, bool o, bool c);
	AStarNode(const AStarNode& r);
	~AStarNode();

	int getXCoord() const;
	int getYCoord() const;
	bool getWall() const;
	bool getOpen() const;
	bool getClosed() const;
	
	void setXCoord(int x);
	void setYCoord(int y);
	void setWall(bool w);
	void setOpen(bool o);
	void setClosed(bool c);

	float getEuclideanDistance(AStarNode& other) const;
	int getManhattanDistance(AStarNode& other) const;
	float getOctileDistance(AStarNode& other) const;
	int getChebyshevDistance(AStarNode& other)const;

	const AStarNode& operator=(AStarNode& rhs);
private:
	int xCoord;
	int yCoord;
	bool isWall;
	bool isOpen;
	bool isClosed;
};

