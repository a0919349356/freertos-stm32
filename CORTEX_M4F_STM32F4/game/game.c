#include "game.h"
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Player1
int16_t player1X = 10;
int16_t player1Y = 10;
uint16_t player1W = 60;
uint16_t player1H = 10;
uint8_t player1IsReversed = 1;

//Player2
int16_t player2X = LCD_PIXEL_WIDTH / 2;
int16_t player2Y = LCD_PIXEL_HEIGHT -20;
int16_t pre_player2X;
int16_t pre_player2Y;
uint16_t player2W = 30;
uint16_t player2H = 10;
uint8_t player2IsReversed = 0;

//Ball
uint16_t ballSize = 5;
int16_t ballX[7];
int16_t ballY[7];
int16_t ballVX = 0;
int16_t ballVY = 3;
uint8_t ballIsRun = 0;
uint8_t lev = 0;

//Mode
uint8_t demoMode = 0;

//game over
uint8_t finish = 0;
void
BallReset()
{
	if(lev == 7) finish = 1; 
	if(finish)
		ballIsRun = 0;
	else
	{
		lev++;
		int i;
		for(i = 0 ; i < lev ; i++)
		{
			ballX[i] =rand()%( LCD_PIXEL_WIDTH - 5 )  ;
			ballY[i] = 10;
		}
		ballVX = 0;
		ballVY = 3 + lev;
		ballIsRun = 1;
	}
}

void
GAME_EventHandler1()
{
	if( STM_EVAL_PBGetState( BUTTON_USER ) ){

		player1IsReversed = 0;

		while( STM_EVAL_PBGetState( BUTTON_USER ) );

		player1IsReversed = 1;
	}
}

void
GAME_EventHandler2()
{
	if( IOE_TP_GetState()->TouchDetected ){

		player2IsReversed = 1^player2IsReversed;

		while( IOE_TP_GetState()->TouchDetected );

		//player2IsReversed = 0;
	}
}

void
GAME_EventHandler3()
{
	if( ballIsRun == 0 ){
		BallReset();
	}
}

void
GAME_Update()
{
	//Player1
	pre_player2X = player2X;
	pre_player2Y = player2Y;

	if( demoMode == 0 ){

		/*if( player1IsReversed )
			player1X -= 5;
		else
			player1X += 5;

		if( player1X <= 0 )
			player1X = 0;
		else if( player1X + player1W >= LCD_PIXEL_WIDTH )
			player1X = LCD_PIXEL_WIDTH - player1W;
		*/
		//Player2
		if( player2IsReversed )
			player2X -= 5;
		else
			player2X += 5;

		if( player2X <= 0 )
			player2X = 0;
		else if( player2X + player2W >= LCD_PIXEL_WIDTH )
			player2X = LCD_PIXEL_WIDTH - player2W;
	        
		LCD_SetLayer( LCD_BACKGROUND_LAYER );
		LCD_SetTextColor( LCD_COLOR_WHITE );
		LCD_DrawFullRect( player2X,player2Y, player2W, player2H );

		LCD_SetLayer( LCD_BACKGROUND_LAYER );
		LCD_SetTransparency(0xFF);
		LCD_SetLayer( LCD_FOREGROUND_LAYER );	
		LCD_SetTransparency(0x00);
		LCD_SetTextColor( LCD_COLOR_BLACK );
		LCD_DrawFullRect( pre_player2X,pre_player2Y, player2W, player2H );
		LCD_SetTextColor( LCD_COLOR_WHITE );
		LCD_DrawFullRect( player2X,player2Y, player2W, player2H );
	
		//Ball
		if( ballIsRun == 1 ){
			int i;
			LCD_SetTextColor( LCD_COLOR_BLACK );
			for(i = 0 ; i < lev ; i++)
				LCD_DrawFullRect( ballX[i], ballY[i], ballSize, ballSize );

			//Touch wall
			/*ballX += ballVX;
			if( ballX <= 0 ){
				ballX = 0;
				ballVX *= -1;
			}
			else if( ballX + ballSize >= LCD_PIXEL_WIDTH ){
				ballX = LCD_PIXEL_WIDTH - ballSize;
				ballVX *= -1;
			}*/

			//PONG!
			for(i = 0 ; i < lev ; i++)
				ballY[i] += ballVY;
			if( ballY[0] + ballSize >= player2Y ){
				int flag = 0;
				for(i = 0 ; i < lev ; i++)
				{
					if( ballX[i] + ballSize >= player2X && ballX[i] <= player2X + player2W ){
						flag = 1;
						break;
					}
				}
				if(flag)
				{
					finish = 1;
					BallReset();
				}
				else
					BallReset();
			}

			/*if( ballY <= player1Y + player1H ){
					if( ballX + ballSize >= player1X && ballX <= player1X + player1W ){
						if( ballX - ballSize <= player1Y + player1W/4 ){
							ballVY = 3;
							ballVX =-7;
						}
						else if( ballX >= player1Y + player1W - player1W/4 ){
							ballVY = 3;
							ballVX = 7;
						}
						else if( ballX + ballSize < player1Y + player1W/2 ){
							ballVY = 7;
							ballVX =-3;
						}
						else if( ballX > player1Y + player1W/2 ){
							ballVY = 7;
							ballVX = 3;
						}
						else{
							ballVY = 9;
							ballVX = 0;
						}
					}
					else
						BallReset();
				}*/
			}
		}
		else{	//if demoMode == 1

			//Player1 move
			/*if( ballVY < 0 ){
				if( player1X + player1W/2 < ballX + ballSize/2 ){
					player1X += 8;
					player2X += 2;
				}
				else{
					player1X -= 8;
					player2X -= 2;
				}
			}

			//Player2 move
			if( ballVY > 0 ){
				if( player2X + player2W/2 < ballX + ballSize/2 ){
					player1X += 2;
					player2X += 8;
				}
				else{
					player1X -= 2;
					player2X -= 8;
				}

			}

			if( player1X <= 0 )
				player1X = 0;
			else if( player1X + player1W >= LCD_PIXEL_WIDTH )
				player1X = LCD_PIXEL_WIDTH - player1W;

			if( player2X <= 0 )
				player2X = 0;
			else if( player2X + player2W >= LCD_PIXEL_WIDTH )
				player2X = LCD_PIXEL_WIDTH - player2W;


			//Ball
			if( ballIsRun == 1 ){

				LCD_SetTextColor( LCD_COLOR_BLACK );
				LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );

				//Touch wall
				ballX += ballVX;
				if( ballX <= 0 ){
					ballX = 0;
					ballVX *= -1;
				}
				else if( ballX + ballSize >= LCD_PIXEL_WIDTH ){
					ballX = LCD_PIXEL_WIDTH - ballSize;
					ballVX *= -1;
				}

				//PONG!
				ballY += ballVY;
				if( ballY + ballSize >= player2Y ){
					if( ballX + ballSize >= player2X && ballX <= player2X + player2W ){
					if( ballX - ballSize <= player2Y + player2W/4 ){
						ballVY =-3;
						ballVX =-7;
					}
					else if( ballX >= player2Y + player2W - player2W/4 ){
						ballVY =-3;
						ballVX = 7;
					}
					else if( ballX + ballSize < player2Y + player2W/2 ){
						ballVY =-7;
						ballVX =-3;
					}
					else if( ballX > player2Y + player2W/2 ){
						ballVY =-7;
						ballVX = 3;
					}
					else{
						ballVY =-9;
						ballVX = 0;
					}
				}
				else
					BallReset();
			}

			if( ballY <= player1Y + player1H ){
				if( ballX + ballSize >= player1X && ballX <= player1X + player1W ){
					if( ballX - ballSize <= player1Y + player1W/4 ){
						ballVY = 3;
						ballVX =-7;
					}
					else if( ballX >= player1Y + player1W - player1W/4 ){
						ballVY = 3;
						ballVX = 7;
					}
					else if( ballX + ballSize < player1Y + player1W/2 ){
						ballVY = 7;
						ballVX =-3;
					}
					else if( ballX > player1Y + player1W/2 ){
						ballVY = 7;
						ballVX = 3;
					}
					else{
						ballVY = 9;
						ballVX = 0;
					}
				}
				else
					BallReset();
			}
		}*/
	}
}

	void
GAME_Render()
{
	int i;
	LCD_SetLayer( LCD_FOREGROUND_LAYER );
	LCD_SetTextColor( LCD_COLOR_RED );
	for(i = 0;i < lev;i++)
		LCD_DrawFullRect( ballX[i], ballY[i], ballSize, ballSize );

	LCD_SetLayer( LCD_FOREGROUND_LAYER );
	LCD_SetTransparency(0xFF);
	LCD_SetLayer( LCD_BACKGROUND_LAYER );
	LCD_SetTransparency(0x00);

	LCD_SetTextColor( LCD_COLOR_BLACK );
	LCD_DrawFullRect( player2X, player2Y, player2W, player2H );
	

}
