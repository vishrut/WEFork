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

#include "Player.h"
#include "DecisionTree.h"
#include "DynamicDebug.h"
#include "Formation.h"
#include "CommandSender.h"
#include "Parser.h"
#include "Thread.h"
#include "UDPSocket.h"
#include "WorldModel.h"
#include "Agent.h"
#include "VisualSystem.h"
#include "Logger.h"
#include "CommunicateSystem.h"
#include "TimeTest.h"
#include "Dasher.h"
#include "Kicker.h"
#include "Tackler.h" 

Player::Player()//:
	//mpDecisionTree( new DecisionTree ) //hereo
{
	isPositioned = false;
}

Player::~Player()
{
	//delete mpDecisionTree; //hereo
}

void Player::SendOptionToServer()
{
	std::cout << "SendOptionToServer called for Player"<<std::endl;
	while (!mpParser->IsClangOk())
	{
		mpAgent->CheckCommands(mpObserver);
		mpAgent->Clang(7, 8);
		mpObserver->SetCommandSend();
		WaitFor(200);
	}
	std::cout << "SendOptionToServer - ClangOK"<<std::endl;
	while (!mpParser->IsSyncOk())
	{
		mpAgent->CheckCommands(mpObserver);
		mpAgent->SynchSee();
		mpObserver->SetCommandSend();
		WaitFor(200);
	}
	std::cout << "SendOptionToServer SyncOK"<<std::endl;
	mpAgent->CheckCommands(mpObserver);
	mpAgent->EarOff(false);
	mpObserver->SetCommandSend();
	WaitFor(200);
	std::cout << "SendOptionToServer returning"<<std::endl;
}



void Player::Run()
{
    //TIMETEST("Run");

	static Time last_time = Time(-100, 0);

	mpObserver->Lock();

	/** 下面几个更新顺序不能变 */
	//Formation::instance.SetTeammateFormations(); //Hereo
	CommunicateSystem::instance().Update(); //在这里解析hear信息，必须首先更新
	mpAgent->CheckCommands(mpObserver);
	mpWorldModel->Update(mpObserver);

	mpObserver->UnLock();
	
    const Time & time = mpAgent->GetWorldState().CurrentTime();

	if (last_time.T() >= 0) {
		if (time != Time(last_time.T() + 1, 0) && time != Time(last_time.T(), last_time.S() + 1)) {
			if (time == last_time) {
				mpAgent->World().SetCurrentTime(Time(last_time.T(), last_time.S() + 1)); //否则决策数据更新会出问题
			}
		}
	}

	last_time = time;

    //Look here - Player logic starts from here
	PlayMode mpCurrentPlayMode = mpAgent->World().GetPlayMode();
	PlayerState mpCurrentPlayerState = mpAgent->GetSelf();
	PositionInfo mpCurrentPositionInfo = mpAgent->Info().GetPositionInfo();
    Vector myPosition = mpAgent->GetSelf().GetPos();
    double myDisToBall = mpCurrentPositionInfo.GetBallDistToPlayer(mpAgent->GetSelfUnum());
    Vector ballpos = mpAgent->GetWorldState().GetBall().GetPos();

    if(mpCurrentPlayMode==PM_Before_Kick_Off){
    	//TODO: Replace with an array + loop
        std::cout<<PlayerParam::instance().teamName()<<std::endl;
        if(!isPositioned){
    		if(mpAgent->GetSelfUnum() == 1){
    			Vector player_pos = Vector(-40.0, 0.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    		}
    		if(mpAgent->GetSelfUnum() == 2){
    			Vector player_pos = Vector(-20.0, -20.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    		}
    		if(mpAgent->GetSelfUnum() == 3){
    			Vector player_pos = Vector(-30.0, -10.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    		}
    		if(mpAgent->GetSelfUnum() == 4){
    			Vector player_pos = Vector(-20.0, 0.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    		}
    		if(mpAgent->GetSelfUnum() == 5){
    			Vector player_pos = Vector(-30.0, 10.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    		}
    		if(mpAgent->GetSelfUnum() == 6){
    			Vector player_pos = Vector(-20.0, 20.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    		}
    		if(mpAgent->GetSelfUnum() == 7){
    			Vector player_pos = Vector(-15.0, -15.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    		}
    		if(mpAgent->GetSelfUnum() == 8){
    			Vector player_pos = Vector(-15.0, 15.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    		}
    		if(mpAgent->GetSelfUnum() == 9){
    			Vector player_pos = Vector(0.0, -20.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    		}
    		if(mpAgent->GetSelfUnum() == 10){
    			Vector player_pos = Vector(0.0, 1.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    		}
    		if(mpAgent->GetSelfUnum() == 11){
    			Vector player_pos = Vector(0.0, 20.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    		}
    	}
        std::cout << "Positioned" << std::endl;
    }
    else {
        if(mpCurrentPlayerState.IsKickable()&&!BallKickableByATeammate()){
            //if suitable holes have players available, pass
            //else, hold on to ball
            Vector nearestHole = RoundToNearestHole(myPosition);
            if(PassPlayersAvailable()){
                std::cout <<"------------------------------------------------------"<<std::endl;
                PassToBestPlayer();
            }
            else{
                ActiveBehavior beh = ActiveBehavior(*mpAgent, BT_Hold);
                if (beh.GetType() != BT_None) {
                    mpAgent->SetActiveBehaviorInAct(beh.GetType());
                    if(beh.Execute());
                        //std::cout<<"Holding ball."<<std::endl;
                }
            }
               
    	}
        /*
        else if(myDisToBall<=1){
            Dasher::instance().GoToPoint(*mpAgent, ballpos, 0.3, 100, true, false);
        }
        */
        else {
            //if(myDisToBall>1)
            if(BallKickableByATeammate()){
                std::cout<<"will decide and occupy hole - "<<mpAgent->GetSelfUnum()<<std::endl;
                DecideAndOccupyHole();
            }
            else if(mpAgent->GetSelfUnum()==10);
                //Dasher::instance().GoToPoint(*mpAgent, ballpos, 0.3, 100, true, false);
            //if ball is with a player
                //if player has empty holes, dash to the hole
                //else, do nothing
            //else, do nothing
        }
    }
    	
	//Formation::instance.UpdateOpponentRole(); //TODO: 暂时放在这里，教练未发来对手阵型信息时自己先计算 //Hereo

	VisualSystem::instance().ResetVisualRequest();
	//mpDecisionTree->Decision(*mpAgent); // TODO:V: Replace or remove //Hereo

	VisualSystem::instance().Decision();
	CommunicateSystem::instance().Decision();

    mpAgent->SetHistoryActiveBehaviors(); //Hereo

	Logger::instance().LogSight();
}
