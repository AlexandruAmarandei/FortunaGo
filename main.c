/* COMP2215: Task 02---MODEL ANSWER */
/* For La Fortuna board.            */


#include <avr/io.h>
#include "lcd.h"
#include <stdbool.h>
#include "keypad.h"
#include <avr/interrupt.h>
#include <util/delay.h>

#define BUFFSIZE 256
#define BOARDSIZE 10


bool visited[BOARDSIZE][BOARDSIZE];
int16_t boardpieces[BOARDSIZE][BOARDSIZE];
const int16_t permx[4] = {0,1,0,-1};
const int16_t permy[4] = {1,0,-1,0};
bool player;
int16_t cursorx, cursory;
int16_t score1, score2, stones1, stones2, taken1, taken2;
void init(void);
bool surrounded(int16_t x, int16_t y);
bool canPutOn(int16_t x, int16_t y);
bool bound(int16_t x, int16_t y);
void place(int16_t x, int16_t y);
bool check(int16_t x, int16_t y);
int16_t remove(int16_t x, int16_t y);
bool checkNoRemove(int16_t x, int16_t y);
void checkAndRemove(int16_t x, int16_t y);
void changeSelected(int16_t x, int16_t y);
void moveCursor(int16_t x, int16_t y);
void resetGame();
void redraw();
bool check1(int16_t x, int16_t y);
bool check2(int16_t x, int16_t y);
void finalizeGame();
void recalculateScore();
void redraw();
void fillFree();
void resetVisited();


void main(void) {
    init();

    int16_t or = 0;
    int16_t skippedMoves = 0;
    bool canPress = true;
    int16_t oor = 0;
    resetGame();
	redraw();
	game_start_screen();
    while(true){
    	
	    int16_t x = getPressed();

	    if(x == 1){
	    	redraw();
	    	canPress = false;
		    while(1 == 1){
		    	int16_t r = getPressed();
		    	_delay_ms(5);
			    if(canPress){
			    	canPress = false;

					if(oor == 1 && or == 1 && r == 1){
						oor = 0;
						or = 0;
						r = 0;
						player = !player;
						skippedMoves+=1;
						if(skippedMoves == 2){
								finalizeGame();
								r = getPressed();
								while(r == 1){
									r = getPressed();
								}
								r = getPressed();
								while(r !=1){
									r = getPressed();
								}
								canPress = false;
							    resetGame();
		    					redraw();
						}

					}else{

					if(r != 6){
						oor = or;
						or = r;
					}

					if(r == 1 ){
						if(canPutOn(cursorx, cursory)){
							skippedMoves = 0;
							oor = 0;
							or = 0;
							place(cursorx, cursory);
							place_on_screen(cursorx, cursory, GREEN, 7);
							recalculateScore();
						}
					}else
					if(r == 2){
						moveCursor(100,0);
					}else
					if(r == 3){
						moveCursor(0,1);
					}else
					if(r == 4){
						moveCursor(1,0);
					}else
					if(r == 5){
						moveCursor(0,100);
					}
					}
				}
				if(r == 6){
					canPress = true;
				}
		    }
		}
	}
}

void finalizeGame(){
	if(score1 > score2){
		display_winning_message(1);
	}
	if(score1 <= score2){
		display_winning_message(2);
	}
}

void redraw(){
	clear_screen();
	display_grid();
	display_Players();
	update_Taken(taken1, 1);
    update_Taken(taken2, 2);
    update_Stones(stones1,1 );
    update_Stones(stones1,2 );
    update_Score(score2, 2);
    update_Score(score1, 1);

	int16_t i,j;
    for(i=0; i<BOARDSIZE;i++){
    	for(j=0; j<BOARDSIZE;j++){
			if(	boardpieces[i][j] == 1){
				place_on_screen(i,j,RED,10);
			}
			if(	boardpieces[i][j] == 2){
				place_on_screen(i,j,BLUE,10);
			}
    	}
    }
    place_on_screen(cursorx, cursory, GREEN, 7);
}

void recalculateScore(){
	resetVisited();
	int16_t p1o, p2o, i, j;
	p1o = taken1;
	p2o = taken2 + 7;
	for(i = 0; i < BOARDSIZE; i++){
		for(j = 0; j < BOARDSIZE; j++){
			if(boardpieces[i][j] == 1){
				check1(i,j);
			}
		}
	}
	for(i = 0; i < BOARDSIZE; i++){
		for(j = 0; j < BOARDSIZE; j++){
			if(boardpieces[i][j] == 0 && visited[i][j] == false){
				p2o++;
			}
		}
	}

	resetVisited();

	for(i = 0; i < BOARDSIZE; i++){
		for(j = 0; j < BOARDSIZE; j++){
			if(boardpieces[i][j] == 2){
				check2(i,j);
			}
		}
	}
	for(i = 0; i < BOARDSIZE; i++){
		for(j = 0; j < BOARDSIZE; j++){
			if(boardpieces[i][j] == 0 && visited[i][j] == false){
				p1o++;
			}
		}
	}
	score1 = p1o;
	score2 = p2o;
	update_Score(score1, 1);
	update_Score(score2, 2);

	resetVisited();

}

 

void fillFree(int16_t x, int16_t y){
	int16_t qx[101], qy[101];
	int16_t i, xx, yy, st, fn;
	for(i =0; i<101;i++){
		qx[i] = -1;
		qy[i] = -1;
	}
	st = 0;
	fn = 0;
	qx[fn] = x;
	qy[fn] = y;
	fn++;
	visited[x][y] = true;
	while(st<fn){
		for(i = 0; i < 4; i++){
			xx = qx[st] + permx[i];
			yy = qy[st] + permy[i];
			if(bound(xx,yy) && boardpieces[xx][yy] == 0 && visited[xx][yy] == false){
				visited[xx][yy] = true;
				qx[fn] = xx;
				qy[fn] = yy;
				fn++;
			}
		}
		st++;
	}
}

bool check1(int16_t x, int16_t y){
	visited[x][y] = true;
	int16_t xx, yy, i;
	for(i=0; i<4;i++){
		xx = x + permx[i];
		yy = y + permy[i];
		if(bound(xx,yy)){
			if(boardpieces[xx][yy] == 0  && visited[xx][yy] == false){
				fillFree(xx,yy);
			}
		}
	}
	return true;
}

bool check2(int16_t x, int16_t y){
	visited[x][y] = true;
	int16_t xx, yy, i;
	for(i=0; i<4;i++){
		xx = x + permx[i];
		yy = y + permy[i];
		if(bound(xx,yy)){
			if(boardpieces[xx][yy] == 0  && visited[xx][yy] == false){
				fillFree(xx,yy);
			}
		}
	}
	return true;
}



void resetGame(){
	int16_t i,j;
    for(i=0; i<BOARDSIZE;i++){
    	for(j=0; j<BOARDSIZE;j++){
    	visited[i][j] = false;
    	boardpieces[i][j] = 0;
    	}
    }
    cursorx = 0;
    cursory = 0;
    stones1 = stones2 = score1 = taken1 = taken2 = 0;
    score2 = 7;
    player = true;
}

bool surrounded(int16_t x, int16_t y){
	int16_t xx, yy, i;
	for(i = 0; i<4;i++){
		xx = x + permx[i];
		yy = y + permy[i];
		if(bound(xx,yy)){
			if(boardpieces[xx][yy] == 0){
				return false;
			}
		}
	}
	return true;
}

bool canPutOn(int16_t x, int16_t y){
	if(!bound(x,y)){
		display_string("Error, invalid x and y");

	}
	if(boardpieces[x][y]!= 0){
		return false;
	}
	if(surrounded(x,y)){
		if(player == true){
			boardpieces[x][y] = 1;
		}
		if(player == false){
			boardpieces[x][y] = 2;
		}
		bool returnValue;
		returnValue = checkNoRemove(x,y);
		if(returnValue == true){
			boardpieces[x][y] = 0;
		 	return true;
		}
		resetVisited();
		returnValue = check(x,y);
		resetVisited();
		if(returnValue == false){
			boardpieces[x][y] = 0;
		 	return false;
		}
		
		boardpieces[x][y] = 0;
		}
	return true;
}

void resetVisited(){
	int i, j;
	for( i=0; i<BOARDSIZE;i++){
    	for( j=0; j<BOARDSIZE;j++){
    	visited[i][j] = false;
    	}
    }
}

bool bound(int16_t x, int16_t y){
	if(x < 0 || x >= BOARDSIZE || y < 0 || y >= BOARDSIZE){
		return false;
	}
	return true;
}

void place(int16_t x, int16_t y){
	if(player == true){
		boardpieces[x][y] = 1;
		stones1++;
		update_Stones(stones1, 1);
	}
	else{
		boardpieces[x][y] = 2;
		stones2++;
		update_Stones(stones2, 2);
	}

	checkAndRemove(x,y);
	int16_t col;
	if(player == true){
		col = RED;
	}
	else{
		col = BLUE;
	}
	place_on_screen(x,y,col,10);

	player = !player;
}

bool check(int16_t x, int16_t y){
	visited[x][y] = true;
	if(boardpieces[x][y]== 0 ){
		return true;
	}
	int16_t xx ;
	int16_t yy , i;
	bool returnValue = false;
	for(i=0; i<4;i++){
		xx = x + permx[i];
		yy = y + permy[i];
		if(bound(xx, yy)){
			if(visited[xx][yy] == false){
				if( boardpieces[xx][yy]== boardpieces[x][y] || boardpieces[xx][yy] == 0 ){
					if(check(xx,yy) == true){
						return true;
					}
				}
			}
		}
	}
	return returnValue;
}

int16_t remove(int16_t x, int16_t y){  
	int16_t xx ;
	int16_t yy, i;
	int16_t aux = boardpieces[x][y];
	int16_t sum = 1;
	boardpieces[x][y] = 0;
	for( i=0; i<4;i++){
		xx = x + permx[i];
		yy = y + permy[i];
		if(bound(xx, yy)){
				if(boardpieces[xx][yy]==aux ){
					sum += remove(xx,yy);
				}
		}
	}
	delete_from_display(x,y);
	return sum;
}


bool checkNoRemove(int16_t x, int16_t y){
	resetVisited();
	int16_t xx ;
	int16_t yy ;
	int16_t opposite = 3;
	if(boardpieces[x][y] == 1){
		opposite = 2;
	}
	if(boardpieces[x][y] == 2){
		opposite = 1;
	}
	int16_t i;
	for(i=0; i<4;i++){
		xx = x + permx[i];
		yy = y + permy[i];
		if(bound(xx, yy) && visited[xx][yy] == false) {
			if(boardpieces[xx][yy] == opposite){
				if(check(xx, yy) == false){
					return true;
				}
			}
		}
	}

	resetVisited();
    return false;
}

void checkAndRemove(int16_t x, int16_t y){
	resetVisited();
	int16_t xx ;
	int16_t yy ;
	int16_t opposite = 3;
	int16_t removedPieces = 0;
	if(boardpieces[x][y] == 1){
		opposite = 2;
	}
	if(boardpieces[x][y] == 2){
		opposite = 1;
	}
	int16_t i;
	for(i=0; i<4;i++){
		xx = x + permx[i];
		yy = y + permy[i];
		if(bound(xx, yy) && visited[xx][yy] == false) {
			if(boardpieces[xx][yy] == opposite){
				if(check(xx, yy) == false){
					removedPieces = remove(xx,yy);
					if(removedPieces != 0){
						if(player == true){
							score1 += removedPieces;
							taken1 += removedPieces;
							stones2 -= removedPieces;
							update_Taken(taken1, 1);
							update_Score(score1, 1);
							update_Stones(stones2, 2);
						}else{
							score2 += removedPieces;
							taken2 += removedPieces;
							stones1 -= removedPieces;
							update_Stones(stones1, 1);
							update_Taken(taken2, 2);
							update_Score(score2, 2);
						}
					}
				}
			}
		}
	}
	resetVisited();
}
void init(void) {
    /* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    init_lcd();
    init_keypad();
}



void changeSelected(int16_t x, int16_t y){
	if(boardpieces[cursorx][cursory] == 1){
		place_on_screen(cursorx, cursory, RED, 10);
	}
	if(boardpieces[cursorx][cursory] == 2){
		place_on_screen(cursorx, cursory, BLUE, 10);
	}
	if(boardpieces[cursorx][cursory] == 0){
		delete_from_display(cursorx, cursory);
	}
    place_on_screen(x,y,GREEN,7);
    cursorx = x;
    cursory = y;
}



void moveCursor(int16_t x, int16_t y){
	int16_t xx,yy;
	xx = cursorx;
	yy = cursory;
	if(x == 100){
		xx = cursorx - 1;
	}
	if(x == 1){
		xx = cursorx + 1;
	}
	if(x == 0){
		xx = cursorx;
	}
	if(y == 100){
		yy = cursory - 1;
	}
	if(y == 1){
		yy = cursory + 1;
	}
	if(y == 0){
		yy = cursory;
	}
	if(!bound(xx, yy)){
		return;
	}
	changeSelected(xx,yy);

}