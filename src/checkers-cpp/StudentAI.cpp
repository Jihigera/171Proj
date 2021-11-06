#include "StudentAI.h"
#include <random>

//The following part should be completed by students.
//The students can modify anything except the class name and exisiting functions and varibles.
StudentAI::StudentAI(int col,int row,int p)
	:AI(col, row, p)
{
    board = Board(col,row,p);
    board.initializeGame();
    player = 2;
    maxdepth = 10;
}

//the game will automatically end in a tie btw if it needs to, I think there is a turn limit or something.
/**
    //This is the line in code to test against the poorAI.
    python3 AI_Runner.py {col} {row} {p} l {AI_1_path} {AI_2_path}

    //this one is for my directory setup
    //I couldn't get poor_ai to work so I used poor_ai_368 instead.
    //I set it to 10 by 10 since that is the maximum size they said they will test us at.
    //This makes our AI Player 1
    python3 AI_Runner.py 10 10 2 l /home/reedgc/cs171/Checkers_Student/src/checkers-cpp/main /home/reedgc/cs171/Checkers_Student/Tools/Sample_AIs/Poor_AI_368/main.py

    //This is to play against our AI manually.
    ./main {col} {row} {p} m {start_player (0 or 1)}

    //Heres what mine looks like
    ./main 10 10 2 m start_player 0

**/

//copies an oldBoard and puts it into newBoard.
void StudentAI::copyBoard(Board& newBoard, const Board& oldBoard)
{
    newBoard.col = oldBoard.col;
    newBoard.row = oldBoard.row;
    newBoard.p = oldBoard.p;
    newBoard.blackCount = oldBoard.blackCount;
    newBoard.whiteCount = oldBoard.whiteCount;
    newBoard.tieCount = oldBoard.tieCount;
    newBoard.tieMax = oldBoard.tieMax;
    newBoard.saved_move_list = oldBoard.saved_move_list;
}

//Creates a new node by copying the board of oldNode.
StudentAI::Node* StudentAI::makeNewNode(const Node& oldNode, int curPlayer)
{
    Node* newNode = new Node(); 
    copyBoard(newNode->board, oldNode.board);
    newNode->visits = 0;
    newNode->wins = 0;
    newNode->moves = newNode->board.getAllPossibleMoves(curPlayer);

    return newNode;
}

int StudentAI::recurseSim(Node* curNode, int depth, int curPlayer)
{
    int winner = -1;
    //first choose a random move, then check if that move has been chosen before. if so then call the function again with the previously
    //created child from curNode.
    //If not then create a new child for curNode, make a move and call the function again.
    //check if depth has exceeded max depth.
 
        //this chooses the random x, y for the move (adjusted from profs random code to avoid making a new vector for no reason that I can think of.)
        int moveX = rand() % (curNode->moves.size());
        int moveY = rand() % (curNode->moves[moveX].size());
        //turns the 2d x,y coords to a long 1d array by making single int values for the map key.
        int moveKey = moveX*board.row + moveY;
        //This checks if this move has been chosen by this node before already.
        if(curNode->children.at(moveKey) == NULL)
        {
            curNode->children[moveKey] = makeNewNode(*curNode, curPlayer);        
            Move simMove = curNode->children[moveKey]->moves[moveX][moveY];
            try
            {
                curNode->children[moveKey]->board.makeMove(simMove, curPlayer);
            }
            catch (InvalidMoveError)
            {
                cerr << "Invalid Move!" << endl;
            }   
            //we need to check if we won after we make the move and are still as this player, that's how gamelogic does it.
            //the isWin function is super wack, I'm not exactly sure how to call it yet to make this work
            //basically I'm not sure if we should call it with our player value of 2 that the studentAi constructor gives us
            //or with our curPlayer variable that keeps track of which player is which inside the recursion simulation. 
            if(curNode->board.isWin(player) == curPlayer)
            {
                winner = curPlayer
                return winner;
            }
            else
            {
                winner = curPlayer == 1 ? 2 : 1;
                return winner;
            }
        } 
        
        //If this move hasn't been chosen before the code above runs which creates a new node at the moveKey in this current node's children
        //then makes the move on the new node's board then we call the function again passing that new node.
        //If this move has been taken before we call the function again, passing the already created node from the last time 
        //we chose this move. it also adds one to the depth counter and changes the current player to the other player.
        winner = recurseSim(curNode->children.at(moveKey), depth++, curPlayer = curPlayer == 1 ? 2:1);

        //If we've returned from recurseSim, that means we know if this path led to a win or not, so time for back propogation.
        curNode->visits++;

        //If this player is the player that won then increment wins too.
        if ((winner == 2 && curPlayer == 2) || (winner == 1 && curPlayer ==1))
            curNode->wins++;
        
        return winner;
}

Move StudentAI::GetMove(Move move)
{
    if (move.seq.empty())
    {
        player = 1;
    } else{
        board.makeMove(move,player == 1?2:1);
    }

    //throttling simulations
    int simulations = 100;
    //Creating a node to store the root node of the tree.
    Node* root = new Node();
    copyBoard(root->board, this->board);
    root->visits = 0;
    root->wins = 0;
    root->moves = board.getAllPossibleMoves(player);

    //using curPlayer to keep track of who's who when in recursion. Our AI is always set to player 2 by professor when studentAI is constructed.
    //this needs to switch between 1 and 2 during recursion, otherwise isWin() won't work properly. (which is why its an int instead of bool)
    int curPlayer = player;

    while(simulations < 100)
    {
        int depth = 0;
        recurseSim(root, depth, curPlayer);

        simulations++;
    }

    //Going through all of the moves in the root node's children map and checking which one scored highest.
    float bestScore = 0.0;
    int bestMoveKey = root->children.begin()->first;
    for (auto it = root->children.begin(); it != root->children.end(); it++) {
        float thisScore = (float)(it->second->wins / it->second->visits);
        if(bestScore < thisScore)
        {
            bestScore = thisScore;
            bestMoveKey = it->first;
        }
    }
    //getting the move indices back from the move key
    int bestMoveKeyY = bestMoveKey % board.row;
    int bestMoveKeyX = (bestMoveKey - bestMoveKeyY)/board.row;
    Move bestMove = root->moves[bestMoveKeyX][bestMoveKeyY]; 
    //deleteAllNodes();
    board.makeMove(bestMove,player);
    return bestMove;


    //random move provided to us
    /*vector<vector<Move> > moves = board.getAllPossibleMoves(player);
    int i = rand() % (moves.size());
    vector<Move> checker_moves = moves[i];
    int j = rand() % (checker_moves.size());
    Move res = checker_moves[j]; */
}

