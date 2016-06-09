#pragma once
#include <limits>
#define EPSILON 0.000001f
class AStarNode
{
public:


	enum HEURISTIC
	{
		EUCLIDEAN,
		OCTILE,
		CHEBYSHEV,
		MANHATTAN

	};
	AStarNode();
	AStarNode(int x, int y);
	AStarNode(int x, int y, bool w);
	AStarNode(int x, int y, bool w, bool o, bool c);
	AStarNode(int x, int y, bool w, bool o, bool c, float h, float t);
	AStarNode(int x, int y, bool w, bool o, bool c, float h, float t, AStarNode* p);  //NOTE: in this case, it IS okay to shallow-copy the pointer, since you're only using it to get at a node's (x,y) coords
	AStarNode(const AStarNode& r);
	~AStarNode();

	int getXCoord() const;
	int getYCoord() const;
	bool getWall() const;
	bool getOpen() const;
	bool getClosed() const;
	float getTotalCost() const;
	float getHeuristicCost() const;
	float getCostToGetToThisNode() const;
	AStarNode* getParent() const;

	//static void intializeStartingCost();
	//static void setStartingCost(float f);
	//static void addToStartingCost(float f);
	//static void subtractFromStartingCost(float f);

	bool updateCostToGetToThisNode(float newCost, AStarNode* newParent);

	friend bool operator<(const AStarNode& lhs, const AStarNode& rhs);

	friend bool operator==(const AStarNode& lhs, const AStarNode& rhs);
	bool operator==(const AStarNode& lhs) const;

	void setXCoord(int x);
	void setYCoord(int y);
	void setWall(bool w);
	void setOpen(bool o);
	void setClosed(bool c);
	void setTotalCost(float f);
	void setCostToGetToThisNode(float f);
	void setParent(AStarNode* p);
	float getEuclideanDistance(const AStarNode& other) const;
	int getManhattanDistance(const AStarNode& other) const;
	float getOctileDistance(const AStarNode& other) const;
	int getChebyshevDistance(const AStarNode& other)const;

	
	void calculateTotalCost(int i, const AStarNode& goal);

	void clearNode();

	const AStarNode& operator=(const AStarNode& rhs);
private:
	int xCoord;
	int yCoord;
	bool isWall;
	bool isOpen;
	bool isClosed;
	float costToGetToThisNode;
	float totalCost;
	AStarNode* parent;
	
};

