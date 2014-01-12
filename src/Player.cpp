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
	//std::cout << "Player run called - " << mpAgent->GetSelfUnum() << std::endl;
	mpObserver->Lock();

	/** 下面几个更新顺序不能变 */
	//Formation::instance.SetTeammateFormations(); //Hereo
	CommunicateSystem::instance().Update(); //在这里解析hear信息，必须首先更新
	mpAgent->CheckCommands(mpObserver);
	mpWorldModel->Update(mpObserver);

	mpObserver->UnLock();
	//std::cout << "Player run called and unlocked - " << mpAgent->GetSelfUnum() << std::endl;
	
    const Time & time = mpAgent->GetWorldState().CurrentTime();

	if (last_time.T() >= 0) {
		if (time != Time(last_time.T() + 1, 0) && time != Time(last_time.T(), last_time.S() + 1)) {
			if (time == last_time) {
				mpAgent->World().SetCurrentTime(Time(last_time.T(), last_time.S() + 1)); //否则决策数据更新会出问题
			}
		}
	}

	last_time = time;

    //Look here
	PlayMode mpCurrentPlayMode = mpAgent->World().GetPlayMode();
	PlayerState mpCurrentPlayerState = mpAgent->GetSelf();
	PositionInfo mpCurrentPositionInfo = mpAgent->Info().GetPositionInfo();
    Vector myPosition = mpAgent->GetSelf().GetPos();
    double myDisToBall = mpCurrentPositionInfo.GetBallDistToPlayer(mpAgent->GetSelfUnum());
    Vector ballpos = mpAgent->GetWorldState().GetBall().GetPos();
	//std::cout << "PlayMode - " << mpCurrentPlayMode <<std::endl;

    if(mpCurrentPlayMode==PM_Before_Kick_Off){
    	//std::cout << "Before kick off - " << mpCurrentPlayMode <<std::endl;
    	if(!isPositioned){
    		std::cout << "Not yet positioned" << std::endl;
    		if(mpAgent->GetSelfUnum() == 1){
    			Vector player_pos = Vector(-40.0, 0.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    			std::cout << "Positioned" << std::endl;
    		}
    		if(mpAgent->GetSelfUnum() == 2){
    			Vector player_pos = Vector(-20.0, -20.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    			std::cout << "Positioned" << std::endl;
    		}
    		if(mpAgent->GetSelfUnum() == 3){
    			Vector player_pos = Vector(-30.0, -10.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    			std::cout << "Positioned" << std::endl;
    		}
    		if(mpAgent->GetSelfUnum() == 4){
    			Vector player_pos = Vector(-20.0, 0.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    			std::cout << "Positioned" << std::endl;
    		}
    		if(mpAgent->GetSelfUnum() == 5){
    			Vector player_pos = Vector(-30.0, 10.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    			std::cout << "Positioned" << std::endl;
    		}
    		if(mpAgent->GetSelfUnum() == 6){
    			Vector player_pos = Vector(-20.0, 20.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    			std::cout << "Positioned" << std::endl;
    		}
    		if(mpAgent->GetSelfUnum() == 7){
    			Vector player_pos = Vector(-10.0, -10.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    			std::cout << "Positioned" << std::endl;
    		}
    		if(mpAgent->GetSelfUnum() == 8){
    			Vector player_pos = Vector(-10.0, 10.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    			std::cout << "Positioned" << std::endl;
    		}
    		if(mpAgent->GetSelfUnum() == 9){
    			Vector player_pos = Vector(0.0, -20.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    			std::cout << "Positioned" << std::endl;
    		}
    		if(mpAgent->GetSelfUnum() == 10){
    			Vector player_pos = Vector(0.0, 1.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    			std::cout << "Positioned" << std::endl;
    		}
    		if(mpAgent->GetSelfUnum() == 11){
    			Vector player_pos = Vector(0.0, 20.0);
    			mpAgent->Move(player_pos);
    			isPositioned = true;
    			std::cout << "Positioned" << std::endl;
    		}
    	}
    }
    else {
    	if(mpCurrentPlayerState.IsKickable()){
            //if suitable holes have players available, pass
            //else, hold on to ball

            Vector nearestHole = RoundToNearestHole(myPosition);
            if(PassPlayersAvailable()){
                std::cout<<"Pass players available"<<std::endl;
                PassToBestPlayer();
            }
            else{
                ActiveBehavior beh = ActiveBehavior(*mpAgent, BT_Hold);

                if (beh.GetType() != BT_None) {
                    std::cout<<"Behavior recvd"<<std::endl;
                    mpAgent->SetActiveBehaviorInAct(beh.GetType());
                    if(beh.Execute())
                        std::cout<<"Behavior executed"<<std::endl;
                }
            }

            /*
            if(mpAgent->GetSelfUnum()!=10){
                ActiveBehavior beh = ActiveBehavior(*mpAgent, BT_Hold);

                if (beh.GetType() != BT_None) {
                    std::cout<<"Behavior recvd"<<std::endl;
                    mpAgent->SetActiveBehaviorInAct(beh.GetType());
                    if(beh.Execute())
                        std::cout<<"Behavior executed"<<std::endl;
                }
            }
            
            else{
                RoundToNearestHole(myPosition);
        		std::cout<<"Ball kickable - "<<mpAgent->GetSelfUnum()<<std::endl;
        		Unum mpClosestPlayer = mpCurrentPositionInfo.GetClosestTeammateToPlayer(mpAgent->GetSelfUnum());          

                std::cout<<"Closest player - "<<mpClosestPlayer<<std::endl;
                Vector target_pos = mpAgent->GetWorldState().GetTeammate(mpClosestPlayer).GetPos();
                std::cout<<"Target player pos - "<< target_pos <<std::endl;

                if(Kicker::instance().KickBall(*mpAgent, target_pos, ServerParam::instance().ballSpeedMax()/2, KM_Hard, 0, false))
                    std::cout <<"Kick successful"<<std::endl;
            }
            */
            std::cout <<"--------------------------------------------------------"<<std::endl;   
            
    	}
        else if(myDisToBall<3){
            //if ball is with a player
                //if player has empty holes, dash to the hole
                //else, do nothing
            //else, do nothing
            Dasher::instance().GoToPoint(*mpAgent, ballpos, 0.3, 100, true, false);
        }
    }
    	
	//Formation::instance.UpdateOpponentRole(); //TODO: 暂时放在这里，教练未发来对手阵型信息时自己先计算 //Hereo

	VisualSystem::instance().ResetVisualRequest();
	//mpDecisionTree->Decision(*mpAgent); // TODO:V: Replace or remove //Hereo

	VisualSystem::instance().Decision();
	CommunicateSystem::instance().Decision();

    mpAgent->SetHistoryActiveBehaviors(); //Hereo

	Logger::instance().LogSight();
	//std::cout << "Player run called and unlocked and completed - " << mpAgent->GetSelfUnum() << std::endl;	
}
