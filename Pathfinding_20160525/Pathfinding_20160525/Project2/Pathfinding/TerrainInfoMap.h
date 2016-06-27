#pragma once
#include "Source/global.h"
#include "Source/terrain.h"
#include "TerrainNode.h"
#include "Source/singleton.h"
#define SIZE_OF_MAP 40
class TerrainInfoMap : public Singleton<TerrainInfoMap>
{
	enum PLAYER_AREA_SIZE { sPAS = 3, lPAS };
	enum RESOURCE_AMOUNTS {sRA=6, mRA = 12, lRA = 20};


public:
	TerrainInfoMap();
	TerrainInfoMap(int p);
	TerrainInfoMap(int p, int fp);
	TerrainInfoMap(int p, int fp, int max);
	TerrainInfoMap(int p, int fp, int max, int pr);
	TerrainInfoMap(int p, int fp, int max, int pr, int pas);
	~TerrainInfoMap();

	int getNumPlayers() const;
	int getFavoredPlayer() const;
	int getMaxAmountOfResource() const;
	int getPreferredResource() const;
	int getPlayerAreaSize() const;

	void setNumPlayers(int x);
	void setFavoredPlayer(int x);
	void setMaxAmountOfResource(int x);
	void setPreferredResource(int x);  //Note:  this is the ONLY method that can break the limit on the resource amounts
	void setPlayerAreaSize(int x);

	


private:
	TerrainNode map[SIZE_OF_MAP][SIZE_OF_MAP];
	int numPlayers;
	int favoredPlayer;  //Player to favor--read: spawn more resources near--corresponds to OWNER_INFO enum
	int maxAmountOfResource;  //Maximum amount of a resource that can spawn--will be the same for all resources
	int preferredResource;  //Weight map to spawn more of a given resource, corresponds to the RESOURCE_INFO enum
	int playerAreaSize;

};

