#include "Animal.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include "../CMUgraphicsLib/auxil.h"
#include <cstdlib>

namespace {
	int getRandomDirection()
	{
		return (rand() % 3) - 1; //{-1, 0, 1}
	}
	int getRandomWolfDirection(const Animal* wolf)
	{
		int Speed;

		if (wolf->pGame->level == 1)
			Speed = 1;
		else if (wolf->pGame->level == 2)
			Speed = 2;
		else if (wolf->pGame->level == 3)
			Speed = 3;
		else
			Speed = 4;

		if (rand() % 2 == 0)
			return -Speed;
		else
			return Speed;

	}

	

	int getRandomVelocityFrames()
	{
		return 80 + (rand() % 101); //[80,180]
	}

	void resetVelocity(Animal* animal)
	{
		animal->curr_vel.x = getRandomDirection();
		animal->curr_vel.y = getRandomDirection();
		animal->velocityFramesLeft = getRandomVelocityFrames(); 
	}
	void resetWolfVelocity(Animal* wolf)
	{
		wolf->curr_vel.x = getRandomWolfDirection(wolf);
		wolf->curr_vel.y = getRandomWolfDirection(wolf);
		wolf->velocityFramesLeft = getRandomVelocityFrames();
	}

	

	void moveWithinField(Animal* animal)
	{
		const int minX = 0; 
		const int maxX = config.windWidth - 60; 
		const int minY = 3 * config.toolBarHeight; 
		const int maxY = config.windHeight - config.statusBarHeight - 60; 

		animal->curr_pos.x += animal->curr_vel.x; 
		animal->curr_pos.y += animal->curr_vel.y; 
		animal->velocityFramesLeft--; 

		//boundary checking and bouncing back if the animal hits the window borders
		if ((animal->curr_pos.x < minX && animal->curr_vel.x == -1) || (animal->curr_pos.x > maxX && animal->curr_vel.x == 1)) {
			animal->curr_vel.x = -animal->curr_vel.x;
			if (animal->curr_pos.x < minX) animal->curr_pos.x = minX;
			if (animal->curr_pos.x > maxX) animal->curr_pos.x = maxX;
		}

		if ((animal->curr_pos.y < minY && animal->curr_vel.y == -1) || (animal->curr_pos.y > maxY - 20 && animal->curr_vel.y == 1)) {
			animal->curr_vel.y = -animal->curr_vel.y;
			if (animal->curr_pos.y < minY) animal->curr_pos.y = minY;
			if (animal->curr_pos.y > maxY) animal->curr_pos.y = maxY;
		}

		animal->RefPoint = animal->curr_pos; //updates the reference point
	}

	void drawChickenShape(window* pWind, point refPoint, int width, int height)
	{
		const int left = refPoint.x;
		const int top = refPoint.y;
		const int right = left + width;
		const int bottom = top + height;

		pWind->SetPen(BLACK, 2);
		pWind->SetBrush(GOLD);
		pWind->DrawEllipse(left + width / 5, top + height / 3, right - width / 6, bottom - height / 8, FILLED);

		pWind->SetBrush(GOLDENROD);
		pWind->DrawCircle(right - width / 4, top + height / 3, width / 7, FILLED);

		pWind->SetBrush(RED);
		pWind->DrawTriangle(right - width / 10, top + height / 3,
			right + width / 15, top + height / 3 + height / 12,
			right - width / 10, top + height / 3 + height / 6, FILLED);

		pWind->SetBrush(RED);
		pWind->DrawCircle(right - width / 4, top + height / 6, width / 16, FILLED);
		pWind->DrawCircle(right - width / 5, top + height / 7, width / 20, FILLED);

		pWind->SetBrush(BLACK);
		pWind->DrawCircle(right - width / 5, top + height / 3, 2, FILLED);

		pWind->SetPen(ORANGE, 3);
		pWind->DrawLine(left + width / 2, bottom - height / 8, left + width / 2, bottom + height / 8);
		pWind->DrawLine(left + width / 2, bottom + height / 8, left + width / 2 - width / 12, bottom + height / 5);
		pWind->DrawLine(left + width / 2, bottom + height / 8, left + width / 2 + width / 12, bottom + height / 5);
		pWind->DrawLine(left + width / 2 + width / 6, bottom - height / 8, left + width / 2 + width / 6, bottom + height / 8);
		pWind->DrawLine(left + width / 2 + width / 6, bottom + height / 8, left + width / 2 + width / 12, bottom + height / 5);
		pWind->DrawLine(left + width / 2 + width / 6, bottom + height / 8, left + width / 2 + width / 4, bottom + height / 5);

		pWind->SetPen(SIENNA, 2);
		pWind->DrawLine(left + width / 5, top + height / 2, left + width / 12, top + height / 4);
		pWind->DrawLine(left + width / 5, top + height / 2, left + width / 18, top + height / 2);
		pWind->DrawLine(left + width / 5, top + height / 2, left + width / 12, top + 3 * height / 4);
	}

	void drawCowShape(window* pWind, point refPoint, int width, int height)
	{
		const int left = refPoint.x;
		const int top = refPoint.y;
		const int right = left + width;
		const int bottom = top + height;

		pWind->SetPen(BLACK, 2);
		pWind->SetBrush(WHITESMOKE);
		pWind->DrawRectangle(left + width / 5, top + height / 3, right - width / 7, bottom - height / 6, FILLED);
		pWind->DrawRectangle(right - width / 4, top + height / 4, right, top + 2 * height / 3, FILLED);

		pWind->SetBrush(PINK);
		pWind->DrawEllipse(right - width / 5, top + height / 2, right, top + 3 * height / 4, FILLED);

		pWind->SetBrush(BLACK);
		pWind->DrawCircle(right - width / 10, top + 2 * height / 5, 2, FILLED);
		pWind->DrawCircle(right - width / 5, top + 2 * height / 5, 2, FILLED);
		pWind->DrawCircle(right - width / 8, top + 5 * height / 8, 2, FILLED);
		pWind->DrawCircle(right - width / 18, top + 5 * height / 8, 2, FILLED);

		pWind->SetBrush(BLACK);
		pWind->DrawEllipse(left + width / 3, top + height / 2, left + width / 2, top + 3 * height / 4, FILLED);
		pWind->DrawEllipse(left + width / 2, top + height / 3, left + 2 * width / 3, top + height / 2, FILLED);
		pWind->DrawEllipse(right - width / 3, top + height / 2, right - width / 6, top + 4 * height / 5, FILLED);

		pWind->SetBrush(SIENNA);
		pWind->DrawTriangle(right - width / 5, top + height / 4,
			right - width / 6, top + height / 10,
			right - width / 10, top + height / 4, FILLED);
		pWind->DrawTriangle(right - width / 20, top + height / 4,
			right, top + height / 10,
			right + width / 20, top + height / 4, FILLED);

		pWind->SetPen(BLACK, 3);
		pWind->DrawLine(left + width / 3, bottom - height / 6, left + width / 3, bottom + height / 5);
		pWind->DrawLine(left + width / 2, bottom - height / 6, left + width / 2, bottom + height / 5);
		pWind->DrawLine(right - width / 3, bottom - height / 6, right - width / 3, bottom + height / 5);
		pWind->DrawLine(right - width / 5, bottom - height / 6, right - width / 5, bottom + height / 5);

		pWind->SetPen(BLACK, 2);
		pWind->DrawLine(left + width / 5, top + height / 2, left, top + height / 6);
	}

	void drawWolfShape(window* pWind, point refPoint, int width, int height)
	{
		const int left = refPoint.x;
		const int top = refPoint.y;
		const int right = left + width;
		const int bottom = top + height;

		pWind->SetPen(BLACK, 2);
		pWind->SetBrush(DARKGRAY);
		pWind->DrawEllipse(left + width / 4, top + height / 3, right - width / 6, bottom - height / 6, FILLED);

		pWind->SetBrush(GRAY);
		pWind->DrawCircle(right - width / 4, top + height / 3, width / 6, FILLED);

		pWind->SetBrush(BLACK);
		pWind->DrawCircle(right - width / 5, top + height / 3, 3, FILLED);

		pWind->SetBrush(WHITE);
		pWind->DrawTriangle(right - width / 8, top + height / 2,
			right, top + height / 2 + height / 10,
			right - width / 8, top + height / 2 + height / 5, FILLED);

		pWind->SetPen(BLACK, 3);
		pWind->DrawLine(left + width / 3, bottom - height / 6, left + width / 3, bottom + height / 5);
		pWind->DrawLine(right - width / 3, bottom - height / 6, right - width / 3, bottom + height / 5);
	}
}

Animal::Animal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
	curr_pos = r_point;
	curr_vel.x = 0;
	curr_vel.y = 0;
	velocityFramesLeft = 0;
	lastProductTime = CurrentTime();
	productIntervalMs = 0;
	
}

void Animal::draw() const
{
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
}

Chick::Chick(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Animal(r_pGame, r_point, r_width, r_height, img_path)
{
	animalType = "chick";
	resetVelocity(this);
	productIntervalMs = 10000;
}

void Chick::draw() const
{
	
		window* pWind = pGame->getWind();
		drawChickenShape(pWind, RefPoint, width, height);

		long currentTime = pGame->paused ? pGame->pauseStartTime : CurrentTime();
		long remainingMs = productIntervalMs - (currentTime - lastProductTime);
		if (remainingMs < 0)
			remainingMs = 0;
		int remainingSeconds = remainingMs / 1000;

		pWind->SetPen(BLACK, 1);
		pWind->SetFont(20, BOLD, BY_NAME, "Arial");
		pWind->DrawString(RefPoint.x, RefPoint.y - 10, to_string(remainingSeconds));
	}
	
		
	


void Chick::moveStep()
{
	 
		if ((CurrentTime() - lastProductTime) >= productIntervalMs) {
			pGame->addEgg(curr_pos);
			lastProductTime = CurrentTime();
		}

		if (velocityFramesLeft <= 0) {
			resetVelocity(this); //resets velocity after the frames for current velocity are over
		}

		moveWithinField(this);
	}


Cow::Cow(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Animal(r_pGame, r_point, r_width, r_height, img_path)
{
	animalType = "cow";
	resetVelocity(this);
	productIntervalMs = 15000;
}

void Cow::draw() const
{
	window* pWind = pGame->getWind();
	drawCowShape(pWind, RefPoint, width, height);
	long currentTime = pGame->paused ? pGame->pauseStartTime : CurrentTime();
	long remainingMs = productIntervalMs - (currentTime - lastProductTime);
	if (remainingMs < 0)
		remainingMs = 0;
	int remainingSeconds = remainingMs  / 1000;

	pWind->SetPen(BLACK, 1);
	pWind->SetFont(20, BOLD, BY_NAME, "Arial");
	pWind->DrawString(RefPoint.x, RefPoint.y - 10, to_string(remainingSeconds));
}


void Cow::moveStep()
{
	if ((CurrentTime() - lastProductTime) >= productIntervalMs) {
		pGame->addMilk(curr_pos);
		lastProductTime = CurrentTime();
	}

	if (velocityFramesLeft <= 0) {
		resetVelocity(this);
	}

	moveWithinField(this);
}

Wolf::Wolf(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Animal(r_pGame, r_point, r_width, r_height, img_path)
{
	animalType = "wolf";
	resetWolfVelocity(this);
	;
	
}

void Wolf::draw() const
{ 
	if ((!pGame->paused)) {
		window* pWind = pGame->getWind();
		drawWolfShape(pWind, RefPoint, width, height);
	}
}

void Wolf::moveStep()
{
	if (velocityFramesLeft <= 0) {
		resetWolfVelocity(this);

	}

	moveWithinField(this);
}

Egg::Egg(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
}

void Egg::draw() const
{
	window* pWind = pGame->getWind();
	pWind->SetPen(BLACK, 1);
	pWind->SetBrush(WHITE);
	pWind->DrawEllipse(RefPoint.x, RefPoint.y, RefPoint.x + width, RefPoint.y + height, FILLED);
}

Milk::Milk(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
}

void Milk::draw() const
{
	window* pWind = pGame->getWind();
	pWind->SetPen(BLACK, 2);
	pWind->SetBrush(WHITESMOKE);
	pWind->DrawRectangle(RefPoint.x + width / 4, RefPoint.y + height / 3,
		RefPoint.x + 3 * width / 4, RefPoint.y + height, FILLED);
	pWind->SetBrush(WHITE);
	pWind->DrawRectangle(RefPoint.x + width / 3, RefPoint.y,
		RefPoint.x + 2 * width / 3, RefPoint.y + height / 3, FILLED);
}

Warehouse::Warehouse(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
}

void Warehouse::draw() const
{
	window* pWind = pGame->getWind();
	pWind->SetPen(BLACK, 2);
	pWind->SetBrush(DARKGRAY);
	pWind->DrawRectangle(RefPoint.x, RefPoint.y + height / 3,
		RefPoint.x + width, RefPoint.y + height, FILLED);
	pWind->SetBrush(RED);
	pWind->DrawTriangle(RefPoint.x, RefPoint.y + height / 3,
		RefPoint.x + width / 2, RefPoint.y,
		RefPoint.x + width, RefPoint.y + height / 3, FILLED);
	pWind->SetBrush(BROWN);
	pWind->DrawRectangle(RefPoint.x + width / 3, RefPoint.y + height / 2,
		RefPoint.x + 2 * width / 3, RefPoint.y + height, FILLED);
}

Water::Water(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
}

void Water::draw() const
{
	window* pWind = pGame->getWind();
	pWind->SetPen(GREEN, 1);
	pWind->SetBrush(GREEN);
	pWind->DrawRectangle(RefPoint.x, RefPoint.y, RefPoint.x + 60, RefPoint.y + 60 , FILLED);
}
