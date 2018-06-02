
#include <stdio.h>  
#include "UARTIO.h"
#include <math.h>

#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "img.h"

int const cellW = 8;
int const cellH = 5;
int const hLineW = 1;
int const vLineW = 2;
int const numOfCol = 7;
int const numOfRow = 6;
int const numOfCoinsForEachPlayer = 21; // (numOfCol*numOfRow)/2
int const coinPadding;
int const coinH = 3;
int const coinW = 6;

int leftMargin;
int topMargin;
int fullGridH;
int fullGridW;
int turn;
int cellCenX;
int cellCenY;
int cellCoins[numOfCol];

int i;
int	j;

//this structure describes each individual cell
//(x, y) are the center point of a cell
//player = 0 >> cell is empty
//player = 1 >> cell is taken by player one
//player = 2 >> cell is taken by player two
struct cell{
	int x;
	int y;
	int player;
};


struct cell theGrid[numOfRow][numOfCol];



struct coin {
	int x;
	int y;
	const unsigned char *image;
	
	void (*draw)(struct coin*);
};

void draw(struct coin* c){
	Nokia5110_PrintBMP((*c).x - coinW/2, (*c).y + ceil((double)coinH/2), (*c).image, 0);
}


struct coin playerOneCoins[numOfCoinsForEachPlayer];
struct coin playerTwoCoins[numOfCoinsForEachPlayer];




void gameInit(){
	
	//the grid dimintions
	fullGridW = (cellW * numOfCol + vLineW*(numOfCol+1));
	fullGridH = (cellH*numOfRow +  vLineW*numOfRow);
	
	
	//get margins
	leftMargin = (SCREENW - fullGridW)/2;
	topMargin = SCREENH - fullGridH;
	
	//individual cell center
	cellCenX = vLineW + ceil((double)cellW/2);
	cellCenY = hLineW + ceil((double)cellH/2);
	
	
	//initialize the grid
	for(i = 0; i < numOfRow; i++){
		for(j = 0; j < numOfCol; j++){
			//calculate center position of each cell
			theGrid[numOfRow - 1 - i][j].x = leftMargin + j*(cellW + vLineW) + cellCenX;
			theGrid[numOfRow - 1 - i][j].y = SCREENH - 1 - (i*(cellH + hLineW) + cellCenY);
			
			//player = 0 mean the cell is empty
			theGrid[numOfRow - 1 - i][j].player = 0;
		}
	}
	
	//initialize the turn
	turn = 0;
	
	
	//initialize players coins
	
	for(i = 0; i < numOfCoinsForEachPlayer; i++){
		
		//locate the coins on the top left of the grid and in the center of the 1st column
		//player one
		playerOneCoins[i].x = leftMargin + cellCenX;
		playerOneCoins[i].y = SCREENH - 1 - fullGridH;
		//player two
		playerTwoCoins[i].x = leftMargin + cellCenX;
		playerTwoCoins[i].y = SCREENH - 1 - fullGridH;
		
		
		//set the img for each player
		//player one
		playerOneCoins[i].image = pl1coin;
		//player two
		playerTwoCoins[i].image = pl2coin;
		
	}
	
	//each column contains 0 coins at the begining
	for(i = 0; i < numOfCol; i++){
		cellCoins[i] = 0;
	}
	
}






void DrawGrid(){
	
	//draw vertical lines
	int yPos = SCREENH - 1;
	int xPos = 0;
	int i = 0;
	while( i <= numOfCol){
		xPos = i*(cellW + vLineW);
		Nokia5110_PrintBMP(xPos + leftMargin,  yPos, vvLine, 0);
		i++;
	}
	
	
	//draw vertical lines
	yPos = 0;
	xPos = leftMargin;
	i = 0;
	while( i < numOfRow){
		yPos = SCREENH - ( 1 +  i*(cellH + hLineW));
		Nokia5110_PrintBMP(xPos, yPos, hLine, 0);
		
		i++;
	}
	
}




int main(void){
	//UART_Init();
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
  Random_Init(1);
  Nokia5110_Init();
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();      // draw buffer
	
	gameInit();
	
	
	DrawGrid();
	Nokia5110_DisplayBuffer(); 
	//Nokia5110_PrintBMP(playerOneCoins[0].x, playerOneCoins[0].y, playerOneCoins[0].image, 0);
	
	//raw(&playerTwoCoins[0]);
	
	for(i = 0; i < numOfRow; i++){
		for(j = 0; j < numOfCol; j++){
			if(turn%2 == 0){
				playerOneCoins[turn/2].x = theGrid[numOfRow - 1 - i][j].x;
				playerOneCoins[turn/2].y = theGrid[numOfRow - 1 - i][j].y;
				draw(&playerOneCoins[turn/2]);
			}else{
				playerTwoCoins[turn/2].x = theGrid[numOfRow - 1 - i][j].x;
				playerTwoCoins[turn/2].y = theGrid[numOfRow - 1 - i][j].y;
				draw(&playerTwoCoins[turn/2]);
			}
			Nokia5110_DisplayBuffer();
			turn++;
		}
		
	}
	
	Nokia5110_DisplayBuffer(); 
  while(1){
  }

}


void Delay100ms(unsigned long count){unsigned long volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}
