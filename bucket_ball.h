/**
 * @file bucket_ball.h
 * @author Reyadath Ullah (SH - 33)
 * @brief Bucket Ball Game (One of the Tasks in the game)
 * @version 0.1
 * @date 30-04-2021
 * @copyright Copyright (c) 2021
 */

#ifndef BUCKETBALL_H
#define BUCKETBALL_H

#include "texture_timer.h"

int TOTAL_POINTS;
int TOTAL_LIVES;
int gCurLavaSprite = 0; //lava splash animation
int gLavaDuration = 0;

bool gIfResumeBucketBall = false;

const int LAVA_WIDTH = 59;
const int LAVA_HEIGHT = 53;
const int LAVA_SPRITE_COUNT = 5;
const int gBucketBallButtonCount = 4;

SDL_Rect gLavaSprite[LAVA_SPRITE_COUNT];
SDL_Rect gBucketBallButtonPosition[gBucketBallButtonCount];
SDL_Rect gBucketBackButtonPosition;
SDL_Rect gBucketHighscorePosition;

std::stringstream gBBscoreText; //strings for prompting score and health

Texture gBackgroundTextureBB; //background texture
Texture gLiveTexture;		  //score and health texture
Texture gScoreTexture;
Texture gLavaTexture;
Texture gBucketButtonTexture[gBucketBallButtonCount];
Texture gBucketMenuTexture;
Texture gBucketBackTexture;
Texture gBucketHighscoreTexture;
Texture gBucketTutorialTexture;

Mix_Chunk *gSplash = NULL;

enum COLLISION_TYPE
{
	NO_COLLISION,
	WALL_COLLISION,
	BUCKET_COLLSION
};

//function prototypes
void initVariable();
void closeBB();
void renderMapBB();
bool loadBucketBallMedia();
int bucketBall();
void lavaAnimation();
COLLISION_TYPE checkCollisionBB(SDL_Rect bucketShape, SDL_Rect ballShape);

struct Bucket
{
	int mPosX, mVelX;
	int mVelocity;

	Texture mBucketTexture;
	SDL_Rect mBucketShape;
	Bucket()
	{
		mPosX = 0;
		mVelX = 0;
		mVelocity = 13;
		mBucketShape = {mPosX, SCREEN_HEIGHT - mBucketTexture.getHeight(), mBucketTexture.getWidth(), mBucketTexture.getHeight()};
	}
	void handleEvent(SDL_Event &e)
	{
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		if (e.type == SDL_MOUSEMOTION)
		{
			mPosX = mouseX;
			mVelX = 0;
		}
		else if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			switch (e.ksym)
			{
			case SDLK_d:
				mVelX += mVelocity;
				break;
			case SDLK_a:
				mVelX -= mVelocity;
				break;
			default:
				break;
			}
		}
		else if (e.type == SDL_KEYUP & e.key.repeat == 0)
		{
			switch (e.ksym)
			{
			case SDLK_d:
				mVelX -= mVelocity;
				break;
			case SDLK_a:
				mVelX += mVelocity;
				break;
			default:
				break;
			}
		}
	}
	void move()
	{
		mPosX += mVelX;
		if (mPosX < 0 || (mPosX + mBucketTexture.getWidth() > SCREEN_WIDTH))
		{
			mPosX -= mVelX;
		}

		mBucketShape = {mPosX, SCREEN_HEIGHT - mBucketTexture.getHeight(), mBucketTexture.getWidth(), mBucketTexture.getHeight()};
	}
	void render()
	{
		mBucketTexture.render(mPosX, SCREEN_HEIGHT - mBucketTexture.getHeight());
	}
	void free()
	{
		mBucketTexture.free();
	}
};

struct Ball
{
	std::vector<int> mBallSpawnPosition;
	const int mSPRITE_COUNT = 8;
	const int mBALL_WIDTH = 21;
	const int mBALL_HEIGHT = 21;

	//variables for controlling ball spawn and drop speed
	int mSpawnSpeed;
	int mEachCatchScore;
	int mSpawnInterval;
	int mBallDropSpeed;

	//variables for time management
	Timer mTimer;
	Uint32 mPrevTime1;
	Uint32 mPrevTime2;

	Texture mBallTexture;

	SDL_Rect mBallShape;
	SDL_Rect mBallSprite[8];

	//Ball position and sprite number
	std::vector<std::pair<int, SDL_Rect>> mBallRects;

	Ball()
	{
		for (int i = 50; i <= SCREEN_WIDTH; i += 80)
		{
			mBallSpawnPosition.push_back(i);
		}
		for (int i = 0; i < mSPRITE_COUNT; i++)
		{
			mBallSprite[i].w = mBALL_WIDTH;
			mBallSprite[i].h = mBALL_HEIGHT;
			mBallSprite[i].x = i * mBALL_WIDTH;
			mBallSprite[i].y = 0;
		}
	}

	void ballSpawner(SDL_Rect bucketShape)
	{

		Uint32 BBTime = mTimer.getTicks() / 1000;
		if (BBTime % 3 == 0 && BBTime != mPrevTime1)
			mSpawnInterval = std::max(53, mSpawnInterval - 3), mPrevTime1 = BBTime;
		if (BBTime % 20 == 0 && BBTime != mPrevTime2)
			mBallDropSpeed += 1, mEachCatchScore += 20, mPrevTime2 = BBTime;

		if (mSpawnSpeed++ % mSpawnInterval == 0)
		{
			int tX = mBallSpawnPosition[rand() % mBallSpawnPosition.size()];
			int tY = 0;

			SDL_Rect tRect = {tX, tY, mBALL_WIDTH, mBALL_HEIGHT};
			mBallRects.push_back(std::make_pair(rand() % mSPRITE_COUNT, tRect));
		}

		//temporary list of ball positions
		std::vector<std::pair<int, SDL_Rect>> tempRect;

		for (int i = 0; i < mBallRects.size(); i++)
		{
			COLLISION_TYPE whatCol = checkCollisionBB(bucketShape, mBallRects[i].second);

			if (whatCol == WALL_COLLISION || whatCol == BUCKET_COLLSION)
			{
				mBallRects[i].second.w = -1, mBallRects[i].second.h = -1;

				if (whatCol == BUCKET_COLLSION)
				{
					TOTAL_POINTS += mEachCatchScore;
					gLavaDuration = 10;
				}
				else
					TOTAL_LIVES -= 1;
			}
			else
				tempRect.push_back(mBallRects[i]);
		}
		mBallRects = tempRect;
	}

	void render(SDL_Rect bucketShape)
	{
		ballSpawner(bucketShape);
		for (int i = 0; i < mBallRects.size(); i++)
		{
			mBallTexture.render(mBallRects[i].second.x, mBallRects[i].second.y,
								&mBallSprite[mBallRects[i].first]);
			mBallRects[i].second.y += mBallDropSpeed;
		}
	}

	void free()
	{
		mBallTexture.free();
		mBallRects.clear();
		mTimer.stop();
	}
};

Bucket gBallCatcher;
Ball gBallDrop;

void initVariable()
{
	TOTAL_LIVES = 5;

	gBallDrop.mSpawnInterval = 173;
	gBallDrop.mSpawnSpeed = 0;
	gBallDrop.mBallDropSpeed = 2;
	gBallDrop.mEachCatchScore = 100;
	gIfResumeBucketBall = false;
	gBallDrop.mTimer.start();
	gBallDrop.mPrevTime1 = gBallDrop.mTimer.getTicks() / 1000;
	gBallDrop.mPrevTime2 = gBallDrop.mTimer.getTicks() / 1000;
}

void closeBB()
{
	SDL_RenderClear(gRender);
	gBackgroundTextureBB.free();
	gBallDrop.free();
	gLiveTexture.free();
	gScoreTexture.free();
}

COLLISION_TYPE checkCollisionBB(SDL_Rect bucketShape, SDL_Rect ballShape)
{
	if (ballShape.y + ballShape.h < SCREEN_HEIGHT - bucketShape.h)
		return NO_COLLISION;
	bool ifBucketCol = true;
	if (ballShape.x + ballShape.w < bucketShape.x)
		ifBucketCol = false;
	if (ballShape.x > bucketShape.x + bucketShape.w)
		ifBucketCol = false;
	if (ballShape.y + ballShape.h < bucketShape.y)
		ifBucketCol = false;
	if (ballShape.y > bucketShape.y + bucketShape.h)
		ifBucketCol = false;

	if (ifBucketCol && ballShape.y <= SCREEN_HEIGHT - bucketShape.h / 2)
	{
		Mix_PlayChannel(2, gSplash, 0);
		return BUCKET_COLLSION;
	}

	else
	{
		if (ballShape.y + ballShape.h > SCREEN_HEIGHT)
			return WALL_COLLISION;
		else
			return NO_COLLISION;
	}
}

bool loadBucketBallMedia()
{
	if (!gBackgroundTextureBB.loadFile("images/BucketBall/bucketBack1.png"))
		return false;
	if (!gBallCatcher.mBucketTexture.loadFile("images/BucketBall/bucketLava.png"))
		return false;
	if (!gBallDrop.mBallTexture.loadFile("images/BucketBall/ball.png"))
		return false;
	if (!gLavaTexture.loadFile("images/BucketBall/lavaSplash.png"))
		return false;
	if (!gLiveTexture.loadFile("images/health.png"))
		return false;
	if (!gBucketButtonTexture[PLAY].loadFile("images/main/play_button.png"))
		return false;
	if (!gBucketButtonTexture[CONTINUE].loadFile("images/main/continue_button.png"))
		return false;
	if (!gBucketButtonTexture[HELP].loadFile("images/main/help_button.png"))
		return false;
	if (!gBucketButtonTexture[EXIT].loadFile("images/main/exit_button.png"))
		return false;
	if (!gBucketMenuTexture.loadFile("images/BucketBall/menu_back.png"))
		return false;
	if (!gBucketBackTexture.loadFile("images/main/back_button.png"))
		return false;
	if (!gBucketHighscoreTexture.loadFile("images/main/highscore_button.png"))
		return false;
	if(!gBucketTutorialTexture.loadFile("images/bucketBall/tutorial_bucket.png"))
		return false;

	gFont = TTF_OpenFont("images/fonts/Oswald-BoldItalic.ttf", 21);
	if (gFont == NULL)
	{
		ERROR_T;
		return false;
	}

	gSplash = Mix_LoadWAV("sounds/splish.wav");
	if (gSplash == NULL)
	{
		ERROR_M;
		return false;
	}

	for (int i = 0; i < LAVA_SPRITE_COUNT; i++)
	{
		gLavaSprite[i].x = i * LAVA_WIDTH;
		gLavaSprite[i].y = 0;
		gLavaSprite[i].w = LAVA_WIDTH;
		gLavaSprite[i].h = LAVA_HEIGHT;
	}

	for (int i = 0; i < gBucketBallButtonCount; i++)
	{
		gBucketBallButtonPosition[i].x = SCREEN_WIDTH / 2 - 200;
		gBucketBallButtonPosition[i].y = (i - 1) * (i != 0) * 100 + 100;
		gBucketBallButtonPosition[i].w = 309;
		gBucketBallButtonPosition[i].h = 55;
	}

	gBucketHighscorePosition.x = SCREEN_WIDTH / 2 - 200;
	gBucketHighscorePosition.y = 3 * 100 + 100;
	gBucketHighscorePosition.w = 309;
	gBucketHighscorePosition.h = 55;

	gBucketBackButtonPosition.x = 0;
	gBucketBackButtonPosition.y = 0;
	gBucketBackButtonPosition.w = gBucketBackTexture.getWidth();
	gBucketBackButtonPosition.h = gBucketBackTexture.getHeight();

	return true;
}

void lavaAnimation()
{
	int posX = gBallCatcher.mPosX + 10;
	int posY = SCREEN_HEIGHT - gBallCatcher.mBucketTexture.getHeight() - 40;
	gLavaTexture.render(posX, posY, &gLavaSprite[gCurLavaSprite++ % LAVA_SPRITE_COUNT]);
}

void renderMapBB()
{
	SDL_Color textColor = {0, 0, 0, 255};
	gBBscoreText.str("");
	gBBscoreText << "Score : " << TOTAL_POINTS;
	if (!gScoreTexture.loadFromText(gBBscoreText.str().c_str(), textColor))
	{
		printf("Unable to render score texture\n");
	}

	SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
	SDL_RenderClear(gRender);
	gBackgroundTextureBB.render(0, 0);
	gScoreTexture.render(1050, 0);
	gBallDrop.render(gBallCatcher.mBucketShape);
	gBallCatcher.render();
	for (int i = 0; i < TOTAL_LIVES; i++)
	{
		gLiveTexture.render(i * gLiveTexture.getWidth(), 0);
	}
	if (--gLavaDuration > 0)
		lavaAnimation();
}

MENU_OPTIONS handleBucketBallUI(SDL_Event &e)
{
	gBallDrop.mTimer.pause();
	bool quit = false;
	while (!quit)
	{
		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);

		// Mouse hover animation on button
		for (int i = 0; i < gBucketBallButtonCount; i++)
		{
			gBucketButtonTexture[i].setAlpha(255);
			if (mouseX >= gBucketBallButtonPosition[i].x && mouseX <= gBucketBallButtonPosition[i].x + gBucketBallButtonPosition[i].w && mouseY >= gBucketBallButtonPosition[i].y && mouseY <= gBucketBallButtonPosition[i].y + gBucketBallButtonPosition[i].h)
			{
				gBucketButtonTexture[i].setAlpha(200);
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
				if (mouseX >= gBucketBallButtonPosition[PLAY].x && mouseX <= gBucketBallButtonPosition[PLAY].x + gBucketBallButtonPosition[PLAY].w && mouseY >= gBucketBallButtonPosition[PLAY].y && mouseY <= gBucketBallButtonPosition[PLAY].y + gBucketBallButtonPosition[PLAY].h)
				{
					gBallDrop.mTimer.unpause();
					return START_GAME;
				}

				if (mouseX >= gBucketBallButtonPosition[EXIT].x && mouseX <= gBucketBallButtonPosition[EXIT].x + gBucketBallButtonPosition[EXIT].w && mouseY >= gBucketBallButtonPosition[EXIT].y && mouseY <= gBucketBallButtonPosition[EXIT].y + gBucketBallButtonPosition[EXIT].h)
					return FULL_EXIT;

				if (mouseX >= gBucketBallButtonPosition[HELP].x && mouseX <= gBucketBallButtonPosition[HELP].x + gBucketBallButtonPosition[HELP].w && mouseY >= gBucketBallButtonPosition[HELP].y && mouseY <= gBucketBallButtonPosition[HELP].y + gBucketBallButtonPosition[HELP].h)
					return HELP_MENU;

				if (mouseX >= gBucketHighscorePosition.x && mouseX <= gBucketHighscorePosition.x + gBucketHighscorePosition.w && mouseY >= gBucketHighscorePosition.y && mouseY <= gBucketHighscorePosition.y + gBucketHighscorePosition.h)
					return SHOW_HIGHSCORE;
			}
		}
		//render UI
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);

		//render UI background and buttons
		gBucketMenuTexture.render(0, 0);
		int st = 0, skip = -1;
		if (gIfResumeBucketBall)
			st = 1;
		else
			skip = 1;

		for (int i = st; i < gBucketBallButtonCount; i++)
		{
			if (skip != -1 && i == skip)
				continue;
			int pos = i;
			if (i == 1)
				pos = i - st;
			gBucketButtonTexture[i].render(gBucketBallButtonPosition[pos].x, gBucketBallButtonPosition[pos].y);
		}
		gBucketHighscoreTexture.render(gBucketHighscorePosition.x, gBucketHighscorePosition.y);
		SDL_RenderPresent(gRender);
	}
	return LOADING_SCREEN;
}

MENU_OPTIONS showBucketBallScore(SDL_Event &e, std::string username)
{
	gBallDrop.mTimer.stop();
	bool quit = false;
	Texture tScoreTexture;
	SDL_Color textColor = {255, 180, 0, 255};
	std::stringstream tempText;
	tempText << "Your Score : " << TOTAL_POINTS;
	tScoreTexture.loadFromText(tempText.str().c_str(), textColor);

	//insert score to high score list
	std::vector<std::pair<std::string, int>> scoreList;
	std::ifstream highscoreFile;

	highscoreFile.open("saved_files/bucketball.score");
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
	highscoreFileOut.open("saved_files/bucketball.score");
	if (!highscoreFileOut)
	{
		printf("Could not open file\n");
	}

	scoreList.push_back(std::make_pair(username, TOTAL_POINTS));
	std::sort(scoreList.begin(), scoreList.end(), comp);
	std::map<std::string, bool> tCheck;

	for (int i = 0; i < std::min(10, (int)scoreList.size()); i++)
	{
		highscoreFileOut << scoreList[i].first << " " << scoreList[i].second << "\n";
	}

	highscoreFileOut.close();
	TOTAL_POINTS = 0;

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
				if (mouseX >= gBucketBackButtonPosition.x && mouseX <= gBucketBackButtonPosition.x + gBucketBackButtonPosition.w && mouseY >= gBucketBackButtonPosition.y && mouseY <= gBucketBackButtonPosition.y + gBucketBackButtonPosition.h)
				{
					gBallDrop.mTimer.start();
					return LOADING_SCREEN;
				}
			}
		}
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);
		gBucketMenuTexture.render(0, 0);
		gBucketBackTexture.render(gBucketBackButtonPosition.x, gBucketBackButtonPosition.y);
		tScoreTexture.render(SCREEN_WIDTH / 2 - tScoreTexture.getWidth(), 100);
		SDL_RenderPresent(gRender);
	}
	return FULL_EXIT;
}

MENU_OPTIONS showBucketBallHighScore(SDL_Event &e)
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
				if (mouseX >= gBucketBackButtonPosition.x && mouseX <= gBucketBackButtonPosition.x + gBucketBackButtonPosition.w && mouseY >= gBucketBackButtonPosition.y && mouseY <= gBucketBackButtonPosition.y + gBucketBackButtonPosition.h)
				{
					return LOADING_SCREEN;
				}
			}
		}
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);
		gBucketMenuTexture.render(0, 0);
		gBucketBackTexture.render(gBucketBackButtonPosition.x, gBucketBackButtonPosition.y);

		std::ifstream highscore;
		highscore.open("saved_files/bucketball.score");
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

MENU_OPTIONS bucketHelpMenuUI(SDL_Event &e)
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
				if (mouseX >= gBucketBackButtonPosition.x && mouseX <= gBucketBackButtonPosition.x + gBucketBackButtonPosition.w && mouseY >= gBucketBackButtonPosition.y && mouseY <= gBucketBackButtonPosition.y + gBucketBackButtonPosition.h)
				{
					return LOADING_SCREEN;
				}
			}
		}
		SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
		SDL_RenderClear(gRender);

		//render background
		gBucketTutorialTexture.render(0, 0);
		gBucketBackTexture.render(0,0);
		SDL_RenderPresent(gRender);
	}
	return FULL_EXIT;
}


int bucketBall(std::string username)
{
	initVariable();

	if (!loadBucketBallMedia())
	{
		printf("Failed to load Media\n");
		return -1;
	}

	bool game_quit = false;
	SDL_Event e;
	MENU_OPTIONS menuState = LOADING_SCREEN;
	while (!game_quit)
	{
		bool quit = true;
		if (menuState == LOADING_SCREEN)
		{
			menuState = handleBucketBallUI(e);
		}
		if (menuState == FULL_EXIT)
		{
			game_quit = true;
		}
		if (menuState == START_GAME)
		{
			gBallDrop.mTimer.start();
			quit = false;
		}
		if (menuState == SHOW_SCORE)
		{
			menuState = showBucketBallScore(e, username);
		}
		if (menuState == SHOW_HIGHSCORE)
		{
			menuState = showBucketBallHighScore(e);
		}
		if(menuState == HELP_MENU)
		{
			menuState = bucketHelpMenuUI(e);
		}
		while (!quit)
		{
			gIfResumeBucketBall = true;
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
				{
					quit = true;
					menuState = LOADING_SCREEN;
				}
				if (e.type == SDL_KEYDOWN && e.ksym == SDLK_ESCAPE)
				{
					quit = true;
					menuState = LOADING_SCREEN;
				}
				gBallCatcher.handleEvent(e);
			}

			if (TOTAL_LIVES <= 0)
			{
				menuState = SHOW_SCORE;
				quit = true;
				initVariable();
				gBallDrop.mTimer.stop();
			}

			gBallCatcher.move();
			renderMapBB();
			SDL_RenderPresent(gRender);
		}
	}
	closeBB();
	return 0;
}

#endif