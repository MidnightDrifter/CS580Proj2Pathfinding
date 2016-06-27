#include "DXUT.h"
#include "TerrainInfoMap.h"


TerrainInfoMap::TerrainInfoMap() : map(), numPlayers(-1), favoredPlayer(-1), maxAmountOfResource(sRA), preferredResource(-1), playerAreaSize(sPAS)
{
	for (int i = 0; i < g_terrain.GetWidth();++i)
	{
		for (int j = 0; j < g_terrain.GetWidth(); ++j)
		{
			map[i][j] = TerrainNode(i, j);
		}
	}
}

TerrainInfoMap::TerrainInfoMap(int p) : map(), numPlayers(p), favoredPlayer(-1), maxAmountOfResource(sRA), preferredResource(-1), playerAreaSize(sPAS)
{
	for (int i = 0; i < g_terrain.GetWidth(); ++i)
	{
		for (int j = 0; j < g_terrain.GetWidth(); ++j)
		{
			map[i][j] = TerrainNode(i, j);
		}
	}
}

TerrainInfoMap::TerrainInfoMap(int p, int fp) : map(), numPlayers(p), favoredPlayer(fp), maxAmountOfResource(sRA), preferredResource(-1), playerAreaSize(sPAS)
{
	for (int i = 0; i < g_terrain.GetWidth(); ++i)
	{
		for (int j = 0; j < g_terrain.GetWidth(); ++j)
		{
			map[i][j] = TerrainNode(i, j);
		}
	}
}

TerrainInfoMap::TerrainInfoMap(int p, int fp, int max) : map(), numPlayers(p), favoredPlayer(fp), maxAmountOfResource(max), preferredResource(-1), playerAreaSize(sPAS)
{
	for (int i = 0; i < g_terrain.GetWidth(); ++i)
	{
		for (int j = 0; j < g_terrain.GetWidth(); ++j)
		{
			map[i][j] = TerrainNode(i, j);
		}
	}
}


TerrainInfoMap::TerrainInfoMap(int p, int fp, int max, int pr) : map(), numPlayers(p), favoredPlayer(fp), maxAmountOfResource(max), preferredResource(pr), playerAreaSize(sPAS)
{
	for (int i = 0; i < g_terrain.GetWidth(); ++i)
	{
		for (int j = 0; j < g_terrain.GetWidth(); ++j)
		{
			map[i][j] = TerrainNode(i, j);
		}
	}
}

TerrainInfoMap::TerrainInfoMap(int p, int fp, int max, int pr, int pas) : map(), numPlayers(p), favoredPlayer(fp), maxAmountOfResource(max), preferredResource(pr), playerAreaSize(pas)
{
	for (int i = 0; i < g_terrain.GetWidth(); ++i)
	{
		for (int j = 0; j < g_terrain.GetWidth(); ++j)
		{
			map[i][j] = TerrainNode(i, j);
		}
	}
}

int TerrainInfoMap::getNumPlayers() const { return numPlayers; }
int TerrainInfoMap::getFavoredPlayer() const { return favoredPlayer; }
int TerrainInfoMap::getMaxAmountOfResource() const { return maxAmountOfResource; }
int TerrainInfoMap::getPreferredResource() const { return preferredResource; }
int TerrainInfoMap::getPlayerAreaSize() const { return playerAreaSize; }

void TerrainInfoMap::setNumPlayers(int x) { numPlayers = x; }
void TerrainInfoMap::setFavoredPlayer(int x) { favoredPlayer = x; }
void TerrainInfoMap::setMaxAmountOfResource(int x) { maxAmountOfResource = x; }
void TerrainInfoMap::setPreferredResource(int x) { preferredResource = x; }
void TerrainInfoMap::setPlayerAreaSize(int x) { numPlayers = x; }





TerrainInfoMap::~TerrainInfoMap()
{
}
