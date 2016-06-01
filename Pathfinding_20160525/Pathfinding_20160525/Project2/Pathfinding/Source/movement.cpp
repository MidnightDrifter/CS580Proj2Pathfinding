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
#include "database.h"
#include "movement.h"
#include "gameobject.h"
#include "body.h"
#include "tiny.h"
#include "debugdrawing.h"
#include "terrain.h"
#include "time.h"


Movement::Movement( GameObject& owner )
: m_owner( &owner ),
  m_speedWalk( 1.f / 5.7f ),
  m_speedJog( 1.f / 2.3f ),
  m_movementMode( MOVEMENT_SEEK_TARGET ),
  m_smooth(false),
  m_rubberband(false),
  m_straightline(false),
  m_singleStep(true),
  m_extracredit(false),
  m_aStarUsesAnalysis(false),
  m_heuristicWeight(1.0f),
  m_heuristicCalc(0),
  m_fogOfWar(false),
	m_AStarGrid(AStar(g_terrain.GetWidth(), g_terrain.GetWidth()))
{
	m_target.x = m_target.y = m_target.z = 0.0f;
	
}

Movement::~Movement( void )
{

}

void Movement::Animate( double dTimeDelta )
{
	if( m_owner->GetBody().GetSpeed() == 0.0f )
	{
		m_owner->GetTiny().SmoothLoiter();
	}
	else
	{
		D3DXVECTOR3 pos = m_owner->GetBody().GetPos();

		if( m_movementMode == MOVEMENT_SEEK_TARGET )
		{
			D3DXVECTOR3 toTarget = m_target - pos;

			if( D3DXVec3Length( &toTarget ) < 0.01f )
			{	//Notify target reached
				//m_owner->GetBody().SetPos( m_target );
				g_database.SendMsgFromSystem( m_owner, MSG_Arrived );
			}
			else
			{	
				//Point character towards target this frame
				D3DXVECTOR3 dir;
				D3DXVec3Normalize( &dir, &toTarget );
				m_owner->GetBody().SetDir( dir );

				//Move character towards target this frame
				double speedScale = m_owner->GetTiny().GetSpeedScale();
				D3DXVec3Scale( &dir, &dir, float( m_owner->GetBody().GetSpeed() * speedScale * dTimeDelta ) );

				D3DXVECTOR3 newPos;
				D3DXVec3Add( &newPos, &dir, &m_owner->GetBody().GetPos() );
				m_owner->GetBody().SetPos( newPos );
			}
		}
		else if( m_movementMode == MOVEMENT_WAYPOINT_LIST )
		{
			D3DXVECTOR3 target = m_waypointList.front();
			D3DXVECTOR3 toTarget = target - pos;

			D3DXVECTOR3 goal = m_waypointList.back();
			D3DXVECTOR3 toGoal = goal - pos;

			if( D3DXVec3Length( &toGoal ) < 0.01f )
			{	//Notify goal reached
				m_waypointList.clear();
				g_database.SendMsgFromSystem( m_owner, MSG_Arrived );
			}
			else
			{	
				if( D3DXVec3Length( &toTarget ) < 0.01f )
				{	//Target reached - get new target
					m_waypointList.pop_front();
					target = m_waypointList.front();
					toTarget = target - pos;
				}


				//Point character towards target this frame
				D3DXVECTOR3 dir;
				D3DXVec3Normalize( &dir, &toTarget );
				m_owner->GetBody().SetDir( dir );

				//Move character towards target this frame
				double speedScale = m_owner->GetTiny().GetSpeedScale();
				D3DXVec3Scale( &dir, &dir, float( m_owner->GetBody().GetSpeed() * speedScale * dTimeDelta * 20.f / g_terrain.GetWidth() ) );

				D3DXVECTOR3 newPos;
				D3DXVec3Add( &newPos, &dir, &m_owner->GetBody().GetPos() );
				m_owner->GetBody().SetPos( newPos );
			}
		}
	}

	m_owner->GetTiny().SetOrientation();

	g_terrain.UpdatePlayerPos(m_owner->GetBody().GetPos(), m_owner->GetBody().GetDir());
}

void Movement::SetIdleSpeed( void )
{
	m_owner->GetBody().SetSpeed( 0.0f );
	m_owner->GetTiny().SetIdleKey( true );
}

void Movement::SetWalkSpeed( void )
{
	m_owner->GetBody().SetSpeed( m_speedWalk );
	m_owner->GetTiny().SetMoveKey();
}

void Movement::SetJogSpeed( void )
{
	m_owner->GetBody().SetSpeed( m_speedJog );
	m_owner->GetTiny().SetMoveKey();
}

bool Movement::ComputePathWithTiming( int r, int c, bool newRequest )
{
	if(newRequest) { g_time.ClearStopwatchPathfinding(); }
	g_time.StartStopwatchPathfinding();
	bool foundPath = ComputePath( r, c, newRequest );
	g_time.StopStopwatchPathfinding();
	
	return foundPath;
}

bool Movement::ComputePath( int r, int c, bool newRequest )
{
	m_goal = g_terrain.GetCoordinates( r, c );
	m_movementMode = MOVEMENT_WAYPOINT_LIST;

	bool useAStar = false;
	if( useAStar )
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////
		//INSERT YOUR A* CODE HERE
		//1. You should probably make your own A* class.
		//2. You will need to make this function remember the current progress if it preemptively exits.
		//3. Return "true" if the path is complete, otherwise "false".
		///////////////////////////////////////////////////////////////////////////////////////////////////

		return false;
	}
	else
	{	
		//Randomly meander toward goal (might get stuck at wall)
		int curR, curC;
		D3DXVECTOR3 cur = m_owner->GetBody().GetPos();
		g_terrain.GetRowColumn( &cur, &curR, &curC );

		m_waypointList.clear();
		m_waypointList.push_back( cur );

		int countdown = 100;
		while( curR != r || curC != c )
		{
			if( countdown-- < 0 ) { break; }

			if( curC == c || (curR != r && rand()%2 == 0) )
			{	//Go in row direction
				int last = curR;
				if( curR < r ) { curR++; }
				else { curR--; }

				if( g_terrain.IsWall( curR, curC ) )
				{
					curR = last;
					continue;
				}
			}
			else
			{	//Go in column direction
				int last = curC;
				if( curC < c ) { curC++; }
				else { curC--; }

				if( g_terrain.IsWall( curR, curC ) )
				{
					curC = last;
					continue;
				}
			}

			D3DXVECTOR3 spot = g_terrain.GetCoordinates( curR, curC );
			m_waypointList.push_back( spot );
			g_terrain.SetColor( curR, curC, DEBUG_COLOR_YELLOW );
		}
		return true;
	}
}

void Movement::DrawDebugVisualization( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj )
{
	//Draw waypoints
	if( !m_waypointList.empty() )
	{
		D3DXVECTOR3 p0, p1;
		D3DXVECTOR3 last = *m_waypointList.begin();
		D3DXVECTOR3 cur = m_owner->GetBody().GetPos();
		p0 = last;
		cur.y = p0.y = 0.01f;
		g_debugdrawing.DrawLine( pd3dDevice, pViewProj, cur, p0, DEBUG_COLOR_RED, true );

		WaypointList::iterator i = m_waypointList.begin();
		for( WaypointList::iterator i = m_waypointList.begin(); i != m_waypointList.end(); i++ )
		{
			p0 = *i;
			p0.y = 0.01f;
			p1 = p0;
			p1.y += 0.03f;
			g_debugdrawing.DrawLine( pd3dDevice, pViewProj, p0, p1, DEBUG_COLOR_BLACK, false );
			g_debugdrawing.DrawLine( pd3dDevice, pViewProj, last, p0, DEBUG_COLOR_RED, true );
			last = p0;
		}
	}

	//Draw goal circle
	if( m_movementMode == MOVEMENT_WAYPOINT_LIST )
	{
		D3DXVECTOR3 cur = m_goal;
		cur.y = 0.006f;

        const int width = g_terrain.GetWidth();
        const float radius = 1.f / width / 8.f;

		g_debugdrawing.DrawCircle( pd3dDevice, pViewProj, &cur, radius, DEBUG_COLOR_WHITE );
		g_debugdrawing.DrawCircle( pd3dDevice, pViewProj, &cur, radius*2, DEBUG_COLOR_BLACK );
		g_debugdrawing.DrawCircle( pd3dDevice, pViewProj, &cur, radius*3, DEBUG_COLOR_WHITE );
		g_debugdrawing.DrawCircle( pd3dDevice, pViewProj, &cur, radius*4, DEBUG_COLOR_BLACK );
		g_debugdrawing.DrawCircle( pd3dDevice, pViewProj, &cur, radius*5, DEBUG_COLOR_WHITE );
	}
}