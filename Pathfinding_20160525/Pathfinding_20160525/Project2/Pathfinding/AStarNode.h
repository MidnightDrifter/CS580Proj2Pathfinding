#pragma once
#include <limits>
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

	static void intializeStartingCost();
	static void setStartingCost(float f);
	static void addToStartingCost(float f);
	static void subtractFromStartingCost(float f);

	bool updateCostToGetToThisNode(float newCost, AStarNode* newParent);

	friend bool operator<(const AStarNode& lhs, const AStarNode& rhs);



	void setXCoord(int x);
	void setYCoord(int y);
	void setWall(bool w);
	void setOpen(bool o);
	void setClosed(bool c);
	void setTotalCost(float f);
	void setCostToGetToThisNode(float f);
	void setParent(AStarNode* p);
	float getEuclideanDistance(AStarNode& other) const;
	int getManhattanDistance(AStarNode& other) const;
	float getOctileDistance(AStarNode& other) const;
	int getChebyshevDistance(AStarNode& other)const;

	
	void calculateTotalCost(int i,  AStarNode& goal);


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
	static float startingCost; 
};

