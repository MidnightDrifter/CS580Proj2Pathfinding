/* Copyright Steve Rabin, 2012. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2012"
 */

#include "DXUT.h"
#include "terrain.h"
#include "global.h"
#include "time.h"
#include "gameobject.h"
#include "database.h"
#include "body.h"

Terrain::Terrain( void )
: m_nextMap(0),
  m_rPlayer(4),
  m_cPlayer(4),
  m_reevaluateAnalysis(false),
  m_analysis(TerrainAnalysis_None),
  m_width(40)
{
    m_maps.push_back(Map(m_width));
}

Terrain::~Terrain()
{
	for (MapList::iterator i = m_maps.begin(); i != m_maps.end(); ++i)
		i->Destroy();
}

void Terrain::Create( void )
{
	NextMap();

	// Create new maps from Maps directory
	WCHAR pathBuffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH-1, pathBuffer);
	std::wstring path(pathBuffer);
	path += L"\\Maps\\";

	WIN32_FIND_DATA file;
	HANDLE h = FindFirstFile((std::wstring(path) += L"*.txt").c_str(), &file);

	if (!h)
		return;

	do 
	{
		if ((file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			continue;
		m_maps.push_back(Map());
		m_maps.back().Serialize(std::wstring(path) += file.cFileName);
	}
	while (FindNextFile(h, &file));

	FindClose(h);
}

void Terrain::NextMap( void )
{
	assert(m_maps.size() && "No maps loaded");
	m_nextMap = ++m_nextMap % m_maps.size();

	Map& map = m_maps[m_nextMap];
	m_width = map.GetWidth();
	m_terrain = map.GetTerrain();
	m_terrainColor = map.GetTerrainColor();
	m_terrainInfluenceMap = map.GetInfluenceMap();

	ResetColors();
	ResetInfluenceMap();
	Analyze();

	g_database.SendMsgFromSystem(MSG_MapChange);
}

D3DXVECTOR3 Terrain::GetCoordinates( int r, int c )
{
	D3DXVECTOR3 pos;

	const float offset = 1.f / m_width / 2.f;

	pos.x = ((float)c/(float)m_width) + offset;
	pos.y = 0.0f;
	pos.z = ((float)r/(float)m_width) + offset;
	
	return( pos );
}

bool Terrain::GetRowColumn( D3DXVECTOR3* pos, int* r, int* c )
{
	if( pos->x >= 0.0f && pos->x <= 1.0f && pos->z >= 0.0f && pos->z <= 1.0f )
	{
		*r = (int)(pos->z * m_width);
		*c = (int)(pos->x * m_width);
		return true;
	}
	else
	{
		return false;
	}
}

void Terrain::UpdatePlayerPos( D3DXVECTOR3& pos, D3DXVECTOR3& dir )
{
	int r, c;
	GetRowColumn(&pos, &r, &c);

	if( (m_rPlayer != r || m_cPlayer != c) &&  (m_analysis == TerrainAnalysis_VisibleToPlayer || m_analysis == TerrainAnalysis_Search))
	{
		m_rPlayer = r;
		m_cPlayer = c;
		m_dirPlayer = dir;
		m_reevaluateAnalysis = true;
	}
}

void Terrain::SetColor( int r, int c, DebugDrawingColor color )
{
	if( m_terrain[r][c] >= 0 )
	{
		m_terrainColor[r][c] = color;
	}
	else
	{
		ASSERTMSG( 0, "Can't set color to a grid square that is a wall" );
	}
}

void Terrain::ResetColors( void )
{
	for( int r=0; r<m_width; r++ )
	{
		for( int c=0; c<m_width; c++ )
		{
			if( m_terrain[r][c] >= 0 )
			{
				m_terrainColor[r][c] = DEBUG_COLOR_WHITE;
			}
			else
			{
				m_terrainColor[r][c] = DEBUG_COLOR_BLACK;
			}
		}
	}
}

void Terrain::ResetInfluenceMap( void )
{
	m_reevaluateAnalysis = true;

	for( int r=0; r<m_width; r++ )
	{
		for( int c=0; c<m_width; c++ )
		{
			m_terrainInfluenceMap[r][c] = 0.0f;
		}
	}
}

void Terrain::DrawDebugVisualization( IDirect3DDevice9* pd3dDevice )
{
	D3DXVECTOR3 pos( 0.0f, 0.001f, 0.0f );

	for( int r=0; r<m_width; r++ )
	{
		for( int c=0; c<m_width; c++ )
		{
			pos.x = (float)c / m_width;
			pos.z = (float)r / m_width;

			switch (m_terrain[r][c])
			{
			case TILE_WALL:
				g_debugdrawing.DrawCube( pd3dDevice, &pos, 1.0f/m_width, DEBUG_COLOR_BLACK );
				break;
			case TILE_EMPTY:
				g_debugdrawing.DrawQuad( pd3dDevice, &pos, 1.0f/m_width, m_terrainColor[r][c] );
				break;
			case TILE_WALL_INVISIBLE:
				g_debugdrawing.DrawCube( pd3dDevice, &pos, 1.f/m_width, DEBUG_COLOR_GRAY );
				break;
			default:
				assert("Invalid tile type");
			}
		}
	}

	if(m_analysis == TerrainAnalysis_Search)
	{	//Discount search tiles
		m_reevaluateAnalysis = true;
		for( int r=0; r<m_width; r++ )
		{
			for( int c=0; c<m_width; c++ )
			{
				m_terrainInfluenceMap[r][c] *= 0.999f;
			}
		}
	}

	pos.y = 0.002f;
	Analyze();
	for( int r=0; r<m_width; r++ )
	{
		for( int c=0; c<m_width; c++ )
		{
			pos.x = (float)c / m_width;
			pos.z = (float)r / m_width;
			D3DXCOLOR color = D3DXCOLOR( 1.0f, 0.0f, 0.0f, m_terrainInfluenceMap[r][c] );
			g_debugdrawing.DrawQuad( pd3dDevice, &pos, 1.0f/m_width, color );
		}
	}
}

void Terrain::Analyze( void )
{
	if(m_reevaluateAnalysis)
	{
		m_reevaluateAnalysis = false;
		g_time.ClearStopwatchAnalysis();
		g_time.StartStopwatchAnalysis();
		switch(m_analysis)
		{
			case TerrainAnalysis_OpennessClosestWall:
				AnalyzeOpennessClosestWall();
				break;

			case TerrainAnalysis_OpennessRearCover:
				AnalyzeOpennessRearCover();
				break;

			case TerrainAnalysis_Visibility:
				AnalyzeVisibility();
				break;

			case TerrainAnalysis_RearCoverWithHighVisibility:
				AnalyzeRearCoverWithHighVisibility();
				break;

			case TerrainAnalysis_VisibleToPlayer:
				AnalyzeVisibleToPlayer();
				break;

			case TerrainAnalysis_Search:
				AnalyzeSearch();
				break;

			default:
				ResetInfluenceMap();
				break;
		}

		g_time.StopStopwatchAnalysis();
	}
}

void Terrain::IncTerrainAnalysis( void )
{
	m_reevaluateAnalysis = true;
	m_analysis = (TerrainAnalysisType)((int)m_analysis + 1);
	if(TerrainAnalysis_Count == g_terrain.m_analysis) { g_terrain.m_analysis = TerrainAnalysis_None; }
	ClearTerrainAnalysis();
	Analyze();
}

bool Terrain::isWallNode(int i, int j)
{
	if (this->isValidNode(i, j))
	{
		return (g_terrain.IsWall(i, j));
	}

	else
	{
		return true;
	}
}


float Terrain::ClosestWall( int row, int col )   //DONE- test it - test SUCCESSFUL
{
	//TODO: Helper function for the Terrain Analysis project (you'll likely need it).

	//Return the distance to the closest wall or side of the map.
	bool foundMin = false;
	int upperX, upperY, lowerX, lowerY;// , currX, currY;

	upperX = row + 1;
	upperY = col + 1;
	lowerX = row - 1;
	lowerY = col - 1;

	//Map is 40x40, max distance is diagonal from one corner to other, so sqrt(40^2 + 40^2)  == sqrt(1600 *2) == 40sqrt(2)
	//56 and change
	//Can set max arbitrarily to 57
	//Will just set arbitrarily high regardless

	float minDist = 100000.f;

	//if (upperX >= g_terrain.GetWidth())
	//{
	//	upperX = g_terrain.GetWidth() - 1;
	//}

	// if(lowerX < 0)
	//{
	//	lowerX = 0;
	//}

	//if (upperY >= g_terrain.GetWidth())
	//{
	//	upperY = g_terrain.GetWidth() - 1;
	//}

	// if(lowerY<0)
	// {
	//	 lowerY = 0;
	// }


	 do
	 {
		 for (int i = lowerX; i <= upperX; i++)
		 {
			 for (int j = lowerY; j <= upperY; j++)
			 {
				 /*if (i > g_terrain.GetWidth() - 1)
					 {
						 i = g_terrain.GetWidth() - 1;
					 }

					 if (j > g_terrain.GetWidth() - 1)
					 {
						 j = g_terrain.GetWidth() - 1;
					 }

					 if (j < 0)
					 {
						 j = 0;
					 }

					 if (i < 0)
					 {
						 i = 0;
					 }*/



					 if (this->isWallNode(i,j) && !(i == row && j == col) && sqrtf((powf(row - i, 2) + powf(col - j, 2))) < minDist)
					 {
						 minDist = sqrtf((powf(row - i, 2) + powf(col - j, 2)));
						 foundMin = true;
					 }





				 
			 }
		 }


		 if (foundMin)
		 {
			 return minDist;
		 }

		 else
		 {
			 ++upperX;
			 ++upperY;
			 --lowerX;
			 --lowerY;

		 }

	 } while (upperX < g_terrain.GetWidth()-1 || upperY < g_terrain.GetWidth()-1 || lowerX >= 1 || lowerY >= 1);

	return 0.0f;	//REPLACE THIS
}

bool Terrain::isValidNode(int r, int c)  //Helper function to test node validity--within 0 - mapSize-1 range
{
	return !(r < 0 || c < 0 || r >= g_terrain.GetWidth() || c >= g_terrain.GetWidth());
}


int  Terrain::numSurroundingWalls(int r, int c)
{
	int count = 0;
	for(int i=r-1; i<=r+1;++i)
	{
		for (int j = c - 1; j <= c + 1; j++)
		{
			if(this->isValidNode(i,j) && g_terrain.IsWall(i,j))
			//if(this->isWallNode(i,j))
			{
				count++;
			}
		}
	}
	return true;

}


float Terrain::RearCoverValue( int row, int col )  //DONE - test it
{

	bool wallList[8];
	int start = 0;
	for(int i=row-1; i<=row+1;i++)
	{
		for (int j = col - 1; j <= col + 1; j++)
		{
			if (!(i == row &&j == col))
			{
				wallList[start] = (this->isWallNode(i,j));
				start++;
			}
		}
	}

	//Order of list:   (-1,-1), (-1,0), (-1,+1), (0,-1), (0,+1), (+1,-1), (+1,0), (+1,+1)
					//	diag.	car.	diag.	  car.	   car.    diag.   car.		diag.
//NO				// NW		N		NE		   W		E		SW		S		  SE			<--- this isn't
	//	REVERSE IT     SE		S		SW		   E		W		NE		N		  NW    <---  this is correct
	//

	if ((wallList[1] && wallList[3] && wallList[4])||(wallList[1]&&wallList[3]&&wallList[6]) || (wallList[1] && wallList[4] && wallList[6]) || (wallList[3] && wallList[4] && wallList[6]))
	{
		return 1.f;
	}

	//TODO: Implement this for the Terrain Analysis project.

	//Note: A cardinal wall is a wall to the East, West, North, or South of (row, col).
	//Note: The sides of the map are considered walls.
	//Being surrounded by 0 cardinal and 0 diagonal walls -> return 0.0f. 
	//Being surrounded by 0 cardinal walls and 1 or more diagonal walls -> return 0.05f.
	//Being surrounded by 1 cardinal wall and any number of diagonal walls -> return 0.10f.
	//Being surrounded by 2 non-adjacent cardinal walls and less than 2 diagonal walls -> return 0.20f.
	//Being surrounded by 2 non-adjacent cardinal walls and 2 or more diagonal walls -> return 0.25f.
	//Being surrounded by 2 adjacent cardinal walls and no adjacent diagonal walls (disregard between cardinal walls) -> return 0.25f.
	//Being surrounded by 2 adjacent cardinal walls and 1 adjacent diagonal wall (not between cardinal walls) -> return 0.40f.
	//Being surrounded by 2 adjacent cardinal walls and 2 adjacent diagonal walls (not between cardinal walls) -> return 0.60f.
	//Being surrounded by 3 of more cardinal walls -> return 1.0f.



	else
	{
		int walls[6] = { 0,0,0,0,0,0 };  //Array to store number of each wall type
		//cardinal, diag, non-adj cardinal, adj cardinal, adj diag, adj diag not between card walls

		//Cardinal walls
		if (wallList[1])
		{//Check for adj cardinal wall  (-1,0)
			//South wall, so check:  (-2,0), (-1,-1), and (-1, +1) -- last two I already have:  wallList[0] & wallList[2]
			//IS card adj wall
			//if (wallList[0] || wallList[2] || this->isWallNode(row-2,col))
			//int numAdjWalls = this->numSurroundingWalls(row - 1, col);
			walls[0]++;
			
			//if(numAdjWalls==0)
			if(wallList[3] || wallList[4])
			{
				walls[3]++;
			}

			else  //Card adj wall
			{
				walls[2]++;// = numAdjWalls;
			}
		}

		if (wallList[3])
		{
			walls[0]++;
			//Check for adj cardinal wall (0,-1)
			///East wall, so check:  (-1, -1), (+1, -1), (0, -2)    Already have first 2:  wallList[0] & wallList[5]
			//int numAdjWalls = this->numSurroundingWalls(row, col-1);
			//if (wallList[0] || wallList[5] || (this->isWallNode(row, col-2)))
			//if (numAdjWalls == 0)
			if(wallList[1]||wallList[6])
			{
				walls[3]++;
			}

			else  //Card adj wall
			{
				walls[2] ++;// = numAdjWalls;
			}
		}

		if (wallList[4])   //West wall  (0,+1)
		{
			walls[0]++;
			//West wall so check:  (0,+2), (-1,+1), and (+1, +1)   Last 2 are:  wallList[7] & wallList[2]
			int numAdjWalls = this->numSurroundingWalls(row, col+1);

			//if (wallList[7] || wallList[2] || (this->isWallNode(row, col+2)))
			//if (numAdjWalls == 0)
			if(wallList[1]||wallList[6])
			{
				walls[3]++;
			}

			else  //Card adj wall
			{
				walls[2] ++;// = numAdjWalls;
			}
		
		}

		if (wallList[6])  //North wall (+1,0)
		{
			walls[0]++;
			//int numAdjWalls = this->numSurroundingWalls(row + 1, col);
			//North wall so check:  (+2,0), (+1,-1), (+1,+1)
			//if (wallList[7] || wallList[5] || this->isWallNode(row+2, col))
			//if (numAdjWalls == 0)
			if(wallList[3]||wallList[4])
			{
				walls[3]++;
			}

		else  //Card adj wall
		{
			walls[2] ++;// = numAdjWalls;
		}
		
		}

		//Diag walls

		if (wallList[0])
		{
			walls[1]++;
			//SE wall, (-1,-1)
			//check for adj
			//check for cardinal walls next to it, so:  (0,-1), (-2,-1), (-1,0), and (-1,-2)-- have 2 already:  wallList[1] and wallList[3]
			//if (wallList[1] || wallList[3] || this->isWallNode(row - 2, col - 1) || this->isWallNode(row - 1, col - 2))  //IS adj diag wall
			int n = this->numSurroundingWalls(row - 1, col - 1);
			//if(n>0)
			if(wallList[1]||wallList[3])
			{
				walls[4]++;

				//check for not in-between cardinal walls--if 2+ cardinal walls, then it IS between cardinal walls--don't increment
				//if (!((wallList[1] && wallList[3]) || (wallList[1] && this->isWallNode(row - 2, col - 1)) || (wallList[3] && this->isWallNode(row - 2, col - 1)) || (wallList[1] && this->isWallNode(row - 1, col - 2)) || (wallList[3] && this->isWallNode(row - 1, col - 2)) || (this->isWallNode(row - 1, col - 2) && this->isWallNode(row - 2, col - 1))))
				//if(n<2)
				if(!(wallList[1]&&wallList[3]))
				{

					walls[5]++;
				}


			}



		}


		if (wallList[2])
		{
			walls[1]++;
			//SW wall, (-1, +1)
			//check for cardinal walls next to it, so:  (0, +1), (-1, 0), (-2, +1), (-1, +2) --- already have 1st 2, wallList[4] and wallList[1]
			int n = this->numSurroundingWalls(row - 1, col + 1);
			//if (wallList[4] || wallList[1] || this->isWallNode(row - 2, col + 1) || this->isWallNode(row - 1, col + 2))
			//if(n>0)
			if(wallList[4]||wallList[1])
			{
				walls[4]++;
				//if (!((wallList[4] && wallList[1]) || (wallList[4] && this->isWallNode(row - 2, col + 1)) || (wallList[1] && this->isWallNode(row - 2, col + 1)) || (wallList[4] && this->isWallNode(row - 1, col + 2)) || (wallList[1] && this->isWallNode(row - 1, col + 2)) || (this->isWallNode(row - 1, col + 2) && this->isWallNode(row - 2, col + 1))))
				//if(n<2)
				if(!(wallList[4]&&wallList[1]))
				{
					walls[5]++;
				}
			}



		}

		if (wallList[5])
		{
			walls[1]++;

			//NE wall (+1, +1)
			//check for cardinal walls next to it, so:  (0, +1), (+1, 0), (+1, +2), and (+2, +1) -- first two are wallList[4] and wallList[6]

			//if (wallList[4] || wallList[6] || this->isWallNode(row + 1, col + 2) || this->isWallNode(row + 2, col + 1))
			//int n = this->numSurroundingWalls(row + 1, col + 1);
			//if(n>0)
			if(wallList[4]||wallList[6])
			{
				walls[4]++;
				//if (!((wallList[4] && wallList[6]) || (wallList[4] && this->isWallNode(row + 1, col + 2)) || (wallList[6] && this->isWallNode(row + 1, col + 2)) || (wallList[4] && this->isWallNode(row + 2, col + 1)) || (wallList[6] && this->isWallNode(row + 2, col + 1)) || (this->isWallNode(row + 1, col + 2) && this->isWallNode(row + 2, col + 1))))
				//if(n<2)
				if(!(wallList[4]&&wallList[6]))
				{
					walls[5]++;
				}
			}

		}

		if (wallList[7])
		{

			walls[1]++;
			//NW wall(+1, -1)
			//check for cardinal walls next to it, so:  (0,-1), (+1, 0), (+2, -1), and (+1, -2):  first two are wallList[3] and wallList[6]
			//if (wallList[3] || wallList[6] || this->isWallNode(row + 2, col - 1) || this->isWallNode(row + 1, col - 2))
			//int n = this->numSurroundingWalls(row + 1, col - 1);
			
			//if(n>0)
			if(wallList[3] || wallList[6])
			{
				walls[4]++;
				//if (!((wallList[3] && wallList[6]) || (wallList[3] && this->isWallNode(row + 2, col - 1)) || (wallList[6] && this->isWallNode(row + 2, col - 1)) || (wallList[3] && this->isWallNode(row + 1, col - 2)) || (wallList[6] && this->isWallNode(row + 1, col - 2)) || (this->isWallNode(row + 2, col - 1) && this->isWallNode(row + 1, col - 2))))
				//if(n<2)
				if(!(wallList[3] && wallList[6]))
				{
					walls[5]++;
				}

			}

		}


		//TODO: Implement this for the Terrain Analysis project.

		//Note: A cardinal wall is a wall to the East, West, North, or South of (row, col).
		//Note: The sides of the map are considered walls.
		//Being surrounded by 0 cardinal and 0 diagonal walls -> return 0.0f. 
		//Being surrounded by 0 cardinal walls and 1 or more diagonal walls -> return 0.05f.
		//Being surrounded by 1 cardinal wall and any number of diagonal walls -> return 0.10f.
		//Being surrounded by 2 non-adjacent cardinal walls and less than 2 diagonal walls -> return 0.20f.
		//Being surrounded by 2 non-adjacent cardinal walls and 2 or more diagonal walls -> return 0.25f.
		//Being surrounded by 2 adjacent cardinal walls and no adjacent diagonal walls (disregard between cardinal walls) -> return 0.25f.
		//Being surrounded by 2 adjacent cardinal walls and 1 adjacent diagonal wall (not between cardinal walls) -> return 0.40f.
		//Being surrounded by 2 adjacent cardinal walls and 2 adjacent diagonal walls (not between cardinal walls) -> return 0.60f.
		//Being surrounded by 3 of more cardinal walls -> return 1.0f.


		//cardinal, diag, non-adj cardinal, adj cardinal, adj diag, adj diag not between card walls


		//Adj diag wall not between cardinal walls is off?  *** HERE *** HERE ***


		if(walls[0]>2)
		{
			return 1.f;
		}

		else if(walls[3] ==2)
		{
			if (walls[5] >= 2)
			{
				return 0.6f;
			}
			else if(walls[5]==1)
			{
				return 0.4f;
			}
			else// if(walls[4]==0 && walls[5]==0)//if(walls[5]==0)
			{
				return 0.25f;
			}

		}

		else if (walls[2]==2)
		{
			if(walls[1]>=2)
			{
				return 0.25f;
			}

			else
			{
				return 0.20f;

			}
		}

		else if(walls[0]==1)
		{
			if (walls[1] > 0) { return 0.1f; }
			else { return 0.f; }
		}

		else
		{
			if (walls[1] > 0) { return 0.05f; }
			else { return 0.f; }
		}



		//if (walls[5] >= 2)// && walls[3] >= 2)
		//{
		//	if (walls[3] >= 2)
		//	{
		//		return 0.60f;
		//	}
		//	else if (walls[3] >= 1)
		//	{
		//		return 0.40f;
		//	}

		//	else
		//	{
		//		return 0.25f;
		//	}
		//}

		//else if (walls[2] >= 2)
		//{
		//	if (walls[2] >= 2)
		//	{
		//		return 0.25f;
		//	}

		//	else
		//	{
		//		return 0.20f;
		//	}
		//}

		//if (walls[0] == 1 && walls[1] > 0)
		//{
		//	return 0.10f;
		//}

		//else
		//{
		//	if (walls[1] > 0)
		//	{
		//		return 0.05f;
		//	}

		//	else
		//	{
		//		return 0.0f;
		//	}
		//}



		//TODO: Implement this for the Terrain Analysis project.

		//Note: A cardinal wall is a wall to the East, West, North, or South of (row, col).
		//Note: The sides of the map are considered walls.
		//Being surrounded by 0 cardinal and 0 diagonal walls -> return 0.0f. 
		//Being surrounded by 0 cardinal walls and 1 or more diagonal walls -> return 0.05f.
		//Being surrounded by 1 cardinal wall and any number of diagonal walls -> return 0.10f.
		//Being surrounded by 2 non-adjacent cardinal walls and less than 2 diagonal walls -> return 0.20f.
		//Being surrounded by 2 non-adjacent cardinal walls and 2 or more diagonal walls -> return 0.25f.
		//Being surrounded by 2 adjacent cardinal walls and no adjacent diagonal walls (disregard between cardinal walls) -> return 0.25f.
		//Being surrounded by 2 adjacent cardinal walls and 1 adjacent diagonal wall (not between cardinal walls) -> return 0.40f.
		//Being surrounded by 2 adjacent cardinal walls and 2 adjacent diagonal walls (not between cardinal walls) -> return 0.60f.
		//Being surrounded by 3 of more cardinal walls -> return 1.0f.


		//WRITE YOUR CODE HERE



		return 0.0f;	//REPLACE THIS
	}
}

void Terrain::AnalyzeOpennessClosestWall( void )   //DONE- test it - test SUCCESSFUL
{
	//TODO: Implement this for the Terrain Analysis project.

	//Mark every square on the terrain (m_terrainInfluenceMap[r][c]) with
	//the value 1/(d*d), where d is the distance to the closest wall in 
	//row/column grid space.
	//Edges of the map count as walls!

	for(int i=0;i<g_terrain.GetWidth();i++)
	{
		for (int j = 0; j < g_terrain.GetWidth(); j++)
		{
			
			

			//m_terrainInfluenceMap[i][j] = 1.f / powf(this->ClosestWall(i, j), 2);
			this->SetInfluenceMapValue(i, j, 1.f / powf(this->ClosestWall(i, j), 2));
		}
	}


	//WRITE YOUR CODE HERE

}

void Terrain::AnalyzeOpennessRearCover( void )  
{
	//Note: You don't need to change this function, but you do need to
	//      write the RearCoverValue function.

	//Mark every square on the terrain (m_terrainInfluenceMap[r][c])
	//with the value returned from RearCoverValue()

	for( int r=0; r<m_width; r++ )
	{
		for( int c=0; c<m_width; c++ )
		{
			if( !IsWall( r, c ) )
			{
				m_terrainInfluenceMap[r][c] = RearCoverValue( r, c );
			}
		}
	}
}

void Terrain::AnalyzeVisibility( void )  //DONE- test it - test SUCCESSFUL
{
	//TODO: Implement this for the Terrain Analysis project.

	//Mark every square on the terrain (m_terrainInfluenceMap[r][c]) with
	//the number of grid squares (that are visible to the square in question)
	//divided by 160 (a magic number that looks good). Cap the value at 1.0.
	
	//Two grid squares are visible to each other if a line between 
	//their centerpoints doesn't intersect the four boundary lines
	//of every walled grid square. Put this code in IsClearPath().


	//WRITE YOUR CODE HERE
	

	for(int i=0; i<g_terrain.GetWidth();i++)
	{
		for (int j = 0; j < g_terrain.GetWidth(); j++)
		{

			int visibleSquares = 0;
			if (!g_terrain.IsWall(i, j))
			{
				for (int x = 0; x < g_terrain.GetWidth(); x++)
				{
					for (int y = 0; y < g_terrain.GetWidth(); y++)
					{

						if (this->IsClearPath(i, j, x, y))
						{
							visibleSquares++;
							if(visibleSquares >=160)
							{
								x = g_terrain.GetWidth() + 1;
								y = x;
								//i = x;
								//j = x;
							}
						}

					}
				}
			}

			//m_terrainInfluenceMap[i][j] = fmaxf(visibleSquares / 160.f, 1.f);
			this->SetInfluenceMapValue(i,j, fminf(visibleSquares / 160.f, 1.f));


		}
	}


}

void Terrain::AnalyzeRearCoverWithHighVisibility( void )   //DONE- test it - Successful
{
	//TODO: Implement this for the Terrain Analysis project.

	//Multiply the results from the AnalyzeOpennessRearCover function
	//and the AnalyzeVisibility function.
	
	this->AnalyzeVisibility();

	for(int i=0;i<g_terrain.GetWidth();i++)
	{
		for (int j = 0; j < g_terrain.GetWidth(); j++)
		{
			m_terrainInfluenceMap[i][j] *= this->RearCoverValue(i, j);
		}
	}

	//WRITE YOUR CODE HERE

}

void Terrain::AnalyzeVisibleToPlayer( void )    //DONE- test it
{
	//TODO: Implement this for the Terrain Analysis project.

	//For every square on the terrain (m_terrainInfluenceMap[r][c])
	//that is visible to the player square, mark it with 1.0.
	//For all non 1.0 squares that are visible and next to 1.0 squares,
	//mark them with 0.5. Otherwise, the square should be marked with 0.0.
	
	//Two grid squares are visible to each other if a line between 
	//their centerpoints doesn't intersect the four boundary lines
	//of every walled grid square. Put this code in IsClearPath().


	//WRITE YOUR CODE HERE

	
	this->ResetInfluenceMap();
	
	
	for (int i = 0; i < g_terrain.GetWidth(); i++)
	{
		for (int j = 0; j < g_terrain.GetWidth(); j++)
		{
			//float valueToStore = 0.f;
			if (!g_terrain.IsWall(i, j))
			{
				if (this->IsClearPath(m_rPlayer, m_cPlayer, i, j))
				{
					//m_terrainInfluenceMap[i][j] = 1.f;
					this->SetInfluenceMapValue(i, j, 1.f);

					for (int x = i - 1; x <= i + 1; x++)
					{
						for (int y = j - 1; y <= j + 1; y++)
						{
							if (this->isValidNode(x, y) && m_terrainInfluenceMap[x][y] < 1.f && this->IsClearPath(i, j, x, y))
							{
								m_terrainInfluenceMap[x][y] = 0.5f;
							}
						}

					}
				}

			}
		}
	}
	

	
}

void Terrain::AnalyzeSearch( void )
{
	//TODO: Implement this for the Terrain Analysis project.

	//For every square on the terrain (m_terrainInfluenceMap[r][c])
	//that is visible by the player square, mark it with 1.0.
	//Otherwise, don't change the value (because it will get
	//decremented with time in the update call).
	//You must consider the direction the player is facing:
	//D3DXVECTOR2 playerDir = D3DXVECTOR2(m_dirPlayer.x, m_dirPlayer.z)
	//Give the player a field of view a tad greater than 180 degrees.
	
	//Two grid squares are visible to each other if a line between 
	//their centerpoints doesn't intersect the four boundary lines
	//of every walled grid square. Put this code in IsClearPath().



	//WRITE YOUR CODE HERE


	//DOT PRODUCT THING:  Dot product of vecA & vecB = magnitude(A) * magnitude(B) * cos(angle between A&B)
	//angle will be ~180
	//cos(180) = -1
	//Tiny moves on a 2D plane, playerDir = (x,z), ignore y
	//Therefore, the vectors to use for 180deg vision are the two vectors perpindicular to (x,z) ->   (-z,x) and (z,-x)
	

	//int leftRayX = -1 * m_dirPlayer.z;
	//int leftRayY = m_dirPlayer.x;
	//int rightRayX = m_dirPlayer.z;
	//int rightRayY = -1 * m_dirPlayer.x;
	D3DXVECTOR2 playerDir = D3DXVECTOR2(m_dirPlayer.x, m_dirPlayer.z);
	D3DXVECTOR3 currentPos;
	D3DXVECTOR3 playerPos = this->GetCoordinates(m_rPlayer, m_cPlayer);
	D3DXVECTOR2 playerToObject;
	for (int i = 0; i < g_terrain.GetWidth(); i++)
	{
		for (int j = 0; j < g_terrain.GetWidth(); j++)
		{
			//Vector from A -> B = B-A
		 currentPos = this->GetCoordinates(i,j);
		 playerToObject = D3DXVECTOR2(currentPos.x - playerPos.x, currentPos.z - playerPos.z);

			if(!g_terrain.IsWall(i,j)&&(D3DXVec2Dot(&playerToObject, &playerDir))> -0.0000001 && this->IsClearPath(m_rPlayer, m_cPlayer, i,j))
			{
				m_terrainInfluenceMap[i][j] = 1.f;
			}
		}
	}






}

void Terrain::ClearTerrainAnalysis( void )
{
	for( int r=0; r<m_width; r++ )
	{
		for( int c=0; c<m_width; c++ )
		{
			m_terrainInfluenceMap[r][c] = 0.0f;
		}
	}
}

bool Terrain::IsClearPath( int r0, int c0, int r1, int c1 )  //DONE- test it - test SUCCESSFUL
{



	//TODO: Implement this for the Terrain Analysis project.

	//Two grid squares (r0,c0) and (r1,c1) are visible to each other 
	//if a line between their centerpoints doesn't intersect the four 
	//boundary lines of every walled grid square. Make use of the
	//function LineIntersect(). You should also puff out the four
	//boundary lines by a very tiny bit so that a diagonal line passing
	//by the corner will intersect it.


	//WRITE YOUR CODE HERE
	




#define EPSILON 0.000000000001f
	
	for(int i=0;i<g_terrain.GetWidth();i++)
	{
		for (int j = 0; j < g_terrain.GetWidth(); j++)
		{
			if (g_terrain.IsWall(i, j))// && (this->LineIntersect(r0,c0, r1,c1,))
			{
				float topLeftX = i - (0.5f + EPSILON);
				float topLeftY = j + (0.5f + EPSILON);
				float topRightX = i + (0.5f + EPSILON);
				float topRightY = j + (0.5f + EPSILON);
				float botLeftX = i - (0.5f + EPSILON);
				float botLeftY = j - (0.5f + EPSILON);
				float botRightX = i + (0.5f + EPSILON);
				float botRightY = j - (0.5f + EPSILON);

				if (this->LineIntersect(r0, c0, r1, c1, topLeftX, topLeftY, topRightX, topRightY) || this->LineIntersect(r0, c0, r1, c1, topLeftX, topLeftY, botLeftX, botLeftY) || this->LineIntersect(r0, c0, r1, c1, topRightX, topRightY, botRightX, botRightY) || this->LineIntersect(r0, c0, r1, c1, botLeftX, botLeftY, botRightX, botRightY))
				{
					return false;
				}
				
			}
		}
	}


	return true;	//REPLACE THIS
}

// Determine the intersection point of two line segments
// Return FALSE if the lines don't intersect
#define ABS(a) (((a) < 0) ? -(a) : (a))
bool Terrain::LineIntersect( float x1, float y1, float x2, float y2,
							 float x3, float y3, float x4, float y4 )
{
	float y4y3 = y4 - y3;
	float y1y3 = y1 - y3;
	float y2y1 = y2 - y1;
	float x4x3 = x4 - x3;
	float x2x1 = x2 - x1;
	float x1x3 = x1 - x3;
	float denom  = y4y3 * x2x1 - x4x3 * y2y1;
	float numera = x4x3 * y1y3 - y4y3 * x1x3;
	float numerb = x2x1 * y1y3 - y2y1 * x1x3;

	const float eps = 0.0001f;
	if (ABS(numera) < eps && ABS(numerb) < eps && ABS(denom) < eps)
	{	//Lines coincident (on top of each other)
		return true;
	}

	if (ABS(denom) < eps)
	{	//Lines parallel
		return false;
	}

	float mua = numera / denom;
	float mub = numerb / denom;
	if (mua < 0 || mua > 1 || mub < 0 || mub > 1)
	{	//No intersection
		return false;
	}
	else
	{	//Intersection
		return true;
	}
}

int Terrain::GetWidth() const
{
	return m_width;
}

void Terrain::InitFogOfWar()
{
	for (int i = 0; i < m_width; ++i)
		for (int j = 0; j < m_width; ++j)
			if (m_terrain[i][j] == TILE_WALL)
				m_terrain[i][j] = TILE_WALL_INVISIBLE;
}

void Terrain::ClearFogOfWar()
{
	for (int i = 0; i < m_width; ++i)
		for (int j = 0; j < m_width; ++j)
			if (m_terrain[i][j] == TILE_WALL_INVISIBLE)
				m_terrain[i][j] = TILE_WALL;
}