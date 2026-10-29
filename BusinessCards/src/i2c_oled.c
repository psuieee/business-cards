// what type of OLED - uncomment just one
//#define SSD1306_64X32
//#define SSD1306_128X32
#define SSD1306_128X64

#include "ch32v003fun.h"
#include <stdio.h>
#include "ssd1306_i2c.h"
#include "ssd1306.h"

#define RIGHT_BTN PA1
#define LEFT_BTN  PD5
#define SCREENW 128
#define SCREENH 64
#define BALLRADIUS 3
#define PADDLEWIDTH 25
#define PADDLEHEIGHT 3
#define SPEEDMULTIPLIER 1.3


uint8_t bricks[4] = {0b11111111, 0b11111111, 0b11111111, 0b11111111};
float ballX = (SCREENW - BALLRADIUS) / 2 + 1;
float ballY = SCREENH - PADDLEHEIGHT - BALLRADIUS*2;
float paddleX = (SCREENW - PADDLEWIDTH) / 2;
float paddleY = SCREENH - PADDLEHEIGHT - 1;
const float basespeed = 1.0;
const float maxspeed = 2.0;
float ballDX = basespeed;
float ballDY = -basespeed;
uint8_t playing = 1;



void sleep(void);
void setup(void);
uint8_t getScore(void);
void drawBricks(void);
void resetState(uint8_t resetBricks);
uint8_t playAgain(void);
float abs(float x) {
	return x < 0 ? -x : x;
}

int main()
{
// 48MHz internal clock
	setup();
	
	printf("Brick Break Demo Fall 2025 by Dominick Zentmayer...");
	
	while(1)
	{
		resetState(1);
		ssd1306_setbuf(0); //clear display
		ssd1306_drawstr(10, 0, "Brick Breaker!", 1);
		ssd1306_drawstr(20, 18, "Press both", 1);
		ssd1306_drawstr(0, 26, "buttons to start", 1);
		ssd1306_drawstr(32, 46, "Dominick", 1);
		ssd1306_drawstr(28, 54, "Zentmayer", 1);
		ssd1306_refresh();
		uint16_t timeout;
		timeout = 1000;
		while(!(!funDigitalRead(LEFT_BTN) && !funDigitalRead(RIGHT_BTN))) {
			Delay_Ms(10);
			if(!--timeout) break;
		}
		if(!timeout) playing = 0;
		else {
		playing = 1;
		ssd1306_setbuf(0); //clear display
		drawBricks();
		ssd1306_fillCircle(ballX, ballY, BALLRADIUS, 1);
		ssd1306_fillRect(paddleX, paddleY, PADDLEWIDTH, PADDLEHEIGHT, 1);
		ssd1306_refresh();
		Delay_Ms(1500);
		}
		//start game
		while (playing) {
			Delay_Ms(5);
			ballX += ballDX;
			ballY += ballDY;
			if(!funDigitalRead(LEFT_BTN) && paddleX > 0) {
				paddleX -= 2;
			}
			if(!funDigitalRead(RIGHT_BTN) && paddleX < (SCREENW - PADDLEWIDTH)) {
				paddleX += 2;
			}
			//Ball collision with walls
			if(ballX <= BALLRADIUS + 1) {
				ballDX = -ballDX;
				ballX = BALLRADIUS + 1;
			}
			else if(ballX >= SCREENW - BALLRADIUS - 1) {
				ballDX = -ballDX;
				ballX = SCREENW - BALLRADIUS - 1;
			}
			if(ballY <= BALLRADIUS + 1) {
				ballDY = -ballDY;
				ballY = BALLRADIUS + 1;
			}

			if((ballY >= SCREENH - BALLRADIUS - PADDLEHEIGHT + 1) && ((ballX > paddleX + BALLRADIUS + PADDLEWIDTH) || (ballX < paddleX - BALLRADIUS))) {
				//Ball missed paddle
				ssd1306_setbuf(0); //clear display
				ssd1306_drawstr(26, 8, "Game Over!", 1);
				char buffer[3];
				mini_itoa(getScore(), 10, 0, 0, buffer);
				ssd1306_drawstr(34, 26, "Score: ", 1);
				ssd1306_drawstr(82, 26, buffer, 1);
				ssd1306_refresh();
				Delay_Ms(1000);
				playing = playAgain();
				break;
			}
			else if((ballY >= SCREENH - BALLRADIUS - PADDLEHEIGHT + 1) && (ballX <= paddleX + BALLRADIUS + PADDLEWIDTH) && (ballX >= paddleX - BALLRADIUS)) {
				//Ball hit paddle
				//Change ball direction based on where it hit the paddle
				if(ballX < paddleX + PADDLEWIDTH / 3 && ballDX > 0) {
					ballDX = -ballDX;
					if(abs(ballDX) < maxspeed) ballDX *= SPEEDMULTIPLIER;
					if(abs(ballDX) > maxspeed) ballDX *= maxspeed/abs(ballDX);
				}
				else if(ballX > paddleX + (2 * PADDLEWIDTH) / 3 && ballDX < 0) {
					ballDX = -ballDX;
					if(abs(ballDX) < maxspeed) ballDX *= SPEEDMULTIPLIER;
					if(abs(ballDX) > maxspeed) ballDX *= maxspeed/abs(ballDX);
				}
				else {
					//Middle of paddle, keep same X direction
					if(abs(ballDX) > basespeed/1.5) ballDX /= SPEEDMULTIPLIER;
					if(abs(ballDX) < basespeed/1.5) ballDX *= (basespeed / 1.5)/abs(ballDX);
				}
				ballDY = -ballDY;
			}

			//Ball collision with bricks
			if(ballY <= 23) {
				uint8_t brickRow = ((ballY) / 6);
				uint8_t brickCol = (ballX + BALLRADIUS/2) / 16;
				if(bricks[brickRow] & (1 << brickCol)) {
					bricks[brickRow] ^= (1 << brickCol);
					ballDY = -ballDY;
					//Check if all bricks are gone
					uint8_t allGone = 1;
					for(uint8_t i = 0; i < 4; i++) {
						if(bricks[i] != 0) {
							allGone = 0;
							break;
						}
					}
					if(allGone) {
						ssd1306_setbuf(0); //clear display
						ssd1306_drawstr(32, 18, "You Win!", 1);
						ssd1306_refresh();
						Delay_Ms(1000);
						playing = playAgain();
						break;
					}
				}
			}

			ssd1306_setbuf(0); //clear display
			drawBricks();
			ssd1306_fillCircle(ballX, ballY, BALLRADIUS, 1);
			ssd1306_fillRect(paddleX, paddleY, PADDLEWIDTH, PADDLEHEIGHT, 1);
			ssd1306_refresh();
			
		}
		if(!playing)sleep();
	}
}


void sleep() {
	funDigitalWrite(PA2, 0); //Turn off OLED

	funPinMode(PD5, FUN_INPUT); //Left button
	funPinMode(PD6, FUN_INPUT); //Right button


	// AFIO is needed for EXTI
	RCC->APB2PCENR |= RCC_AFIOEN;

	// assign pin 2 interrupt from portD (0b11) to EXTI channel 2
	AFIO->EXTICR |= (uint32_t)(0b11 << (2*5))
	| (uint32_t)(0b11 << (2*6));

	// enable line2 interrupt event
	EXTI->EVENR |= EXTI_Line5 | EXTI_Line6;

	EXTI->FTENR |= EXTI_Line5 | EXTI_Line6; // falling edge trigger

	// select standby on power-down
	PWR->CTLR |= PWR_CTLR_PDDS;

	// peripheral interrupt controller send to deep sleep
	PFIC->SCTLR |= (1 << 2);

	Delay_Ms(500); //debounce
	__WFE(); // go to sleep

	setup();
}

void setup(void)
{
	SystemInit();
	funGpioInitAll();
	//Delay_Ms(1000); //sanity
	funPinMode(PC4, FUN_OUTPUT);
	funPinMode(PA2, FUN_OUTPUT);
	funPinMode(RIGHT_BTN, FUN_INPUT);
	funPinMode(LEFT_BTN, FUN_INPUT);
	funDigitalWrite(PA2, 0);
	funDigitalWrite(PC4, 0);
	//while(!(!funDigitalRead(LEFT_BTN) && !funDigitalRead(RIGHT_BTN))) Delay_Ms(10);
	funDigitalWrite(PA2, FUN_HIGH);
	printf("\r\r\n\ni2c_oled example\n\r");
	
	// init i2c and oled
	Delay_Ms( 100 );	// give OLED some more time
	printf("initializing i2c oled...");
	if(!ssd1306_i2c_init())
	{
		ssd1306_init();
		printf("done.\n\r");
		return;
	}
	else
	{
		printf("failed!\n\r");
		while(1);
	}
}

void drawBricks() {
	for (uint8_t i = 0; i < 4; i++) {
		for(uint8_t j = 0; j < 8; j++) {
			if (bricks[i] & (1 << j)) {
				ssd1306_fillRect(j * 16 + 1, i * 6 + 1, 14, 4, 1);
			}
		}
	}
}

uint8_t getScore() {
	uint8_t score = 32;
	for (uint8_t i = 0; i < 4; i++) {
		for(uint8_t j = 0; j < 8; j++) {
			if (bricks[i] & (1 << j)) {
				score--;
			}
		}
	}
	return score;
}

void resetState(uint8_t resetBricks) {
	ballX = (SCREENW - BALLRADIUS) / 2 + 1;
	ballY = SCREENH - PADDLEHEIGHT - BALLRADIUS*2;
	paddleX = (SCREENW - PADDLEWIDTH) / 2;
	ballDX = 1;
	ballDY = -1;
	if(resetBricks) 
		for (uint8_t i = 0; i < 4; i++) {
			bricks[i] = 0b11111111;
		}
}

uint8_t playAgain() {
	ssd1306_drawstr(24, 46, "Play Again?", 1);
	ssd1306_drawstr(0, 56, "Yes:L", 1);
	ssd1306_drawstr(SCREENW - 8*4, 56, "No:R", 1);
	ssd1306_refresh();
	uint16_t timeout;
	timeout = 1000;
	while(!(!funDigitalRead(LEFT_BTN) || !funDigitalRead(RIGHT_BTN))) {
		Delay_Ms(10);
		if(!--timeout) break;
	}
	if(!timeout) 
		return 0;
	else 
		return funDigitalRead(RIGHT_BTN);
}