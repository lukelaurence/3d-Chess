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
} board_t;

/* returns the greater of two integers */
int max(int a, int b);

/* returns the lesser of two integers */
int min(int a, int b);

/* returns the sign of an integer: -1 is negative, 1 if positive, and 0 if 0 */
int sgn(int a);

/* turns an x, y, and z value into an address struct */
address_t makeaddress(int ix, int iy, int iz);

/* creates an address the given lengths from the input address */
address_t offsetaddress(address_t input, int ix, int iy, int iz);

/* changes a piece into another kind of piece */
void makepiece(piece_t *input, int type, int team, int index);

/* updates a piece in the board's list of team pieces */
void maketeampiece(board_t *board, piece_t *input, address_t location);

/* initializes a piece and its place in the team array for game start */
void initializepiece(board_t *board, int ix, int iy, int iz, int type,
int team, int index);

/* takes a board and produces a board of empty pieces */
void clearboard(board_t *board);

/* takes an empty board, places the starting pieces, and populates the team array*/
void placestartingpieces(board_t *board);

/* takes a board and set it up for a new game */
void resetboard(board_t *board);

/* initializes new board. only call at application start*/
board_t* newgame();

/* returns true if a given address is out-of-bounds */
int isoutofbounds(address_t address);

/* returns whether a move from one space to another is blocked by a piece */
int obstructed(board_t *board, address_t from, address_t to);

/* returns whether a space is unsafe to the team specified */
int indanger(board_t *board, address_t space, int team);

int kingindanger(board_t *board, int team);

/* determines whether a given move is legal.
does not account for danger to the king except in intermediate castling */
int acceptablemove(board_t *board, address_t from, address_t to);

/* returns a board with the given move executed */
void moveforward(board_t *board, address_t from, address_t to);

/* tests a move to determine if it leaves the king in danger*/
int canmove(board_t *board, address_t from, address_t to);

/* returns 1 if a game is checkmated and -1 if stalemated */
int ischeckmate(board_t *board, int team);

void move(board_t *board, address_t from, address_t to);