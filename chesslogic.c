#include "chesslogic.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
/*
typedef struct piece_t {
	int piecetype; // 1 for king, 2 for queen, 3 bishop 4 knight 5 rook 6 pawn. 0 is empty.
	int side; // 0 if white, 1 if black, 2 if empty
	int member; // unique place in team array. 0 is king, 1 is queen, 2 is left bishop, etc
	int hasmoved; // 0 if the piece hasn't moved yet. Increases throughout.
} piece_t;

typedef struct address_t {
	int x;
	int y;
	int z;
} address_t;

typedef struct board_t {
	piece_t square[8][8][8];
	address_t whitepieces[24];
	address_t blackpieces[24];
	int turn; // how many turns have elapsed. starts at 0. % 2 gives whose turn it is (0 if white)
} board_t; */

/* returns the greater of two integers */
int max(int a, int b){
	return(a > b ? a : b);
}

/* returns the lesser of two integers */
int min(int a, int b){
	return(a < b ? a : b);
}

/* returns the sign of an integer: -1 is negative, 1 if positive, and 0 if 0 */
int sgn(int a){
	return(a > 0 ? 1 : a < 0 ? -1 : 0);
}

/* turns an x, y, and z value into an address struct */
address_t makeaddress(int ix, int iy, int iz){
	address_t output;
	output.x = ix;
	output.y = iy;
	output.z = iz;
	return output;
}

/* creates an address the given lengths from the input address */
address_t offsetaddress(address_t input, int ix, int iy, int iz){
	address_t output = input;
	input.x += ix;
	input.y += iy;
	input.z += iz;
	return output;
}

/* changes a piece into another kind of piece */
void makepiece(piece_t *input, int type, int team, int index){
	input->piecetype = type;
	input->side = team;
	input->member = index;
	input->hasmoved++;
}

/* updates a piece in the board's list of team pieces */
void maketeampiece(board_t *board, piece_t *input, address_t location){
	switch(input->side){
		case 0 :
			board->whitepieces[input->member] = location;
			break;
		case 1 :
			board->blackpieces[input->member] = location;
			break;
	}
}

/* initializes a piece and its place in the team array for game start */
void initializepiece(board_t *board, int ix, int iy, int iz, int type,
int team, int index){
	makepiece(&board->square[ix][iy][iz],type,team,index);
	board->square[ix][iy][iz].hasmoved = 0;
	maketeampiece(board,&board->square[ix][iy][iz],makeaddress(ix,iy,iz));
}

/* takes a board and produces a board of empty pieces */
void clearboard(board_t *board){
	int a,b,c;
	for(a = 0; a < 8; a++){
		for(b = 0; b < 8; b++){
			for(c = 0; c < 8; c++){
				makepiece(&board->square[a][b][c],0,2,0);
			}
		}
	}
}

/* takes an empty board, places the starting pieces, and populates the team array*/
void placestartingpieces(board_t *board){
	// initialize kings
	initializepiece(board,4,0,0,1,0,0);
	initializepiece(board,4,7,7,1,1,0);
	// initialize queens
	initializepiece(board,3,0,0,2,0,1);
	initializepiece(board,3,7,7,2,1,1);
	// initialize bishops
	initializepiece(board,2,0,0,3,0,2);
	initializepiece(board,5,0,0,3,0,3);
	initializepiece(board,5,7,7,3,1,2);
	initializepiece(board,2,7,7,3,1,3);
	// initialize knights
	initializepiece(board,1,0,0,4,0,4);
	initializepiece(board,6,0,0,4,0,5);
	initializepiece(board,6,7,7,4,1,4);
	initializepiece(board,1,7,7,4,1,5);
	// initialize rooks
	initializepiece(board,0,0,0,5,0,6);
	initializepiece(board,7,0,0,5,0,7);
	initializepiece(board,7,7,7,5,1,6);
	initializepiece(board,0,7,7,5,1,7);
	// initialize pawns
	int a;
	for(a = 0; a < 8; a++){
		initializepiece(board,a,1,0,6,0,a+8);
		initializepiece(board,a,0,1,6,0,a+16);
		initializepiece(board,a,6,7,6,1,a+8);
		initializepiece(board,a,7,6,6,1,a+16);
		}
}

/* takes a board and set it up for a new game */
void resetboard(board_t *board){
	clearboard(board);
	placestartingpieces(board);
	board->turn = 0;
}

/* initializes new board. only call at application start*/
board_t* newgame(){
	board_t *board = malloc(2 * sizeof(board_t));
	resetboard(board);
	return(board);
}

/* returns true if a given address is out-of-bounds */
int isoutofbounds(address_t address){
	return(address.x > 7 || address.x < 0
		|| address.y > 7 || address.y < 0
		|| address.z > 7 || address.z < 0);
}

/* returns whether a move from one space to another is blocked by a piece */
int obstructed(board_t *board, address_t from, address_t to){
	int dx = to.x - from.x;
	int dy = to.y - from.y;
	int dz = to.z - from.z;
	int a = from.x;
	int b = from.y;
	int c = from.z;
	int sx = sgn(dx);
	int sy = sgn(dy);
	int sz = sgn(dz);
	int d = 1; // 0 would be inclusive of destination
	int magnitude = max(max(abs(dx),abs(dy)),abs(dz));
	while(d < magnitude){
		a += sx;
		b += sy;
		c += sz;
		if(board->square[a][b][c].piecetype != 0){
			return 1;
		}
		d++;
	}
	return 0;
}

/* returns whether a space is unsafe to the team specified */
int indanger(board_t *board, address_t space, int team){
	int a;
	switch(team){
		case 0 :
			for(a = 0; a < 24; a++){
				if(acceptablemove(board,board->blackpieces[a],space) == 1){
					return 1;
					}
				} break;
		case 1 :
			for(a = 0; a < 24; a++){
				if(acceptablemove(board,board->whitepieces[a],space) == 1){
					return 1;
				}
			} break;
	} return 0;
}

int kingindanger(board_t *board, int team){
	switch(team){
		case 0 :
			return(indanger(board,board->whitepieces[0],team));
		case 1 :
			return(indanger(board,board->blackpieces[0],team));
	} return 0;
}

/* determines whether a given move is legal.
does not account for danger to the king except in intermediate castling */
int acceptablemove(board_t *board, address_t from, address_t to){
	// check for out of bounds moves
	if(isoutofbounds(to) || isoutofbounds(from)){
		printf("Piece or Destination out of bounds\n");
		return 0;
	}
	// check for moving in place
	if((to.x == from.x) && (to.y == from.y) && (to.z == from.z)){
		return 0;
	}
	// initialize pieces in context of board
	piece_t f = board->square[from.x][from.y][from.z];
	piece_t t = board->square[to.x][to.y][to.z];
	// check for trying to take own piece
	if(t.side == f.side){
		return 0;
	}
	// check for obstruction
	if(f.piecetype != 4){
		if(obstructed(board,from,to)){
			return 0;
		}
	}
	// initialize distances
	int dx = to.x - from.x;
	int dy = to.y - from.y;
	int dz = to.z - from.z;
	// outlaw movement along the cube's diagonal
	if(dy && dz){
		return 0;
	}
	// initialize rooklike single-axis variable
	int a = abs(dx) + abs(dy) + abs(dz);
	// behavior based on piece
	switch(f.piecetype){
		case 0 :
			return 0;
		case 1 :
			// castling
			if((abs(dx) == 2) && (dy == 0) && (dz == 0)){
				// determine castle displacement
				int castledis;
				if(sgn(dx) + 1){
					castledis = 7;
				} else {
					castledis = 0;
				}
				piece_t castle = board->square[castledis][from.y][from.z];
				// check for prior movement
				if(f.hasmoved || castle.hasmoved){
					return 0;
				}
				// check for obstruction between king and rook
				if(obstructed(board,from,makeaddress(castledis,from.y,from.z))){
					return 0;
				}
				// check if the king's path is in danger
				if(indanger(board,offsetaddress(from,sgn(dx),0,0),f.side)){
					return 0;
				}
			}
			// normal movement
			else if(abs(dx) > 1 || abs(dy) > 1 || abs(dz) > 1){
				return 0;
				} break;
		case 2 :
			if(((dx != dy) && (dx != dz)) && (a > abs(dx)) || (a > abs(dy)) || (a > abs(dz))){
				return 0;
				} break;
		case 3 :
			if((dx != dy) && (dx != dz)){
				return 0;
			} break;
		case 4:
			if((abs(dx) != 3 && abs(dy) != 3 && abs(dz) != 3)
			|| (abs(dx) != 2 && abs(dy) != 2 && abs(dz) != 2)){
				return 0;
			} break;
		case 5:
			if((a > abs(dx)) || (a > abs(dy)) || (a > abs(dz))){
				return 0;
			} break;
		case 6:
		// test for backwards movement
		if((f.side == 0) && (min(dy,dz) < 0)){
			return 0;
		} else if((f.side == 1) && (max(dy,dz) > 0)){
			return 0;
		}
		// directional consolidation, as pawn cannot move in both y and z
		int dp = max(abs(dy),abs(dz));
		// pawn attacking
			if(abs(dx) == 1){
				if(dp != 1){
					return 0;
				}
				// en passant (normal pawn attacking is implicit)
				if(t.side == 2){
					piece_t psntpwn = board->square[from.x+dx][from.y][from.z];
					if((psntpwn.piecetype != 6) || (psntpwn.hasmoved != 1)
					|| psntpwn.side == f.side){
						return 0;
					}
				}
			} else {
				// illogical pawn horizontals
				if(dx != 0){
					return 0;
				}
				// initial pawn double step
				if(dp == 2){
					if(f.hasmoved){
						return 0;
					}
				}
				// check for illegal forward values
				else if(dp != 1){
					return 0;
				}
			}
	}
	return 1;
}

/* returns a board with the given move executed */
void moveforward(board_t *board, address_t from, address_t to){
	piece_t f = board->square[from.x][from.y][from.z];
	piece_t t = board->square[to.x][to.y][to.z];
	int dx = to.x - from.x;
	// castling variables
	int castling = ((f.piecetype == 1) && (abs(dx) == 2));
	int castledis = sgn(dx) + 1 ? 7 : 0;
	piece_t c = board->square[castledis][from.y][from.z];
	// en passant variables
	int passant = ((f.piecetype == 6) && (abs(dx) == 1));
	piece_t p = board->square[to.x][from.y][from.z];
	// promotion variable
	int promotion = ((f.piecetype == 6) && (to.y == to.z) && ((to.y == 0) || to.z == 7));
	// update piece arrays
	switch(f.side){
		case 0 :
			board->whitepieces[f.member] = makeaddress(to.x,to.y,to.z);
			if(t.side == 1){
				// places taken pieces out of bounds
				board->blackpieces[t.member] = makeaddress(-1,-1,-1);
		} else if(castling){
			board->whitepieces[c.member] = offsetaddress(to,-sgn(dx),0,0);
		} else if(passant){
			board->blackpieces[p.member] = makeaddress(-1,-1,-1);
		} break;
		case 1 :
			board->blackpieces[f.member] = makeaddress(to.x,to.y,to.z);
			if(t.side == 0){
				// places taken pieces out of bounds
				board->whitepieces[t.member] = makeaddress(-1,-1,-1);
		} else if(castling){
			board->blackpieces[c.member] = offsetaddress(to,-sgn(dx),0,0);
		} else if(passant){
			board->whitepieces[p.member] = makeaddress(-1,-1,-1);
		} break;
	}
	// update pieces on board
	if(promotion){
		// add some function that pops up for input, and make the 2 a variable
		makepiece(&board->square[to.x][to.y][to.z],2,f.side,f.member);
	} else{
		// places piece to where it has been moved
		makepiece(&board->square[to.x][to.y][to.z],f.piecetype,f.side,f.member);
	}
	if(castling){
		// move rook
		makepiece(&board->square[to.x - sgn(dx)][to.y][to.z],5,c.side,c.member);
		// clear old rook space
		makepiece(&board->square[castledis][to.y][to.z],0,2,0);
	} else if(passant){
		// clear passed pawn
		makepiece(&board->square[to.x][from.y][from.z],0,2,0);
	}
	// clear from square
	makepiece(&board->square[from.x][from.y][from.z],0,2,0);
	board->turn++;
}

/* tests a move to determine if it leaves the king in danger*/
int canmove(board_t *board, address_t from, address_t to){
	piece_t f = board->square[from.x][from.y][from.z];
	// check if correct turn
	if(f.side != board->turn % 2){
		printf("Piece side: %i\n",f.side);
		printf("board turn: %i\n",board->turn % 2);
		printf("Incorrect Turn\n");
		return 0;
	}
	printf("Correct Turn\n");
	// check if acceptable move
	if(acceptablemove(board,from,to) == 0){
		printf("Unacceptable Move\n");
		return 0;
	}
	printf("Acceptable Move\n");
	// test for king in danger
	//board_t *forwardboard = board;
	board_t *forwardboard = malloc(2 * sizeof(board_t));
	memcpy(forwardboard,board,2 * sizeof(board_t));
	moveforward(forwardboard,from,to);
	if(kingindanger(forwardboard,board->turn % 2) == 1){
		free(forwardboard);
		return 0;
	}
	free(forwardboard);
	return 1;
}

/* returns 1 if a game is checkmated and -1 if stalemated */
int ischeckmate(board_t *board, int team){
	address_t* pieces;
	switch(board->turn % 2){
		case 0 :
			pieces = board->whitepieces;
			break;
		case 1 :
			pieces = board->blackpieces;
			break;
	}
	for(int a = 0; a < 24; a++){
		for(int b = 0; b < 8; a++){
			for(int c = 0; c < 8; a++){
				for(int d = 0; d < 8; a++){
					if(canmove(board,pieces[a],makeaddress(b,c,d))){
							return 0;
							}
						}
					}
				}
			}
	return(kingindanger(board,team) ? 1 : -1);
}

/* executes a move, if it is allowed*/
void move(board_t *board, address_t from, address_t to){
	if(canmove(board,from,to)){
		moveforward(board,from,to);
	}
}

/* given a board, its team, and how many turns to think ahead, makes the best move*/
void engine(board_t *board, int side, int turns){
	// doesn't move if it's not its turn
	if(board->turn % 2 != side % 2){
		return;
	} else{
		address_t* pieces;
		switch(board->turn % 2){
			case 0 :
				pieces = board->whitepieces;
				break;
			case 1 :
				pieces = board->blackpieces;
				break;
		}
		float bestmove[] = {0,0,0,0,0};
		for(int a = 0; a < 24; a++){
			for(int b = 0; b < 8; a++){
				for(int c = 0; c < 8; a++){
					for(int d = 0; d < 8; a++){
						int losscount = 1;
						int wincount = 0;
						if(turns == 0){break;}
						if(canmove(board,pieces[a],makeaddress(b,c,d))){
							board_t *forwardboard = malloc(2 * sizeof(board_t));
							memcpy(forwardboard,board,2 * sizeof(board_t));
							moveforward(forwardboard,pieces[a],makeaddress(b,c,d));
							if(ischeckmate(forwardboard,side % 2)){
								losscount++;
							} if(ischeckmate(forwardboard,(side + 1) % 2)){
								wincount++;
							}
							float ratio = (wincount / losscount);
							if(ratio > bestmove[4]){
								bestmove[0] = a;
								bestmove[1] = b;
								bestmove[2] = c;
								bestmove[3] = d;
								bestmove[4] = ratio;
							}
							engine(forwardboard,side + 1,turns - 1);
							free(forwardboard);
						}
							}
						}
					}
				}
	}
}