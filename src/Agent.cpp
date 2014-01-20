/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2013                                                    *
 * Copyright (c) 1998-2013 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China            *
 * All rights reserved.                                                             *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without               *
 * modification, are permitted provided that the following conditions are met:      *
 *     * Redistributions of source code must retain the above copyright             *
 *       notice, this list of conditions and the following disclaimer.              *
 *     * Redistributions in binary form must reproduce the above copyright          *
 *       notice, this list of conditions and the following disclaimer in the        *
 *       documentation and/or other materials provided with the distribution.       *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the      *
 *       names of its contributors may be used to endorse or promote products       *
 *       derived from this software without specific prior written permission.      *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer Simulation Team BE LIABLE    *
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL       *
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR       *
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       *
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    *
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF *
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
 ************************************************************************************/

#include "Agent.h"
#include "WorldModel.h"
  #include <iostream>
 #include <cstdlib>
 #include <stdio.h>
#include <stdlib.h>
 #include <string>

/**
 * Constructor.
 */
Agent::Agent(Unum unum, WorldModel *world_model, bool reverse):
	mSelfUnum( abs(unum) ),
	mReverse( reverse ),
	mpWorldModel( world_model ),
	mpWorldState( &(world_model->World(reverse)) ),
	mpInfoState( new InfoState( mpWorldState)),
	mIsNewSight (false),
	mpStrategy(0), //Hereo
	mpAnalyser(0), //Hereo
    mpActionEffector(0)
    //mpFormation(0) //Hereo
{
	//Recvdmsg = "initialmsg";
}

/**
 * Destructor.
 */
Agent::~Agent()
{
	SetHistoryActiveBehaviors();

	for (int type = BT_None + 1; type < BT_Max; ++type) {
		delete mLastActiveBehavior[type];
	}

	delete mpInfoState;
    //delete mpFormation; //Hereo
	delete mpActionEffector;
	delete mpStrategy; //Hereo
	delete mpAnalyser; //Hereo
}

/**
 * Interface to create an agent which represents a team mate.
 * \param unum positive number represents the uniform number of the team mate.
 * \return an agent created by "new" operator, which should be manually deleted when there will be no
 *         use any more.
 */
Agent * Agent::CreateTeammateAgent(Unum unum) ///反算队友
{
    Assert(unum != 0);
    return new Agent(unum, mpWorldModel, mReverse); //reverse属性不变
}

/**
 * Interface to create an agent which represents an opponent.
 * \param unum positive number represents the uniform number of the opponent.
 * \return an agent created by "new" operator, which should be manually deleted when there will be no
 *         use any more.
 */
Agent * Agent::CreateOpponentAgent(Unum unum) ///反算对手
{
	return new Agent(unum, mpWorldModel, !mReverse); //reverse属性相反
}


void Agent::SaveActiveBehavior(const ActiveBehavior & beh)
{
	BehaviorType type = beh.GetType();

	Assert(type > BT_None && type < BT_Max);

	if (mActiveBehavior[type] != 0) {
		if (*mActiveBehavior[type] < beh) {
			delete mActiveBehavior[type];
			mActiveBehavior[type] = new ActiveBehavior(beh);
		}
	}
	else {
		mActiveBehavior[type] = new ActiveBehavior(beh);
	}
}

void Agent::SetRecvdMsg(std::string strmsg){
		
		std::cout<<"The agent "<< GetSelfUnum() <<" recvd msg - "<<strmsg<<std::endl;
		unsigned pos = strmsg.find("X");
		unsigned Ppos = strmsg.find("p");
		unsigned len = pos - Ppos-1;
		std::string sub0 = strmsg.substr(Ppos+1,len);
		int passer = atoi(sub0.c_str());
		//std::cout<<"X is at "<<pos<<std::endl;
		std::string sub = strmsg.substr(pos+1);
		int value = atoi(sub.c_str());
		int passerX = GetWorldState().GetTeammate(passer).GetPos().X();
		int passeeX = GetWorldState().GetTeammate(value).GetPos().X();
		if(passeeX<=passerX){
			SetCenter(passer);
			std::cout<<"backpass detected"<<std::endl;
		}
		else{
			SetCenter(-1);
			SetResetVal(true);
		}
		SetTargetUnum(value);
		std::cout<<"Target player is "<<value<<std::endl;
		if(value==GetSelfUnum()){
			std::cout<<"Player "<<GetSelfUnum()<<" setting follow ball"<<std::endl;
			SetFollowBall(true);
		}
		else
			SetFollowBall(false);
		if(strmsg.substr(0,3).compare("cus")==0){
			std::string resend = "r"+strmsg;
			while(!Say(resend));
		}
	}


ActiveBehavior *Agent::GetLastActiveBehavior(BehaviorType type) const
{
	Assert(type > BT_None && type < BT_Max);

	return mLastActiveBehavior[type];
}

void Agent::SetActiveBehaviorInAct(BehaviorType type)
{
	Assert(type > BT_None && type < BT_Max);
	Assert(mActiveBehavior[0] == 0);

	mActiveBehavior[0] = mActiveBehavior[type];
}

void Agent::SaveActiveBehaviorList(const std::list<ActiveBehavior> & behavior_list)
{
	for (std::list<ActiveBehavior>::const_iterator it = behavior_list.begin(); it != behavior_list.end(); ++it) {
		SaveActiveBehavior(*it);
	}
}

void Agent::SetHistoryActiveBehaviors()
{
    for (int type = BT_None + 1; type < BT_Max; ++type) {
        delete mLastActiveBehavior[type];

        mLastActiveBehavior[type] = mActiveBehavior[type];
        mActiveBehavior[type] = 0;
    }

    mLastActiveBehavior[0] = mActiveBehavior[0];
    mActiveBehavior[0] = 0;
}

