/**
 * @file dinorun.h
 * @author Reyadath Ullah (SH - 33)
 * @brief An infinite runner game
 * @version 0.1
 * @date 30-05-2021
 * @copyright Copyright (c) 2021
 */

#ifndef DINORUN_H
#define DINORUN_H

#include "texture_timer.h"

bool loadDinoMedia();
void renderMapDino();
void spawnObjects();
bool checkCollisionDino(SDL_Rect player, SDL_Rect object);
void initVariableDino();
void freeDino();
MENU_OPTIONS showDinoHighScore(SDL_Event &e);
MENU_OPTIONS showDinoScore(SDL_Event &e, std::string username);
MENU_OPTIONS handleDinoUI(SDL_Event &e);
MENU_OPTIONS dinoHelpMenuUI(SDL_Event &e);


struct Player
{
	Texture mPlayerTexture;

	const int PLAYER_WIDTH = 59;
	const int PLAYER_HEIGHT = 65;
	const int PLAYER_VEL = 80;
	const int PLAYER_SETP = 10;
	const int JUMP_VEL = 240; //use 200
	const int JUMP_STEP = 14;
	const int GRAVITY = 8;
	const int SPRITE_COUNT = 6;
	const int SPRITE_SPEED = 8;
	const int BASE_HEIGHT = 570;

	int mPosX, mPosY;
	int mVelX, mVelY;
	int mCurSprite = 0;

	SDL_Rect *mPlayerSprite;

	SDL_RendererFlip mFlipType = SDL_FLIP_NONE;

	bool mJumping = false;

	Player()
	{
		mPosX = 300, mPosY = BASE_HEIGHT - PLAYER_HEIGHT;
		mVelX = 0, mVelY = 0;
		mPlayerSprite = (SDL_Rect *)malloc(SPRITE_COUNT * sizeof(SDL_Rect));

		for (int i = 0; i < SPRITE_COUNT; i++)
		{
			mPlayerSprite[i].w = PLAYER_WIDTH;
			mPlayerSprite[i].h = PLAYER_HEIGHT;
			mPlayerSprite[i].x = i * PLAYER_WIDTH;
			mPlayerSprite[i].y = 0;
		}
	}
	~Player()
	{
		free();
	}
	void handleEvent(SDL_Event &e)
	{
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			switch (e.ksym)
			{
			case SDLK_w:
				if (mPosY < BASE_HEIGHT - PLAYER_HEIGHT - 120)
					return;
				mVelY -= JUMP_VEL;
				mFlipType = SDL_FLIP_NONE;
				mJumping = true;
				break;
			case SDLK_a:
				mVelX -= PLAYER_VEL;
				mFlipType = SDL_FLIP_HORIZONTAL;
				break;
			case SDLK_d:
				mVelX += PLAYER_VEL;
				mFlipType = SDL_FLIP_NONE;
				break;
			}
		}
	}
	void gravity()
	{
		if (!mJumping)
		{
			mPosY = std::min(BASE_HEIGHT - PLAYER_HEIGHT, mPosY + GRAVITY);
		}
	}
	void move()
	{
		if (mJumping)
		{
			mPosY -= JUMP_STEP;
			mVelY += JUMP_STEP;
			if (mVelY >= 0)
				mVelY = 0, mJumping = false;
		}
		if (mVelX > 0)
		{
			mPosX += PLAYER_SETP;
			mVelX -= PLAYER_SETP;
			if (mPosX < 0 || mPosX + PLAYER_WIDTH > SCREEN_WIDTH)
				mPosX -= PLAYER_SETP;
		}
		else if (mVelX < 0)
		{
			mPosX -= PLAYER_SETP;
			mVelX += PLAYER_SETP;
			if (mPosX < 0 || mPosX + PLAYER_WIDTH > SCREEN_WIDTH)
				mPosX += PLAYER_SETP;
		}
	}
	void spriteChanger()
	{
		mCurSprite++;
		if (mCurSprite / SPRITE_SPEED >= SPRITE_COUNT)
			mCurSprite = 0;
	}
	void render()
	{
		gravity();
		mPlayerTexture.render(mPosX, mPosY, &mPlayerSprite[mCurSprite / SPRITE_SPEED], mFlipType);
		if (mPosY == BASE_HEIGHT - PLAYER_HEIGHT)
			spriteChanger();
	}
	void free()
	{
		mPlayerTexture.free();
	}
};

enum BACKGROUND_LAYER
{
	BUSH_LAYER_1,
	GROUND_LAYER_2,
	TREE1_LAYER_3,
	TREE2_LAYER_4,
	TREE3_LAYER_5,
	STAR_LAYER_6,
	MOON_LAYER_7,
	LAYER_COUNT
};
enum BLOCK_TYPE
{
	BLOCK_T1,
	BLOCK_T2,
	BLOCK_T3,
	BLOCK_COUNT
};

int gGroundScroll = 0;
int gTree1Scroll = 0;
int gTree2Scroll = 0;
int gTree3Scroll = 0;
int gStarScroll = 0;
int gStarDecrease = 1;
int gSpawnInterval = 0;
int gCurColAnimation = 0;
int gBlockSpawnGap = 600;
int gScoreDelay = 0;
int TOTAL_SCORE = 0;
int TOTAL_HEALTH = 3;

bool gIfResumeDino = false;

Uint32 gPrevTime;

const int gCurCollisionDelay = 10;
const int gGroundSpeed = 6;
const int gTree1Speed = 5;
const int gTree2Speed = 4;
const int gTree3Speed = 2;
const int gStarSpeed = 1;
const int gColAnimationCount = 5;
const int gDinoButtonCount = 4;

Player gMyPlayer; //Player object

Texture gBgTextureDino[LAYER_COUNT];
Texture gBlockTexture[BLOCK_COUNT];
Texture gHealthTexture;
Texture gCollisionTexture;
Texture gDinoScoreTexture;
Texture gDinoButtonTexture[gDinoButtonCount];
Texture gDinoMenuTexture;
Texture gDinoBackTexture;
Texture gDinoHighscoreTexture;
Texture gDinoTutorialTexture;

SDL_Rect gCollisionRect[gColAnimationCount];
SDL_Rect gDinoButtonPosition[gDinoButtonCount];
SDL_Rect gDinoBackButtonPosition;
SDL_Rect gDinoHighscorePosition;

std::vector<std::pair<int, BLOCK_TYPE>> gBlockPos; //storing block x and block type
std::vector<int> gBlockIfCollieded;
std::vector<int> gHealthPos;

Timer gDinoTimer;

Mix_Chunk *gCollisionSound = NULL;

bool loadDinoMedia()
{
	if (!gMyPlayer.mPlayerTexture.loadFile("images/dino/dino_runner.png"))
		return false;
	if (!gBgTextureDino[BUSH_LAYER_1].loadFile("images/dino/dino_bush.png"))
		return false;
	if (!gBgTextureDino[GROUND_LAYER_2].loadFile("images/dino/dino_ground.png"))
		return false;
	if (!gBgTextureDino[TREE1_LAYER_3].loadFile("images/dino/dino_tree1.png"))
		return false;
	if (!gBgTextureDino[TREE2_LAYER_4].loadFile("images/dino/dino_tree3.png"))
		return false;
	if (!gBgTextureDino[TREE3_LAYER_5].loadFile("images/dino/dino_tree3.png"))
		return false;
	if (!gBgTextureDino[STAR_LAYER_6].loadFile("images/dino/dino_star.png"))
		return false;
	if (!gBgTextureDino[MOON_LAYER_7].loadFile("images/dino/dino_moon.png"))
		return false;
	if (!gBlockTexture[BLOCK_T1].loadFile("images/dino/block1.png"))
		return false;
	if (!gBlockTexture[BLOCK_T2].loadFile("images/dino/block2.png"))
		return false;
	if (!gBlockTexture[BLOCK_T3].loadFile("images/dino/block3.png"))
		return false;
	if (!gCollisionTexture.loadFile("images/dino/collision_animation.png"))
		return false;
	if (!gHealthTexture.loadFile("images/health.png"))
		return false;
	if (!gDinoButtonTexture[PLAY].loadFile("images/main/play_button.png"))
		return false;
	if (!gDinoButtonTexture[CONTINUE].loadFile("images/main/continue_button.png"))
		return false;
	if (!gDinoButtonTexture[HELP].loadFile("images/main/help_button.png"))
		return false;
	if (!gDinoButtonTexture[EXIT].loadFile("images/main/exit_button.png"))
		return false;
	if (!gDinoMenuTexture.loadFile("images/dino/menu_back.png"))
		return false;
	if (!gDinoBackTexture.loadFile("images/main/back_button.png"))
		return false;
	if (!gDinoHighscoreTexture.loadFile("images/main/highscore_button.png"))
		return false;
	if (!gDinoTutorialTexture.loadFile("images/dino/tutorial_dino.png"))
		return false;
	gCollisionTexture.setBlendMode(SDL_BLENDMODE_ADD);

	gFont = TTF_OpenFont("images/fonts/Oswald-Medium.ttf", 21);
	if (gFont == NULL)
	{
		ERROR_T;
		return false;
	}

	gCollisionSound = Mix_LoadWAV("sounds/collision.wav");
	if (gCollisionSound == NULL)
	{
		ERROR_M;
		return false;
	}

	//initializing collision animation sprite
	const int tWidth = 60;
	const int tHeight = 60;
	for (int i = 0, j = 0; i < gColAnimationCount; i++)
	{
		gCollisionRect[i].x = (i % 5) * tWidth;
		gCollisionRect[i].y = j * tHeight;
		gCollisionRect[i].w = tWidth;
		gCollisionRect[i].h = tHeight;

		if (i % 5 == 0)
			j++;
	}

	for (int i = 0; i < gDinoButtonCount; i++)
	{
		gDinoButtonPosition[i].x = SCREEN_WIDTH / 3 - 350;
		gDinoButtonPosition[i].y = (i - 1) * (i != 0) * 100 + 100;
		gDinoButtonPosition[i].w = 309;
		gDinoButtonPosition[i].h = 55;
	}
	gDinoHighscorePosition.x = SCREEN_WIDTH / 3 - 350;
	gDinoHighscorePosition.y = 3 * 100 + 100;
	gDinoHighscorePosition.w = 309;
	gDinoHighscorePosition.h = 55;

	gDinoBackButtonPosition.x = 0;
	gDinoBackButtonPosition.y = 0;
	gDinoBackButtonPosition.w = gDinoBackTexture.getWidth();
	gDinoBackButtonPosition.h = gDinoBackTexture.getHeight();

	return true;
}

void freeDino()
{
	SDL_RenderClear(gRender);
	for (int i = 0; i < LAYER_COUNT; i++)
		gBgTextureDino[i].free();
	for (int i = 0; i < BLOCK_COUNT; i++)
		gBlockTexture[i].free();
	for (int i = 0; i < gDinoButtonCount; i++)
		gDinoButtonTexture[i].free();
	gHealthTexture.free();
	gCollisionTexture.free();
}

//checks collision between two objects
bool checkCollisionDino(SDL_Rect player, SDL_Rect object)
{
	bool tFlag = true;
	player.w -= 10, player.h -= 10;
	if (player.x + player.w <= object.x)
		tFlag = false;
	if (player.x >= object.x + object.w)
		tFlag = false;
	if (player.y + player.h <= object.y)
		tFlag = false;
	if (player.y >= object.y + object.h)
		tFlag = false;
	return tFlag;
}


void spawnObjects()
{
	//Increasing Difficulty and spawing health points
	Uint32 curTime = gDinoTimer.getTicks() / 1000;
	if (curTime % 15 == 0 && curTime != gPrevTime)
	{
		printf("Time : %d\n", curTime);
		gPrevTime = curTime;
		gBlockSpawnGap = std::max(100, gBlockSpawnGap - 10);

		int tX = SCREEN_WIDTH;
		bool if_collision = false;
		for (int i = 0; i < gBlockPos.size(); i++)
		{
			if (abs(gBlockPos[i].first - tX) < 100)
			{
				if_collision = true;
				break;
			}
		}
		if (!if_collision)
			gHealthPos.push_back(tX);
	}

	//clear blocks out of screen
	std::vector<std::pair<int, BLOCK_TYPE>> tBlocks;
	std::vector<int> tIfCollieded;
	std::vector<int> tHealth;

	for (int i = 0; i < gHealthPos.size(); i++)
	{
		if (gHealthPos[i] >= -gHealthTexture.getWidth())
			tHealth.push_back(gHealthPos[i]);
	}
	gHealthPos = tHealth;

	for (int i = 0; i < gBlockPos.size(); i++)
	{
		if (gBlockPos[i].first < -gBlockTexture[gBlockPos[i].second].getWidth())
		{
			gBlockPos[i].first = -1;
		}
		else
			tBlocks.push_back(gBlockPos[i]), tIfCollieded.push_back(gBlockIfCollieded[i]);
	}
	gBlockPos = tBlocks;
	gBlockIfCollieded = tIfCollieded;

	int random_intervals[] = {57, 91, 12, 87, 173, 273, 182, 753};

	if (gSpawnInterval++ % random_intervals[rand() % 8] == 0 && (int)gBlockPos.size() < 50)
	{
		int tWidth = SCREEN_WIDTH + 400;
		int tBlock = rand() % BLOCK_COUNT;
		bool if_collision = false;

		for (int i = 0; i < (int)gBlockPos.size(); i++)
		{
			if (abs(tWidth - gBlockPos[i].first) < gBlockSpawnGap)
			{
				if_collision = true;
				break;
			}
		}
		if (!if_collision)
		{
			gBlockPos.push_back(std::make_pair(tWidth, (BLOCK_TYPE)tBlock));
			gBlockIfCollieded.push_back(false);
		}

		// gSpawnInterval %= 14000;
	}
}

void renderMapDino()
{
	spawnObjects(); //spawing new blocks

	SDL_SetRenderDrawColor(gRender, 0, 0, 0, 155);
	SDL_RenderClear(gRender);

	gGroundScroll -= gGroundSpeed;
	if (gGroundScroll < -SCREEN_WIDTH)
		gGroundScroll = 0;
	gTree1Scroll -= gTree1Speed;
	if (gTree1Scroll < -SCREEN_WIDTH)
		gTree1Scroll = 0;
	gTree2Scroll -= gTree2Speed;
	if (gTree2Scroll < -SCREEN_WIDTH)
		gTree2Scroll = 0;
	gTree3Scroll -= gTree3Speed;
	if (gTree3Scroll < -SCREEN_WIDTH)
		gTree3Scroll = 0;
	gStarScroll -= gStarSpeed;
	if (gStarSpeed < -SCREEN_WIDTH)
		gStarScroll = 0;

	gBgTextureDino[MOON_LAYER_7].render(0, 0);
	gBgTextureDino[STAR_LAYER_6].render(gStarScroll, 0);
	gBgTextureDino[STAR_LAYER_6].render(gStarScroll + SCREEN_WIDTH, 0);
	gBgTextureDino[TREE3_LAYER_5].render(gTree3Scroll, 0);
	gBgTextureDino[TREE3_LAYER_5].render(gTree3Scroll + SCREEN_WIDTH, 0);
	gBgTextureDino[TREE2_LAYER_4].render(gTree2Scroll, 0);
	gBgTextureDino[TREE2_LAYER_4].render(gTree2Scroll + SCREEN_WIDTH, 0);
	gBgTextureDino[TREE1_LAYER_3].render(gTree1Scroll, 0);
	gBgTextureDino[TREE1_LAYER_3].render(gTree1Scroll + SCREEN_WIDTH, 0);
	gBgTextureDino[GROUND_LAYER_2].render(gGroundScroll, 0);
	gBgTextureDino[GROUND_LAYER_2].render(gGroundScroll + SCREEN_WIDTH, 0);
	gBgTextureDino[BUSH_LAYER_1].render(gGroundScroll, 0);
	gBgTextureDino[BUSH_LAYER_1].render(gGroundScroll + SCREEN_WIDTH, 0);

	//Rendering blocks on map
	for (int i = 0; i < gBlockPos.size(); i++)
	{
		gBlockPos[i].first -= gGroundSpeed;
		BLOCK_TYPE block = gBlockPos[i].second;
		int posY = gMyPlayer.BASE_HEIGHT - gBlockTexture[block].getHeight();
		int posX = gBlockPos[i].first;
		gBlockTexture[block].render(posX, posY);

		//rendering animation
		if (gBlockIfCollieded[i] > 0)
		{
			gBlockIfCollieded[i] = std::max(0, gBlockIfCollieded[i] - 1);
			gCollisionTexture.render(gBlockPos[i].first, posY, &gCollisionRect[gCurColAnimation / gCurCollisionDelay]);
			if (++gCurColAnimation / gCurCollisionDelay >= gColAnimationCount)
				gCurColAnimation = 0;
		}

		SDL_Rect player = {gMyPlayer.mPosX, gMyPlayer.mPosY, gMyPlayer.PLAYER_HEIGHT, gMyPlayer.PLAYER_WIDTH};
		SDL_Rect object = {gBlockPos[i].first, posY, gBlockTexture[block].getWidth(), gBlockTexture[block].getHeight()};

		//collision detection
		if (checkCollisionDino(player, object) && gBlockIfCollieded[i] == 0)
		{
			TOTAL_HEALTH--;
			Mix_PlayChannel(2, gCollisionSound, 0);
			gBlockIfCollieded[i] = 80;
			printf("%d Collided\n", TOTAL_HEALTH);
		}
	}

	//rendering health points on map
	for (int i = 0; i < gHealthPos.size(); i++)
	{
		gHealthPos[i] -= gGroundSpeed;
		int posY = gMyPlayer.BASE_HEIGHT - gHealthTexture.getHeight();
		gHealthTexture.render(gHealthPos[i], posY);

		SDL_Rect player = {gMyPlayer.mPosX, gMyPlayer.mPosY, gMyPlayer.PLAYER_HEIGHT, gMyPlayer.PLAYER_WIDTH};
		SDL_Rect object = {gHealthPos[i], posY, gHealthTexture.getWidth(), gHealthTexture.getHeight()};
		if (checkCollisionDino(player, object))
		{
			TOTAL_HEALTH = std::min(5, TOTAL_HEALTH + 1);
			gHealthPos[i] = -400;
			printf("%d HEALTH INC\n", TOTAL_HEALTH);
		}
	}

	if (gScoreDelay++ % 5 == 0)
		TOTAL_SCORE += 1;
	//rendering health bar
	for (int i = 0; i < TOTAL_HEALTH; i++)
	{
		gHealthTexture.render(i * gHealthTexture.getWidth(), 0);
	}
	//rendering score bar
	SDL_Color textColor = {255, 180, 0, 255};
	std::stringstream tScoreText;
	tScoreText.str("");
	tScoreText << "Score : " << TOTAL_SCORE;
	gDinoScoreTexture.loadFromText(tScoreText.str().c_str(), textColor);
	gDinoScoreTexture.render(SCREEN_WIDTH - 200, 0);

	gMyPlayer.render();
}

void initVariableDino()
{
	gDinoTimer.start();
	gPrevTime = gDinoTimer.getTicks() / 1000;

	gGroundScroll = 0;
	gTree1Scroll = 0;
	gTree2Scroll = 0;
	gTree3Scroll = 0;
	gStarScroll = 0;
	gStarDecrease = 1;
	gSpawnInterval = 0;
	gCurColAnimation = 0;
	gBlockSpawnGap = 600;
	gScoreDelay = 0;
	gIfResumeDino = false;

	TOTAL_HEALTH = 5;

	gMyPlayer.mPosX = 300;
	gMyPlayer.mPosY = gMyPlayer.BASE_HEIGHT - gMyPlayer.PLAYER_HEIGHT;
	gMyPlayer.mFlipType = SDL_FLIP_NONE;

	gBlockPos.clear();
	gHealthPos.clear();
	gBlockIfCollieded.clear();
}

MENU_OPTIONS handleDinoUI(SDL_Event &e)
{
	bool quit = false;
	while (!quit)
	{
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		// Mouse hover animation on button
		for (int i = 0; i < gDinoButtonCount; i++)
		{
			gDinoButtonTexture[i].setAlpha(255);
			if (mouseX >= gDinoButtonPosition[i].x && mouseX <= gDinoButtonPosition[i].x + gDinoButtonPosition[i].w && mouseY >= gDinoButtonPosition[i].y && mouseY <= gDinoButtonPosition[i].y + gDinoButtonPosition[i].h)
			{
				gDinoButtonTexture[i].setAlpha(200);
			}
		}

		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
				return FULL_EXIT;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				//check mouse_button position
				if (mouseX >= gDinoButtonPosition[PLAY].x && mouseX <= gDinoButtonPosition[PLAY].x + gDinoButtonPosition[PLAY].w && mouseY >= gDinoButtonPosition[PLAY].y && mouseY <= gDinoButtonPosition[PLAY].y + gDinoButtonPosition[PLAY].h)
					return START_GAME;

				if (mouseX >= gDinoButtonPosition[EXIT].x && mouseX <= gDinoButtonPosition[EXIT].x + gDinoButtonPosition[EXIT].w && mouseY >= gDinoButtonPosition[EXIT].y && mouseY <= gDinoButtonPosition[EXIT].y + gDinoButtonPosition[EXIT].h)
					return FULL_EXIT;

				if (mouseX >= gDinoButtonPosition[HELP].x && mouseX <= gDinoButtonPosition[HELP].x + gDinoButtonPosition[HELP].w && mouseY >= gDinoButtonPosition[HELP].y && mouseY <= gDinoButtonPosition[HELP].y + gDinoButtonPosition[HELP].h)
					return HELP_MENU;

				if (mouseX >= gDinoHighscorePosition.x && mouseX <= gDinoHighscorePosition.x + gDinoHighscorePosition.w && mouseY >= gDinoHighscorePosition.y && mouseY <= gDinoHighscorePosition.y + gDinoHighscorePosition.h)
					return SHOW_HIGHSCORE;
			}
		}
		//render UI
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);

		//render UI background and buttons
		gDinoMenuTexture.render(0, 0);
		int st = 0, skip = -1;
		if (gIfResumeDino)
			st = 1;
		else
			skip = 1;

		for (int i = st; i < gDinoButtonCount; i++)
		{
			if (skip != -1 && i == skip)
				continue;
			int pos = i;
			if (i == 1)
				pos = i - st;
			gDinoButtonTexture[i].render(gDinoButtonPosition[pos].x, gDinoButtonPosition[pos].y);
		}
		gDinoHighscoreTexture.render(gDinoHighscorePosition.x, gDinoHighscorePosition.y);
		SDL_RenderPresent(gRender);
	}
	return LOADING_SCREEN;
}

MENU_OPTIONS showDinoScore(SDL_Event &e, std::string username)
{
	bool quit = false;
	Texture tScoreTexture;
	SDL_Color textColor = {255, 180, 0, 255};
	std::stringstream tempText;
	tempText << "Your Score : " << TOTAL_SCORE;
	tScoreTexture.loadFromText(tempText.str().c_str(), textColor);

	//insert score to high score list
	std::vector<std::pair<std::string, int>> scoreList;
	std::ifstream highscoreFile;

	highscoreFile.open("saved_files/dinoscore.score");
	if (!highscoreFile)
	{
		printf("Could not open file\n");
	}
	std::string tStr;
	int tScore;
	while (highscoreFile.eof() == false)
	{
		highscoreFile >> tStr >> tScore;
		if (highscoreFile.eof() == false)
			scoreList.push_back(std::make_pair(tStr, tScore));
	}
	highscoreFile.close();

	std::ofstream highscoreFileOut;
	highscoreFileOut.open("saved_files/dinoscore.score");
	if (!highscoreFileOut)
	{
		printf("Could not open file\n");
	}

	scoreList.push_back(std::make_pair(username, TOTAL_SCORE));
	std::sort(scoreList.begin(), scoreList.end(), comp);
	std::map<std::string, bool> tCheck;

	for (int i = 0; i < std::min(10, (int)scoreList.size()); i++)
	{
		highscoreFileOut << scoreList[i].first << " " << scoreList[i].second << "\n";
	}

	highscoreFileOut.close();
	TOTAL_SCORE = 0;

	while (!quit)
	{
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				return FULL_EXIT;
			}
			else if (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE)
			{
				return LOADING_SCREEN;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (mouseX >= gDinoBackButtonPosition.x && mouseX <= gDinoBackButtonPosition.x + gDinoBackButtonPosition.w && mouseY >= gDinoBackButtonPosition.y && mouseY <= gDinoBackButtonPosition.y + gDinoBackButtonPosition.h)
				{
					return LOADING_SCREEN;
				}
			}
		}
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);
		gDinoMenuTexture.render(0, 0);
		gDinoBackTexture.render(gDinoBackButtonPosition.x, gDinoBackButtonPosition.y);
		tScoreTexture.render(SCREEN_WIDTH / 2 - tScoreTexture.getWidth(), 100);
		SDL_RenderPresent(gRender);
	}
	return FULL_EXIT;
}

MENU_OPTIONS showDinoHighScore(SDL_Event &e)
{
	bool quit = false;
	while (!quit)
	{
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				return FULL_EXIT;
			}
			else if (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE)
			{
				return LOADING_SCREEN;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (mouseX >= gDinoBackButtonPosition.x && mouseX <= gDinoBackButtonPosition.x + gDinoBackButtonPosition.w && mouseY >= gDinoBackButtonPosition.y && mouseY <= gDinoBackButtonPosition.y + gDinoBackButtonPosition.h)
				{
					return LOADING_SCREEN;
				}
			}
		}
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);
		gDinoMenuTexture.render(0, 0);
		gDinoBackTexture.render(gDinoBackButtonPosition.x, gDinoBackButtonPosition.y);

		std::ifstream highscore;
		highscore.open("saved_files/dinoscore.score");
		int position = 1;
		Texture tHighscore;
		tHighscore.loadFromText("User           Score", {255, 188, 0, 255});
		tHighscore.render(200, 0);
		while (highscore.eof() == false)
		{
			std::string str;
			int score;

			highscore >> str >> score;

			// std::cout<<str<<"  "<<score<<"\n";

			if (highscore.eof() == false)
			{
				std::stringstream tempText;
				tempText << position++ << ". " << str << "           " << score;
				tHighscore.loadFromText(tempText.str().c_str(), {255, 255, 255, 255});
				tHighscore.render(200, (position - 1) * 50);
			}
		}
		highscore.close();
		SDL_RenderPresent(gRender);
	}
	return FULL_EXIT;
}

MENU_OPTIONS dinoHelpMenuUI(SDL_Event &e)
{
	bool quit = false;
	while (!quit)
	{
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				return FULL_EXIT;
			}
			else if (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE)
			{
				return LOADING_SCREEN;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if (mouseX >= gDinoBackButtonPosition.x && mouseX <= gDinoBackButtonPosition.x + gDinoBackButtonPosition.w && mouseY >= gDinoBackButtonPosition.y && mouseY <= gDinoBackButtonPosition.y + gDinoBackButtonPosition.h)
				{
					return LOADING_SCREEN;
				}
			}
		}
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);

		//render background
		gDinoTutorialTexture.render(0, 0);
		gDinoBackTexture.render(0,0);
		SDL_RenderPresent(gRender);
	}
	return FULL_EXIT;
}


int dinoRun(std::string username)
{
	if (!loadDinoMedia())
	{
		printf("Failed to load Media(dinorun)\n");
		return -1;
	}

	initVariableDino();

	bool game_quit = false;
	SDL_Event e;
	MENU_OPTIONS menuState = LOADING_SCREEN;

	while (!game_quit)
	{
		bool quit = true;
		if (menuState == LOADING_SCREEN)
		{
			menuState = handleDinoUI(e);
		}
		if (menuState == FULL_EXIT)
		{
			game_quit = true;
		}
		if (menuState == START_GAME)
		{
			quit = false;
		}
		if (menuState == SHOW_SCORE)
		{
			menuState = showDinoScore(e, username);
		}
		if (menuState == SHOW_HIGHSCORE)
		{
			menuState = showDinoHighScore(e);
		}
		if(menuState == HELP_MENU)
		{
			menuState = dinoHelpMenuUI(e);
		}
		while (!quit)
		{
			gIfResumeDino = true;

			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
				{
					quit = true;
					game_quit = true;
				}
				if (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE)
				{
					quit = true;
					menuState = LOADING_SCREEN;
				}
				gMyPlayer.handleEvent(e);
			}
			if (TOTAL_HEALTH <= 0)
			{

				menuState = SHOW_SCORE;
				quit = true;
				initVariableDino();
			}
			gMyPlayer.move();
			// printf("%d %d\n",gMyPlayer.mPosX,gMyPlayer.mPosY);
			renderMapDino();
			SDL_RenderPresent(gRender);
		}
	}
	freeDino();
	return 0;
}

#endif