#pragma once
#include "../Core/Drawable.h"
#include <string>
using std::string;

class Animal :public Drawable
{
protected:
	string image_path;
public:
	point curr_pos;
	point curr_vel;
	int velocityFramesLeft;
	long lastProductTime;
	int productIntervalMs;
	string animalType;
	Animal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void moveStep() = 0;
	
};

class Chick : public Animal
{
public:
	Chick(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void moveStep();
};

class Cow : public Animal
{
public:
	Cow(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void moveStep();
};

class Wolf : public Animal
{
public:
	Wolf(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void moveStep();
};

class Egg : public Drawable
{
public:
	Egg(Game* r_pGame, point r_point, int r_width, int r_height);
	virtual void draw() const override;
};

class Milk : public Drawable
{
public:
	Milk(Game* r_pGame, point r_point, int r_width, int r_height);
	virtual void draw() const override;
};

class Warehouse : public Drawable
{
public:
	Warehouse(Game* r_pGame, point r_point, int r_width, int r_height);
	virtual void draw() const override;
};

class Water : public Drawable
{
public:
	Water(Game* r_pGame, point r_point, int r_width, int r_height);
	virtual void draw() const override;
};
