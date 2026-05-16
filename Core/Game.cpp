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
	budgetBackgroundImage.Open("images\\actionbar.jpeg");
	topbars.Open("images\\upperb.jpeg");
	grass.Open("images\\grass.jpeg");
	warehouse.Open("images\\warehouse.jpeg");
	pWind->DrawImage(topbars, 0, 0, config.windWidth, config.toolBarHeight);
	pWind->DrawImage(topbars,0 ,config.toolBarHeight,config.windWidth,config.toolBarHeight*2);
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
	pWindLocal->DrawImage(grass,foodAreaTopLeft.x,foodAreaTopLeft.y - 30,foodAreaBottomRight.x - foodAreaTopLeft.x,foodAreaBottomRight.y - (foodAreaTopLeft.y - 30));



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
	int left = 160;
	int top = config.windHeight - config.statusBarHeight - 285;
	int right = 260;
	int bottom = config.windHeight - config.statusBarHeight - 195;
	if (y >= top && y <= bottom && x >= left && x <= right) {
		window* ptWind = CreateWind(500, 500, 10, 10); //can't close this window fom pressing the top right X
		return ptWind;
	}
	return nullptr;
}


bool Game::CheckFood(Animal* animal, Water* water , int animalIndex) const
{
	if (animal == nullptr || water == nullptr)
		return false;

	if (animal->RefPoint.x <= (water->RefPoint.x + water->width) &&
		animal->RefPoint.x + animal->width >= water->RefPoint.x &&
		animal->RefPoint.y <= (water->RefPoint.y + water->height) &&
		animal->RefPoint.y + animal->height >= water->RefPoint.y &&
		animalHealth[animalIndex] < 500) return true;
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
	pWind->DrawString(440, y, "Current Animals: " + to_string(animalCount));
	pWind->DrawString(760, y, "Eggs: " + to_string(eggsCount));
	pWind->DrawString(930, y, "Milk: " + to_string(producedMilkCount));
}

void Game::drawbudgetbar() const
{
	pWind->DrawImage(budgetBackgroundImage, 0, 2 * config.toolBarHeight,
		config.windWidth, config.toolBarHeight);

	pWind->SetPen(WHITE, 2);
	pWind->SetFont(18, BOLD, BY_NAME, "Arial");
	int y = 2 * config.toolBarHeight + 20;
	pWind->DrawString(10, y, "Budget: $" + to_string(budget));
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
	pWind->SetPen(WHITE, 1);
	pWind->SetBrush(WHITE);
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
		config.windWidth, config.windHeight - (3 * config.toolBarHeight));
}

void Game::clearStatusBar() const
{
	pWind->DrawImage(budgetBackgroundImage, 0, config.windHeight - config.statusBarHeight, config.windWidth, config.windHeight);

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
	std::uniform_int_distribution<int> yDist(5 * config.toolBarHeight, config.windHeight - config.statusBarHeight - objectHeight - 20);

	point p;
	p.x = xDist(rng);
	p.y = yDist(rng);
	return p;
}

void Game::addRandomAnimal(string animalType, int objectWidth, int objectHeight)
{
	if (animalCount >= 100) return;

	if (animalCount == 0)
		lastWolfSpawnTime = CurrentTime();

	point p = getRandomFieldPoint(objectWidth, objectHeight); // get random point for the animal within the field boundaries

	if (animalType == "chick") {
		Chick* chick = new Chick(this, p, objectWidth, objectHeight, "images\\chick.jpg");//create a new animal object with the random point
		animalList[animalCount++] = chick;//add animal to the animal list and chickAnimals vector (is there to check number of animals since the wolf is in the same family)
		chickAnimals.push_back(chick);
		animalHealth.push_back(animalsHealthBar);

	}
	else if (animalType == "cow") {
		Cow* cow = new Cow(this, p, objectWidth, objectHeight, "images\\cow.jpg");
		animalList[animalCount++] = cow;
		cowAnimals.push_back(cow);
		animalHealth.push_back(animalsHealthBar);
	}
}
void Game::displayprices() {
	pWind->DrawString(10, config.toolBarHeight + 30,"100$");
	pWind->DrawString(2*config.toolbarItemWidth, config.toolBarHeight + 30, "200$");
	pWind->DrawString(4 * config.toolbarItemWidth, config.toolBarHeight + 30, "100$");
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
	srand(static_cast<unsigned int>(time(0)));
	randomNumE.push_back((rand() % 10) + 1);
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

static long spawnInterval = 18000; // static value for the spawn interval

void Game::generateRandomWolf()
{
	int maxWolves;
	if (level == 1) {
		maxWolves = 1;
		spawnInterval = 18000; // 18s
	}
	else if (level == 2) {
		maxWolves = 2;
		spawnInterval = 15000; // 15s
	}
	else if (level == 3) {
		maxWolves = 3;
		spawnInterval = 12000; // 12s
	}
	else {
		maxWolves = 4;
		spawnInterval = 10000; // 10s
	}

	if (wolfCount >= maxWolves) return;
	point p = getRandomFieldPoint(60, 60);
	wolfHealth.push_back(wolfHealthBar);
	wolfList[wolfCount++] = new Wolf(this, p, 60, 60, "images\\wolf.jpg");
}

void Game::updateAutoWolfGeneration()
{
	if (currentAnimals > 0) {
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
		int randomNumberE = (rand() % 10) + 1; //from 1-10 to show the feature but in general i want it from 1-100
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
		
		clearbackground();
		updatetimer();
		click = getMouseClick(x, y);
		if (click != NO_CLICK && y >= 0 && y < config.toolBarHeight) {
			gameToolbar->handleClick(x, y);
		}
		if (click != NO_CLICK && y >= config.toolBarHeight && y < 2 * config.toolBarHeight) {
			gameBudgetbar->handleClick(x, y);
		}
		if (click != NO_CLICK && y >= 2 * config.toolBarHeight) {
			if (waterCounter == 1) {
				point p;
				p.x = x - (30);
				p.y = y - (30);
				waterList[waterCount++] = new Water(this, p, 60, 60);
				waterCounter = 0;
				waterHealth.push_back(waterHealthBar);
			}
			window* ptWind = CreateWind(x, y);
			if (ptWind != nullptr) {
				ptWind->SetBuffering(true);
				//ptWind->SetPen(config.bkGrndColor, 1);
				//ptWind->SetBrush(config.bkGrndColor);
				//ptWind->DrawRectangle(0, 0, ptWind->GetWidth(), ptWind->GetHeight(), FILLED);
				//ptWind->ChangeTitle("warehouse");

				//ptWind->SetPen(BLACK, 1);
				//ptWind->SetFont(30, BOLD, BY_NAME, "Arial");
				//ptWind->DrawString(20, 20, "EGGS");
				//ptWind->SetFont(15, BOLD, BY_NAME, "Arial");
				//ptWind->DrawString(20, 50, "Price of each Egg 20$");
				//ptWind->DrawString(20, 70, "Eggs: " + to_string(eggsCount));
				//ptWind->SetFont(30, BOLD, BY_NAME, "Arial");
				//ptWind->SetPen(BLACK, 1);
				//ptWind->SetFont(30, BOLD, BY_NAME, "Arial");
				//ptWind->DrawString(20, 220, "MILKS");
				//ptWind->SetFont(15, BOLD, BY_NAME, "Arial");
				//ptWind->DrawString(20, 250, "Price of each Milk 20$");
				//ptWind->DrawString(20, 270, "Milk: " + to_string(producedMilkCount));
				//ptWind->SetFont(30, BOLD, BY_NAME, "Arial");

				//ptWind->SetPen(WHITE, 1);
				//ptWind->SetBrush(GREEN);
				//ptWind->DrawRectangle(160, 20, 280, 80, FILLED);
				//ptWind->SetPen(WHITE, 1);
				//ptWind->SetFont(24, BOLD, BY_NAME, "Arial");
				//ptWind->DrawString(185, 35, "SELL");

				//ptWind->SetBrush(GREEN);
				//ptWind->DrawRectangle(160, 220, 280, 270, FILLED);
				//ptWind->SetPen(WHITE, 1);
				//ptWind->SetFont(24, BOLD, BY_NAME, "Arial");
				//ptWind->DrawString(200, 240, "SELL");

				//ptWind->SetPen(BLACK, 1);
				//ptWind->SetFont(30, BOLD, BY_NAME, "Arial");
				//ptWind->DrawString(110, 450, "Press ESCAPE to leave");

				//ptWind->UpdateBuffer();
				ptWind->DrawImage(warehouse,0,0,ptWind->GetWidth(), ptWind->GetHeight());
				ptWind->ChangeTitle("warehouse");

				ptWind->SetPen(BLACK, 1);
				ptWind->SetFont(35, BOLD, BY_NAME, "Arial");
				ptWind->DrawString(190, 220, "Eggs:" + to_string(eggsCount));
				ptWind->DrawString(190, 370, "Milk:" + to_string(producedMilkCount));
				ptWind->UpdateBuffer();

				bool warehouseOpen = true;
				while (warehouseOpen) {
					click2 = ptWind->GetMouseClick(x1, y1);

					if (click2 != NO_CLICK && x1 >= 340 && x1 <= 410 && y1 >= 165 && y1 <= 215) {
						budget += (eggsCount * 20);
						eggsCount = 0;
						ptWind->DrawImage(warehouse, 0, 0, ptWind->GetWidth(), ptWind->GetHeight());
						ptWind->SetPen(BLACK, 1);
						ptWind->SetFont(35, BOLD, BY_NAME, "Arial");
						ptWind->DrawString(190, 220, "Eggs:" + to_string(eggsCount));
						ptWind->DrawString(190, 370, "Milk:" + to_string(producedMilkCount));
						ptWind->UpdateBuffer();
					}
					if (click2 != NO_CLICK && x1 >= 340 && x1 <= 410 && y1 >= 325 && y1 <= 375) {
						budget += (producedMilkCount * 20);
						producedMilkCount = 0;
						ptWind->DrawImage(warehouse, 0, 0, ptWind->GetWidth(), ptWind->GetHeight());
						ptWind->SetPen(BLACK, 1);
						ptWind->SetFont(35, BOLD, BY_NAME, "Arial");
						ptWind->DrawString(190, 220, "Eggs:" + to_string(eggsCount));
						ptWind->DrawString(190, 370, "Milk:" + to_string(producedMilkCount));
						ptWind->UpdateBuffer();
					}
					if (click2 != NO_CLICK && x1 >= 110 && x1 <= 410 && y1 >= 400 && y1 <= 500) {
						warehouseOpen = false;
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
				if (eggList[i] == nullptr || eggsCount + milkCount >= maxCapcity)
					continue;

				point eP = eggList[i]->RefPoint;
				if ((y >= eP.y && y <= eP.y + 28) && ((x >= eP.x && x <= eP.x + 20))) {
					if (randomNumberE == randomNumE[i]) {
						delete eggList[i];
						randomNumE.erase(randomNumE.begin() + i);
						for (int j = i; j < eggCount - 1; j++) {
							eggList[j] = eggList[j + 1];
						}
						point newP = eP;
						newP.x - 75;
						newP.y - 75;
						eggList[eggCount - 1] = nullptr;
						eggCount--;
						Chick* chick = new Chick(this, newP, 60, 60, "images\\chick.jpg");
						if (animalCount == 0)
							lastWolfSpawnTime = CurrentTime();
						animalList[animalCount++] = chick;
						animalHealth.push_back(animalsHealthBar);
						currentAnimals++;
						chickAnimals.push_back(chick);
						break;
					}
					delete eggList[i];
					randomNumE.erase(randomNumE.begin() + i);
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
				if (milkList[i] == nullptr || eggCount + producedMilkCount >= maxCapcity)
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

		for (int i = 0; i < waterCount; i++) {
			if (waterList[i] == nullptr)
				continue;
			waterList[i]->draw();
			pWind->SetPen(BLACK, 1);
			pWind->SetFont(24, BOLD, BY_NAME, "Arial");
			pWind->DrawString(waterList[i]->RefPoint.x + 25, waterList[i]->RefPoint.y + 20, to_string(waterHealth[i]));
			for (int j = 0; j < animalCount; j++) {
				if (animalList[j] == nullptr)
					continue;

				if (CheckFood(animalList[j], waterList[i] , j))
				{
					waterHealth[i]--;
					if (waterHealth[i] <= 0) {
						delete waterList[i];
						waterHealth.erase(waterHealth.begin() + i);
						for (int k = i; k < waterCount - 1; k++)
						{
							waterList[k] = waterList[k + 1];
						}
						drawstatusbar();
						waterList[waterCount - 1] = nullptr;
						waterCount--;
						//foodCount++;

						i--;
						break;
					}
					drawstatusbar();
					animalList[j]->productIntervalMs += 5000;
					animalHealth[j] = 1000;
					pWind->SetPen(GREEN, 1);
					pWind->SetBrush(GREEN);
					pWind->DrawRectangle(waterList[i]->RefPoint.x, waterList[i]->RefPoint.y, waterList[i]->RefPoint.x + 60, waterList[i]->RefPoint.y + 60, FILLED);
					pWind->SetPen(BLACK, 1);
					pWind->SetFont(24, BOLD, BY_NAME, "Arial");
					pWind->DrawString(waterList[i]->RefPoint.x + 25, waterList[i]->RefPoint.y + 20, to_string(waterHealth[i]));
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

			if (animalHealth[i] <= 0) {
				delete animalList[i];
				animalHealth.erase(animalHealth.begin() + i);

				for (int j = i; j < animalCount - 1; j++) {
					animalList[j] = animalList[j + 1];
				}

				animalList[animalCount - 1] = nullptr;
				animalCount--;
				if (currentAnimals > 0)
					currentAnimals--;
				i--;
				continue;
			}

			if (!paused) {
				animalHealth[i]--;
				animalList[i]->moveStep(animalHealth[i] >= 500);
			}
				
				
			animalList[i]->draw();
			pWind->SetPen(BLACK, 1);

			if (animalHealth[i] > 500) 
				pWind->SetBrush(GREEN);
			else if (animalHealth[i] > 200)
				pWind->SetBrush(YELLOW);
			else
				pWind->SetBrush(RED);
			pWind->DrawCircle(animalList[i]->RefPoint.x +(animalList[i]->width + 10), (animalList[i]->RefPoint.y + 10), 8, FILLED);

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
					animalHealth.erase(animalHealth.begin() + j);
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
		drawstatusbar();
		
		pWind->UpdateBuffer();
		displayprices();
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
	randomNumE.clear();
	waterHealth.clear();

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
void Game::playCowSound()
{
	mciSendString("open \"sounds\\cow.wav\" type waveaudio alias cows", NULL, 0, NULL);
	mciSendString("play cows", NULL, 0, NULL);
	
}
void Game::playChickSound()
{
	mciSendString("open \"sounds\\chick.wav\" type waveaudio alias chicks", NULL, 0, NULL);
	mciSendString("play chicks", NULL, 0, NULL);
}
