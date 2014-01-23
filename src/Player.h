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
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <functional>   // std::bind
#include <map>
#include <vector>

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
    bool IsOccupying;
    bool mpIntransit;
    bool ResetCallOccupy;
    std::map<Vector, int> holemap;
    std::map<int, Vector> playermap;
    std::vector<Vector> holevector;
    std::vector<int> playervector;
    Vector mpTarget = Vector(0,0);

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

        Vector fronthor = Vector(currentHole.X()+20, currentHole.Y());
        Vector backhor = Vector(currentHole.X()-20, currentHole.Y());
        Vector upvert = Vector(currentHole.X(), currentHole.Y()-20);
        Vector downvert = Vector(currentHole.X(), currentHole.Y()+20);
    	
    	double buffer = 1.0;
    	
    	//TODO: Can be replaced by the IsOccupied function
        //TODO: Return true only if pass is advantageous
    	
    	for(Unum i=4; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if( AreSamePoints(player_pos, frontup, buffer)||
    		    AreSamePoints(player_pos, frontdown, buffer)||
                AreSamePoints(player_pos, backup, buffer)||
                AreSamePoints(player_pos, backdown, buffer)||
                AreSamePoints(player_pos, fronthor, buffer)
                //||
                //AreSamePoints(player_pos, backhor, buffer)||
                //AreSamePoints(player_pos, upvert, buffer)||
                //AreSamePoints(player_pos, downvert, buffer)
    			){
    			std::cout<<"pass available"<<std::endl;
    			return true;
    		}
    	}
    	return false;	
    }

    bool PassToBestPlayer(){
        //TODO: Use transit variable for faster calling of the OccupyHole/Dasher functions
        Vector myPosition = mpAgent->GetSelf().GetPos();
        Vector currentHole = RoundToNearestHole(myPosition);
        
        Vector frontup = Vector(currentHole.X()+10, currentHole.Y()-10);
        Vector backup = Vector(currentHole.X()-10, currentHole.Y()-10);
        Vector frontdown = Vector(currentHole.X()+10, currentHole.Y()+10);
        Vector backdown = Vector(currentHole.X()-10, currentHole.Y()+10);

        Vector fronthor = Vector(currentHole.X()+20, currentHole.Y());
        Vector backhor = Vector(currentHole.X()-20, currentHole.Y());
        Vector upvert = Vector(currentHole.X(), currentHole.Y()-20);
        Vector downvert = Vector(currentHole.X(), currentHole.Y()+20);
        
        double buffer = 1.0;
        double f = 0.6;
                
        //sstm << msgstr << mynum;
        //result = sstm.str();
        //mpAgent->Say(result);
        
        for(Unum i=4; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, frontup, buffer)){
                std::string msgstr = "cus";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                //std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=4; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, frontdown, buffer)){
                std::string msgstr = "cus";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                //std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=4; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, fronthor, buffer)){
                std::string msgstr = "cus";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                //std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=4; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, upvert, buffer)){
                std::string msgstr = "cus";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                //std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=4; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, downvert, buffer)){
                std::string msgstr = "cus";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                //std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=4; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, backhor, buffer)){
                std::string msgstr = "cus";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                //std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=4; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, backup, buffer)){
                std::string msgstr = "cus";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                //std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=4; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, backdown, buffer)){
                std::string msgstr = "cus";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                //std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        return false;
    }

    bool IsOccupied(Vector target, double buffer){
    	//Returns true if target is occupied by a player
    	for(Unum i=4; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if(AreSamePoints(player_pos, target, buffer)&&i!=mpAgent->GetSelfUnum())
    			return true;
    	}
    	return false;
    }

    Unum GetOccupierUnum(Vector target, double buffer){
        for(Unum i=4; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, target, buffer))
                return i;
        }
        return -1;
    }

    Unum GetClosest(Vector target){
    	//Excluded ballholder from the below equation
    	double mindis = 99999;
    	Unum mindisUnum = 99;
    	Unum BHUnum = GetBHUnum();
    	if(BHUnum==-1){
    		std::cout<<"No ball holder"<<std::endl;
            return -1;
        }
    	for(Unum i=4; i<=11; i++){
    		double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
    		if(PlayerDis<=mindis&&i!=BHUnum){
    			mindis = PlayerDis;
    			mindisUnum = i;
    		}
    	}

        return mindisUnum;
    	//std::cout<<"Player "<<mpAgent->GetSelfUnum()<<"thinks player "<<mindisUnum<<"is closest to hole"<<std::endl;
    	/*
        if(mpAgent->GetSelfUnum()==mindisUnum)
    		return true;
    	else 
    		return false;
            */
    }

    Unum GetClosestExcl1(Vector target, Unum ex1){
        //Excluded ballholder and ex1 from the below equation
        //ex1 would already have been assigned to another hole
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=4; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
        //std::cout<<"Player "<<mpAgent->GetSelfUnum()<<"thinks player "<<mindisUnum<<"is closest to hole"<<std::endl;
        /*
        if(mpAgent->GetSelfUnum()==mindisUnum)
            return true;
        else 
            return false;
            */
    }

    Unum GetClosestExcl2(Vector target, Unum ex1, Unum ex2){
        //Excluded ballholder and ex1,2 from the below equation
        //ex1,2 would already have been assigned to another hole
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=4; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
       return mindisUnum;
        //std::cout<<"Player "<<mpAgent->GetSelfUnum()<<"thinks player "<<mindisUnum<<"is closest to hole"<<std::endl;
        /*
        if(mpAgent->GetSelfUnum()==mindisUnum)
            return true;
        else 
            return false;
            */
    }

    Unum GetClosestExcl3(Vector target, Unum ex1, Unum ex2, Unum ex3){
        //Excluded ballholder and ex1,2,3 from the below equation
        //ex1,2,3 would already have been assigned to another hole
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=4; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2&&i!=ex3){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
        //std::cout<<"Player "<<mpAgent->GetSelfUnum()<<"thinks player "<<mindisUnum<<"is closest to hole"<<std::endl;
        /*
        if(mpAgent->GetSelfUnum()==mindisUnum)
            return true;
        else 
            return false;
            */
    }

    Unum GetClosestExcl4(Vector target, Unum ex1, Unum ex2, Unum ex3, Unum ex4){
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=4; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2&&i!=ex3&&i!=ex4){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
    }

    Unum GetClosestExcl5(Vector target, Unum ex1, Unum ex2, Unum ex3, Unum ex4, Unum ex5){
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=4; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2&&i!=ex3&&i!=ex4&&i!=ex5){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
    }

    Unum GetClosestExcl6(Vector target, Unum ex1, Unum ex2, Unum ex3, Unum ex4, Unum ex5, Unum ex6){
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=4; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2&&i!=ex3&&i!=ex4&&i!=ex5&&i!=ex6){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
    }

    Unum GetClosestExcl7(Vector target, Unum ex1, Unum ex2, Unum ex3, Unum ex4, Unum ex5, Unum ex6, Unum ex7){
        double mindis = 999;
        Unum mindisUnum = 99;
        Unum BHUnum = GetBHUnum();
        if(BHUnum==-1)
            std::cout<<"No ball holder"<<std::endl;
        for(Unum i=4; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2&&i!=ex3&&i!=ex4&&i!=ex5&&i!=ex6&&i!=ex7){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
    }

    Unum GetClosestTeammateTo(Vector target){
        double mindis = 99999;
        Unum mindisUnum = -1;
        for(Unum i=4; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            double dis = player_pos.Dist(target);
            if(dis<mindis){
                mindisUnum = i;
                mindis=dis;
            }
        }
        return mindisUnum;
    }

    Unum GetClosestFromArray(Vector target){
        double mindis = 99999;
        Unum mindisUnum = -1;
        for(int i=0; i<playervector.size(); i++){
            if(playervector[i]!=-1){
                Vector player_pos = mpAgent->GetWorldState().GetTeammate(playervector[i]).GetPos();
                double dis = player_pos.Dist(target);
                if(dis<mindis){
                    mindisUnum = playervector[i];
                    mindis=dis;
                }
            }
        }
        return mindisUnum;
    }

    void DecideAndOccupyHole(Unum target){
        //Called when another teammate would have the ball
        //target == -1 means a player already has the ball
        //target == -2 means ballpos+somevector assumed as ballholder pos
        //target == -3 means player has the ball, and the current player couldnt occupy a hole
        IsOccupying = false;
        
        Vector BHPos;
                
        if(target==-1){
            Unum i=1;
            for(i=4; i<=11; i++){
                if(mpAgent->GetWorldState().GetTeammate(i).IsKickable()&&(i!=mpAgent->GetSelfUnum())){
                    BHPos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
                    break;
                }
            }
            if(i==12)
                return;
        }
        /*
        else if(target==-2){
            BHPos = mpAgent->GetWorldState().GetBall().GetPos();
            BHPos = Vector(BHPos.X()+40,BHPos.Y());
        }
        else if(target ==-3){
             for(Unum i=1; i<=11; i++){
                if(mpAgent->GetWorldState().GetTeammate(i).IsKickable()&&(i!=mpAgent->GetSelfUnum())){
                    BHPos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
                    BHPos = Vector(BHPos.X()+20, BHPos.Y());
                    break;
                }
            }
        }
        */
        else if(target!=mpAgent->GetSelfUnum()){
            BHPos = mpAgent->GetWorldState().GetTeammate(target).GetPos();
        }
        else
            return;
        
        BHPos = RoundToNearestHole(BHPos);

        Vector BHfrontup = Vector(BHPos.X()+10, BHPos.Y()-10);
        Vector BHbackup = Vector(BHPos.X()-10, BHPos.Y()-10);
        Vector BHfrontdown = Vector(BHPos.X()+10, BHPos.Y()+10);
        Vector BHbackdown = Vector(BHPos.X()-10, BHPos.Y()+10);

        Vector BHfronthor = Vector(BHPos.X()+20, BHPos.Y());
        Vector BHbackhor = Vector(BHPos.X()-20, BHPos.Y());
        Vector BHupvert = Vector(BHPos.X(), BHPos.Y()-20);
        Vector BHdownvert = Vector(BHPos.X(), BHPos.Y()+20);

        ////////////////////////////////////////////////////////////
        holevector.clear();
        playervector.clear();
        holemap.clear();
        playermap.clear();

        holevector.push_back(BHfrontup);
        holevector.push_back(BHfronthor);
        holevector.push_back(BHfrontdown);
        holevector.push_back(BHupvert);
        holevector.push_back(BHdownvert);
        holevector.push_back(BHbackup);
        holevector.push_back(BHbackdown);
        holevector.push_back(BHbackhor);

        //holevector[0] = Vector(100,100);
        //holevector[1] = Vector(100,100);
        //holevector[2] = Vector(100,100);
        holevector[3] = Vector(100,100);
        holevector[4] = Vector(100,100);
        //holevector[5] = Vector(100,100);
        //holevector[6] = Vector(100,100);
        holevector[7] = Vector(100,100);
            
        for(Unum i=4; i<=11; i++)
            playervector.push_back(i);

        Vector mypos = mpAgent->GetSelf().GetPos();
        double buffer = 1;
        Vector test;
        for(int i=0; i<holevector.size(); i++){
            if(holevector[i]!=Vector(100,100)){
                test = holevector[i];
                Unum X = GetOccupierUnum(test, buffer); //X will be the smaller unum if two players occupy a hole
                if(X==mpAgent->GetSelfUnum() && test.X()>=-50 && test.X()<=50 && test.Y()>=-25 && test.Y()<=25){
                    OccupyHole(test);
                    return;
                }
            }
        }

        for(int i=0; i<holevector.size(); i++){
            if(holevector[i]!=Vector(100,100)){
                Unum occupier = GetOccupierUnum(holevector[i], buffer);
                if(occupier!=-1){
                    playermap[occupier] = holevector[i];
                    holemap[holevector[i]] = occupier; 
                }
            }
        }

        std::map<Vector,int>::iterator holeit;
        std::map<int,Vector>::iterator playit;
        
        for(int i=0; i<holevector.size(); i++){
            if(holevector[i]!=Vector(100,100)){
                holeit = holemap.find(holevector[i]);
                if(holeit!=holemap.end()){
                    std::cout<<"Found hole in map"<<std::endl;
                    holevector[i] = Vector(100, 100);
                }
            }
        }

        for(int i=0; i<playervector.size(); i++){
            if(playervector[i]!=-1){
                playit = playermap.find(playervector[i]);
                if(playit!=playermap.end()){
                    std::cout<<"Found player in map"<<std::endl;
                    playervector[i] = -1;
                }
            }
        }

        for(int i=0; i<holevector.size(); i++){
            if(holevector[i]!=Vector(100,100)){
                Unum X = GetClosestFromArray(holevector[i]);
                if(X==mpAgent->GetSelfUnum() && holevector[i].X()>=-50 && holevector[i].X()<=50 && holevector[i].Y()>=-25 && holevector[i].Y()<=25){
                    OccupyHole(holevector[i]);
                    return;
                }
            }
        }
        //////////////////////////////////////////////////////////////////

        /*
        Unum FU, FD, BU, BD;
        Unum FH, BH, UV, DV;

        //&&BHfrontup.X()>=-45&&BHfrontup.X()<=45&&BHfrontup.Y()>=-25&&BHfrontup.Y()<=25

        FU = GetClosest(BHfrontup);
        std::cout<<"For player "<<mpAgent->GetSelfUnum()<<", FU = "<<FU<<std::endl;
        if(FU==mpAgent->GetSelfUnum())
            std::cout<<"might be eligible - FU - "<<BHfrontup<<std::endl;
        if(FU==mpAgent->GetSelfUnum()&&BHfrontup.X()>=-50&&BHfrontup.X()<=50&&BHfrontup.Y()>=-25&&BHfrontup.Y()<=25){
            OccupyHole(BHfrontup);
            std::cout<<"Player "<<FU<<" occupying frontup"<<std::endl;
            return;
        }

        FD = GetClosestExcl1(BHfrontdown, FU);
        if(FD==mpAgent->GetSelfUnum())
            std::cout<<"might be eligible - FD - "<<BHfrontdown<<std::endl;
        std::cout<<"For player "<<mpAgent->GetSelfUnum()<<", FD = "<<FD<<std::endl;
        if(FD==mpAgent->GetSelfUnum()&&BHfrontdown.X()>=-50&&BHfrontdown.X()<=50&&BHfrontdown.Y()>=-25&&BHfrontdown.Y()<=25){
            OccupyHole(BHfrontdown);
            std::cout<<"Player "<<FD<<" occupying frontdown"<<std::endl;
            return;
        }

        FH = GetClosestExcl2(BHfronthor, FU, FD);
        if(FH==mpAgent->GetSelfUnum())
            std::cout<<"might be eligible - FH - "<<BHfronthor<<std::endl;
        std::cout<<"For player "<<mpAgent->GetSelfUnum()<<", FH = "<<FH<<std::endl;
        if(FH==mpAgent->GetSelfUnum()&&BHfronthor.X()>=-50&&BHfronthor.X()<=50&&BHfronthor.Y()>=-25&&BHfronthor.Y()<=25){
            OccupyHole(BHfronthor);
            std::cout<<"Player "<<FH<<" occupying fronthor"<<std::endl;
            return;
        }

        UV = GetClosestExcl3(BHupvert, FU, FD, FH);
        if(UV==mpAgent->GetSelfUnum())
            std::cout<<"might be eligible - UV - "<<BHupvert<<std::endl;
        std::cout<<"For player "<<mpAgent->GetSelfUnum()<<", UV = "<<UV<<std::endl;
        if(UV==mpAgent->GetSelfUnum()&&BHupvert.X()>=-50&&BHupvert.X()<=50&&BHupvert.Y()>=-25&&BHupvert.Y()<=25){
            OccupyHole(BHupvert);
            std::cout<<"Player "<<UV<<" occupying upvert"<<std::endl;
            return;
        }

        DV = GetClosestExcl4(BHdownvert, FU, FD, FH, UV);
        if(DV==mpAgent->GetSelfUnum())
            std::cout<<"might be eligible - DV - "<<BHdownvert<<std::endl;
        std::cout<<"For player "<<mpAgent->GetSelfUnum()<<", DV = "<<DV<<std::endl;
        if(DV==mpAgent->GetSelfUnum()&&BHdownvert.X()>=-50&&BHdownvert.X()<=50&&BHdownvert.Y()>=-25&&BHdownvert.Y()<=25){
            OccupyHole(BHdownvert);
            std::cout<<"Player "<<DV<<" occupying downvert"<<std::endl;
            return;
        }

        BU = GetClosestExcl5(BHbackup, FU, FD, FH, UV, DV);
        if(BU==mpAgent->GetSelfUnum())
            std::cout<<"might be eligible - BU - "<<BHbackup<<std::endl;
        std::cout<<"For player "<<mpAgent->GetSelfUnum()<<", BU = "<<BU<<std::endl;
        if(BU==mpAgent->GetSelfUnum()&&BHbackup.X()>=-50&&BHbackup.X()<=50&&BHbackup.Y()>=-25&&BHbackup.Y()<=25){
            OccupyHole(BHbackup);
            std::cout<<"Player "<<BU<<" occupying backup"<<std::endl;
            return;
        }
        
        BD = GetClosestExcl6(BHbackdown, FU, FD, FH, UV, DV, BU);
        if(BD==mpAgent->GetSelfUnum())
            std::cout<<"might be eligible - BD - "<<BHbackdown<<std::endl;
        std::cout<<"For player "<<mpAgent->GetSelfUnum()<<", BD = "<<BD<<std::endl;
        if(BD==mpAgent->GetSelfUnum()&&BHbackdown.X()>=-50&&BHbackdown.X()<=50&&BHbackdown.Y()>=-25&&BHbackdown.Y()<=25){
            OccupyHole(BHbackdown);
            std::cout<<"Player "<<BD<<" occupying backdown"<<std::endl;
            return;
        }

        //BH = GetClosestExcl7(BHbackhor, FU, FD, FH, UV, DV, BU, BD);
        BH = mpAgent->GetSelfUnum();
        if(BH==mpAgent->GetSelfUnum())
            std::cout<<"might be eligible - BH - "<<BHbackhor<<std::endl;
        std::cout<<"For player "<<mpAgent->GetSelfUnum()<<", BH = "<<BH<<std::endl;
        if(BH==mpAgent->GetSelfUnum()&&BHbackhor.X()>=-50&&BHbackhor.X()<=50&&BHbackhor.Y()>=-25&&BHbackhor.Y()<=25){
            OccupyHole(BHbackhor);
            std::cout<<"Player "<<BH<<" occupying backhor"<<std::endl;
            return;
        }
        */
        
        /*
        std::vector<Vector> holes;
        holes.push_back(BHfrontup);
        holes.push_back(BHbackup);
        holes.push_back(BHfrontdown);
        holes.push_back(BHbackdown);
        holes.push_back(BHfronthor);
        holes.push_back(BHbackhor);
        holes.push_back(BHupvert);
        holes.push_back(BHdownvert);

        Vector targethole;
        double mindis = 999999;
        Vector myPos = mpAgent->GetSelf().GetPos();

        for(int i=0; i<8; i++){
            double dis = myPos.Dist(holes[i]);
            if(dis<mindis){
                mindis = dis;
                targethole = holes[i];
            }
        }

        OccupyHole(targethole);
        */

        //if frontup&frontdown not occupied (+ other conditions), move there
        //TODO: Currently, it is possible that one player is expected to fill both the holes

        /*
        
        */
    }

    bool BallKickableByATeammate(){
    	//TODO: Replace IsKickable with AreSamePoints + larger buffer
    	for(Unum i=1; i<=11; i++){
    		if(mpAgent->GetWorldState().GetTeammate(i).IsKickable()&&(i!=mpAgent->GetSelfUnum())){
    			//std::cout<<"Player "<<mpAgent->GetSelfUnum()<<" thinks ball kickable by Player "<<i<<std::endl;
    			return true;
    		}
    	}
    	return false;
    }

    Unum GetBHUnum(){
    	//Only to be called if BallKickableByATeammate
        if(ResetCallOccupy)
            return 100; //Return passer unum
    	for(Unum i=1; i<=11; i++){
    		if(mpAgent->GetWorldState().GetTeammate(i).IsKickable())
    			return i;
    	}
    	return GetClosestUnumToBall();
    }

    Unum GetClosestUnumToBall(){
        Vector ballpos = mpAgent->GetWorldState().GetBall().GetPos();
        Unum mindisUnum = 0;
        double mindis = 9999;
        for(Unum i=1; i<=11; i++){
            Vector UnumPos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            double dis = UnumPos.Dist(ballpos);
            if(dis<mindis){
                mindis = dis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
    }


	bool AreSamePoints(Vector A, Vector B, double buffer){
		//Check if and b are the same points +/- buffer
		if( (abs(A.X()-B.X())<buffer) && (abs(A.Y()-B.Y())<buffer))
    		return true;
    	else
    		return false;
	}

	void OccupyHole(Vector target){
	    //Dash to target
	    //while(!AreSamePoints(mpAgent->GetSelf().GetPos(),target,2))
		    //Dasher::instance().GoToPoint(*mpAgent, target, 0.3, 100, true, false);
        mpIntransit = true;
        mpTarget = target;
        IsOccupying = true;
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
	    //std::cout<<"Rounded Tens - "<<roundedTens<<std::endl;
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
	    //std::cout<<"Rounding up point - "<<P<<std::endl;
	    Vector roundedTens = RoundToNearestTens(P);
	    if(isRTaHole(roundedTens)){
	        //std::cout<<"RT is a hole - "<<roundedTens<<std::endl;
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
	            //std::cout<<"Rounded hole - "<<roundedHole<<std::endl;
	            return roundedHole;
	        }
	}
};

#endif

