#pragma once
#include "../Core/Drawable.h"
#include "../Entities/Animal.h"
#include "../Config/GameConfig.h"

class BudgetbarIcon :public Drawable
{
public:
	string image_path;
	image iconSprite;
	BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void onClick() = 0;
};

class ChickIcon : public BudgetbarIcon
{
public:
	Chick** chickList;
	int count = 0;
	ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};

class CowIcon : public BudgetbarIcon
{
public:
	Cow** cowList;
	int count = 0;
	CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};

class WaterIcon : public BudgetbarIcon
{
public:
	WaterIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};

enum ANIMAL_ICONS
{
	ICON_CHICK,
	ICON_COW,
	ICON_WATER,
	ANIMAL_COUNT
};

class Budgetbar : public Drawable
{
public:
	BudgetbarIcon** iconsList;
	string iconsImages[ANIMAL_COUNT];

	Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height);
	~Budgetbar();
	void draw() const override;
	bool handleClick(int x, int y);
	void resetAnimals();
};
