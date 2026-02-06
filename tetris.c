#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include "conio.h"

#define BLACKBG "\e[40m"
#define REDBG "\e[41m"
#define GREENBG "\e[42m"
#define YELLOWBG "\e[43m"
#define BLUEBG "\e[44m"
#define PURPLEBG "\e[45m"
#define CYANBG "\e[46m"
#define WHITEBG "\e[47m"

#define BLACKTEXT "\e[0;30m"
#define REDTEXT "\e[0;31m"
#define GREENTEXT "\e[0;32m"
#define YELLOWTEXT "\e[0;33m"
#define BLUETEXT "\e[0;34m"
#define PURPLETEXT "\e[0;35m"
#define CYANTEXT "\e[0;36m"
#define WHITETEXT "\e[0;37m"

#define SQUARECH "\u25a0"
#define RECTCH "\u2588"

const char colours[7][20] = {
	REDBG,
	GREENBG,
	YELLOWBG,
	BLUEBG,
	PURPLEBG,
	CYANBG,
	WHITEBG
};

const char* filename = "tetris_scores.txt";
const char* bindfilename = "tetris_bindings.txt";

//const char tcolours[7][20] = {
//	REDTEXT,
//	GREENTEXT,
//	YELLOWTEXT,
//	BLUETEXT,
//	PURPLETEXT,
//	CYANTEXT,
//	WHITETEXT
//};

#define RESETTEXT "\e[0m"

//we keep track of all 4 parts of a piece

typedef struct PPart {
	int x;
	int y;
} PPart;

typedef struct Piece {
	PPart
	p1,
	p2,
	p3,
	p4;

	PPart* origin; //for rotations
	char type; //for colours
} Piece;

typedef enum Piecetype {
	T = 0,
	STRAIGHT,
	CUBE,
	S,
	Z,
	L,
	BL
} Piecetype;

void shuffleBag(Piecetype bag[7]) {
	int i;
	for (i = 0; i < 7 - i; i++) {
		int j = i + rand() / (RAND_MAX / (7 - i) + 1);
		Piecetype temp = bag[i];
		bag[i] = bag[j];
		bag[j] = temp;
		//bag[j] = (bag[i] ^ bag[j]);
		//bag[i] = (bag[j] ^ bag[i]);
		//bag[j] = (bag[i] ^ bag[j]);
	}
}

//we cannot have the same piece appear 3 times in a row.
Piecetype getRandomPiecetype() {
	const int count = 7;
	const int choice = rand() % count;
	
	Piecetype new = (Piecetype) choice;
	
	return (Piecetype) choice;
}

Piece generatePiece(Piecetype type) {
	Piece piece;
	piece.type = (char) type;

	switch (type) {
		case T: {
			piece.p1.x = 5; piece.p1.y = 0;
			piece.p2.x = 5; piece.p2.y = 1;
			piece.p3.x = 4; piece.p3.y = 1;
			piece.p4.x = 6; piece.p4.y = 1;
			piece.origin = &piece.p2;
			break;
		}
		case STRAIGHT: {
			piece.p1.x = 5; piece.p1.y = 0;
			piece.p2.x = 5; piece.p2.y = 1;
			piece.p3.x = 5; piece.p3.y = 2;
			piece.p4.x = 5; piece.p4.y = 3;
			piece.origin = &piece.p2;
			break;
		}
		case CUBE: {
			piece.p1.x = 5; piece.p1.y = 0;
			piece.p2.x = 5; piece.p2.y = 1;
			piece.p3.x = 6; piece.p3.y = 0;
			piece.p4.x = 6; piece.p4.y = 1;
			piece.origin = &piece.p2;
			break;
		}
		case S: {
			piece.p1.x = 6; piece.p1.y = 0;
			piece.p2.x = 5; piece.p2.y = 0;
			piece.p3.x = 5; piece.p3.y = 1;
			piece.p4.x = 4; piece.p4.y = 1;
			piece.origin = &piece.p3;
			break;
		}
		case Z: {
			piece.p1.x = 4; piece.p1.y = 0;
			piece.p2.x = 5; piece.p2.y = 0;
			piece.p3.x = 5; piece.p3.y = 1;
			piece.p4.x = 6; piece.p4.y = 1;
			piece.origin = &piece.p3;
			break;
		}
		case L: {
			piece.p1.x = 5; piece.p1.y = 0;
			piece.p2.x = 5; piece.p2.y = 1;
			piece.p3.x = 5; piece.p3.y = 2;
			piece.p4.x = 6; piece.p4.y = 2;
			piece.origin = &piece.p2;
			break;
		}
		case BL: {
			piece.p1.x = 5; piece.p1.y = 0;
			piece.p2.x = 5; piece.p2.y = 1;
			piece.p3.x = 5; piece.p3.y = 2;
			piece.p4.x = 4; piece.p4.y = 2;
			piece.origin = &piece.p2;
			break;
		}
	}

	return piece;
}



void rotatePiece(Piece* piece, bool direction) {
	//we rotate in accordance with the origin.
	int originx = piece->origin->x;
	int originy = piece->origin->y;

	Piece newpiece = *piece;
	newpiece.p1.x -= originx;
	newpiece.p1.y -= originy;
	newpiece.p2.x -= originx;
	newpiece.p2.y -= originy;
	newpiece.p3.x -= originx;
	newpiece.p3.y -= originy;
	newpiece.p4.x -= originx;
	newpiece.p4.y -= originy;

	//then rotate
	//90 clock
	//[0  1] 
	//[-1 0]
	//
	//90 anticlock
	//[0 -1]
	//[1  0]
	if (direction) { //clockwise
		int x = newpiece.p1.x;
		int y = newpiece.p1.y;
		newpiece.p1.x = y;
		newpiece.p1.y = -1 * x;

		x = newpiece.p2.x;
		y = newpiece.p2.y;
		newpiece.p2.x = y;
		newpiece.p2.y = -1 * x;
		
		x = newpiece.p3.x;
		y = newpiece.p3.y;
		newpiece.p3.x = y;
		newpiece.p3.y = -1 * x;
		
		x = newpiece.p4.x;
		y = newpiece.p4.y;
		newpiece.p4.x = y;
		newpiece.p4.y = -1 * x;
	}
	else { //anticlockwise
		int x = newpiece.p1.x;
		int y = newpiece.p1.y;
		newpiece.p1.x = -1 * y;
		newpiece.p1.y = x;

		x = newpiece.p2.x;
		y = newpiece.p2.y;
		newpiece.p2.x = -1 * y;
		newpiece.p2.y = x;
		
		x = newpiece.p3.x;
		y = newpiece.p3.y;
		newpiece.p3.x = -1 * y;
		newpiece.p3.y = x;
		
		x = newpiece.p4.x;
		y = newpiece.p4.y;
		newpiece.p4.x = -1 * y;
		newpiece.p4.y = x;
	}
	
	//reset back to origin
	newpiece.p1.x += originx;
	newpiece.p1.y += originy;
	newpiece.p2.x += originx;
	newpiece.p2.y += originy;
	newpiece.p3.x += originx;
	newpiece.p3.y += originy;
	newpiece.p4.x += originx;
	newpiece.p4.y += originy;
	
	*piece = newpiece;

	
}

bool validPiecePosition(int glength, int gheight, char grid[glength][gheight], Piece* piece) {
	//assume this piece has just been updated, we return if it overlaps with any other piece or not
	
	for (int x = 0; x < glength; x++) {
		for (int y = 0; y < gheight; y++) {
			if ((grid[x][y] >= 0) && ((piece->p1.x == x && piece->p1.y == y) || (piece->p2.x == x && piece->p2.y == y) || (piece->p3.x == x && piece->p3.y == y) || (piece->p4.x == x && piece->p4.y == y))) {
				return false;
			}
		}
	}

	//we also want to check if the piece goes outside borders, specifically left right and bottom borders
	
	if (piece->p1.x < 0 || piece->p2.x < 0 || piece->p3.x < 0 || piece->p4.x < 0) {
		return false;
	} 

	if (piece->p1.x >= glength || piece->p2.x >= glength || piece->p3.x >= glength || piece->p4.x >= glength) {
		return false;
	}

	if (piece->p1.y >= gheight || piece->p2.y >= gheight || piece->p3.y >= gheight || piece->p4.y >= gheight) {
		return false;
	}


	return true;
}

bool loop(int glength, int gheight, char grid[glength][gheight], Piece* piece, char shiftCount, char bindings[5]) {
	//order of what happens
	//first move down
	//second check for collisions with floor / other pieces. if true return immediately.
	//then check io for rotating and moving. make sure the result doesnt go inside other pieces.
	

	Piece newpiece = *piece;
	
	//move down only when shiftcount is 0
	if (shiftCount == 0) {
		newpiece.p1.y++;
		newpiece.p2.y++;
		newpiece.p3.y++;
		newpiece.p4.y++;
		
		if (!validPiecePosition(glength, gheight, grid, &newpiece)) {
			newpiece.p1.y--;
			newpiece.p2.y--;
			newpiece.p3.y--;
			newpiece.p4.y--;

			*piece = newpiece;
			return false;
		}
	}

	//this means that the piece will continue on moving down, so we allow IO here.
	//for each io, we check if we can make that move. if we cant, we revert changes.
	if (kbhit()) {
		char key = getch();
		if (key == bindings[0]) { // move left
		    newpiece.p1.x--;
		    newpiece.p2.x--;
		    newpiece.p3.x--;
		    newpiece.p4.x--;
		    if (!validPiecePosition(glength, gheight, grid, &newpiece)) {
			newpiece.p1.x++;
			newpiece.p2.x++;
			newpiece.p3.x++;
			newpiece.p4.x++;
		    }
		} 
		else if (key == bindings[1]) { // move right
		    newpiece.p1.x++;
		    newpiece.p2.x++;
		    newpiece.p3.x++;
		    newpiece.p4.x++;
		    if (!validPiecePosition(glength, gheight, grid, &newpiece)) {
			newpiece.p1.x--;
			newpiece.p2.x--;
			newpiece.p3.x--;
			newpiece.p4.x--;
		    }
		} 
		else if (key == bindings[2]) { // rotate clockwise
		    rotatePiece(&newpiece, true);
		    if (!validPiecePosition(glength, gheight, grid, &newpiece)) {
			rotatePiece(&newpiece, false);
		    }
		} 
		else if (key == bindings[3]) { // rotate anticlockwise
		    rotatePiece(&newpiece, false);
		    if (!validPiecePosition(glength, gheight, grid, &newpiece)) {
			rotatePiece(&newpiece, true);
		    }
		} 
		else if (key == bindings[4]) { // move straight down
		    while (validPiecePosition(glength, gheight, grid, &newpiece)) {
			newpiece.p1.y++;
			newpiece.p2.y++;
			newpiece.p3.y++;
			newpiece.p4.y++;
		    }
		    newpiece.p1.y--;
		    newpiece.p2.y--;
		    newpiece.p3.y--;
		    newpiece.p4.y--;
		    *piece = newpiece;
		    return false;
		}
	}

	//once done, save the piece and return true

	*piece = newpiece;
	return true;
	
}

void addPieceToGrid(int glength, int gheight, char grid[glength][gheight], Piece piece) {
	int x, y;

	x = piece.p1.x;
	y = piece.p1.y;
	grid[x][y] = piece.type;

	x = piece.p2.x;
	y = piece.p2.y;
	grid[x][y] = piece.type;

	x = piece.p3.x;
	y = piece.p3.y;
	grid[x][y] = piece.type;

	x = piece.p4.x;
	y = piece.p4.y;
	grid[x][y] = piece.type;

}

int scoreRows(int glength, int gheight, char grid[glength][gheight]) {
	//we check from the top what rows are complete and what to move down.
	//we will use the original BulletProofSoftware scoring system (https://tetris.wiki/Tetris_(BPS))
	
	const int scoreInc = 10;
	int score = 0;

	int scores[] = {40, 100, 300, 1200};
	int scoreindex = -1;
	bool inARow = false;
	

	for (int y = 0; y < gheight; y++) {
		int tally = 0;
		for (int x = 0; x < glength; x++) {
			if (grid[x][y] >= 0) tally++;
		}
		
		if (tally == glength) { //if the row is complete
			inARow = true;
			if (scoreindex < 3) scoreindex += 1;
			//move everything down
			for (int i = y; i > 0; i--) {
				for (int x = 0; x < glength; x++) {
					grid[x][i] = grid[x][i - 1];
				}
			}
		}
		else {
			inARow = false;
			if (scoreindex >= 0) {
				score += scores[scoreindex];
				scoreindex = -1;
			}
		}
	}
	
	if (scoreindex >= 0) {
		score += scores[scoreindex];
		scoreindex = -1;
	}
	
	return score;
}

void display(int glength, int gheight, char grid[glength][gheight], Piece piece, Piece nextpiece, int score) {

	//first we edit nextpiece so that it is outside of the grid.
	//at most, the horizontal length is 3 and the vertical length is 4
	
	const int npyinc = 2;
	nextpiece.p1.y += npyinc;
	nextpiece.p2.y += npyinc;
	nextpiece.p3.y += npyinc;
	nextpiece.p4.y += npyinc;

	const int npxinc = (glength >> 1) + 2;
	nextpiece.p1.x += npxinc;
	nextpiece.p2.x += npxinc;
	nextpiece.p3.x += npxinc;
	nextpiece.p4.x += npxinc;
	
//	nextpiece.p1.x += (5 + 1);
//	nextpiece.p2.x += (5 + 1);
//	nextpiece.p3.x += (5 + 1);
//	nextpiece.p4.x += (5 + 1);

	const char* reset = "\033[2J\033[H\e[?25l";
	
	int totalsize = (((glength + 2) * (gheight + 1)) + sizeof(reset)) * 8;
	char* buffer = (char*) calloc(totalsize, 1);
	char* ptr = buffer;

	ptr += sprintf(ptr, reset);
	ptr += sprintf(ptr, "score: %i\n", score);
	
	int x = 0;
	for (int y = 0; y < gheight; y++) {
		ptr += sprintf(ptr, "|");
		for (x = 0; x < glength; x++) {

			if (grid[x][y] >= 0) ptr += sprintf(ptr, "%s ", colours[grid[x][y]]);
			else if ((piece.p1.x == x && piece.p1.y == y) || (piece.p2.x == x && piece.p2.y == y) || (piece.p3.x == x && piece.p3.y == y) || (piece.p4.x == x && piece.p4.y == y)) {
				//ptr += sprintf(ptr, "#");
				ptr += sprintf(ptr, "%s ", colours[piece.type]);
			}
			else ptr += sprintf(ptr, " ");
			ptr += sprintf(ptr, RESETTEXT);
		}


		ptr += sprintf(ptr, "| ");
		if (y == 0) {
			ptr += sprintf(ptr, "next:");
		}
		else if (1 < y && y < 6) { //the piece
			for (int i = x; i < (x + 4); i++) {
				if ((nextpiece.p1.x == i && nextpiece.p1.y == y) || (nextpiece.p2.x == i && nextpiece.p2.y == y) || (nextpiece.p3.x == i && nextpiece.p3.y == y) || (nextpiece.p4.x == i && nextpiece.p4.y == y)) {
					ptr += sprintf(ptr, "%s ", colours[nextpiece.type]);
				}
				else {
					ptr += sprintf(ptr, RESETTEXT);
					ptr += sprintf(ptr, " ");
				}
			}
		}
		ptr += sprintf(ptr, RESETTEXT);
		ptr += sprintf(ptr, "\n");
	}
	ptr += sprintf(ptr, " ");
	for (int i = 0; i < glength; i++) {
		ptr += sprintf(ptr, "-");
	}
	ptr += sprintf(ptr, "\n");

	write(1, buffer, ptr - buffer);
	free(buffer);
}

unsigned long hash(char* string) {
    unsigned long h = 5381;
    int c;

    while (c = *string++) {
        h = ((h << 5) + h) + c;
    }

    return h;
}

void printHelp() {
	printf("Hello, welcome to tetris :D\nTo play, just enter tetris (or ./tetris)\nControls are:\na -> move left\nd -> move right\nq -> rotate clockwise\ne -> rotate anticlockwise\nd -> drop piece down\n\nList of flags:\n-h/--help -> lists this message.\n-s/--seed -> enter your seed.\n-n/--new -> enter a username to save your score.\n-sh/--show -> show all your scores.\n-cl/--clear -> clears all your scores.\n-shb/--showbindings -> shows your current keyboard bindings.\n-sb/--setbindings -> configures your bindings to your liking.\n-rb/--resetbindings -> resets your bindings to the standard.\nHave fun!!!\n\n");
}

void showScore() {
	//we open the score file and print it out
	FILE* file = fopen(filename, "r");

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* buffer = (char*) calloc(size, sizeof(char));
	fread(buffer, 1, size, file);
	printf("\nHere are the following scores:\n%s\n", buffer);
	
	free(buffer);
	fclose(file);
}

void clearScore() {
	printf("Are you sure you want to do this? (y/n) ");
	char response = 0;
	scanf(" %c", &response);
	printf("\n");
	if (response == 'y') {
		printf("clearing scores...\n");
		FILE* file = fopen(filename, "w");
		fclose(file);
		printf("scores cleared.\n");
	}
}

void showBindings() {
	FILE* bindfile = fopen(bindfilename, "r");
	if (!bindfile) {
		bindfile = fopen(bindfilename, "w");
		fprintf(bindfile, "a:d:e:q:s");
		fclose(bindfile);
		printf("\n\nControls are:\na -> left\nd -> right\ne -> clockwise\nq -> anticlockwise\ns -> place down\n\n");
		return;
	}
	printf("\n\nControls are:\n");

	fseek(bindfile, 0, SEEK_END);
	size_t fsize = ftell(bindfile);
	fseek(bindfile, 0, SEEK_SET);

	char* buffer = (char*) calloc(fsize, sizeof(char));
	fread(buffer, sizeof(char), fsize, bindfile);
	int i = -2;
	printf("%c -> left\n", buffer[i += 2]);
	printf("%c -> right\n", buffer[i += 2]);
	printf("%c -> clockwise\n", buffer[i += 2]);
	printf("%c -> anticlockwise\n", buffer[i += 2]);
	printf("%c -> place down\n\n", buffer[i += 2]);
	
	free(buffer);
	fclose(bindfile);
}

void setBindings() {
	//with this, we open our bindings file and save bindings
	FILE* bindfile = fopen(bindfilename, "w");
	char left, right, clock, anticlock, down;
	printf("\n\n");
	printf("Enter the key for MOVING LEFT: ");
	scanf(" %c", &left);
	printf("\nEnter the key for MOVING RIGHT: ");
	scanf(" %c", &right);
	printf("\nEnter the key for ROTATING CLOCKWISE: ");
	scanf(" %c", &clock);
	printf("\nEnter the key for ROTATING ANTICLOCKWISE: ");
	scanf(" %c", &anticlock);
	printf("\nEnter the key for PLACING DOWN: ");
	scanf(" %c", &down);

	fprintf(bindfile, "%c:%c:%c:%c:%c", left, right, clock, anticlock, down);
	
	printf("\n\nFinished setting bindings");
	fclose(bindfile);
	showBindings();
}

void resetBindings() {
	FILE* bindfile = fopen(bindfilename, "w");
	fprintf(bindfile, "a:d:e:q:s");
	fclose(bindfile);
	printf("\n\nReset bindings");
	showBindings();
}



int main(int argc, char** argv) {
	srand(time(NULL));
	bool saving = false;
	char* user = NULL;
	const int maxUserLength = 10;

	//set our bindings first.
	
	char bindings[5] = {0};
	FILE* bindfile = fopen(bindfilename, "r");
	if (!bindfile) {
		bindings[0] = 'a';
		bindings[1] = 'd';
		bindings[2] = 'e';
		bindings[3] = 'q';
		bindings[4] = 's';
	}
	else {	
		fseek(bindfile, 0, SEEK_END);
		size_t fsize = ftell(bindfile);
		fseek(bindfile, 0, SEEK_SET);

		char* buffer = (char*) calloc(fsize, sizeof(char));
		fread(buffer, sizeof(char), fsize, bindfile);
		int i = -2;
		bindings[0] = buffer[i += 2];
		bindings[1] = buffer[i += 2];
		bindings[2] = buffer[i += 2];
		bindings[3] = buffer[i += 2];
		bindings[4] = buffer[i += 2];
	}


	if (argc <= 1);
	else {
		//we add now flags
		//our flags will be:
		//	-s (--seed): seed	
		//	-n (--new): username (for saving score to a save file)
		//	-h (--help): help (doesnt play tetris at all just shows controls and stuff
		//	-sh (--show): shows scores
		//	-cl (--clear): deletes all scores
		bool isFlag = false;
		char* flag;
		for (int i = 1; i < argc; i++) {
			char* cur = argv[i];
			if (!strcmp(cur, "-h") || !strcmp(cur, "--help")) {
				//print out the help;
				printHelp();
				return 0;
			}
			else if (!strcmp(cur, "-sh") || !strcmp(cur, "--show")) {	
				showScore();
				return 0;
			}
			else if (!strcmp(cur, "-cl") || !strcmp(cur, "--clear")) {
				clearScore();
				return 0;
			}
			else if (!strcmp(cur, "-shb") || !strcmp(cur, "--showbindings")) {
				showBindings();
				return 0;
			}
			else if (!strcmp(cur, "-sb") || !strcmp(cur, "--setbindings")) {
				setBindings();
				return 0;
			}
			else if (!strcmp(cur, "-rb") || !strcmp(cur, "--resetbindings")) {
				resetBindings();
				return 0;
			}

			//the rest of these do require arguments
			if ((!strcmp(cur, "-s") || !strcmp(cur, "--seed") || !strcmp(cur, "-n") || !strcmp(cur, "--new")) && isFlag == false) {
				isFlag = true;
				flag = cur;
			}
			else {
				if (isFlag == true) {
					//now we compare the flag
//					if (!strcmp(flag, "-h")) {
//						//print out the help;
//						printHelp();
//						return 0;
//					}
					if (!strcmp(flag, "-s") || !strcmp(flag, "--seed")) {
						//convert the string into an int and use it as the seed
						const unsigned long seed = hash(cur);
						srand(seed);
					}
					else if (!strcmp(flag, "-n") || !strcmp(flag, "--new")) {
						saving = true;
						user = cur;
						if (strlen(user) > maxUserLength) {
							printf("\n\nerror, username cannot be greater than %i characters long\n\n", maxUserLength);
							return 0;
						}
						for (int i = 0; i < strlen(user); i++) {
							if (user[i] == ':') { printf("\n\nerror, username cannot have a ':' character\n\n"); return 0; }
						}
					}

				}
				isFlag = false;
			}
		}
	}

	enable_raw_mode(); //for kbhit
	const char* reset = "\033[2J\033[H\e[?25l";
	
	int bagindex = 0;
	//int nextbagindex = (bagindex + 1) % 7;
	Piecetype bag[] = {
		T,
		STRAIGHT,
		CUBE,
		S,
		Z,
		L,
		BL
	};
	shuffleBag(bag);
	printf("\n");
	for (int i = 0; i < 7; i++) {
		printf("%i, ", bag[i]);
	}
	printf("\n");
	//return 0;


	int
	gheight = 20,
	glength = 10;
	
	//we keep track of the current grid, which holds the pieces that have fallen already
	char grid[glength][gheight];
	memset(grid, -1, sizeof grid);

	//char** grid = (char**) calloc(glength, sizeof(char*));
	//for (int i = 0; i < 
		
	//Piece piece = generatePiece(getRandomPiecetype());
	//used to show what piece is next
	//Piece nextpiece = generatePiece(getRandomPiecetype());
	
	Piece piece = generatePiece(bag[bagindex]);
	//used to show what piece is next
	Piece nextpiece = generatePiece(bag[bagindex + 1]);
	bagindex++;


	int score = 0;
	
	const char shiftMod = 3;
	char shiftCount = 0;
	
	unsigned frames = 200000 / shiftMod;

	//open up our bindings folder to get our bindings.
	//in the format left:right:clock:anticlock:down
	


	bool going = true;
	while (going) {
		if (!loop(glength, gheight, grid, &piece, shiftCount, bindings)) { //piece was placed
			addPieceToGrid(glength, gheight, grid, piece);
			//now we want to check the grid for any completed rows.
			score += scoreRows(glength, gheight, grid);
			
			bagindex = (bagindex + 1) % 7;
			//nextbagindex = (bagindex + 1) % 7;
			if (bagindex == 0) { //we've reached the end
				shuffleBag(bag);
				//bagindex = 0;
			} 
			piece = nextpiece;
			nextpiece = generatePiece(bag[bagindex]);
			//in order to check if we have lost, we need to check if this newpiece at the beginning is valid
			if (!validPiecePosition(glength, gheight, grid, &piece)) {
				going = false;
			}
			continue;
		}
		display(glength, gheight, grid, piece, nextpiece, score);

		//printf("\n");
		//for (int i = 0; i < 7; i++) {
		//	printf("%i, ", bag[i]);
		//}
		//printf("\n");
		
		usleep(frames);
		shiftCount++;
		shiftCount %= shiftMod;
	}
	printf("\n\nYou lost! your score was %i\n", score);

	if (saving) {
		printf("Saving this score under %s\n\n", user);

		//format of save file is user:score\n
		//to make this work, ensure that the entered user has no : characters in it
		FILE* file = fopen(filename, "r");
		if (!file) { 
			printf("error, cannot open 'tetris_scores.txt'. Creating it...\n");
			file = fopen(filename, "w");
			printf("'tetris_scores.txt' created.\n");
			fclose(file);
			fopen(filename, "r");
		}
		
		fseek(file, 0, SEEK_END);
		size_t fsize = ftell(file);
		fseek(file, 0, SEEK_SET);

		if (fsize == 0) { //the file is empty
			fclose(file);
			file = fopen(filename, "w");
			fprintf(file, "%s:%i\n", user, score);
			printf("score saved.\n");
			return 0;
		}
		

		//two buffers, one for reading the file and one for writing
		char* scorebuffer = (char*) calloc(fsize, sizeof(char));
		fread(scorebuffer, sizeof(char), fsize, file);

		char* scorewriter = (char*) calloc(fsize + 200, sizeof(char));
		char* scoreptr = scorewriter;

		//we analyse this file, and add/edit entries
		//also impose a max username length
		char* curUser = (char*) calloc(maxUserLength + 1, sizeof(char));
		//for loop to go through stuff
		
		bool userFound = false;

		int j = 0;
		for (size_t i = 0; i < fsize; i++, j++) {
			scoreptr += sprintf(scoreptr, "%c", scorebuffer[i]);

			if (scorebuffer[i] == ':') {
				//we finish reading and compare
				if (!strcmp(curUser, user)) { //do they match?
					//if so, we have to edit the score.
					//first we have to get the current score

					int k = i + 1, currentscore = 0;
					while (scorebuffer[k] != '\n') {
						currentscore += (int) (scorebuffer[k] - 48);
						currentscore *= 10;
						k++;
					}
					currentscore /= 10;
					//printf("currentscore is %i\n", currentscore);
					if (score > currentscore) scoreptr += sprintf(scoreptr, "%i\n", score);
					else scoreptr += sprintf(scoreptr, "%i\n", currentscore);
					userFound = true;
					
					while (scorebuffer[i] != '\n') i++;
				}
				else {
					//printf("user not found\n");
					i++;
					while (scorebuffer[i] != '\n') {
						scoreptr += sprintf(scoreptr, "%c", scorebuffer[i]);
						i++;
					}
					scoreptr += sprintf(scoreptr, "%c", scorebuffer[i]);
				}
				memset(curUser, 0, maxUserLength + 1);
				j = -1;
				continue;
			}
			
			curUser[j] = scorebuffer[i];
		}

		if (!userFound) {
			scoreptr += sprintf(scoreptr, "%s:%i\n", user, score);
		}
		
		fclose(file);
		file = fopen(filename, "w");
		fwrite(scorewriter, sizeof(char), strlen(scorewriter), file);
		fclose(file);



	}

	return 0;

}
