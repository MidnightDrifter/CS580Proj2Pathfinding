/* Copyright Steve Rabin, 2012. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2012"
 */

#pragma warning(disable: 4995)

#include "DXUT.h"
#include "world.h"
#include "time.h"
#include "database.h"
#include "msgroute.h"
#include "gameobject.h"
#include "movement.h"
#include "debuglog.h"
#include "debugdrawing.h"
#include "terrain.h"
#include "MultiAnimation.h"
#include "Tiny.h"

// State machines
#include "agent.h"

// Unit test state machines
#include "unittest1.h"
#include "unittest2a.h"
#include "unittest2b.h"
#include "unittest2c.h"
#include "unittest3a.h"
#include "unittest3b.h"
#include "unittest4.h"
#include "unittest5.h"
#include "unittest6.h"

//#define UNIT_TESTING


World::World(void)
: m_initialized(false)
{

}

World::~World(void)
{
	delete m_time;
	delete m_database;
	delete m_msgroute;
	delete m_debuglog;
	delete m_debugdrawing;
	delete m_terrain;
}

void World::InitializeSingletons( void )
{
	//Create Singletons
	m_time = new Time();
	m_database = new Database();
	m_msgroute = new MsgRoute();
	m_debuglog = new DebugLog();
	m_debugdrawing = new DebugDrawing();
	m_terrain = new Terrain();
}

void World::Initialize( CMultiAnim *pMA, std::vector< CTiny* > *pv_pChars, CSoundManager *pSM, double dTimeCurrent )
{
	if(!m_initialized)
	{
		m_initialized = true;
	
		g_terrain.Create();


#ifdef UNIT_TESTING

		//Create unit test game objects
		GameObject* unittest1 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest1" );
		GameObject* unittest2 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest2" );
		GameObject* unittest3a = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest3a" );
		GameObject* unittest3b = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest3b" );
		GameObject* unittest4 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest4" );
		GameObject* unittest5 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest5" );
		GameObject* unittest6 = new GameObject( g_database.GetNewObjectID(), OBJECT_Ignore_Type, "UnitTest6" );
		
		D3DXVECTOR3 pos(0.0f, 0.0f, 0.0f);
		unittest1->CreateBody( 100, pos );
		unittest2->CreateBody( 100, pos );
		unittest3a->CreateBody( 100, pos );
		unittest3b->CreateBody( 100, pos );
		unittest4->CreateBody( 100, pos );
		unittest5->CreateBody( 100, pos );
		unittest6->CreateBody( 100, pos );

		unittest1->CreateStateMachineManager();
		unittest2->CreateStateMachineManager();
		unittest3a->CreateStateMachineManager();
		unittest3b->CreateStateMachineManager();
		unittest4->CreateStateMachineManager();
		unittest5->CreateStateMachineManager();
		unittest6->CreateStateMachineManager();
		
		g_database.Store( *unittest1 );
		g_database.Store( *unittest2 );
		g_database.Store( *unittest3a );
		g_database.Store( *unittest3b );
		g_database.Store( *unittest4 );
		g_database.Store( *unittest5 );
		g_database.Store( *unittest6 );

		//Give the unit test game objects a state machine
		unittest1->GetStateMachineManager()->PushStateMachine( *new UnitTest1( *unittest1 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest2->GetStateMachineManager()->PushStateMachine( *new UnitTest2a( *unittest2 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest3a->GetStateMachineManager()->PushStateMachine( *new UnitTest3a( *unittest3a ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest3b->GetStateMachineManager()->PushStateMachine( *new UnitTest3b( *unittest3b ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest4->GetStateMachineManager()->PushStateMachine( *new UnitTest4( *unittest4 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest5->GetStateMachineManager()->PushStateMachine( *new UnitTest5( *unittest5 ), STATE_MACHINE_QUEUE_0, TRUE );
		unittest6->GetStateMachineManager()->PushStateMachine( *new UnitTest6( *unittest6 ), STATE_MACHINE_QUEUE_0, TRUE );

#else

	{	//Agent
		GameObject* agent = new GameObject( g_database.GetNewObjectID(), OBJECT_Player, "Player" );
		D3DXVECTOR3 pos(0.1f, 0.0f, 0.1f);
		agent->CreateBody( 100, pos );
		agent->CreateMovement();
		agent->CreateTiny( pMA, pv_pChars, pSM, dTimeCurrent );
		g_database.Store( *agent );
		agent->CreateStateMachineManager();
		agent->GetStateMachineManager()->PushStateMachine( *new Agent( *agent ), STATE_MACHINE_QUEUE_0, true );
	}


#endif

	}
}


void World::PostInitialize()
{
	g_database.Initialize();
}


void World::Update()
{
	g_time.MarkTimeThisTick();
	g_database.Update();
}

void World::Animate( double dTimeDelta )
{
	g_database.Animate( dTimeDelta );
}

void World::AdvanceTimeAndDraw( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj, double dTimeDelta, D3DXVECTOR3 *pvEye )
{
	g_database.AdvanceTimeAndDraw( pd3dDevice, pViewProj, dTimeDelta, pvEye );
	g_terrain.DrawDebugVisualization( pd3dDevice );
}

void World::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	return( g_database.RestoreDeviceObjects( pd3dDevice ) );
}

void World::InvalidateDeviceObjects( void )
{
	g_database.InvalidateDeviceObjects();
}
