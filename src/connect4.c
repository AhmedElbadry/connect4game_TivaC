
#include <stdio.h>  
#include "UARTIO.h"
#include <math.h>

#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "img.h"

//port f
void PortF_Init(void);




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
int lastTurn;
int cellCenX;
int cellCenY;
int cellCoins[numOfCol];
int colCenter[numOfCol];
int playerPos;
int winner;

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
struct coin playersCoins[2][numOfCoinsForEachPlayer];




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
	
	playerPos = 0;
	
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
	lastTurn = 0;
	
	
	//initialize players coins
	
	for(i = 0; i < numOfCoinsForEachPlayer; i++){
		
		//locate the coins on the top left of the grid and in the center of the 1st column
		//player one
		//playerOneCoins[i].x = leftMargin + cellCenX;
		//playerOneCoins[i].y = SCREENH - 1 - fullGridH;
		playersCoins[0][i].x = leftMargin + cellCenX;
		playersCoins[0][i].y = SCREENH - 1 - fullGridH;
		
		//player two
		//playerTwoCoins[i].x = leftMargin + cellCenX;
		//playerTwoCoins[i].y = SCREENH - 1 - fullGridH;
		playersCoins[1][i].x = leftMargin + cellCenX;
		playersCoins[1][i].y = SCREENH - 1 - fullGridH;
		
		
		//set the img for each player
		//player one
		//playerOneCoins[i].image = pl1coin;
		//player two
		//playerTwoCoins[i].image = pl2coin;
		playersCoins[0][i].image = pl1coin;
		playersCoins[1][i].image = pl2coin;
		
		
	}
	
	//each column contains 0 coins at the begining
	for(i = 0; i < numOfCol; i++){
		cellCoins[i] = 0;
		
		colCenter[i] = leftMargin + i*(cellW + vLineW) + cellCenX;
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

void update(){
	Nokia5110_ClearBuffer();
	
	
	//show grid
	DrawGrid();
	
	//show current player
	draw(&playersCoins[turn%2][turn/2]);
	
	
	//show the coins inside the grid
	
	for(i = 0; i < turn; i++){
		draw(&playersCoins[i%2][i/2]);
	}
	
	
	Nokia5110_DisplayBuffer();
	

}




//check if there is a winner
//return 1 if player one is the winner
//return 2 if player two is the winner
int isThereAwinner(){
	int status = 0;
	for(i = 0; i < numOfRow; i++){
		for(j = 0; j < numOfCol; j++){
			//check vertically
			if(i + 3 < numOfRow){
				if(
					theGrid[i][j].player == theGrid[i+1][j].player &&
					theGrid[i+1][j].player == theGrid[i+2][j].player &&
					theGrid[i+2][j].player == theGrid[i+3][j].player &&
					theGrid[i][j].player != 0
					){
						status = theGrid[i][j].player;
						break;
					}
			}
			
			//horizontally
			if(j + 3 < numOfCol){
				if(
					theGrid[i][j].player == theGrid[i][j+1].player &&
					theGrid[i][j+1].player == theGrid[i][j+2].player &&
					theGrid[i][j+2].player == theGrid[i][j+3].player &&
					theGrid[i][j].player != 0
					){
						status = theGrid[i][j].player;
						break;
					}
			}
			
			//diagonally right
			if(i + 3 < numOfRow && j + 3 < numOfCol){
				if(
					theGrid[i][j].player == theGrid[i+1][j+1].player &&
					theGrid[i+1][j+1].player == theGrid[i+2][j+2].player &&
					theGrid[i+2][j+2].player == theGrid[i+3][j+3].player &&
					theGrid[i][j].player != 0
					){
						status = theGrid[i][j].player;
						break;
					}
			}
			
			//diagonally left
			if(i + 3 < numOfRow && j - 3 >= 0){
				if(
					theGrid[i][j].player == theGrid[i+1][j-1].player &&
					theGrid[i+1][j-1].player == theGrid[i+2][j-2].player &&
					theGrid[i+2][j-2].player == theGrid[i+3][j-3].player &&
					theGrid[i][j].player != 0
					){
						status = theGrid[i][j].player;
						break;
					}
			}
		}
		
		if(status != 0)
			break;
	}
	
	
	return status ;
}

unsigned int SW1, flag1;
unsigned int SW2, flag2;

int main(void){
	//UART_Init();
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
  Random_Init(1);
  Nokia5110_Init();
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();      // draw buffer
	PortF_Init();
	
	gameInit();
	
	
	DrawGrid();
	Nokia5110_DisplayBuffer(); 
	//Nokia5110_PrintBMP(playerOneCoins[0].x, playerOneCoins[0].y, playerOneCoins[0].image, 0);
	
	//raw(&playerTwoCoins[0]);
	/*
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
		
	}*/
	
	
	flag1 = 0;
	flag2 = 0;
	
  while(1){
		Nokia5110_ClearBuffer();
		SW1 = GPIO_PORTF_DATA_R&0x10;
		SW2 = GPIO_PORTF_DATA_R&0x01;
		
		if(turn > lastTurn){
			playerPos = 0;
			lastTurn = turn;
		}
		update();
		winner = isThereAwinner();
		if(winner){
			Nokia5110_ClearBuffer();
			Nokia5110_DisplayBuffer();
			Nokia5110_Clear();
			Nokia5110_SetCursor(1, 1);
			
			if(winner == 1 )
				Nokia5110_OutString("Player one wins");
			else
				Nokia5110_OutString("Player two wins");
			
			break;
		}
		//if it's player one turn
		//if(turn % 2 == 0){
			
			//while(1){

				
				//draw player coin
				//draw(&playersCoins[turn%2][turn/2]);
				//draw grid
				//DrawGrid();
				
				while(SW1 && SW2){
					SW1 = GPIO_PORTF_DATA_R&0x10;
					SW2 = GPIO_PORTF_DATA_R&0x01;
				};
				
				//SW1 on release, move to the next position
				if(!SW1){
					
					//wait untill SW1 is released
					while(!SW1){SW1 = GPIO_PORTF_DATA_R&0x10;}
					playerPos = (playerPos + 1)%numOfCol;
					playersCoins[turn%2][turn/2].x = colCenter[playerPos];
				}
				
				//SW2 on release, place the coin in the column if possible
				if(!SW2){
					//wait untill SW2 is released
					while(!SW2){SW2 = GPIO_PORTF_DATA_R&0x01;}
					
					if(cellCoins[playerPos] < numOfRow){
						theGrid[numOfRow - 1 - cellCoins[playerPos]][playerPos].player = turn%2 + 1;
						playersCoins[turn%2][turn/2].y = theGrid[numOfRow - 1 - cellCoins[playerPos]][playerPos].y;
						cellCoins[playerPos]++;
						turn++;
					}
				}
				
			//}
			
		//}
		
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

void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        
}
