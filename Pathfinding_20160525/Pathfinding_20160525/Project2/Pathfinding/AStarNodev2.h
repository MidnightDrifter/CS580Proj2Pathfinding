#pragma once
#include <limits>
class AStarNodev2
{
public:
	AStarNodev2();
	AStarNodev2(int x, int y);
	AStarNodev2(int x, int y, bool o, bool c);
	AStarNodev2(int x, int y, bool o, bool c, float tc, float cc);
	AStarNodev2(const AStarNodev2& a);
	~AStarNodev2();

	int getX() const;
	int getY() const;
	float getCost() const;
	float getTotalCost() const;
	bool getClosed() const;
	bool getOpen() const;
	AStarNodev2* getParent() const;
	

	void setX(int x);
	void setY(int y);
	void setCost(float c);
	void setTotalCost(float tc);
	void setOpen(bool o);
	void setClosed(bool c);
	void setParent(AStarNodev2* p);
	void clearNode();  //Keeps (x,y)
	void deleteNode(); //Deletes (x,y)


	const AStarNodev2& operator=(const AStarNodev2& n);

private:
	int xCoord;
	int yCoord;
	bool closed;
	bool open;
	float totalCost;
	float cost;
	AStarNodev2* parent;



};

