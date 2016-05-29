
#include "DXUT.h"
#include "AStar.h"



AStar::AStar()
{
}


AStar::~AStar()
{
	for (int i = 0; i < map.size(); i++)
	{
		for (int j = 0; j < map.size(); j++)
		{
			delete(&(map.at(i).at(j)));
		}
		delete(&(map.at(i)));
	}
	delete(&map);
}



	