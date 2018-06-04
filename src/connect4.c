

#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "Random.h"
#include "TExaS.h"
#include "img.h"
void Delay100ms(unsigned long count);
	
//port f
void PortF_Init(void);




int const cellW = 8; //cell width
int const cellH = 5; //cell height
int const hLineW = 1; //horizontal line width
int const vLineW = 2; // vertical line width
int const numOfCol = 7; 
int const numOfRow = 6;
int const numOfCoinsForEachPlayer = 21; // (numOfCol*numOfRow)/2
int const coinH = 3; //cell height
int const coinW = 6; //cell width

int leftMargin; //empty space on the left of the grid
int topMargin; //empy space on the top of the grid
int fullGridH; //grid height
int fullGridW; //grid width
int turn; //current turn
int lastTurn; //turn in the previous loop
int cellCenX; //half width of the cell
int cellCenY; //half height of the cell
int colCoins[numOfCol]; //number of coins in each column
int colCenter[numOfCol]; //each column center on x axis
int playerPos; //position of the player coin before playing
int winner; // who is the winner
int gameMode; // 0: menu, 1: 2players, 2: 1player vs ai,  3: ai vs ai
int menuCursor;
int i;
int	j;
int kitsNum;
int isMaster;
int willWePlayFirst;
int currPlayer;
int opponentPlayerNum;
int isMenuMode;
int menuNum;

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

//the full grid cells
struct cell theGrid[numOfRow][numOfCol];


//structure that describes the coins
//(x, y) are the center point of a coin
//image: the image of a coin
//draw(): draws a coin at its position
struct coin {
	int x;
	int y;
	const unsigned char *image;
	
	void (*draw)(struct coin*);
};

void draw(struct coin* c){
	Nokia5110_PrintBMP((*c).x - coinW/2, (*c).y + (coinH/2), (*c).image, 0);
	
}


//conis for each players
//playersCoins[0]: first player
//playersCoins[1]: second player
struct coin playersCoins[2][numOfCoinsForEachPlayer];





void gameInit(){
	
	willWePlayFirst = 1;
	isMenuMode = 1;
	menuNum = 0;
	
	//the grid dimintions
	fullGridW = (cellW * numOfCol + vLineW*(numOfCol+1));
	fullGridH = (cellH*numOfRow +  hLineW*numOfRow);
	
	
	//get margins
	leftMargin = (SCREENW - fullGridW)/2;
	topMargin = SCREENH - fullGridH;
	
	//individual cell center
	cellCenX = vLineW + (cellW/2);
	cellCenY = hLineW + (cellH/2);
	
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
		playersCoins[0][i].x = leftMargin + cellCenX;
		playersCoins[0][i].y = SCREENH - 1 - fullGridH - 3;
		//player two
		playersCoins[1][i].x = leftMargin + cellCenX;
		playersCoins[1][i].y = SCREENH - 1 - fullGridH - 3;
		
		
		//set the img for each player
		//player one
		playersCoins[0][i].image = pl1coin;
		//player two
		playersCoins[1][i].image = pl2coin;
		
		
	}
	
	//each column contains 0 coins at the begining
	for(i = 0; i < numOfCol; i++){
		colCoins[i] = 0;
		
		colCenter[i] = leftMargin + i*(cellW + vLineW) + cellCenX;
	}
	
	gameMode = 0;
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
	if(!winner)
		draw(&playersCoins[currPlayer][turn/2]);
	
	
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


void checkTriples(){
	int cellReq;
		for(i = 0; i < numOfRow; i++){
			for(j = 0; j < numOfCol; j++){
				//check vertically
				if(i + 2 < numOfRow){
					if(
						theGrid[i][j].player == theGrid[i+1][j].player &&
						theGrid[i+1][j].player == theGrid[i+2][j].player &&
						theGrid[i][j].player != 0
						){
							cellReq = j ;
							break;
						}
				}
				
				//horizontally
				if(j + 2 < numOfCol){
					if(
						theGrid[i][j].player == theGrid[i][j+1].player &&
						theGrid[i][j+1].player == theGrid[i][j+2].player &&
						theGrid[i][j].player != 0
						){
							if(
								j+3 < numOfCol &&
								theGrid[i][j+3].player == 0 && 
								((i == 0 ) || colCoins[j+3] == i))
								{
									cellReq = j+3;
									break;
								}
							else if(
								j-1 >= 0 &&
								theGrid[i][j-1].player == 0 && 
								((i == 0 ) || colCoins[j-1] == i)
							)
								{
									cellReq = j-1;
									break;
								}
						}
				}
				
				//diagonally right
				if(i + 2 < numOfRow && j + 2 < numOfCol){
					if(
						theGrid[i][j].player == theGrid[i+1][j+1].player &&
						theGrid[i+1][j+1].player == theGrid[i+2][j+2].player &&
						theGrid[i][j].player != 0
						)
						{
								if(
									j+3 < numOfCol &&
									i+3 < numOfRow &&
									theGrid[i][j+3].player == 0 && 
									((i == 0 ) || colCoins[j+3] == i))
									{
										cellReq = j+3;
										break;
									}
								else if(
									j-1 >= 0 &&
									theGrid[i][j-1].player == 0 && 
									((i == 0 ) || colCoins[j-1] == i)
								)
									{
										cellReq = j-1;
										break;
									}
							}
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
						cellReq = theGrid[i][j].player;
						break;
					}
			}
		}
	
	
}

int shouldPlayWithSw(){
	if(
		gameMode == 1 || //if p1 vs p2
		(gameMode == 2 && opponentPlayerNum != currPlayer) // if p1 vs ai : p1 only should play with the swiches;
		)
	
		return 1;
	else
		return 0;
}

int playInAcol(){
	if(colCoins[playerPos] < numOfRow){
		theGrid[numOfRow - 1 - colCoins[playerPos]][playerPos].player = currPlayer + 1;
		playersCoins[currPlayer][turn/2].y = theGrid[numOfRow - 1 - colCoins[playerPos]][playerPos].y;
		colCoins[playerPos]++;
		turn++;
		return 1;
	}else
		return 0;
	
}

//should return a valid position
int getAiNextPos(){
	int decision;
	
	decision = rand()%7;
	
	return decision;
}

unsigned int SW1;
unsigned int SW2;
char x;
int xx;

int main(void){
	//UART_Init();
  TExaS_Init(SSI0_Real_Nokia5110_Scope);  // set system clock to 80 MHz
  Random_Init(1);
  Nokia5110_Init();
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();      // draw buffer
	PortF_Init();
	
	gameInit();
	
	//UART0_OutChar((char)50);
	
	gameMode = 0;
	menuCursor = 0;
	kitsNum = 1;
	
	//kitsNum
	//isMaster
	
	willWePlayFirst = 0;
  while(1){
		Nokia5110_ClearBuffer();
		SW1 = GPIO_PORTF_DATA_R&0x10;
		SW2 = GPIO_PORTF_DATA_R&0x01;
		
		if(isMenuMode){
				//menu code 
			if(menuNum == 0){
				Nokia5110_Clear();
				Nokia5110_SetCursor(4, 0);
				Nokia5110_OutString("MENU");
				Nokia5110_SetCursor(2, 2);
				Nokia5110_OutString("P1 vs P2");
				Nokia5110_SetCursor(2, 3);
				Nokia5110_OutString("P1 vs AI");			
				Nokia5110_SetCursor(2, 4);
				Nokia5110_OutString("AI vs AI");
				
				Nokia5110_SetCursor( 0 , menuCursor + 2);
				Nokia5110_OutString(">>"); 
				//wait for an input
					while(SW1 && SW2){
						SW1 = GPIO_PORTF_DATA_R&0x10;
						SW2 = GPIO_PORTF_DATA_R&0x01;
					};
				//move down in menu	
				if(!SW1){
					while(!SW1){SW1 = GPIO_PORTF_DATA_R&0x10;}
					menuCursor = (menuCursor + 1) % 3;
				}
				//choose selection
				else if(!SW2){
					while(!SW2){SW2 = GPIO_PORTF_DATA_R&0x01;}
					gameMode = menuCursor + 1 ;
					
					//this variable should be deleted to complete the menu <<<<<<<<<<<<<<<<<<<
					isMenuMode = 0;
					continue;
					

				}
				Nokia5110_SetCursor( 0 ,menuCursor + 2);
				Nokia5110_OutString(">>"); 
			}else if (menuNum == 1){
				
			}
			else if (menuNum == 2){
				
			}else if (menuNum == 3){
				
			}

			
		}//menu code end
		
		
		else if(gameMode == 1 || gameMode == 2 || gameMode == 3){
			
			
			currPlayer = willWePlayFirst^(turn%2);
			opponentPlayerNum = willWePlayFirst;
		
			if(turn > lastTurn){
				playerPos = 0;
				lastTurn = turn;
			}
			update();
			winner = isThereAwinner();
			Nokia5110_SetCursor(1, 0);
			if(winner){
				//Nokia5110_Clear();
				
				if(winner == 1 ){
					Nokia5110_Clear();
					Nokia5110_OutString("P1 wins");
				}
				else{
					//Nokia5110_Clear();
					Nokia5110_OutString("P2 wins");
				}
				break;
			}

			
				//when playing with switches
				if(shouldPlayWithSw()){
					//wait for an input
					while( SW1 && SW2){
						SW1 = GPIO_PORTF_DATA_R&0x10;
						SW2 = GPIO_PORTF_DATA_R&0x01;
					};
				
				
				
					//SW1 on release, move to the next position
					if(!SW1){
						
						//wait untill SW1 is released
						while(!SW1){SW1 = GPIO_PORTF_DATA_R&0x10;}
						playerPos = (playerPos + 1)%numOfCol;
						
						//turn = 0; turn%2 = 0; first player >> turn/2 = 0; first coin
						//turn = 1; turn%2 = 1; second player >> turn/2 = 0; first coin
						//turn = 2; turn%2 = 0; first player >> turn/2 = 1; second coin
						//turn = 3; turn%2 = 1; second player >> turn/2 = 1; second coin
						playersCoins[currPlayer][turn/2].x = colCenter[playerPos];
					}
					//SW2 on release, place the coin in the column if possible
					if(!SW2){
						//wait untill SW2 is released
						while(!SW2){SW2 = GPIO_PORTF_DATA_R&0x01;}
						playInAcol();
					}
					
				}else{
					
					if(
							((gameMode == 2 && opponentPlayerNum == currPlayer) || //p1 vs ai
							(gameMode == 3)) && // ai vs ai
							kitsNum == 1
						){
						Delay100ms(1);
						playerPos = getAiNextPos();
						playersCoins[currPlayer][turn/2].x = colCenter[playerPos];
						update();
						
						playInAcol();
						Delay100ms(1);
					}
					
				}
				
			}
		
			
		
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
