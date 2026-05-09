#pragma once
#include <vector>
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../Entities/Animal.h"

class Game
{
private:
	window* pWind;
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
	image backgroundImage;
	long gameStartTime;
	long lastWolfSpawnTime;
	long lastProductionUpdateTime;
	point foodAreaTopLeft;
	point foodAreaBottomRight;
	int foodCount = 0;
	bool foodAreaVisible = true;
	std::vector<Chick*> chickAnimals;
	std::vector<long> chickTimers;
	std::vector<Cow*> cowAnimals;
	std::vector<long> cowTimers;
	std::vector<int> wolfHealth;
	// moved eggsCount and producedMilkCount to public to allow external access from UI
	Animal* animalList[100] = {};
	Animal* wolfList[5] = {};
	int animalCount = 0;
	Egg* eggList[100] = {};
	int eggCount = 0;
	Milk* milkList[100] = {};
	int milkCount = 0;
	Water* waterList[100] = {};
	int waterCount = 0;
	int wolfCount = 0;
	int wolfHealthBar = 5;
	void updateOneSecond();
	void updateAnimalProduction(long deltaMs);
	void clearDynamicObjects();

	//bool CheckFood(Animal* animal , Water* water) const;
	void drawWarehouse() const;

public:
	int budget = 2000;
	int timer = 120;
	int goal = 5;
	int level = 1;
	int score = 0;
	int currentAnimals = 0;
	int animalBuyingPrice = 200;
	int waterBuyingPrice = 100;
	int waterCounter = 0;
	bool paused = false;
	bool restart = false;
	bool isExit = false;
	int eggsCount = 0;
	int producedMilkCount = 0;
	int windEgg = 0;
	int windMilk = 0;
	Game();
	~Game();

	window* CreateWind(int x, int y);

	clicktype getMouseClick(int& x, int& y) const;
	string getSrting() const;
	window* CreateWind(int, int, int, int) const;
	void createToolbar();
	void drawFoodArea() const;
	void consumeFood(int amount);
	bool CheckFood(Animal* animal, Water* water) const;
	void createBudgetbar();
	void clearBudget() const;
	void drawstatusbar() const;
	void drawbudgetbar() const;
	void resetgame();
	void restartGame();
	void updatetimer();
	void registerChickPurchased();
	void registerCowPurchased();
	point getRandomFieldPoint(int objectWidth, int objectHeight) const;
	void addRandomAnimal(string animalType, int objectWidth, int objectHeight);
	void addRandomWaterArea();
	void addEgg(point p);
	void addMilk(point p);
	void generateRandomWolf();
	void updateAutoWolfGeneration();
	void printBudget(string msg) const;
	void clearStatusBar() const;
	void clearbackground() const;
	void printMessage(string msg) const;
	void go();
	window* getWind() const;
};
