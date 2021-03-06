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

float Movement::DIAG_DISTANCE = sqrtf(2);
int Movement::HV_DISTANCE = 1;
Movement::Movement(GameObject& owner)
	: m_owner(&owner),
	m_speedWalk(1.f / 5.7f),
	m_speedJog(1.f / 2.3f),
	m_movementMode(MOVEMENT_SEEK_TARGET),
	m_smooth(false),
	m_rubberband(false),
	m_straightline(false),
	m_singleStep(true),
	m_extracredit(false),
	m_aStarUsesAnalysis(false),
	m_heuristicWeight(1.0f),
	m_heuristicCalc(0),
	m_fogOfWar(false),
//	m_AStarGrid(AStar(g_terrain.GetWidth(), g_terrain.GetWidth())),
	m_rubberBandList(new std::list<D3DXVECTOR3>()),
	m_tempVector(new D3DXVECTOR3()),
	m_splineNodesList(new std::list<D3DXVECTOR3>),
	m_splineNodesVector(new std::vector<D3DXVECTOR3>),
	m_firstTempSmoothingVector(new D3DXVECTOR3()),
	m_secondTempSmoothingVector(new D3DXVECTOR3()),
	//m_AStarV2(AStarV2()),
	//m_AStarV3(AStarV3()),
	m_AStarV4(AStarV4())
	
{
	

	m_target.x = m_target.y = m_target.z = 0.0f;
	//m_AStarGrid.initialize();
	


}

Movement::~Movement( void )
{
	m_rubberBandList->clear();
	delete m_rubberBandList;
	m_splineNodesList->clear();
	delete m_splineNodesList;
	m_splineNodesVector->clear();
	delete m_splineNodesVector;
	delete m_firstTempSmoothingVector;
	delete m_secondTempSmoothingVector;
	delete m_tempVector;
	//m_waypointList.clear();
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

bool Movement::ComputePath(int r, int c, bool newRequest)
{
	m_goal = g_terrain.GetCoordinates(r, c);
	m_movementMode = MOVEMENT_WAYPOINT_LIST;

	int curR, curC;
	D3DXVECTOR3 cur = m_owner->GetBody().GetPos();
	g_terrain.GetRowColumn(&cur, &curR, &curC);

	if (this->GetStraightlinePath() && newRequest)
	{
		bool straightLineExists = true;
		for (int i = min(r, curR); i <= max(r, curR); i++)
		{
			for (int j = min(curC, c); j <= max(curC, c); j++)
			{
				if (g_terrain.IsWall(i, j))
				{
					straightLineExists = false;
					j = g_terrain.GetWidth() + 1;
					i = j;
				}
			}
		}


		if (straightLineExists)
		{
			m_waypointList.clear();
			m_waypointList.push_front(D3DXVECTOR3(g_terrain.GetCoordinates(r, c)));
			m_waypointList.push_front(D3DXVECTOR3(g_terrain.GetCoordinates(curR, curC)));
			this->editAStarV4().clear();
			return true;
		}

	}





	bool useAStar = true;
	if (useAStar)
	{
		AStarV4& myAStarV4 = this->editAStarV4();


		if (newRequest)
		{
			m_waypointList.clear();
			g_terrain.ResetColors();
		}

		bool pathFound = myAStarV4.findPath(newRequest, this->GetSingleStep(), this->GetHeuristicCalc(), this->GetHeuristicWeight(), curR, curC, r, c, this->GetAnalysis(), this->GetFogOfWar());

		if (!pathFound && !this->GetSingleStep())
		{
			//Isn't single step and no path is found, no path exists
			myAStarV4.clear();
			m_waypointList.push_front(D3DXVECTOR3(g_terrain.GetCoordinates(curR, curC)));

			return true;
		}

		else if (!pathFound && myAStarV4.getSizeOfOpenList() == 0)
		{
			myAStarV4.clear();
			m_waypointList.push_front(D3DXVECTOR3(g_terrain.GetCoordinates(curR, curC)));
			//m_waypointList.clear();
			return true;
		}

		else if (pathFound)
		{
			AStarNodeV3 goal = myAStarV4.getMapNode(myAStarV4.getGoalRow(), myAStarV4.getGoalCol());


			if (this->GetRubberbandPath())
			{
				bool straightLineExists1 = true;
				for (int i = min(r, curR); i <= max(r, curR); i++)
				{
					for (int j = min(curC, c); j <= max(curC, c); j++)
					{
						if (g_terrain.IsWall(i, j))
						{
							straightLineExists1 = false;
							j = g_terrain.GetWidth() + 1;
							i = j;
						}
					}
				}


				if (straightLineExists1)
				{
					m_waypointList.push_front(D3DXVECTOR3(g_terrain.GetCoordinates(r, c)));
					m_waypointList.push_front(D3DXVECTOR3(g_terrain.GetCoordinates(curR, curC)));
				}

				else
				{
					myAStarV4.rubberband();
					if (myAStarV4.getRubberbandList().size() >= 3)
					{
						std::list<AStarNodeV3>::iterator st = myAStarV4.getRubberbandList().begin();

						while (st != myAStarV4.getRubberbandList().end())
						{
							m_waypointList.push_back(D3DXVECTOR3(g_terrain.GetCoordinates((*st).getX(), (*st).getY())));
							++st;

						}
					}

					else
					{
						while (!(goal.getX() == myAStarV4.getStartRow() && goal.getY() == myAStarV4.getStartCol()))
						{
							m_waypointList.push_front(D3DXVECTOR3(g_terrain.GetCoordinates(goal.getX(), goal.getY())));
							goal = myAStarV4.getMapNode(goal.getParentX(), goal.getParentY());
						}


						m_waypointList.push_front(D3DXVECTOR3(g_terrain.GetCoordinates(myAStarV4.getStartRow(), myAStarV4.getStartCol())));
					}



				}
			}
			else {
				while (!(goal.getX() == myAStarV4.getStartRow() && goal.getY() == myAStarV4.getStartCol()))
				{
					m_waypointList.push_front(D3DXVECTOR3(g_terrain.GetCoordinates(goal.getX(), goal.getY())));
					goal = myAStarV4.getMapNode(goal.getParentX(), goal.getParentY());
				}


				m_waypointList.push_front(D3DXVECTOR3(g_terrain.GetCoordinates(myAStarV4.getStartRow(), myAStarV4.getStartCol())));

			}
			myAStarV4.clear();
			//myAStarV4.clearMap();
			//myAStarV4.clearOpenList();
			//m_AStarV3.clearMap();
			//m_AStarV3.clearOpenList();

			//Checking the 1.5 size based on the GRID not the actual 3D space
			//Use (1/gridwidth) * 1.5 to check
			if (this->GetSmoothPath() && this->GetRubberbandPath() && m_waypointList.size() >= 3)
			{




				std::list<D3DXVECTOR3>::iterator first = m_waypointList.begin();
				std::list<D3DXVECTOR3>::iterator second = m_waypointList.begin();
				second++;
				std::list<D3DXVECTOR3>::iterator end = m_waypointList.end();


				do
				{
					if (first != end && second != end && sqrtf(pow(((*first).x - (*second).x), 2) + pow(((*first).z - (*second).z), 2)) > 1.5f)
					{
						m_waypointList.insert(second, D3DXVECTOR3(((*first).x + (*second).x) / 2.f, 0.f, ((*first).z + (*second).z) / 2.f));
						--second;
					}

					else
					{
						++first;
						++second;
					}
				} while (second != end && first != end);



				//std::list<D3DXVECTOR3>* smoothingList = this->editSplineNodesList();
				//D3DXVECTOR3* first = this->editFirstTempSmoothingVector();
				//D3DXVECTOR3* second = this->editSecondTempSmoothingVector();

				//if (m_waypointList.size() >= 3)
				//{
				//	*first = m_waypointList.front();
				//	m_waypointList.pop_front();
				//	*second = m_waypointList.front();
				//	m_waypointList.pop_front();

				//	do {
				//		if (sqrtf(pow((first->x - second->x), 2) + pow((first->y - second->y), 2)) > 1.5f)
				//		{
				//			m_waypointList.push_front(*second);
				//			//m_waypointList.push_front(D3DXVECTOR3((first->x + second->x) / 2.f, (first->y + second->y) / 2.f, (first->z + second->z) / 2.f));
				//			//*second = m_waypointList.front();
				//			(second->x) = ((first->x + second->x) / 2.f);
				//			(second->y) = ((first->y + second->y) / 2.f);
				//			(second->z) = ((first->z + second->z) / 2.f);

				//		}

				//		else
				//		{
				//			smoothingList->push_back(*first);
				//			smoothingList->push_back(*second);
				//			*first = *second;
				//			*second = m_waypointList.front();
				//			m_waypointList.pop_front();
				//		}

				//	} while (m_waypointList.size() >= 1);

				//	m_waypointList.merge(*smoothingList);

				//	smoothingList->erase(smoothingList->begin(), smoothingList->end());

				if (m_waypointList.size() < 3)
				{
					//int bob = 0;
				}

				else if (m_waypointList.size() == 3)
				{
					D3DXVECTOR3* temp = this->getTempVector();
					std::vector<D3DXVECTOR3>* splineVector = this->editSplineNodesVector();
					splineVector->resize(9);
					splineVector->at(0) = m_waypointList.front();
					m_waypointList.pop_front();
					splineVector->at(4) = m_waypointList.front();
					m_waypointList.pop_front();
					splineVector->at(8) = m_waypointList.front();
					m_waypointList.pop_front();

					for (int i = 0; i < 5; i += 4)
					{
						if (i == 0)
						{
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.25f);
							splineVector->at(i + 1) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.55f);
							splineVector->at(i + 2) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.75f);
							splineVector->at(i + 3) = *temp;
						}

						else
						{
							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 4)), 0.25f);
							splineVector->at(i + 1) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 4)), 0.5f);
							splineVector->at(i + 2) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 4)), 0.75f);
							splineVector->at(i + 3) = *temp;
						}
					}

					for (auto i = splineVector->begin(); i != splineVector->end(); i++)
					{
						m_waypointList.push_back(*(i));
					}


					splineVector->erase(splineVector->begin(), splineVector->end());

				}

				else {
					//m_waypointList.begin();
					int waypointListSize = m_waypointList.size();
					D3DXVECTOR3* temp = this->getTempVector();
					//std::list<D3DXVECTOR3>* splineList = this->editSplineNodesList();
					std::vector< D3DXVECTOR3>* splineVector = this->editSplineNodesVector();
					//splineList->resize()
					splineVector->resize(1 + (4 * (waypointListSize - 1)));
					int k = 0;
					for (int j = 0; j < waypointListSize - 1; j += 1)
					{
						splineVector->at(k) = m_waypointList.front();
						m_waypointList.pop_front();
						k += 4;
					}
					splineVector->at((4 * (waypointListSize - 1))) = m_waypointList.front();
					m_waypointList.pop_front();

					int splineVectorSize = splineVector->size();

					for (int i = 0; i < splineVectorSize - 4; i += 4)
					{
						if (i == 0)
						{
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.25f);
							splineVector->at(i + 1) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.5f);
							splineVector->at(i + 2) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.75f);
							splineVector->at(i + 3) = *temp;
						}

						else if (i == (splineVector->size() - 5))
						{

							D3DXVec3CatmullRom(temp, &(splineVector->at(splineVectorSize - 9)), &(splineVector->at(splineVectorSize - 5)), &(splineVector->at(splineVectorSize - 1)), &(splineVector->at(splineVectorSize - 1)), 0.25f);
							splineVector->at(splineVectorSize - 4) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(splineVectorSize - 9)), &(splineVector->at(splineVectorSize - 5)), &(splineVector->at(splineVectorSize - 1)), &(splineVector->at(splineVectorSize - 1)), 0.5f);
							splineVector->at(splineVectorSize - 3) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(splineVectorSize - 9)), &(splineVector->at(splineVectorSize - 5)), &(splineVector->at(splineVectorSize - 1)), &(splineVector->at(splineVectorSize - 1)), 0.75f);
							splineVector->at(splineVectorSize - 2) = *temp;

						}
						else
						{

							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.25f);
							splineVector->at(i + 1) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.5f);
							splineVector->at(i + 2) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.75f);
							splineVector->at(i + 3) = *temp;
						}

					}

					for (auto i = splineVector->begin(); i != splineVector->end(); i++)
					{
						m_waypointList.push_back(*(i));
					}


					splineVector->erase(splineVector->begin(), splineVector->end());

				}

			}


			else if (this->GetSmoothPath() && m_waypointList.size() >= 3)
			{
				if (m_waypointList.size() < 3)
				{
					//do nothing
					int bob = 0;
				}

				else if (m_waypointList.size() == 3)
				{
					D3DXVECTOR3* temp = this->getTempVector();
					std::vector<D3DXVECTOR3>* splineVector = this->editSplineNodesVector();
					splineVector->resize(9);
					splineVector->at(0) = m_waypointList.front();
					m_waypointList.pop_front();
					splineVector->at(4) = m_waypointList.front();
					m_waypointList.pop_front();
					splineVector->at(8) = m_waypointList.front();
					m_waypointList.pop_front();

					for (int i = 0; i < 5; i += 4)
					{
						if (i == 0)
						{
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.25f);
							splineVector->at(i + 1) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.55f);
							splineVector->at(i + 2) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.75f);
							splineVector->at(i + 3) = *temp;
						}

						else
						{
							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 4)), 0.25f);
							splineVector->at(i + 1) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 4)), 0.5f);
							splineVector->at(i + 2) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 4)), 0.75f);
							splineVector->at(i + 3) = *temp;
						}
					}

					for (auto i = splineVector->begin(); i != splineVector->end(); i++)
					{
						m_waypointList.push_back(*(i));
					}


					splineVector->erase(splineVector->begin(), splineVector->end());
				}

				else {
					//	m_waypointList.begin();
					int waypointListSize = m_waypointList.size();
					D3DXVECTOR3* temp = this->getTempVector();
					//std::list<D3DXVECTOR3>* splineList = this->editSplineNodesList();
					std::vector< D3DXVECTOR3>* splineVector = this->editSplineNodesVector();
					//splineList->resize()
					splineVector->resize(1 + (4 * (waypointListSize - 1)));
					int k = 0;
					for (int j = 0; j < waypointListSize - 1; j += 1)
					{
						splineVector->at(k) = m_waypointList.front();
						m_waypointList.pop_front();
						k += 4;
					}
					splineVector->at((4 * (waypointListSize - 1))) = m_waypointList.front();
					m_waypointList.pop_front();

					int splineVectorSize = splineVector->size();

					for (int i = 0; i < splineVectorSize - 4; i += 4)
					{
						if (i == 0)
						{
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.25f);
							splineVector->at(i + 1) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.5f);
							splineVector->at(i + 2) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.75f);
							splineVector->at(i + 3) = *temp;
						}

						else if (i == (splineVector->size() - 5))
						{

							D3DXVec3CatmullRom(temp, &(splineVector->at(splineVectorSize - 9)), &(splineVector->at(splineVectorSize - 5)), &(splineVector->at(splineVectorSize - 1)), &(splineVector->at(splineVectorSize - 1)), 0.25f);
							splineVector->at(splineVectorSize - 4) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(splineVectorSize - 9)), &(splineVector->at(splineVectorSize - 5)), &(splineVector->at(splineVectorSize - 1)), &(splineVector->at(splineVectorSize - 1)), 0.5f);
							splineVector->at(splineVectorSize - 3) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(splineVectorSize - 9)), &(splineVector->at(splineVectorSize - 5)), &(splineVector->at(splineVectorSize - 1)), &(splineVector->at(splineVectorSize - 1)), 0.75f);
							splineVector->at(splineVectorSize - 2) = *temp;

						}
						else
						{

							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.25f);
							splineVector->at(i + 1) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.5f);
							splineVector->at(i + 2) = *temp;
							D3DXVec3CatmullRom(temp, &(splineVector->at(i - 4)), &(splineVector->at(i)), &(splineVector->at(i + 4)), &(splineVector->at(i + 8)), 0.75f);
							splineVector->at(i + 3) = *temp;
						}

					}

					for (auto i = splineVector->begin(); i != splineVector->end(); i++)
					{
						m_waypointList.push_back(*(i));
					}


					splineVector->erase(splineVector->begin(), splineVector->end());

				}
			}

			//if (m_waypointList.size() >= 3)
			//{
			//	std::list<D3DXVECTOR3>::iterator it = m_waypointList.end();

			//	D3DXVECTOR3 a = *it; //m_waypointList.back();
			//	--it;
			//	m_waypointList.pop_back();
			//	
			//	D3DXVECTOR3 b = *it; //m_waypointList.back();
			//	--it;
			//	m_waypointList.pop_back();
			//	D3DXVECTOR3 c = *it;  //m_waypointList.back();
			//	--it;
			//	
			//	do
			//	{
			//		

			//		bool bb = true;
			//		for (int i = min(a.x, c.x); i <= max(a.x, a.y); ++i)
			//		{
			//			for (int j = min(a.y, c.y); j <= max(a.y, c.y); j++)
			//			{
			//				if (g_terrain.IsWall(i, j))
			//				{
			//					bb = false;
			//					break;
			//				}
			//			}
			//		}
			//		if (bb)
			//		{
			//			m_waypointList.push_back(a);
			//			it = m_waypointList.end();
			//		}
			//		else
			//		{
			//			m_waypointList.push_back(b);
			//			m_waypointList.push_back(a);
			//			--it;
			//		}


			//	} while (m_waypointList.size() >= 3);

			else if (m_waypointList.size() < 3)
			{
				m_waypointList.clear();
				m_waypointList.push_back(D3DXVECTOR3(g_terrain.GetCoordinates(r, c)));

			}
			//}
			myAStarV4.clear();
			//m_waypointList.clear();
			return true;


		}

		else
		{
			return false;
		}

		}
	
		else
		{
			//Randomly meander toward goal (might get stuck at wall)
			//int curR, curC;
			//D3DXVECTOR3 cur = m_owner->GetBody().GetPos();
			//g_terrain.GetRowColumn( &cur, &curR, &curC );

			m_waypointList.clear();
			m_waypointList.push_back(cur);

			int countdown = 100;
			while (curR != r || curC != c)
			{
				if (countdown-- < 0) { break; }

				if (curC == c || (curR != r && rand() % 2 == 0))
				{	//Go in row direction
					int last = curR;
					if (curR < r) { curR++; }
					else { curR--; }

					if (g_terrain.IsWall(curR, curC))
					{
						curR = last;
						continue;
					}
				}
				else
				{	//Go in column direction
					int last = curC;
					if (curC < c) { curC++; }
					else { curC--; }

					if (g_terrain.IsWall(curR, curC))
					{
						curC = last;
						continue;
					}
				}

				D3DXVECTOR3 spot = g_terrain.GetCoordinates(curR, curC);
				m_waypointList.push_back(spot);
				g_terrain.SetColor(curR, curC, DEBUG_COLOR_YELLOW);
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