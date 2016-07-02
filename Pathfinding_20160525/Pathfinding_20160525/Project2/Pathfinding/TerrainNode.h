#pragma once
class TerrainNode
{
	enum OWNER_INFO {
		NO_OWNER=-1,
		PLAYER_1,   //red
		PLAYER_2,	//green
		PLAYER_3,	//yellow
		PLAYER_4	//blue

	};

	enum RESOURCE_INFO {
		
		NO_RESOURCE = -1,
		MINERALS,   //Minerals - red
		GOLD,  //yellow
		FOOD, //green
		SCIENCE  //blue
	};



public:
	TerrainNode();
	TerrainNode(int i, int j);
	TerrainNode(int x, int y, int i, int j);
	TerrainNode(const TerrainNode& a);
	const TerrainNode& operator=(const TerrainNode& a);
	~TerrainNode();

	int getResource() const;
	int getOwner() const;
	int getX() const;
	int getY() const;

	void setResource(int x);
	void setOwner(int x);
	void setX(int x);
	void setY(int x);
	void clear();




private:
	int myX;
	int myY;
	int resource;
	int owner;

};

