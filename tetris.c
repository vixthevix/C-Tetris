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

const char colours[7][20] = {
	REDBG,
	GREENBG,
	YELLOWBG,
	BLUEBG,
	PURPLEBG,
	CYANBG,
	WHITEBG
};

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

Piecetype getRandomPiecetype() {
	const int count = 7;
	const int choice = rand() % count;
	
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

bool loop(int glength, int gheight, char grid[glength][gheight], Piece* piece) {
	//order of what happens
	//first move down
	//second check for collisions with floor / other pieces. if true return immediately.
	//then check io for rotating and moving. make sure the result doesnt go inside other pieces.
	
	Piece newpiece = *piece;
	//move down
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

	//this means that the piece will continue on moving down, so we allow IO here.
	//for each io, we check if we can make that move. if we cant, we revert changes.
	if (kbhit()) {
		char key = getch();
		switch (key) {
			case 'a': { //move left
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
				break;
			}
			case 'd': { //move right
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
				break;
			}
			case 'q': { //rotatte anticlockwise
				rotatePiece(&newpiece, false);
				if (!validPiecePosition(glength, gheight, grid, &newpiece)) rotatePiece(&newpiece, true);
				break;
			}
			case 'e': { //rotate clockwise
				rotatePiece(&newpiece, true);
				if (!validPiecePosition(glength, gheight, grid, &newpiece)) rotatePiece(&newpiece, false);
				break;
			}
			case 's': { //move straight down.
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

				break;
			}
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
	
	const int scoreInc = 10;
	int score = 0;
	

	for (int y = 0; y < gheight; y++) {
		int tally = 0;
		for (int x = 0; x < glength; x++) {
			if (grid[x][y] >= 0) tally++;
		}
		
		if (tally == glength) { //if the row is complete
			score += scoreInc;
			//move everything down
			for (int i = y; i > 0; i--) {
				for (int x = 0; x < glength; x++) {
					grid[x][i] = grid[x][i - 1];
				}
			}
		}	
	}

	return score;
}

void display(int glength, int gheight, char grid[glength][gheight], Piece piece, int score) {
	const char* reset = "\033[2J\033[H\e[?25l";
	
	int totalsize = (((glength + 2) * (gheight + 1)) + sizeof(reset)) * 8;
	char* buffer = (char*) calloc(totalsize, 1);
	char* ptr = buffer;

	ptr += sprintf(ptr, reset);
	ptr += sprintf(ptr, "score: %i\n", score);
	
	for (int y = 0; y < gheight; y++) {
		ptr += sprintf(ptr, "|");
		for (int x = 0; x < glength; x++) {
			if (grid[x][y] >= 0) ptr += sprintf(ptr, "%s ", colours[grid[x][y]]);
			else if ((piece.p1.x == x && piece.p1.y == y) || (piece.p2.x == x && piece.p2.y == y) || (piece.p3.x == x && piece.p3.y == y) || (piece.p4.x == x && piece.p4.y == y)) {
				//ptr += sprintf(ptr, "#");
				ptr += sprintf(ptr, "%s ", colours[piece.type]);
			}
			else ptr += sprintf(ptr, " ");
			ptr += sprintf(ptr, RESETTEXT);
		}
		ptr += sprintf(ptr, "|\n");
	}
	ptr += sprintf(ptr, " ");
	for (int i = 0; i < glength; i++) {
		ptr += sprintf(ptr, "-");
	}
	ptr += sprintf(ptr, "\n");

	write(1, buffer, ptr - buffer);
	free(buffer);
}



int main(int argc, char** argv) {
	enable_raw_mode(); //for kbhit
	if (argc <= 1) srand(time(NULL));
	else {
		//convert the string into an int and use it as the seed
		const int seed = atoi(argv[1]);
		srand(seed);
	}
		

	const char* reset = "\033[2J\033[H\e[?25l";

	int
	gheight = 20,
	glength = 10;
	
	//we keep track of the current grid, which holds the pieces that have fallen already
	char grid[glength][gheight];
	memset(grid, -1, sizeof grid);

	//char** grid = (char**) calloc(glength, sizeof(char*));
	//for (int i = 0; i < 
		
	Piece testpiece = generatePiece(getRandomPiecetype());

	unsigned frames = 150000;

	int score = 0;

	bool going = true;
	while (going) {
		if (!loop(glength, gheight, grid, &testpiece)) { //piece was placed
			addPieceToGrid(glength, gheight, grid, testpiece);
			//now we want to check the grid for any completed rows.
			score += scoreRows(glength, gheight, grid);
			testpiece = generatePiece(getRandomPiecetype());
			//in order to check if we have lost, we need to check if this newpiece at the beginning is valid
			if (!validPiecePosition(glength, gheight, grid, &testpiece)) {
				going = false;
			}
		}
		display(glength, gheight, grid, testpiece, score);
		usleep(frames);
	}
	printf("\n\nYou lost! your score was %i\n", score);

	return 0;

}
