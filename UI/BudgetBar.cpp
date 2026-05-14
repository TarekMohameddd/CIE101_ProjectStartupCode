#include "Budgetbar.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include "../CMUgraphicsLib/colors.h"

BudgetbarIcon::BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
	iconSprite.Open(img_path);
}

void BudgetbarIcon::draw() const
{
	window* pWind = pGame->getWind();
	pWind->DrawImage(iconSprite, RefPoint.x, RefPoint.y, width, height);
}

ChickIcon::ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
	chickList = new Chick * [15];
	for (int i = 0; i < 15; i++) {
		chickList[i] = nullptr;
	}
}

void ChickIcon::onClick()
{	
	if (!pGame->paused &&count < 15 && pGame->budget >= pGame->chickBuyingPrice) { 
		pGame->budget -= pGame->chickBuyingPrice; 
		pGame->currentAnimals++;
		pGame->drawbudgetbar();
		pGame->drawstatusbar();
		pGame->addRandomAnimal("chick", 60, 60);
		count++;
		pGame->registerChickPurchased();	
		pGame->playChickSound();
	}
	
}

CowIcon::CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
	cowList = new Cow * [15];
	for (int i = 0; i < 15; i++) {
		cowList[i] = nullptr;
	}
}

void CowIcon::onClick()
{
	if (!pGame->paused &&count < 15 && pGame->budget >= pGame->cowBuyingPrice) {
		pGame->budget -= pGame->cowBuyingPrice;
		pGame->currentAnimals++;
		pGame->drawbudgetbar();
		pGame->drawstatusbar();
		pGame->addRandomAnimal("cow", 60, 60); 
		pGame->registerCowPurchased();
		pGame->playCowSound();
		count++;
	}
	
}

WaterIcon::WaterIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
}

void WaterIcon::onClick()
{
	if (!pGame->paused && pGame->budget >= pGame->waterBuyingPrice) {
		if (pGame->waterCounter != 0) return;
		pGame->budget -= pGame->waterBuyingPrice;
		pGame->drawbudgetbar();
		//pGame->addRandomWaterArea();
		pGame->waterCounter++;
	}
}

Budgetbar::Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
	iconsImages[ICON_CHICK] = "images\\chick.jpeg";
	iconsImages[ICON_COW] = "images\\cow.jpeg";
	iconsImages[ICON_WATER] = "images\\water.jpeg";

	point p;
	p.x = 0;
	p.y = config.toolBarHeight;

	iconsList = new BudgetbarIcon * [ANIMAL_COUNT];

	iconsList[ICON_CHICK] = new ChickIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_CHICK]);
	p.x += config.iconWidth;
	iconsList[ICON_COW] = new CowIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_COW]);
	p.x += config.iconWidth;
	iconsList[ICON_WATER] = new WaterIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_WATER]);
}

Budgetbar::~Budgetbar()
{
	for (int i = 0; i < ANIMAL_COUNT; i++)
		delete iconsList[i];
	delete[] iconsList;
}

void Budgetbar::draw() const
{
	window* pWind = pGame->getWind();

	for (int i = 0; i < ANIMAL_COUNT; i++)
		iconsList[i]->draw();
	pWind->SetPen(BLACK, 3);
	pWind->DrawLine(0, 2 * config.toolBarHeight, pWind->GetWidth(), 2 * config.toolBarHeight);
}

bool Budgetbar::handleClick(int x, int y)
{
	if (x >= ANIMAL_COUNT * config.iconWidth)
		return false;

	int clickedIconIndex = x / config.iconWidth;
	iconsList[clickedIconIndex]->onClick();
	return false;
}

void Budgetbar::resetAnimals()
{
	ChickIcon* chickIcon = dynamic_cast<ChickIcon*>(iconsList[ICON_CHICK]);
	CowIcon* cowIcon = dynamic_cast<CowIcon*>(iconsList[ICON_COW]);

	if (chickIcon != nullptr) {
		for (int i = 0; i < chickIcon->count; i++) {
			delete chickIcon->chickList[i];
			chickIcon->chickList[i] = nullptr;
		}
		chickIcon->count = 0;
	}

	if (cowIcon != nullptr) {
		for (int i = 0; i < cowIcon->count; i++) {
			delete cowIcon->cowList[i];
			cowIcon->cowList[i] = nullptr;
		}
		cowIcon->count = 0;
	}
}
