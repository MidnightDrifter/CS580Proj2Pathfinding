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
  m_rPlayer(-1),
  m_cPlayer(-1),
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

float Terrain::ClosestWall( int row, int col )
{
	//TODO: Helper function for the Terrain Analysis project (you'll likely need it).

	//Return the distance to the closest wall or side of the map.

	return 0.0f;	//REPLACE THIS
}

float Terrain::RearCoverValue( int row, int col )
{
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

void Terrain::AnalyzeOpennessClosestWall( void )
{
	//TODO: Implement this for the Terrain Analysis project.

	//Mark every square on the terrain (m_terrainInfluenceMap[r][c]) with
	//the value 1/(d*d), where d is the distance to the closest wall in 
	//row/column grid space.
	//Edges of the map count as walls!

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

void Terrain::AnalyzeVisibility( void )
{
	//TODO: Implement this for the Terrain Analysis project.

	//Mark every square on the terrain (m_terrainInfluenceMap[r][c]) with
	//the number of grid squares (that are visible to the square in question)
	//divided by 160 (a magic number that looks good). Cap the value at 1.0.
	
	//Two grid squares are visible to each other if a line between 
	//their centerpoints doesn't intersect the four boundary lines
	//of every walled grid square. Put this code in IsClearPath().


	//WRITE YOUR CODE HERE

}

void Terrain::AnalyzeRearCoverWithHighVisibility( void )
{
	//TODO: Implement this for the Terrain Analysis project.

	//Multiply the results from the AnalyzeOpennessRearCover function
	//and the AnalyzeVisibility function.


	//WRITE YOUR CODE HERE

}

void Terrain::AnalyzeVisibleToPlayer( void )
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

bool Terrain::IsClearPath( int r0, int c0, int r1, int c1 )
{
	//TODO: Implement this for the Terrain Analysis project.

	//Two grid squares (r0,c0) and (r1,c1) are visible to each other 
	//if a line between their centerpoints doesn't intersect the four 
	//boundary lines of every walled grid square. Make use of the
	//function LineIntersect(). You should also puff out the four
	//boundary lines by a very tiny bit so that a diagonal line passing
	//by the corner will intersect it.


	//WRITE YOUR CODE HERE


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