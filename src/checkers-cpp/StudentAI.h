#ifndef STUDENTAI_H
#define STUDENTAI_H
#include "AI.h"
#include "Board.h"
#pragma once

//The following part should be completed by students.
//Students can modify anything except the class name and exisiting functions and varibles.
class StudentAI :public AI
{
public:
    Board board;
	StudentAI(int col, int row, int p);
	virtual Move GetMove(Move board);
	//throttle int.
	int maxdepth;

private: 
	struct Node
	{
		//since we have to copy the board anyway theres no real reason to make this a pointer as far as I can tell.
		Board board;
		//added a vector of moves so that we can call getallpossible moves just once and then
		//populate the children vector as we need to.
		vector<vector<Move> > moves;
		//this is a map to map the x, y coordinates of whatever move we choose  to the child node that that move corresponds to.
		//This way we can keep track of what moves we've already taken and skip the process of making new nodes when we don't need to.
		//the int is calculated like x * rows + y, that way its impossible for two moves to get the same map key.
		map<int, Node*> children;
		int visits;
		int wins;
	};
	
	int recurseSim(Node* curNode, int depth, int curPlayer);
	void copyBoard(Board& newBoard, const Board& oldBoard);
	Node* makeNewNode(const Node& oldNode, int curPlayer);

};

#endif //STUDENTAI_H
