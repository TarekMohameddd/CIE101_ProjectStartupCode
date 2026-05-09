#include "Game.h"
#include <ctime>
#include <cstdlib>
#include <random>
#include "../Config/GameConfig.h"
#include "../CMUgraphicsLib/auxil.h"

Game::Game()
{
	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);
	srand((int)time(0));
	gameStartTime = CurrentTime();
	lastWolfSpawnTime = gameStartTime;
	lastProductionUpdateTime = gameStartTime;
	pWind->SetBuffering(true);

	clearBudget();
	createToolbar();
	createBudgetbar();
	drawbudgetbar();

	foodAreaTopLeft.x = 1100;
	foodAreaTopLeft.y = 180;
	foodAreaBottomRight.x = 1200;
	foodAreaBottomRight.y = 550;

	clearbackground();
	drawFoodArea();
	clearStatusBar();
	drawstatusbar();
}

Game::~Game()
{
	clearDynamicObjects();
}

void Game::drawFoodArea() const
{
	if (!foodAreaVisible)
		return;

	window* pWindLocal = getWind();
	pWindLocal->SetPen(LIGHTGREEN, 1);
	pWindLocal->SetBrush(LIGHTGREEN);
	pWindLocal->DrawRectangle(
		foodAreaTopLeft.x,
		foodAreaTopLeft.y - 30,
		foodAreaBottomRight.x,
		foodAreaBottomRight.y
	);

	pWindLocal->SetPen(DARKGREEN, 3);
	pWindLocal->SetBrush(LIGHTGREEN);
	pWindLocal->DrawRectangle(
		foodAreaTopLeft.x,
		foodAreaTopLeft.y,
		foodAreaBottomRight.x,
		foodAreaBottomRight.y
	);

	string counterText = "Food: " + to_string(foodCount);
	pWindLocal->SetPen(BLACK, 1);
	pWindLocal->SetFont(18, BOLD, BY_NAME, "Arial");
	pWindLocal->DrawString(foodAreaTopLeft.x + 10, foodAreaTopLeft.y - 25, counterText);
}

void Game::consumeFood(int amount)
{
	if (amount <= 0 || foodCount <= 0)
		return;

	foodCount -= amount;

	if (foodCount <= 0)
	{
		foodCount = 0;
		window* pWindLocal = getWind();
		pWindLocal->SetPen(config.bkGrndColor, 1);
		pWindLocal->SetBrush(config.bkGrndColor);
		pWindLocal->DrawRectangle(
			foodAreaTopLeft.x,
			foodAreaTopLeft.y - 30,
			foodAreaBottomRight.x,
			foodAreaBottomRight.y
		);
		foodAreaVisible = false;
		return;
	}

	drawFoodArea();
}

void Game::drawstatusbar() const
{
	clearStatusBar();
	pWind->SetPen(WHITE, 1);
	pWind->SetFont(20, BOLD, BY_NAME, "Arial");
	int y = config.windHeight - 35;
	pWind->DrawString(10, y, "Timer: " + to_string(timer));
	pWind->DrawString(220, y, "Level: " + to_string(level));
	pWind->DrawString(330, y, "Goal: " + to_string(goal));
	pWind->DrawString(440, y, "Current Animals: " + to_string(currentAnimals));
	pWind->DrawString(760, y, "Eggs: " + to_string(eggsCount));
	pWind->DrawString(930, y, "Milk: " + to_string(producedMilkCount));
}

void Game::drawbudgetbar() const
{
	pWind->SetPen(BLACK, 1);
	pWind->SetBrush(BLACK);
	clearBudget();
	pWind->SetPen(WHITE, 2);
	pWind->SetFont(18, BOLD, BY_NAME, "Arial");
	int y = (2 * config.toolBarHeight + 20);
	pWind->DrawString(10, y, "Budget: $" + to_string(budget));
	pWind->DrawString(230, y, "Animal: $" + to_string(animalBuyingPrice));
	pWind->DrawString(380, y, "Water: $" + to_string(waterBuyingPrice));
}

clicktype Game::getMouseClick(int& x, int& y) const
{
	return pWind->GetMouseClick(x, y);
}

string Game::getSrting() const
{
	string Label;
	char Key;
	keytype ktype;
	pWind->FlushKeyQueue();
	while (1)
	{
		ktype = pWind->WaitKeyPress(Key);
		if (ktype == ESCAPE)
			return "";
		if (Key == 13)
			return Label;
		if (Key == 8)
			if (Label.size() > 0)
				Label.resize(Label.size() - 1);
			else
				Key = '\0';
		else
			Label += Key;
		printMessage(Label);
	}
}

window* Game::CreateWind(int w, int h, int x, int y) const
{
	window* pW = new window(w, h, x, y);
	pW->SetBrush(config.bkGrndColor);
	pW->SetPen(config.bkGrndColor, 1);
	pW->DrawRectangle(0, 0, w, h);
	return pW;
}

void Game::createToolbar()
{
	point toolbarUpperleft;
	toolbarUpperleft.x = 0;
	toolbarUpperleft.y = 0;

	gameToolbar = new Toolbar(this, toolbarUpperleft, 0, config.toolBarHeight);
	gameToolbar->draw();
}

void Game::createBudgetbar()
{
	point budgetbarUpperleft;
	budgetbarUpperleft.x = 0;
	budgetbarUpperleft.y = 2 * config.toolBarHeight;

	gameBudgetbar = new Budgetbar(this, budgetbarUpperleft, 0, config.toolBarHeight);
	gameBudgetbar->draw();
}

void Game::clearBudget() const
{
	pWind->SetPen(BLACK, 1);
	pWind->SetBrush(BLACK);
	pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, 3 * config.toolBarHeight);
}

void Game::printBudget(string msg) const
{
	clearBudget();
	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(config.windWidth - 200, config.toolBarHeight + 10, msg);
}

void Game::clearbackground() const
{
	pWind->DrawImage("images\\farmback.jpg", 0, 3 * config.toolBarHeight,
		config.windWidth, config.windHeight - (3 * config.toolBarHeight) - config.statusBarHeight);
}

void Game::clearStatusBar() const
{
	pWind->SetPen(config.statusBarColor, 1);
	pWind->SetBrush(config.statusBarColor);
	pWind->DrawRectangle(0, config.windHeight - config.statusBarHeight, config.windWidth, config.windHeight);
}

void Game::updatetimer()
{
	if (!paused && timer > 0 && ElapsedTime(1000))
	{
		timer--;
		drawbudgetbar();
		drawstatusbar();
	}
}

point Game::getRandomFieldPoint(int objectWidth, int objectHeight) const
{
	static std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> xDist(0, config.windWidth - objectWidth);
	std::uniform_int_distribution<int> yDist(3 * config.toolBarHeight, config.windHeight - config.statusBarHeight - objectHeight);

	point p;
	p.x = xDist(rng);
	p.y = yDist(rng);
	return p;
}

void Game::addRandomChick()
{
	if (animalCount >= 100) return;
	point p = getRandomFieldPoint(50, 50);
	Chick* chick = new Chick(this, p, 50, 50, "images\\chick.jpg");
	animalList[animalCount++] = chick;
	chickAnimals.push_back(chick);
}

void Game::addRandomCow()
{
	if (animalCount >= 100) return;
	point p = getRandomFieldPoint(60, 60);
	Cow* cow = new Cow(this, p, 60, 60, "images\\cow.jpg");
	animalList[animalCount++] = cow;
	cowAnimals.push_back(cow);
}

void Game::addRandomWaterArea()
{
	if (waterCount >= 100) return;
	point p = getRandomFieldPoint(60, 60);
	waterList[waterCount++] = new Water(this, p, 60, 60);
}

void Game::addEgg(point p)
{
	if (eggCount >= 100) return;
	point productPoint = p;
	productPoint.x += 15;
	productPoint.y += 15;
	eggList[eggCount++] = new Egg(this, productPoint, 20, 28);
	eggsCount++;
}

void Game::addMilk(point p)
{
	if (milkCount >= 100) return;
	point productPoint = p;
	productPoint.x += 18;
	productPoint.y += 18;
	milkList[milkCount++] = new Milk(this, productPoint, 24, 32);
	producedMilkCount++;
}

void Game::generateRandomWolf()
{
	if (animalCount >= 100) return;
	point p = getRandomFieldPoint(60, 60);
	animalList[animalCount++] = new Wolf(this, p, 60, 60, "images\\wolf.jpg");
}

void Game::updateAutoWolfGeneration()
{
	if (currentAnimals > 0) {
		long elapsedMs = CurrentTime() - gameStartTime;
		long spawnInterval = 8000;
		if ((CurrentTime() - lastWolfSpawnTime) >=  spawnInterval) {
			generateRandomWolf();
			lastWolfSpawnTime = CurrentTime();
			drawstatusbar();
		}
	}
}

void Game::updateOneSecond()
{
	int animalsCount = static_cast<int>(chickAnimals.size()) + static_cast<int>(cowAnimals.size());
	if (animalsCount <= 0)
		return;

	if (foodCount > 0) {
		consumeFood(animalsCount);
	}

	drawstatusbar();
}

void Game::updateAnimalProduction(long deltaMs)
{
	(void)deltaMs;
}

void Game::registerChickPurchased()
{
	chickTimers.push_back(0);
}

void Game::registerCowPurchased()
{
	cowTimers.push_back(0);
}

void Game::printMessage(string msg) const
{
	clearStatusBar();
	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(10, config.windHeight - (int)(0.85 * config.statusBarHeight), msg);
}

window* Game::getWind() const
{
	return pWind;
}

void Game::go()
{
	int x, y;
	time_t lastTick = time(nullptr);
	clicktype click;

	pWind->ChangeTitle("- - - - - - - - - - Hog Raiders Farm - - - - - - - - - -");

	do
	{
		long currentMs = CurrentTime();
		if (!paused) {
			updateAnimalProduction(currentMs - lastProductionUpdateTime);
		}
		lastProductionUpdateTime = currentMs;

		time_t now = time(nullptr);
		if (!paused && now > lastTick)
		{
			int elapsed = static_cast<int>(now - lastTick);
			for (int i = 0; i < elapsed; i++) {
				updateOneSecond();
			}
			lastTick = now;
		}
		else if (paused)
		{
			lastTick = now;
		}

		updatetimer();
		click = getMouseClick(x, y);
		if (click != NO_CLICK && y >= 0 && y < config.toolBarHeight) {
			gameToolbar->handleClick(x, y);
		}
		if (click != NO_CLICK && y >= config.toolBarHeight && y < 2 * config.toolBarHeight) {
			gameBudgetbar->handleClick(x, y);
		}

		updateAutoWolfGeneration();
		clearbackground();
		drawFoodArea();

		for (int i = 0; i < waterCount; i++) {
			waterList[i]->draw();
		}
		for (int i = 0; i < eggCount; i++) {
			eggList[i]->draw();
		}
		for (int i = 0; i < milkCount; i++) {
			milkList[i]->draw();
		}
		for (int i = 0; i < animalCount; i++) {
			animalList[i]->moveStep();
			animalList[i]->draw();
		}

		pWind->UpdateBuffer();
		Pause(30);
	} while (!isExit);
}

void Game::restartGame()
{
	clearDynamicObjects();
	budget = 2000;
	timer = 120;
	goal = 5;
	level = 1;
	score = 0;
	currentAnimals = 0;
	animalBuyingPrice = 200;
	waterBuyingPrice = 50;
	foodCount = 20;
	foodAreaVisible = true;
	eggsCount = 0;
	producedMilkCount = 0;
	chickAnimals.clear();
	chickTimers.clear();
	cowAnimals.clear();
	cowTimers.clear();
	lastProductionUpdateTime = CurrentTime();
	paused = false;
	restart = false;

	gameBudgetbar->resetAnimals();

	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);

	clearbackground();
	drawFoodArea();
	drawbudgetbar();
	drawstatusbar();
}

void Game::resetgame()
{
	clearDynamicObjects();
	clearbackground();
	budget = 2000;
	timer = 120;
	goal = 5;
	level = 1;
	score = 0;
	currentAnimals = 0;
	animalBuyingPrice = 200;
	waterBuyingPrice = 50;
	paused = false;
	restart = false;
	foodCount = 20;
	foodAreaVisible = true;
	eggsCount = 0;
	producedMilkCount = 0;
	chickAnimals.clear();
	chickTimers.clear();
	cowAnimals.clear();
	cowTimers.clear();
	lastProductionUpdateTime = CurrentTime();
	createToolbar();
	createBudgetbar();
	drawFoodArea();
	drawbudgetbar();
	drawstatusbar();
}

void Game::clearDynamicObjects()
{
	for (int i = 0; i < animalCount; i++) {
		delete animalList[i];
		animalList[i] = nullptr;
	}
	animalCount = 0;
	chickAnimals.clear();
	chickTimers.clear();
	cowAnimals.clear();
	cowTimers.clear();

	for (int i = 0; i < eggCount; i++) {
		delete eggList[i];
		eggList[i] = nullptr;
	}
	eggCount = 0;

	for (int i = 0; i < milkCount; i++) {
		delete milkList[i];
		milkList[i] = nullptr;
	}
	milkCount = 0;

	for (int i = 0; i < waterCount; i++) {
		delete waterList[i];
		waterList[i] = nullptr;
	}
	waterCount = 0;
}
