/*
 * MinimaxPlayer.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: wong
 */
#include <iostream>
#include <assert.h>
#include "MinimaxPlayer.h"

using std::vector;

MinimaxPlayer::MinimaxPlayer(char symb) :
		Player(symb) {

}

MinimaxPlayer::~MinimaxPlayer() {

}

// Calls first generate_successors
void MinimaxPlayer::get_move(OthelloBoard* b, int& col, int& row) {
    // To be filled in by you
	Move move;

	char curr_symbol = get_symbol();
	move = generate_successors(b, curr_symbol);

	col = move.col;
	row = move.row;
}

// Generates the successors to the current OthelloBoard state based on the current symbol (MAX/MIN)
Move generate_successors(OthelloBoard *b, char symbol){
	Move curr_move;

	if(symbol == 'X'){ // AI is maximizing player
		curr_move.score = -1000; // Set current move score to -inf

	}else{ // AI is minimizing player
		curr_move.score = 1000; // Set current move score to inf
	}

	if(b->has_legal_moves_remaining(symbol)){ // Check if current board has any legal moves left
		
		for(int c = 0; c < b->get_num_cols(); c++){ // make a move for every possible col row combination
			for(int r = 0; r < b->get_num_rows(); r++){
				make_move(b, curr_move, c, r, symbol);
			}
		}	
		return curr_move;
	}else{ // If no legal moves left, calc utility of board state and return 
		curr_move.score = calculate_utility(b); 
		return curr_move;
	}

}

// Based on the OthelloBoard state and the selected col, row, and symbol (MAX/MIN), make a simulated move.
// Does nothing if not a legal move. Otherwise, Creates new board state based on move and calls generate_successors.
// After receiving a best MAX/MIN new_move from generate successors, checks to see if it improves upon curr_move.score. 
// If new_score is better than curr_score, store the new score, row, and column into curr_move.
void make_move(OthelloBoard *b, Move& curr_move, int c, int r, char symbol){
	char next_symbol;
	OthelloBoard* new_board;

	if(symbol == 'X'){
		next_symbol = b->get_p2_symbol();

	}else{
		next_symbol = b->get_p1_symbol();
	}

	if(b->is_legal_move(c, r, symbol)){
		new_board = new OthelloBoard(*b);
		new_board->play_move(c, r, symbol);
		Move new_move = generate_successors(new_board, next_symbol);
		int new_score = new_move.score;
		if(symbol == 'X' && new_score > curr_move.score || symbol == 'O' && new_score < curr_move.score){
			curr_move.score = new_score;
			curr_move.row = r;
			curr_move.col = c;
		}
		new_board->~OthelloBoard();
	}
}

MinimaxPlayer* MinimaxPlayer::clone() {
	MinimaxPlayer* result = new MinimaxPlayer(symbol);
	return result;
}

// Calculates utility based on player 1's score - player 2's score
// Higher score for p1 is better. Lower score for p2 is better
int calculate_utility(OthelloBoard *b){
	int utility = b->count_score(b->get_p1_symbol())-b->count_score(b->get_p2_symbol());
	return utility;
}