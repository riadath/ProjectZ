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

//lava splash animation
Texture gLavaTexture;
int gCurLavaSprite = 0;
int gLavaDuration = 0;

const int LAVA_WIDTH = 59;
const int LAVA_HEIGHT = 53;
const int LAVA_SPRITE_COUNT = 5;
SDL_Rect gLavaSprite[LAVA_SPRITE_COUNT];

//strings for prompting score and health
std::stringstream scoreText, liveText, timeText;
//background texture
Texture gBackgroundTextureBB;
//score and health texture
Texture gLiveTexture, gScoreTexture;

Mix_Chunk *gSplash = NULL;

enum COLLISION_TYPE
{
	NO_COLLISION,
	WALL_COLLISION,
	BUCKET_COLLSIION
};

//function prototypes
void initVariable();
void closeBB();
void renderRoomBB();
bool loadBucketBallMedia();
int bucketBall();
void lavaAnimation();
COLLISION_TYPE checkCollision(SDL_Rect bucketShape, SDL_Rect ballShape);

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
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			switch (e.ksym)
			{
			case SDLK_RIGHT:
				mVelX += mVelocity;
				break;
			case SDLK_LEFT:
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
			case SDLK_RIGHT:
				mVelX -= mVelocity;
				break;
			case SDLK_LEFT:
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

	Timer mTimer;

	//variables for time management
	Uint32 mPrevTime1;
	Uint32 mPrevTime2;

	Texture mBallTexture;

	SDL_Rect mBallShape;
	SDL_Rect mBallSprite[8];

	//Ball poistion and sprite number
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
			COLLISION_TYPE whatCol = checkCollision(bucketShape, mBallRects[i].second);

			if (whatCol == WALL_COLLISION || whatCol == BUCKET_COLLSIION)
			{
				mBallRects[i].second.w = -1, mBallRects[i].second.h = -1;

				if (whatCol == BUCKET_COLLSIION)
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
	TOTAL_LIVES = 2;
	TOTAL_POINTS = 0;

	gBallDrop.mSpawnInterval = 173;
	gBallDrop.mSpawnSpeed = 0;
	gBallDrop.mBallDropSpeed = 2;
	gBallDrop.mEachCatchScore = 100;

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

COLLISION_TYPE checkCollision(SDL_Rect bucketShape, SDL_Rect ballShape)
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

	if (ifBucketCol)
	{
		Mix_PlayChannel(2, gSplash, 0);
		return BUCKET_COLLSIION;
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
	if (!gBackgroundTextureBB.loadFile("images/png/bucketBack.png"))
		return false;
	if (!gBallCatcher.mBucketTexture.loadFile("images/png/bucketLava.png"))
		return false;
	if (!gBallDrop.mBallTexture.loadFile("images/png/ball.png"))
		return false;
	if (!gLavaTexture.loadFile("images/png/lavaSplash.png"))
		return false;

	gFont = TTF_OpenFont("images/fonts/Oswald-BoldItalic.ttf", 24);
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

	return true;
}

void lavaAnimation()
{
	int posX = gBallCatcher.mPosX + 10;
	int posY = SCREEN_HEIGHT - gBallCatcher.mBucketTexture.getHeight() - 40;
	gLavaTexture.render(posX, posY, &gLavaSprite[gCurLavaSprite++ % LAVA_SPRITE_COUNT]);
}

void renderRoomBB()
{
	SDL_Color textColor = {0, 0, 0, 255};
	scoreText.str("");
	scoreText << "Score : " << TOTAL_POINTS;
	if (!gScoreTexture.loadFromText(scoreText.str().c_str(), textColor))
	{
		printf("Unable to render score texture\n");
	}
	liveText.str("");
	liveText << "Lives : " << TOTAL_LIVES;
	if (!gLiveTexture.loadFromText(liveText.str().c_str(), textColor))
	{
		printf("Unable to render health texture\n");
	}

	SDL_SetRenderDrawColor(gRender, 255, 255, 255, 255);
	SDL_RenderClear(gRender);
	gBackgroundTextureBB.render(0, 0);
	gScoreTexture.render(1050, 0);
	gLiveTexture.render(0, 0);
	gBallDrop.render(gBallCatcher.mBucketShape);
	gBallCatcher.render();
	if (--gLavaDuration > 0)
		lavaAnimation();
}

int bucketBall()
{
	initVariable();
	if (!loadBucketBallMedia())
	{
		printf("Failed to load Media\n");
		return -1;
	}
	bool quit = false;
	SDL_Event e;

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
				quit = true;
			gBallCatcher.handleEvent(e);
		}

		if (TOTAL_LIVES <= 0)
			quit = true;

		gBallCatcher.move();
		renderRoomBB();
		SDL_RenderPresent(gRender);
	}
	closeBB();
	return TOTAL_POINTS;
}

#endif