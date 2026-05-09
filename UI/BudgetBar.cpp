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
	if (count < 15 && pGame->budget >= pGame->animalBuyingPrice) {
		pGame->budget -= pGame->animalBuyingPrice;
		pGame->currentAnimals++;
		pGame->drawbudgetbar();
		pGame->drawstatusbar();
		pGame->addRandomChick();
		count++;
		pGame->registerChickPurchased();
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
	if (count < 15 && pGame->budget >= pGame->animalBuyingPrice) {
		pGame->budget -= pGame->animalBuyingPrice;
		pGame->currentAnimals++;
		pGame->drawbudgetbar();
		pGame->drawstatusbar();
		pGame->addRandomCow();
		pGame->registerCowPurchased();
		count++;
	}
}

WaterIcon::WaterIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
}

void WaterIcon::onClick()
{
	if (pGame->budget >= pGame->waterBuyingPrice) {
		pGame->budget -= pGame->waterBuyingPrice;
		pGame->drawbudgetbar();
		pGame->addRandomWaterArea();
	}
}

Budgetbar::Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
	iconsImages[ICON_CHICK] = "images\\chick.jpg";
	iconsImages[ICON_COW] = "images\\cow.jpg";
	iconsImages[ICON_WATER] = "images\\water.jpg";

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
	pWind->SetPen(BLACK, 1);
	pWind->SetBrush(BLACK);
	pWind->DrawRectangle(0, config.toolBarHeight, pWind->GetWidth(), 2 * config.toolBarHeight);

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
