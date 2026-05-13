#include "Game.h"
#include <ctime>
#include <cstdlib>
#include <random>
#include <utility>
#include "../Config/GameConfig.h"
#include "../CMUgraphicsLib/auxil.h"
#include <windows.h>
#include <mmsystem.h>


Game::Game()
{
	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);
	srand((int)time(0));
	gameStartTime = CurrentTime();
	lastWolfSpawnTime = gameStartTime;
	lastProductionUpdateTime = gameStartTime;
	pauseStartTime = 0;
	backgroundImage.Open("images\\farmback.jpg");
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
	clearStatusBar();
	drawstatusbar();
	playBackgroundMusic();

}

Game::~Game()
{
	clearDynamicObjects();
	stopBackgroundMusic();
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

window* Game::CreateWind(int x, int y)
{
	// x1 = 160 , x2 = 260 , y1 = config.windHeight - config.statusBarHeight - 400 +25 , y2 = config.windHeight - config.statusBarHeight - 400 + 105
	int left = 160;
	int top = config.windHeight - config.statusBarHeight - 400 + 25;
	int right = 260;
	int bottom = config.windHeight - config.statusBarHeight - 400 + 105;
	if (y >= top && y <= bottom && x >= left && x <= right) {
		window* ptWind = CreateWind(500, 500, 10, 10); //can't close this window fom pressing the top right X
		return ptWind;
	}
	return nullptr;
}


void Game::drawWarehouse() const
{
	int x = 160;
	int y = config.windHeight - config.statusBarHeight - 400;

	pWind->SetPen(BLACK, 2);
	pWind->SetBrush(BROWN);
	pWind->DrawRectangle(x, y + 25, x + 100, y + 105, FILLED);

	pWind->SetBrush(RED);
	pWind->DrawTriangle(x - 10, y + 25, x + 50, y, x + 110, y + 25, FILLED);
}


bool Game::CheckFood(Animal* animal, Water* water) const
{
	if (animal == nullptr || water == nullptr)
		return false;

	if (animal->RefPoint.x <= (water->RefPoint.x + water->width) &&
		animal->RefPoint.x + animal->width >= water->RefPoint.x &&
		animal->RefPoint.y <= (water->RefPoint.y + water->height) &&
		animal->RefPoint.y + animal->height >= water->RefPoint.y) return true;
	else return false;
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
	pWind->DrawImage(backgroundImage, 0, 3 * config.toolBarHeight,
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

		if (timer == 0)
		{
			level++;
			setInitialTimerByLevel();
		}

		drawbudgetbar();
		drawstatusbar();
	}
}


point Game::getRandomFieldPoint(int objectWidth, int objectHeight) const
{
	static std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<int> xDist(0, config.windWidth - objectWidth);
	std::uniform_int_distribution<int> yDist(3 * config.toolBarHeight, config.windHeight - config.statusBarHeight - objectHeight - 20);

	point p;
	p.x = xDist(rng);
	p.y = yDist(rng);
	return p;
}

void Game::addRandomAnimal(string animalType, int objectWidth, int objectHeight)
{
	if (animalCount >= 100) return;

	point p = getRandomFieldPoint(objectWidth, objectHeight); // get random point for the animal within the field boundaries

	if (animalType == "chick") {
		Chick* chick = new Chick(this, p, objectWidth, objectHeight, "images\\chick.jpg");//create a new animal object with the random point
		animalList[animalCount++] = chick;//add animal to the animal list and chickAnimals vector (is there to check number of animals since the wolf is in the same family)
		chickAnimals.push_back(chick);

	}
	else if (animalType == "cow") {
		Cow* cow = new Cow(this, p, objectWidth, objectHeight, "images\\cow.jpg");
		animalList[animalCount++] = cow;
		cowAnimals.push_back(cow);
	}
}

//void Game::addRandomWaterArea()
//{
//	if (waterCount >= 100) return;
//	point p = getRandomFieldPoint(60, 60);
//	waterList[waterCount++] = new Water(this, p, 60, 60);
//}

void Game::addEgg(point p)
{
	if (eggCount >= 100) return;
	point productPoint = p;
	productPoint.x += 15;
	productPoint.y += 15;

	eggList[eggCount++] = new Egg(this, productPoint, 20, 28);
}

void Game::addMilk(point p)
{
	if (milkCount >= 100) return;
	point productPoint = p;
	productPoint.x += 18;
	productPoint.y += 18;
	milkList[milkCount++] = new Milk(this, productPoint, 24, 32);
}

void Game::generateRandomWolf()
{
	int maxWolves;
	if (level == 1)
		maxWolves = 1;
	else if (level == 2)
		maxWolves = 2;
	else if (level == 3)
		maxWolves = 3;
	else
		maxWolves = 4;

	if (wolfCount >= maxWolves) return;
	point p = getRandomFieldPoint(60, 60);
	wolfHealth.push_back(wolfHealthBar);
	wolfList[wolfCount++] = new Wolf(this, p, 60, 60, "images\\wolf.jpg");
}

void Game::updateAutoWolfGeneration()
{
	if (currentAnimals > 0) {
		long elapsedMs = CurrentTime() - gameStartTime;
		long spawnInterval = 8000;
		if ((CurrentTime() - lastWolfSpawnTime) >= spawnInterval) {
			generateRandomWolf();
			lastWolfSpawnTime = CurrentTime();
			drawstatusbar();
		}
	}
}

bool isCollide(Animal* wolf, Animal* animal) {
	if (wolf == nullptr || animal == nullptr)
		return false;

	if (wolf->RefPoint.x <= (animal->RefPoint.x + animal->width) &&
		wolf->RefPoint.x + wolf->width >= animal->RefPoint.x &&
		wolf->RefPoint.y <= (animal->RefPoint.y + animal->height) &&
		wolf->RefPoint.y + wolf->height >= animal->RefPoint.y) return true;
	else return false;
}

void Game::updateOneSecond()
{
	int animalsCount = static_cast<int>(chickAnimals.size()) + static_cast<int>(cowAnimals.size());
	if (animalsCount <= 0)
		return;

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
	int x, y, x1, y1;
	time_t lastTick = time(nullptr);
	clicktype click, click2;

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
		if (click != NO_CLICK && y >= 2 * config.toolBarHeight) { //me
			if (waterCounter == 1) {
				point p;
				p.x = x - (30);
				p.y = y - (30);

				waterList[waterCount++] = new Water(this, p, 60, 60);

				waterCounter = 0;
			}
			window* ptWind = CreateWind(x, y);
			if (ptWind != nullptr) {
				ptWind->SetBuffering(true);
				ptWind->SetPen(config.bkGrndColor, 1);
				ptWind->SetBrush(config.bkGrndColor);
				ptWind->DrawRectangle(0, 0, ptWind->GetWidth(), ptWind->GetHeight(), FILLED);
				ptWind->ChangeTitle("warehouse");

				ptWind->SetPen(BLACK, 1);
				ptWind->SetFont(30, BOLD, BY_NAME, "Arial");
				ptWind->DrawString(20, 20, "EGGS");
				ptWind->SetFont(15, BOLD, BY_NAME, "Arial");
				ptWind->DrawString(20, 50, "Price of each Egg 20$");
				ptWind->DrawString(20, 70, "Eggs: " + to_string(eggsCount));
				ptWind->SetFont(30, BOLD, BY_NAME, "Arial");
				ptWind->SetPen(BLACK, 1);
				ptWind->SetFont(30, BOLD, BY_NAME, "Arial");
				ptWind->DrawString(20, 220, "MILKS");
				ptWind->SetFont(15, BOLD, BY_NAME, "Arial");
				ptWind->DrawString(20, 250, "Price of each Milk 20$");
				ptWind->DrawString(20, 270, "Milk: " + to_string(producedMilkCount));
				ptWind->SetFont(30, BOLD, BY_NAME, "Arial");

				ptWind->SetPen(WHITE, 1);
				ptWind->SetBrush(GREEN);
				ptWind->DrawRectangle(160, 20, 280, 80, FILLED);
				ptWind->SetPen(WHITE, 1);
				ptWind->SetFont(24, BOLD, BY_NAME, "Arial");
				ptWind->DrawString(185, 35, "SELL");

				ptWind->SetBrush(GREEN);
				ptWind->DrawRectangle(160, 220, 280, 270, FILLED);
				ptWind->SetPen(WHITE, 1);
				ptWind->SetFont(24, BOLD, BY_NAME, "Arial");
				ptWind->DrawString(200, 240, "SELL");

				ptWind->SetPen(BLACK, 1);
				ptWind->SetFont(30, BOLD, BY_NAME, "Arial");
				ptWind->DrawString(110, 450, "Press ESCAPE to leave");

				ptWind->UpdateBuffer();

				bool warehouseOpen = true;
				while (warehouseOpen) {
					click2 = ptWind->GetMouseClick(x1, y1);

					if (click2 != NO_CLICK && x1 >= 160 && x1 <= 280 && y1 >= 20 && y1 <= 80) {
						budget += (eggsCount * 20);
						eggsCount = 0;

						ptWind->SetPen(config.bkGrndColor, 1);
						ptWind->SetBrush(config.bkGrndColor);
						ptWind->DrawRectangle(47, 270, 80, 285, FILLED);
						ptWind->DrawRectangle(53, 65, 80, 90, FILLED);
						ptWind->SetPen(BLACK, 1);
						ptWind->SetFont(15, BOLD, BY_NAME, "Arial");
						ptWind->DrawString(55, 70, to_string(eggsCount));
						ptWind->DrawString(55, 270, to_string(producedMilkCount));
						ptWind->UpdateBuffer();

					}
					if (click2 != NO_CLICK && x1 >= 160 && x1 <= 280 && y1 >= 220 && y1 <= 270) {
						budget += (producedMilkCount * 20);
						producedMilkCount = 0;

						ptWind->SetPen(config.bkGrndColor, 1);
						ptWind->SetBrush(config.bkGrndColor);
						ptWind->DrawRectangle(47, 270, 80, 285, FILLED);
						ptWind->DrawRectangle(53, 65, 80, 90, FILLED);
						ptWind->SetPen(BLACK, 1);
						ptWind->SetFont(15, BOLD, BY_NAME, "Arial");
						ptWind->DrawString(55, 70, to_string(eggsCount));
						ptWind->DrawString(55, 270, to_string(producedMilkCount));
						ptWind->UpdateBuffer();
					}

					char Key2;
					keytype ktype2 = ptWind->GetKeyPress(Key2);
					if (ktype2 == ESCAPE) {
						warehouseOpen = false;
					}

					Pause(30);
				}

				delete ptWind;
				ptWind = nullptr;
			}
			for (int i = 0; i < eggCount; i++) {
				if (eggList[i] == nullptr || eggsCount >= 10)
					continue;

				point eP = eggList[i]->RefPoint;
				if ((y >= eP.y && y <= eP.y + 28) && ((x >= eP.x && x <= eP.x + 20))) {
					delete eggList[i];

					for (int j = i; j < eggCount - 1; j++) {
						eggList[j] = eggList[j + 1];
					}

					eggList[eggCount - 1] = nullptr;
					eggCount--;
					eggsCount++;
					break;
				}
			}
			for (int i = 0; i < milkCount; i++) {
				if (milkList[i] == nullptr || producedMilkCount >= 10)
					continue;

				point mP = milkList[i]->RefPoint;
				if ((y >= mP.y && y <= mP.y + 32) && (x >= mP.x && x <= mP.x + 24)) {
					delete milkList[i];

					for (int j = i; j < milkCount - 1; j++) {
						milkList[j] = milkList[j + 1];
					}

					milkList[milkCount - 1] = nullptr;
					milkCount--;
					producedMilkCount++;
					break;
				}
			}
			for (int i = 0; i < wolfCount; i++) {
				if (wolfList[i] == nullptr)
					continue;
				point wP = wolfList[i]->RefPoint;
				if ((y >= wP.y && y <= wP.y + 60) && (x >= wP.x && x <= wP.x + 60)) {
					wolfHealth[i]--;

					if (wolfHealth[i] <= 0) {
						delete wolfList[i];
						wolfHealth.erase(wolfHealth.begin() + i);

						for (int j = i; j < wolfCount - 1; j++) {
							wolfList[j] = wolfList[j + 1];
						}

						wolfList[wolfCount - 1] = nullptr;
						wolfCount--;
						budget += 100;
						break;
					}
				}
			}

		}
		else {

		}

		updateAutoWolfGeneration();
		clearbackground();
		drawWarehouse();
		//me

		for (int i = 0; i < waterCount; i++) {
			if (waterList[i] == nullptr)
				continue;
			waterList[i]->draw();

			for (int j = 0; j < animalCount; j++) {
				if (animalList[j] == nullptr)
					continue;

				if (CheckFood(animalList[j], waterList[i]))
				{
					delete waterList[i];
					for (int k = i; k < waterCount - 1; k++)
					{
						waterList[k] = waterList[k + 1];
					}
					addRandomAnimal(animalList[j]->animalType, animalList[j]->width, animalList[j]->height);
					currentAnimals++;
					drawstatusbar();
					waterList[waterCount - 1] = nullptr;
					waterCount--;
					foodCount++;
					i--;
					break;
				}
			}

		}
		for (int i = 0; i < eggCount; i++) {
			if (eggList[i] != nullptr)
				eggList[i]->draw();
		}
		for (int i = 0; i < milkCount; i++) {
			if (milkList[i] != nullptr)
				milkList[i]->draw();
		}

		for (int i = 0; i < animalCount; i++) {
			if (animalList[i] == nullptr)
				continue;

			if (!paused)
				animalList[i]->moveStep();
			animalList[i]->draw();

		}
		for (int i = 0; i < wolfCount; i++) {
			if (wolfList[i] == nullptr)
				continue;
			if (!paused)
				wolfList[i]->moveStep();
			wolfList[i]->draw();
			for (int j = 0; j < animalCount; j++) {
				if (animalList[j] == nullptr)
					continue;

				if (isCollide(wolfList[i], animalList[j])) {
					delete animalList[j];
					for (int k = j; k < animalCount - 1; k++) {
						animalList[k] = animalList[k + 1];
					}

					animalList[animalCount - 1] = nullptr;
					animalCount--;
					currentAnimals--;
					break;
				}
			}
		}

		pWind->UpdateBuffer();
		Pause(30);
	} while (!isExit);
}
void Game::restartGame()
{
	clearDynamicObjects();
	budget = 2000;
	goal = 5;
	level = 1;
	setInitialTimerByLevel();
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
	drawbudgetbar();
	drawstatusbar();
}

void Game::resetgame()
{
	clearDynamicObjects();
	clearbackground();
	budget = 2000;
	goal = 5;
	level = 1;
	setInitialTimerByLevel();
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
void Game::setInitialTimerByLevel() {
	if (level == 1) {
		timer = 120;
	}
	else if (level == 2) {
		timer = 100;
	}
	else if (level == 3) {
		timer = 80;
	}

	else if (level == 4) {
		timer = 60;
	}
	else {
		timer = 40;
	}

	
}
void Game::adjustProductionTimersAfterPause(long pausedDuration)
{
	for (int i = 0; i < animalCount; i++) {
		if (animalList[i] != nullptr)
			animalList[i]->lastProductTime += pausedDuration;
	}
}
void Game::playBackgroundMusic()
{
	mciSendString("open \"sounds\\bgmusic.wav\" type waveaudio alias bgm", NULL, 0, NULL);
	mciSendString("play bgm", NULL, 0, NULL);
}

void Game::pauseBackgroundMusic()
{
	mciSendString("pause bgm", NULL, 0, NULL);
}

void Game::resumeBackgroundMusic()
{
	mciSendString("resume bgm", NULL, 0, NULL);
}

void Game::stopBackgroundMusic()
{
	mciSendString("stop bgm", NULL, 0, NULL);
	mciSendString("close bgm", NULL, 0, NULL);
}
