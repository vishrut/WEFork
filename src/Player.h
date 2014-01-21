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
    bool mpIntransit = false;
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
    	
    	double buffer = 0.5;
    	
    	//TODO: Can be replaced by the IsOccupied function
        //TODO: Return true only if pass is advantageous
    	
    	for(Unum i=1; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if( AreSamePoints(player_pos, frontup, buffer)||
    		    AreSamePoints(player_pos, frontdown, buffer)||
                //AreSamePoints(player_pos, backup, buffer)||
                //AreSamePoints(player_pos, backdown, buffer)||
                AreSamePoints(player_pos, fronthor, buffer)||
                //AreSamePoints(player_pos, backhor, buffer)||
                AreSamePoints(player_pos, upvert, buffer)||
                AreSamePoints(player_pos, downvert, buffer)
    			){
    			std::cout<<"pass available"<<std::endl;
    			return true;
    		}
    	}
    	return false;	
    }

    bool IsOccupied(Vector target){
    	//Returns true if target is occupied by a player
    	double buffer = 0.3;
    	for(Unum i=1; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if(AreSamePoints(player_pos, target, buffer)&&i!=mpAgent->GetSelfUnum())
    			return true;
    	}
    	return false;
    }

    Unum GetClosest(Vector target){
    	//Excluded ballholder from the below equation
    	double mindis = 999;
    	Unum mindisUnum = 99;
    	Unum BHUnum = GetBHUnum();
    	if(BHUnum==-1)
    		std::cout<<"No ball holder"<<std::endl;
    	for(Unum i=1; i<=11; i++){
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
        for(Unum i=1; i<=11; i++){
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
        for(Unum i=1; i<=11; i++){
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
        for(Unum i=1; i<=11; i++){
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
        for(Unum i=1; i<=11; i++){
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
        for(Unum i=1; i<=11; i++){
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
        for(Unum i=1; i<=11; i++){
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
        for(Unum i=1; i<=11; i++){
            double PlayerDis = mpAgent->GetWorldState().GetTeammate(i).GetPos().Dist(target);
            if(PlayerDis<=mindis&&i!=BHUnum&&i!=ex1&&i!=ex2&&i!=ex3&&i!=ex4&&i!=ex5&&i!=ex6&&i!=ex7){
                mindis = PlayerDis;
                mindisUnum = i;
            }
        }
        return mindisUnum;
    }


    void DecideAndOccupyHole(){
    	//Called when another teammate has the ball
    	//Decide if the player should support the ballholder by moving to an appropriate hole or not
    	//Act as per the decision
    	
    	Vector BHPos;
    	
    	double buffer = 0.1;
    	for(Unum i=1; i<=11; i++){
    		if(mpAgent->GetWorldState().GetTeammate(i).IsKickable()&&(i!=mpAgent->GetSelfUnum())){
    			BHPos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    			break;
    		}
    	}
    	BHPos = RoundToNearestHole(BHPos);

    	Vector BHfrontup = Vector(BHPos.X()+10, BHPos.Y()-10);
    	Vector BHbackup = Vector(BHPos.X()-10, BHPos.Y()-10);
    	Vector BHfrontdown = Vector(BHPos.X()+10, BHPos.Y()+10);
    	Vector BHbackdown = Vector(BHPos.X()-10, BHPos.Y()+10);

        Vector BHfronthor = Vector(BHPos.X()+20, BHPos.Y());
        Vector BHbackhor = Vector(BHPos.X()-20, BHPos.Y());
        Vector BHupvert = Vector(BHPos.X(), BHPos.Y()-20);
        Vector BHdownvert = Vector(BHPos.X(), BHPos.Y()+20);
    	//if frontup&frontdown not occupied (+ other conditions), move there
    	//TODO: Currently, it is possible that one player is expected to fill both the holes

        Unum FU, FD, BU, BD;
        Unum FH, BH, UV, DV;

        //&&BHfrontup.X()>=-45&&BHfrontup.X()<=45&&BHfrontup.Y()>=-25&&BHfrontup.Y()<=25

        FU = GetClosest(BHfrontup);
        if(FU==mpAgent->GetSelfUnum()&&BHfrontup.X()>=-45&&BHfrontup.X()<=45&&BHfrontup.Y()>=-25&&BHfrontup.Y()<=25){
            OccupyHole(BHfrontup);
            std::cout<<"Player "<<FU<<" occupying frontup"<<std::endl;
        }

        FD = GetClosestExcl1(BHfrontdown, FU);
        if(FD==mpAgent->GetSelfUnum()&&BHfrontdown.X()>=-45&&BHfrontdown.X()<=45&&BHfrontdown.Y()>=-25&&BHfrontdown.Y()<=25){
            OccupyHole(BHfrontdown);
            std::cout<<"Player "<<FD<<" occupying frontdown"<<std::endl;
        }

        FH = GetClosestExcl2(BHfronthor, FU, FD);
        if(FH==mpAgent->GetSelfUnum()&&BHfronthor.X()>=-45&&BHfronthor.X()<=45&&BHfronthor.Y()>=-25&&BHfronthor.Y()<=25){
            OccupyHole(BHfronthor);
            std::cout<<"Player "<<FH<<" occupying backdown"<<std::endl;
        }

        UV = GetClosestExcl3(BHupvert, FU, FD, FH);
        if(UV==mpAgent->GetSelfUnum()&&BHupvert.X()>=-45&&BHupvert.X()<=45&&BHupvert.Y()>=-25&&BHupvert.Y()<=25){
            OccupyHole(BHupvert);
            std::cout<<"Player "<<UV<<" occupying backdown"<<std::endl;
        }

        DV = GetClosestExcl4(BHdownvert, FU, FD, FH, UV);
        if(DV==mpAgent->GetSelfUnum()&&BHdownvert.X()>=-45&&BHdownvert.X()<=45&&BHdownvert.Y()>=-25&&BHdownvert.Y()<=25){
            OccupyHole(BHdownvert);
            std::cout<<"Player "<<DV<<" occupying backdown"<<std::endl;
        }

        BU = GetClosestExcl5(BHbackup, FU, FD, FH, UV, DV);
        if(BU==mpAgent->GetSelfUnum()&&BHbackup.X()>=-45&&BHbackup.X()<=45&&BHbackup.Y()>=-25&&BHbackup.Y()<=25){
            OccupyHole(BHbackup);
            std::cout<<"Player "<<BU<<" occupying backup"<<std::endl;
        }
        
        BD = GetClosestExcl6(BHbackdown, FU, FD, FH, UV, DV, BU);
        if(BD==mpAgent->GetSelfUnum()&&BHbackdown.X()>=-45&&BHbackdown.X()<=45&&BHbackdown.Y()>=-25&&BHbackdown.Y()<=25){
            OccupyHole(BHbackdown);
            std::cout<<"Player "<<BD<<" occupying backdown"<<std::endl;
        }


        BH = GetClosestExcl7(BHbackhor, FU, FD, FH, UV, DV, BU, BD);
        if(BH==mpAgent->GetSelfUnum()&&BHbackhor.X()>=-45&&BHbackhor.X()<=45&&BHbackhor.Y()>=-25&&BHbackhor.Y()<=25){
            OccupyHole(BHbackhor);
            std::cout<<"Player "<<BH<<" occupying backdown"<<std::endl;
        }



        /*
    	if(!IsOccupied(BHfrontup)||!IsOccupied(BHfrontdown)){
    		//&&mpAgent->GetSelfUnum()!=10
    		//if closest to frontup/frontdown, occupy it
    		if(IsClosest(BHfrontup)&&!IsOccupied(BHfrontup)){
                OccupyHole(RoundToNearestHole(BHfrontup));
                return;
            }
    		else if(IsClosest(BHfrontdown)&&!IsOccupied(BHfrontdown)){
    			OccupyHole(RoundToNearestHole(BHfrontdown));
                return;
            }
    	}
    	
        if(!IsOccupied(BHbackup)||!IsOccupied(BHbackdown)){
            //&&mpAgent->GetSelfUnum()==10    
    		//if closest to frontup/frontdown, occupy it
    		if(IsClosest(BHbackup)&&!IsOccupied(BHbackup)){
    			OccupyHole(RoundToNearestHole(BHbackup));
                return;
            }
    		else if(IsClosest(BHbackdown)&&!IsOccupied(BHbackdown)){
    			OccupyHole(RoundToNearestHole(BHbackdown));
                return;
            }
    	}
        */
    }

    Unum GetClosestTeammateTo(Vector target){
        double mindis = 99999;
        Unum mindisUnum = -1;
        for(Unum i=1; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            double dis = (player_pos.X()-target.X())*(player_pos.X()-target.X())+(player_pos.Y()-target.Y())*(player_pos.Y()-target.Y());
            if(dis<mindis){
                mindisUnum = i;
                mindis=dis;
            }
        }
        return mindisUnum;
    }

    void DecideAndOccupyHole(Unum target){
        //Called when another teammate has the ball
        //Decide if the player should support the ballholder by moving to an appropriate hole or not
        //Act as per the decision
        
        Vector BHPos;
        
        double buffer = 0.3;
        
        if(target!=mpAgent->GetSelfUnum()){
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
        //if frontup&frontdown not occupied (+ other conditions), move there
        //TODO: Currently, it is possible that one player is expected to fill both the holes

        Unum FU, FD, BU, BD;
        Unum FH, BH, UV, DV;

        //&&BHfrontup.X()>=-45&&BHfrontup.X()<=45&&BHfrontup.Y()>=-25&&BHfrontup.Y()<=25

        FU = GetClosest(BHfrontup);
        if(FU==mpAgent->GetSelfUnum()&&BHfrontup.X()>=-45&&BHfrontup.X()<=45&&BHfrontup.Y()>=-25&&BHfrontup.Y()<=25){
            OccupyHole(BHfrontup);
            std::cout<<"Player "<<FU<<" occupying frontup"<<std::endl;
        }

        FD = GetClosestExcl1(BHfrontdown, FU);
        if(FD==mpAgent->GetSelfUnum()&&BHfrontdown.X()>=-45&&BHfrontdown.X()<=45&&BHfrontdown.Y()>=-25&&BHfrontdown.Y()<=25){
            OccupyHole(BHfrontdown);
            std::cout<<"Player "<<FD<<" occupying frontdown"<<std::endl;
        }

        FH = GetClosestExcl2(BHfronthor, FU, FD);
        if(FH==mpAgent->GetSelfUnum()&&BHfronthor.X()>=-45&&BHfronthor.X()<=45&&BHfronthor.Y()>=-25&&BHfronthor.Y()<=25){
            OccupyHole(BHfronthor);
            std::cout<<"Player "<<FH<<" occupying backdown"<<std::endl;
        }

        UV = GetClosestExcl3(BHupvert, FU, FD, FH);
        if(UV==mpAgent->GetSelfUnum()&&BHupvert.X()>=-45&&BHupvert.X()<=45&&BHupvert.Y()>=-25&&BHupvert.Y()<=25){
            OccupyHole(BHupvert);
            std::cout<<"Player "<<UV<<" occupying backdown"<<std::endl;
        }

        DV = GetClosestExcl4(BHdownvert, FU, FD, FH, UV);
        if(DV==mpAgent->GetSelfUnum()&&BHdownvert.X()>=-45&&BHdownvert.X()<=45&&BHdownvert.Y()>=-25&&BHdownvert.Y()<=25){
            OccupyHole(BHdownvert);
            std::cout<<"Player "<<DV<<" occupying backdown"<<std::endl;
        }

        BU = GetClosestExcl5(BHbackup, FU, FD, FH, UV, DV);
        if(BU==mpAgent->GetSelfUnum()&&BHbackup.X()>=-45&&BHbackup.X()<=45&&BHbackup.Y()>=-25&&BHbackup.Y()<=25){
            OccupyHole(BHbackup);
            std::cout<<"Player "<<BU<<" occupying backup"<<std::endl;
        }
        
        BD = GetClosestExcl6(BHbackdown, FU, FD, FH, UV, DV, BU);
        if(BD==mpAgent->GetSelfUnum()&&BHbackdown.X()>=-45&&BHbackdown.X()<=45&&BHbackdown.Y()>=-25&&BHbackdown.Y()<=25){
            OccupyHole(BHbackdown);
            std::cout<<"Player "<<BD<<" occupying backdown"<<std::endl;
        }


        BH = GetClosestExcl7(BHbackhor, FU, FD, FH, UV, DV, BU, BD);
        if(BH==mpAgent->GetSelfUnum()&&BHbackhor.X()>=-45&&BHbackhor.X()<=45&&BHbackhor.Y()>=-25&&BHbackhor.Y()<=25){
            OccupyHole(BHbackhor);
            std::cout<<"Player "<<BH<<" occupying backdown"<<std::endl;
        }



        /*
        if(!IsOccupied(BHfrontup)||!IsOccupied(BHfrontdown)){
            //&&mpAgent->GetSelfUnum()!=10
            //if closest to frontup/frontdown, occupy it
            if(IsClosest(BHfrontup)&&!IsOccupied(BHfrontup)){
                OccupyHole(RoundToNearestHole(BHfrontup));
                return;
            }
            else if(IsClosest(BHfrontdown)&&!IsOccupied(BHfrontdown)){
                OccupyHole(RoundToNearestHole(BHfrontdown));
                return;
            }
        }
        
        if(!IsOccupied(BHbackup)||!IsOccupied(BHbackdown)){
            //&&mpAgent->GetSelfUnum()==10    
            //if closest to frontup/frontdown, occupy it
            if(IsClosest(BHbackup)&&!IsOccupied(BHbackup)){
                OccupyHole(RoundToNearestHole(BHbackup));
                return;
            }
            else if(IsClosest(BHbackdown)&&!IsOccupied(BHbackdown)){
                OccupyHole(RoundToNearestHole(BHbackdown));
                return;
            }
        }
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
    	for(Unum i=1; i<=11; i++){
    		if(mpAgent->GetWorldState().GetTeammate(i).IsKickable())
    			return i;
    	}
    	return -1;
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
        
    	double buffer = 0.5;
        double f = 0.6;
                
        //sstm << msgstr << mynum;
        //result = sstm.str();
        //mpAgent->Say(result);
    	
    	for(Unum i=1; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if(AreSamePoints(player_pos, frontup, buffer)){
                std::string msgstr = "cusp";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
    			return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
    			//return;
    		}
    	}
    	for(Unum i=1; i<=11; i++){
    		Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
    		if(AreSamePoints(player_pos, frontdown, buffer)){
                std::string msgstr = "cusp";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
    			//return;
    		}
    	}
        for(Unum i=1; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, fronthor, buffer)){
                std::string msgstr = "cusp";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=1; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, upvert, buffer)){
                std::string msgstr = "cusp";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=1; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, downvert, buffer)){
                std::string msgstr = "cusp";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=1; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, backhor, buffer)){
                std::string msgstr = "cusp";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=1; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, backup, buffer)){
                std::string msgstr = "cusp";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        for(Unum i=1; i<=11; i++){
            Vector player_pos = mpAgent->GetWorldState().GetTeammate(i).GetPos();
            if(AreSamePoints(player_pos, backdown, buffer)){
                std::string msgstr = "cusp";
                Unum mynum = mpAgent->GetSelfUnum();
                std::string result;
                std::stringstream sstm;
                sstm << msgstr << mynum <<"X"<< i;
                result.append(sstm.str());
                std::cout<<"saying - "<<result<<std::endl;
                while(!(mpAgent->Say(result)));
                return Kicker::instance().KickBall(*mpAgent, player_pos, ServerParam::instance().ballSpeedMax()*f, KM_Hard, 0, false);
                //return;
            }
        }
        return false;
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

