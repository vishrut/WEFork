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

#ifndef __Player_H__
#define __Player_H__

#include "Client.h"
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

class DecisionTree;
class  BeliefState;

class Player: public Client
{
	//DecisionTree *mpDecisionTree; //hereo

public:
    /**
     * 构造函数和析构函数
     */
    bool isPositioned; 

    Player();
    virtual ~Player();

    void Run();
    void SendOptionToServer();

    bool PassPlayersAvailable(){
    	Vector myPosition = mpAgent->GetSelf().GetPos();
    	Vector currentHole = RoundToNearestHole(myPosition);
    	Vector frontup = Vector(currentHole.X()+10, currentHole.Y()-10);
    	Vector backup = Vector(currentHole.X()-10, currentHole.Y()-10);
    	Vector frontdown = Vector(currentHole.X()+10, currentHole.Y()+10);
    	Vector backdown = Vector(currentHole.X()-10, currentHole.Y()+10);
    	
    	double buffer = 3;
    	
    	for(Unum i=1; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if( AreSamePoints(player_pos, frontup, buffer)||
    		    AreSamePoints(player_pos, frontdown, buffer)||
    			AreSamePoints(player_pos, backup, buffer)||
    			AreSamePoints(player_pos, backdown, buffer))
    			return true;
    	}
    	return false;	
    }

    void PassToBestPlayer(){
    	Vector myPosition = mpAgent->GetSelf().GetPos();
    	Vector currentHole = RoundToNearestHole(myPosition);
    	Vector frontup = Vector(currentHole.X()+10, currentHole.Y()-10);
    	Vector backup = Vector(currentHole.X()-10, currentHole.Y()-10);
    	Vector frontdown = Vector(currentHole.X()+10, currentHole.Y()+10);
    	Vector backdown = Vector(currentHole.X()-10, currentHole.Y()+10);
    	
    	double buffer = 3;
    	
    	for(Unum i=1; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if(AreSamePoints(player_pos, frontup, buffer)){
    			Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()/2, KM_Hard, 0, false);
    			return;
    		}
    	}
    	for(Unum i=1; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if(AreSamePoints(player_pos, frontdown, buffer)){
    			Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()/2, KM_Hard, 0, false);
    			return;
    		}
    	}
    	for(Unum i=1; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if(AreSamePoints(player_pos, backup, buffer)){
    			Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()/2, KM_Hard, 0, false);
    			return;
    		}
    	}
    	for(Unum i=1; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if(AreSamePoints(player_pos, backdown, buffer)){
    			Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()/2, KM_Hard, 0, false);
    			return;
    		}
    	}
    }

	bool AreSamePoints(Vector A, Vector B, double buffer){
		//Check if and b are the same points +/- buffer
		// TODO: Add conditions to handle tolerance
    	if( (abs(A.X()-B.X())<buffer) && (abs(A.Y()-B.Y())<buffer))
    		return true;
    	else
    		return false;
	}

	void OccupyHole(Vector target){
	    //Dash to target
	    Dasher::instance().GoToPoint(*mpAgent, target, 0.3, 100, true, false);
	}

	double abs(double d){
	    if (d>0.00)
	            return d;
	        else
	            return d*(-1.00);
	}

	Vector RoundToNearestTens(Vector P){
	    // This method rounds a given position to its nearest tens - for example, the rounded position for (12, -17) would be (10, -20)
	    // This helps in locating nearby holes more easily
	    double multX = 10.00;
	    double multY = 10.00;
	    if(P.X()<0.00)
	        multX = -10.00;
	    if(P.Y()<0.00)
	        multY = -10.00;
	    int roundX = static_cast<int>((abs(P.X())+5.00)/10);
	    int roundY = static_cast<int>((abs(P.Y())+5.00)/10);
	    Vector roundedTens = Vector(multX*roundX, multY*roundY);
	    std::cout<<"Rounded Tens - "<<roundedTens<<std::endl;
	    return roundedTens;
	}

	bool isRTaHole(Vector P){
	    // This method is only for rounded tens
	    // Returns true iff rounded-ten point is a hole    
	    int normalX = static_cast<int>(abs(P.X())/10);
	    int normalY = static_cast<int>(abs(P.Y())/10);
	    if(normalX%2==normalY%2)
	        return true;
	    else
	        return false;
	}

	Vector RoundToNearestHole(Vector P){
	    std::cout<<"Rounding up point - "<<P<<std::endl;
	    Vector roundedTens = RoundToNearestTens(P);
	    if(isRTaHole(roundedTens)){
	        std::cout<<"RT is a hole - "<<roundedTens<<std::endl;
	        return roundedTens;
	    }
	    else{
	        Vector roundedHole;
	        double diffX = P.X()-roundedTens.X();
	        double diffY = P.Y()-roundedTens.Y();
	            
	        if(abs(diffX)<abs(diffY)){
	            //Point closer to vertical axis of the diamond
	            if(diffY>0)
	                roundedHole = Vector(roundedTens.X(), roundedTens.Y()+10);
	            else
	                roundedHole = Vector(roundedTens.X(), roundedTens.Y()-10);
	        }
	        else{
	            //Point closer to horizontal axis of the diamond
	            if(diffX>0)
	                roundedHole = Vector(roundedTens.X()+10, roundedTens.Y());
	            else
	                roundedHole = Vector(roundedTens.X()-10, roundedTens.Y());
	        }
	            std::cout<<"Rounded hole - "<<roundedHole<<std::endl;
	            return roundedHole;
	        }
	}

	void FindHoleToOccupy(){
	    //Round current position to nearest hole
	    //Determine the best hole to occupy
	}
};

#endif

