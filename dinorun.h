
#ifndef DINORUN_H
#define DINORUN_H

#include "texture_timer.h"

bool loadDinoMedia();

struct Player
{
	Texture mPlayerTexture;

	const int PLAYER_WIDTH = 59;
	const int PLAYER_HEIGHT = 65;
	const int PLAYER_VEL = 100;
	const int PLAYER_SETP = 10;
	const int JUMP_VEL = 150 + PLAYER_HEIGHT;
	const int JUMP_STEP = 9;
	const int GRAVITY = 8;
	const int SPRITE_COUNT = 6;
	const int SPRITE_SPEED = 8;
	const int BASE_HEIGHT = 570;

	int mPosX, mPosY;
	int mVelX, mVelY;
	int mCurSprite = 0;

	SDL_Rect *mPlayerSprite;
	SDL_Rect mPlayerShape;

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

	void handleEvent(SDL_Event &e)
	{
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		{
			switch (e.ksym)
			{
			case SDLK_w:
				if (mPosY < BASE_HEIGHT - PLAYER_HEIGHT)
					return;
				mVelY -= JUMP_VEL;
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

int TOTAL_SCORE = 0;

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

Player gMyPlayer;
Texture gBgTextureDino[LAYER_COUNT];

int gGroundScroll = 0;
int gTree1Scroll = 0;
int gTree2Scroll = 0;
int gTree3Scroll = 0;
int gStarScroll = 0;
int gStarDecrease = 1;

const int gGroundSpeed = 6;
const int gTree1Speed = 5;
const int gTree2Speed = 4;
const int gTree3Speed = 2;
const int gStarSpeed = 1;


bool loadDinoMedia()
{
	if (!gMyPlayer.mPlayerTexture.loadFile("images/png/dino/dino_runner.png"))
		return false;
	if (!gBgTextureDino[BUSH_LAYER_1].loadFile("images/png/dino/dino_bush.png"))
		return false;
	if (!gBgTextureDino[GROUND_LAYER_2].loadFile("images/png/dino/dino_ground.png"))
		return false;
	if (!gBgTextureDino[TREE1_LAYER_3].loadFile("images/png/dino/dino_tree1.png"))
		return false;
	if (!gBgTextureDino[TREE2_LAYER_4].loadFile("images/png/dino/dino_tree3.png"))
		return false;
	if (!gBgTextureDino[TREE3_LAYER_5].loadFile("images/png/dino/dino_tree3.png"))
		return false;
	if(!gBgTextureDino[STAR_LAYER_6].loadFile("images/png/dino/dino_star.png"))
		return false;
	if(!gBgTextureDino[MOON_LAYER_7].loadFile("images/png/dino/dino_moon.png"))
		return false;
	return true;
}

void renderMapDino()
{
	SDL_SetRenderDrawColor(gRender, 0, 0, 0, 155);
	SDL_RenderClear(gRender);

	gGroundScroll -= gGroundSpeed;
	if (gGroundScroll < -SCREEN_WIDTH)gGroundScroll = 0;
	gTree1Scroll -= gTree1Speed;
	if(gTree1Scroll < -SCREEN_WIDTH)gTree1Scroll = 0;
	gTree2Scroll -= gTree2Speed;
	if(gTree2Scroll < -SCREEN_WIDTH)gTree2Scroll = 0;
	gTree3Scroll -= gTree3Speed;
	if(gTree3Scroll < -SCREEN_WIDTH)gTree3Scroll = 0;
	gStarScroll -= gStarSpeed;
	if(gStarSpeed < -SCREEN_WIDTH)gStarScroll = 0;

	gBgTextureDino[MOON_LAYER_7].render(0,0);

	gBgTextureDino[STAR_LAYER_6].render(gStarScroll,0);
	gBgTextureDino[STAR_LAYER_6].render(gStarScroll + SCREEN_WIDTH,0);
	gBgTextureDino[TREE3_LAYER_5].render(gTree3Scroll,0);
	gBgTextureDino[TREE3_LAYER_5].render(gTree3Scroll + SCREEN_WIDTH,0);
	gBgTextureDino[TREE2_LAYER_4].render(gTree2Scroll,0);
	gBgTextureDino[TREE2_LAYER_4].render(gTree2Scroll + SCREEN_WIDTH,0);
	gBgTextureDino[TREE1_LAYER_3].render(gTree1Scroll,0);
	gBgTextureDino[TREE1_LAYER_3].render(gTree1Scroll + SCREEN_WIDTH,0);
	gBgTextureDino[GROUND_LAYER_2].render(gGroundScroll, 0);
	gBgTextureDino[GROUND_LAYER_2].render(gGroundScroll + SCREEN_WIDTH, 0);
	gBgTextureDino[BUSH_LAYER_1].render(gGroundScroll, 0);
	gBgTextureDino[BUSH_LAYER_1].render(gGroundScroll + SCREEN_WIDTH, 0);

	

	gMyPlayer.render();
}

int dinoRun()
{
	if (!loadDinoMedia())
	{
		printf("Failed to load Media(dinorun)\n");
		return -1;
	}
	bool quit = false;
	SDL_Event e;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			gMyPlayer.handleEvent(e);
		}
		gMyPlayer.move();

		renderMapDino();
		SDL_RenderPresent(gRender);
	}

	return TOTAL_SCORE;
}

#endif