#pragma once
class AStarNodeV3
{
public:
	AStarNodeV3();
	AStarNodeV3(int x, int y);
	AStarNodeV3(int x, int y, int a, int b);
	AStarNodeV3(int x, int y, bool o, bool c);
	AStarNodeV3(int x, int y, bool o, bool c, float co, float tc);
	AStarNodeV3(const AStarNodeV3& r);
	const AStarNodeV3& operator=(const AStarNodeV3& rhs);
	~AStarNodeV3();

	int getX() const;
	int getY() const;
	int getParentX() const;
	int getParentY() const;
	bool getOpen() const;
	bool getClosed() const;
	float getCost() const;
	float getTotalCost() const;

	void setX(int x);
	void setY(int y);
	void setParentX(int x);
	void setParentY(int x);
	void setOpen(bool b);
	void setClosed(bool b);
	void setCost(float c);
	void setTotalCost(float c);
	void clear();
	void del();  //same as clear, but gets rid of (x,y) coords as well

	bool operator==(const AStarNodeV3& r);


private:
	int myX;
	int myY;
	int parentX;
	int parentY;
	bool open;
	bool closed;
	float cost;
	float totalCost;
};

