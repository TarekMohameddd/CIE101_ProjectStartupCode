#include "Toolbar.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include "../CMUgraphicsLib/auxil.h"

ToolbarIcon::ToolbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
}

void ToolbarIcon::draw() const
{
	//draw image of this object
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
}
PauseIcon::PauseIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {
}

void PauseIcon::onClick()
{
	pGame->paused = true;
	pGame->pauseStartTime = CurrentTime();
	pGame->pauseBackgroundMusic();
}
ResumeIcon::ResumeIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {
}

void ResumeIcon::onClick()
{
	long pausedDuration = CurrentTime() - pGame->pauseStartTime;
	pGame->adjustProductionTimersAfterPause(pausedDuration);
	pGame->paused = false;
	pGame->resumeBackgroundMusic();
}
SaveIcon::SaveIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {
}

void SaveIcon::onClick()
{
	pGame->printMessage("Save clicked");
}
LoadGameIcon::LoadGameIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path) {
}
void LoadGameIcon::onClick()
{
	pGame->printMessage("Loaded");
}
void RestartIcon::onClick()
{
	pGame->restartGame();
}

RestartIcon::RestartIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{}


ExitIcon::ExitIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : ToolbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{}

void ExitIcon::onClick()
{
	pGame->printMessage("Exit");
	pGame->isExit = true;
}

Toolbar::Toolbar(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
	// each icon uses one image from the images folder
	iconsImages[ICON_RESTART] = "images\\RESTART.jpeg";
	iconsImages[ICON_PAUSE] = "images\\PAUSE.jpeg";
	iconsImages[ICON_RESUME] = "images\\PLAY.jpeg";
	iconsImages[ICON_SAVE] = "images\\SAVE.jpeg";
	iconsImages[ICON_LOAD] = "images\\LOAD.jpeg";
	iconsImages[ICON_EXIT] = "images\\toolbar_Exit.jpg";
	point p;
	p.x = 0;
	p.y = 0;

	iconsList = new ToolbarIcon * [ICON_COUNT];

	// create one object for each toolbar icon
	iconsList[ICON_RESTART] = new RestartIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_RESTART]);
	p.x += config.iconWidth;

	iconsList[ICON_PAUSE] = new PauseIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_PAUSE]);
	p.x += config.iconWidth;

	iconsList[ICON_RESUME] = new ResumeIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_RESUME]);
	p.x += config.iconWidth;

	iconsList[ICON_SAVE] = new SaveIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_SAVE]);
	p.x += config.iconWidth;
	iconsList[ICON_LOAD] = new LoadGameIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_LOAD]);

	p.x += config.iconWidth;

	iconsList[ICON_EXIT] = new ExitIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_EXIT]);
}

Toolbar::~Toolbar()
{
	for (int i = 0; i < ICON_COUNT; i++)
		delete iconsList[i];
	delete[] iconsList;
}

void Toolbar::draw() const
{
	for (int i = 0; i < ICON_COUNT; i++)
		iconsList[i]->draw();
	window* pWind = pGame->getWind();
	pWind->SetPen(BLACK, 3);
	pWind->DrawLine(0, config.toolBarHeight, pWind->GetWidth(), config.toolBarHeight);
}

bool Toolbar::handleClick(int x, int y)
{
	if (x > ICON_COUNT * config.iconWidth)	//click outside toolbar boundaries
		return false;


	//Check whick icon was clicked
	//==> This assumes that menu icons are lined up horizontally <==
	//Divide x co-ord of the point clicked by the icon width (int division)
	//if division result is 0 ==> first icon is clicked, if 1 ==> 2nd icon and so on

	int clickedIconIndex = (x / config.iconWidth);
	iconsList[clickedIconIndex]->onClick();	//execute onClick action of clicled icon

	if (clickedIconIndex == ICON_EXIT) return true;

	return false;

}

